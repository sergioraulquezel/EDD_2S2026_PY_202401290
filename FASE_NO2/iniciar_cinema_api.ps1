$ErrorActionPreference = 'Stop'
$backendDir = Join-Path $PSScriptRoot 'backend'
$serverPath = Join-Path $backendDir 'CinemaServer_vNext.exe'
$port = 18080
$stamp = Get-Date -Format 'yyyyMMdd-HHmmss'
$stdoutLog = Join-Path $backendDir "CinemaServer-$stamp.log"
$stderrLog = Join-Path $backendDir "CinemaServer-$stamp-error.log"
$backendPrefix = $backendDir.TrimEnd('\') + '\'

if (-not (Test-Path -LiteralPath $serverPath)) {
    throw "No se encontró el backend compilado: $serverPath"
}

$listeners = Get-NetTCPConnection -LocalPort $port -State Listen -ErrorAction SilentlyContinue
foreach ($listener in ($listeners | Sort-Object OwningProcess -Unique)) {
    $processInfo = Get-CimInstance Win32_Process -Filter "ProcessId = $($listener.OwningProcess)" -ErrorAction SilentlyContinue
    if (-not $processInfo) { continue }
    $processPath = $processInfo.ExecutablePath
    if ($processPath -and $processPath.StartsWith($backendPrefix, [System.StringComparison]::OrdinalIgnoreCase) -and $processInfo.Name -like 'CinemaServer*') {
        Stop-Process -Id $listener.OwningProcess -Force
    } else {
        throw "El puerto $port lo ocupa otro programa ($($processInfo.Name), PID $($listener.OwningProcess)). Ciérralo y vuelve a intentar."
    }
}

$previousPort = $env:CINEMA_PORT
$previousPath = $env:PATH
try {
    $env:CINEMA_PORT = [string]$port
    $env:PATH = "C:\msys64\ucrt64\bin;$previousPath"
    $serverProcess = Start-Process -FilePath $serverPath -WorkingDirectory $backendDir -WindowStyle Hidden -PassThru -RedirectStandardOutput $stdoutLog -RedirectStandardError $stderrLog
} finally {
    if ($null -eq $previousPort) { Remove-Item Env:CINEMA_PORT -ErrorAction SilentlyContinue } else { $env:CINEMA_PORT = $previousPort }
    $env:PATH = $previousPath
}

Start-Sleep -Seconds 1
if ($serverProcess.HasExited) {
    $startupError = if (Test-Path $stderrLog) { Get-Content -LiteralPath $stderrLog -Raw } else { '' }
    $startupOutput = if (Test-Path $stdoutLog) { Get-Content -LiteralPath $stdoutLog -Raw } else { '' }
    throw "CinemaServer terminó al iniciar. STDERR: $startupError STDOUT: $startupOutput"
}

foreach ($route in @('/api/health', '/api/sedes', '/api/sedes/ranking')) {
    try {
        $response = Invoke-RestMethod -Uri "http://127.0.0.1:$port$route" -TimeoutSec 4
        if ($response.ok -ne $true) { throw "La ruta respondió ok=false." }
    } catch {
        $statusCode = if ($_.Exception.Response) { [int]$_.Exception.Response.StatusCode } else { 'sin respuesta HTTP' }
        $stderrText = if (Test-Path $stderrLog) { Get-Content -LiteralPath $stderrLog -Raw } else { '' }
        throw "Falló $route (HTTP $statusCode): $($_.Exception.Message). Log: $stderrLog. $stderrText"
    }
}

Write-Host "CinemaServer actualizado activo en http://localhost:$port"
Write-Host 'Rutas verificadas: /api/health, /api/sedes y /api/sedes/ranking'
Write-Host "Log del servidor: $stdoutLog"

