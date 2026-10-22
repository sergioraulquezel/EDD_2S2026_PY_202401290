# Cinema API

Servidor HTTP construido con Crow para exponer las operaciones de `CinemaSystem`.

## Ejecutar

El ejecutable generado es:

`build_manual_codex/Debug/CinemaServer_final.exe`

El puerto predeterminado es `18080`. Puede cambiarse con la variable de entorno `CINEMA_PORT`.

Ejemplo:

`set CINEMA_PORT=18082`

## Rutas

### Salud

`GET /api/health`

Comprueba que el servidor está activo.

### Películas

`GET /api/peliculas`

Lista las películas del BST mediante recorrido inorden. Puede recibir `?recorrido=inorden`, `?recorrido=preorden` o `?recorrido=postorden`.

`GET /api/peliculas/{codigo}`

Busca una película por su código.

`POST /api/peliculas`

Registra una película. El cuerpo JSON utiliza los campos `codigo`, `titulo`, `genero`, `duracion`, `clasificacion`, `idioma`, `fechaEstreno` y `fechaFin`.

### Funciones

`GET /api/funciones`

Lista las funciones registradas en el AVL mediante recorrido inorden. Puede recibir `?recorrido=inorden`, `?recorrido=preorden` o `?recorrido=postorden`.

`GET /api/funciones/{codigoFuncion}`

Busca una función por su código.

`POST /api/funciones`

Crea una función. El cuerpo JSON utiliza `codigoFuncion`, `codigoPelicula`, `sala`, `horario`, `filas` y `columnas`.

`PUT /api/funciones/{codigoFuncion}`

Edita una función conservando las reservas que todavía caben dentro de sus nuevas dimensiones.

`GET /api/funciones/{codigoFuncion}/asientos`

Devuelve la matriz de asientos persistida y sus estadísticas.

### Reservas

`POST /api/reservas`

Reserva un asiento. El cuerpo utiliza `codigoReserva`, `codigoCliente`, `codigoFuncion`, `fila` y `columna`.

`DELETE /api/clientes/{codigoCliente}/reservas/{codigoReserva}`

Cancela una reserva y libera el asiento correspondiente.

### Autenticación

`POST /api/login`

Recibe `correo` y `password`. El administrador utiliza las credenciales configuradas por el sistema; los clientes se validan contra el Árbol B.

## CORS

La API permite solicitudes desde cualquier origen para facilitar la conexión posterior con el frontend web.
