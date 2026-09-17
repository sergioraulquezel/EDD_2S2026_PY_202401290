#include "utils/PersistenciaAsientos.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

static std::string leerArchivoAsientos(const std::string& ruta)
{
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) return "";
    std::stringstream buffer;
    buffer << archivo.rdbuf();
    return buffer.str();
}

static size_t saltarEspaciosAsientos(const std::string& texto, size_t pos)
{
    while (pos < texto.size() && std::isspace(static_cast<unsigned char>(texto[pos]))) ++pos;
    return pos;
}

static std::string obtenerStringAsientos(const std::string& objeto, const std::string& clave)
{
    std::string patron = "\"" + clave + "\"";
    size_t pos = objeto.find(patron);
    if (pos == std::string::npos) return "";
    pos = objeto.find(':', pos);
    if (pos == std::string::npos) return "";
    pos = saltarEspaciosAsientos(objeto, pos + 1);
    if (pos >= objeto.size() || objeto[pos] != '"') return "";

    std::string valor;
    for (++pos; pos < objeto.size(); ++pos) {
        char ch = objeto[pos];
        if (ch == '\\' && pos + 1 < objeto.size()) {
            valor += objeto[++pos];
        } else if (ch == '"') {
            break;
        } else {
            valor += ch;
        }
    }
    return valor;
}

static int obtenerIntAsientos(const std::string& objeto, const std::string& clave)
{
    std::string patron = "\"" + clave + "\"";
    size_t pos = objeto.find(patron);
    if (pos == std::string::npos) return 0;
    pos = objeto.find(':', pos);
    if (pos == std::string::npos) return 0;
    pos = saltarEspaciosAsientos(objeto, pos + 1);

    std::string numero;
    while (pos < objeto.size() && (std::isdigit(static_cast<unsigned char>(objeto[pos])) || objeto[pos] == '-')) {
        numero += objeto[pos++];
    }
    return numero.empty() ? 0 : std::stoi(numero);
}

static std::string extraerArregloAsientos(const std::string& objeto, const std::string& clave)
{
    std::string patron = "\"" + clave + "\"";
    size_t pos = objeto.find(patron);
    if (pos == std::string::npos) return "";
    pos = objeto.find('[', pos);
    if (pos == std::string::npos) return "";

    int nivel = 0;
    size_t inicio = pos;
    bool enString = false;
    for (; pos < objeto.size(); ++pos) {
        char ch = objeto[pos];
        if (ch == '"' && (pos == 0 || objeto[pos - 1] != '\\')) {
            enString = !enString;
        }
        if (enString) continue;

        if (ch == '[') ++nivel;
        else if (ch == ']') {
            --nivel;
            if (nivel == 0) return objeto.substr(inicio + 1, pos - inicio - 1);
        }
    }
    return "";
}

static std::vector<std::string> extraerObjetosAsientos(const std::string& texto)
{
    std::vector<std::string> objetos;
    int nivel = 0;
    size_t inicio = std::string::npos;
    bool enString = false;

    for (size_t i = 0; i < texto.size(); ++i) {
        char ch = texto[i];
        if (ch == '"' && (i == 0 || texto[i - 1] != '\\')) {
            enString = !enString;
        }
        if (enString) continue;

        if (ch == '{') {
            if (nivel == 0) inicio = i;
            ++nivel;
        } else if (ch == '}') {
            --nivel;
            if (nivel == 0 && inicio != std::string::npos) {
                objetos.push_back(texto.substr(inicio, i - inicio + 1));
                inicio = std::string::npos;
            }
        }
    }
    return objetos;
}

bool guardarAsientosFuncion(FuncionCine& funcion)
{
    std::ofstream archivo(funcion.archivoAsientos);
    if (!archivo.is_open()) {
        std::cout << "[ASIENTOS] No se pudo escribir " << funcion.archivoAsientos << ".\n";
        return false;
    }

    archivo << "{\n";
    archivo << "  \"codigo_funcion\": \"" << funcion.codigoFuncion << "\",\n";
    archivo << "  \"asientos_ocupados\": [\n";

    bool primero = true;
    for (int f = 1; f <= funcion.asientos.obtenerFilas(); ++f) {
        for (int c = 1; c <= funcion.asientos.obtenerColumnas(); ++c) {
            NodoMatriz* asiento = funcion.asientos.obtenerAsiento(f, c);
            if (asiento == nullptr) continue;

            if (!primero) archivo << ",\n";
            primero = false;
            archivo << "    {\n";
            archivo << "      \"fila\": " << f << ",\n";
            archivo << "      \"columna\": " << c << ",\n";
            archivo << "      \"codigo_reserva\": \"" << asiento->titular << "\"\n";
            archivo << "    }";
        }
    }

    archivo << "\n  ]\n";
    archivo << "}\n";
    return true;
}

bool crearArchivoAsientosFuncion(FuncionCine& funcion)
{
    return guardarAsientosFuncion(funcion);
}

bool cargarAsientosFuncion(FuncionCine& funcion)
{
    int filas = funcion.asientos.obtenerFilas();
    int columnas = funcion.asientos.obtenerColumnas();
    if (filas <= 0 || columnas <= 0) {
        std::cout << "[ASIENTOS] Funcion sin dimensiones validas: " << funcion.codigoFuncion << ".\n";
        return false;
    }

    std::string contenido = leerArchivoAsientos(funcion.archivoAsientos);
    funcion.asientos.configurarSala(filas, columnas);

    if (contenido.empty()) {
        std::cout << "[ASIENTOS] Archivo no existe o esta vacio. Se crea: " << funcion.archivoAsientos << ".\n";
        return guardarAsientosFuncion(funcion);
    }

    std::string arreglo = extraerArregloAsientos(contenido, "asientos_ocupados");
    std::vector<std::string> asientos = extraerObjetosAsientos(arreglo);
    int cargados = 0;

    for (const std::string& asientoObj : asientos) {
        int fila = obtenerIntAsientos(asientoObj, "fila");
        int columna = obtenerIntAsientos(asientoObj, "columna");
        std::string codigoReserva = obtenerStringAsientos(asientoObj, "codigo_reserva");
        if (fila <= 0 || columna <= 0 || codigoReserva.empty()) {
            std::cout << "[ASIENTOS] Asiento ignorado en " << funcion.archivoAsientos << ".\n";
            continue;
        }
        if (funcion.asientos.reservarAsiento(fila, columna, codigoReserva, false)) {
            ++cargados;
        }
    }

    std::cout << "[ASIENTOS] " << cargados << " asientos cargados para " << funcion.codigoFuncion << ".\n";
    return true;
}
