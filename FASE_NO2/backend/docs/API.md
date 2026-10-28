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

Crea una función asignada obligatoriamente a una sede. El cuerpo JSON utiliza `pelicula`, `sede`, `sala`, `horario`, `filas` y `columnas`. Al guardarla, recalcula las relaciones del grafo.

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

### Sedes y grafo no dirigido

`GET /api/sedes` lista sedes y `POST /api/sedes` registra `{ "codigo", "nombre", "direccion", "cantidad_salas", "telefono" }`. Una sede nueva empieza sin aristas.

`GET /api/sedes/{codigo}/peliculas` consulta las películas programadas en esa sede con horario, sala y código de función.

`GET /api/sedes/camino?origen=S001&destino=S002` usa BFS para devolver una ruta con el menor número de conexiones.

`GET /api/sedes/ranking` ordena las aristas por peso descendente; el peso es el número de películas compartidas.
