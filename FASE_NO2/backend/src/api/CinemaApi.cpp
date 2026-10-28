#include "api/CinemaApi.h"

#include "crow/middlewares/cors.h"
#include "utils/PersistenciaAsientos.h"
#include "utils/CargaJSONFase2.h"
#include "utils/CargaJSONClientesReservas.h"
#include <string>
#include <utility>
#include <set>
#include <map>
#include <vector>
#include <limits>

namespace
{
crow::response respuestaJson(int estado, bool ok, const std::string& mensaje)
{
    crow::json::wvalue body;
    body["ok"] = ok;
    body["message"] = mensaje;

    crow::response respuesta(estado);
    respuesta.set_header("Content-Type", "application/json");
    respuesta.write(body.dump());
    return respuesta;
}

crow::response respuestaJson(const crow::json::wvalue& body)
{
    crow::response respuesta;
    respuesta.set_header("Content-Type", "application/json");
    respuesta.write(body.dump());
    return respuesta;
}

crow::response respuestaJson(int estado, const crow::json::wvalue& body)
{
    crow::response respuesta(estado);
    respuesta.set_header("Content-Type", "application/json");
    respuesta.write(body.dump());
    return respuesta;
}

bool tieneTexto(const crow::json::rvalue& body, const char* campo)
{
    return body[campo] && body[campo].t() == crow::json::type::String &&
           !std::string(body[campo].s()).empty();
}

crow::json::wvalue peliculaJson(const Pelicula& pelicula)
{
    return crow::json::wvalue({
        {"id", pelicula.getId()},
        {"codigo", pelicula.getCodigo()},
        {"titulo", pelicula.getTitulo()},
        {"genero", pelicula.getGenero()},
        {"duracion", pelicula.getDuracion()},
        {"clasificacion", pelicula.getClasificacion()},
        {"idioma", pelicula.getIdioma()},
        {"fecha_estreno", pelicula.getFechaEstreno()},
        {"fecha_fin", pelicula.getFechaFin()}
    });
}

crow::json::wvalue funcionJson(const FuncionCine& funcion)
{
    crow::json::wvalue resultado({
        {"id", funcion.id},
        {"codigo_funcion", funcion.codigoFuncion},
        {"pelicula", funcion.pelicula != nullptr ? funcion.pelicula->getCodigo() : ""},
        {"titulo", funcion.pelicula != nullptr ? funcion.pelicula->getTitulo() : "Sin pelicula"},
        {"horario", funcion.horario},
        {"sala", funcion.sala}, {"sede", funcion.sede},
        {"filas", funcion.asientos.obtenerFilas()},
        {"columnas", funcion.asientos.obtenerColumnas()}
    });
    resultado["reservados"] = funcion.asientos.contarReservados();
    return resultado;
}

bool leerCamposFuncion(const crow::json::rvalue& body, std::string& horario,
                       std::string& sala, int& filas, int& columnas)
{
    if (!tieneTexto(body, "horario") || !tieneTexto(body, "sala") ||
        !body["filas"] || !body["columnas"]) {
        return false;
    }

    horario = body["horario"].s();
    sala = body["sala"].s();
    filas = body["filas"].i();
    columnas = body["columnas"].i();
    return true;
}
}

void configurarRutasCinema(crow::App<crow::CORSHandler>& app, CinemaSystem& sistema)
{
    CROW_ROUTE(app, "/api/health")
    ([] {
        crow::json::wvalue body;
        body["ok"] = true;
        body["service"] = "Cinema API";
        body["message"] = "Servidor funcionando";
        return respuestaJson(body);
    });

    CROW_ROUTE(app, "/api/reportes/matriz/<string>")
    .methods(crow::HTTPMethod::Post)
    ([&sistema](const std::string& codigoFuncion) {
        FuncionCine* funcion = sistema.buscarFuncion(codigoFuncion);
        if (funcion == nullptr) {
            return respuestaJson(404, false, "Funcion no encontrada.");
        }

        // Recupera las reservas persistidas antes de dibujar el reporte.
        cargarAsientosFuncion(*funcion);
        const std::string titulo = "Funcion: " +
            (funcion->pelicula != nullptr ? funcion->pelicula->getTitulo() : "Sin pelicula") +
            " - " + funcion->horario + " - Sala " + funcion->sala;
        funcion->asientos.generarReporteGraphviz(titulo);

        crow::json::wvalue body;
        body["ok"] = true;
        body["message"] = "Reporte de matriz de asientos generado.";
        body["codigo_funcion"] = codigoFuncion;
        const int total = funcion->asientos.obtenerFilas() * funcion->asientos.obtenerColumnas();
        const int reservados = funcion->asientos.contarReservados();
        body["total"] = total;
        body["reservados"] = reservados;
        body["libres"] = total - reservados;
        body["archivo_dot"] = "reporte_matriz_asientos.dot";
        body["archivo_png"] = "reporte_matriz_asientos.png";
        return respuestaJson(body);
    });
    CROW_ROUTE(app, "/api/reportes/<string>")
    .methods(crow::HTTPMethod::Post)
    ([&sistema](const std::string& tipo) {
        if (tipo == "bst") sistema.obtenerCartelera().generarReporteGraphviz();
        else if (tipo == "avl") sistema.obtenerArbolFunciones().generarReporteGraphviz();
        else if (tipo == "promociones") sistema.obtenerPromociones().generarReporteGraphviz();
        else if (tipo == "sedes") sistema.obtenerGrafoSedes().generarReporteGraphviz();
        else return respuestaJson(404, false, "Tipo de reporte no reconocido.");

        crow::json::wvalue body;
        body["ok"] = true;
        body["message"] = "Reporte Graphviz generado";
        body["tipo"] = tipo;
        body["archivo_dot"] = "reporte_" + tipo + ".dot";
        body["archivo_png"] = tipo == "sedes" ? "reporte_grafo_sedes.png" : "reporte_" + tipo + ".png";
        return respuestaJson(body);
    });

    CROW_ROUTE(app, "/api/clientes")
    ([&sistema] {
        crow::json::wvalue body;
        body["ok"] = true;
        body["message"] = "Clientes obtenidos";
        crow::json::wvalue::list items;
        for (const Cliente& cliente : sistema.obtenerClientes().listarOrdenado()) {
            crow::json::wvalue item({{"id", cliente.id}, {"nombre", cliente.nombre},
                                     {"correo", cliente.correo}, {"telefono", cliente.telefono},
                                     {"tipo", cliente.tipo},
                                     {"reservas", static_cast<int>(cliente.codigosReserva.size())}});
            items.emplace_back(std::move(item));
        }
        body["data"] = std::move(items);
        return respuestaJson(body);
    });

    CROW_ROUTE(app, "/api/clientes")
    .methods(crow::HTTPMethod::Post)
    ([&sistema](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !tieneTexto(body, "id") || !tieneTexto(body, "nombre") ||
            !tieneTexto(body, "correo") || !tieneTexto(body, "telefono") ||
            !tieneTexto(body, "password")) {
            return respuestaJson(400, false, "Faltan datos obligatorios del cliente.");
        }
        if (sistema.obtenerClientes().buscarPorId(body["id"].s()) != nullptr ||
            sistema.obtenerClientes().buscarPorCorreo(body["correo"].s()) != nullptr) {
            return respuestaJson(400, false, "El ID o correo del cliente ya existe.");
        }
        bool creado = sistema.obtenerClientes().insertar(Cliente(body["id"].s(), body["nombre"].s(),
            body["correo"].s(), body["telefono"].s(), body["password"].s()));
        return respuestaJson(creado ? 201 : 400, creado, creado ? "Cliente agregado al Arbol B." : "No se pudo agregar el cliente.");
    });

    CROW_ROUTE(app, "/api/clientes/<string>")
    .methods(crow::HTTPMethod::Delete)
    ([&sistema](const std::string& idCliente) {
        std::string mensaje;
        bool eliminado = sistema.eliminarCliente(idCliente, mensaje);
        return respuestaJson(eliminado ? 200 : 404, eliminado, mensaje);
    });

    CROW_ROUTE(app, "/api/reservas")
    ([&sistema] {
        crow::json::wvalue body;
        body["ok"] = true;
        body["message"] = "Reservas obtenidas";
        crow::json::wvalue::list items;
        for (const Reserva& reserva : sistema.obtenerReservas().listarTodas()) {
            items.emplace_back(crow::json::wvalue({{"codigo", reserva.codigoReserva},
                {"cliente", reserva.idCliente}, {"funcion", reserva.codigoFuncion},
                {"fecha", reserva.fechaReserva}, {"fila", reserva.fila},
                {"columna", reserva.columna}, {"estado", reserva.estado}}));
        }
        body["data"] = std::move(items);
        return respuestaJson(body);
    });

    CROW_ROUTE(app, "/api/solicitudes")
    ([&sistema] {
        crow::json::wvalue body;
        body["ok"] = true;
        crow::json::wvalue::list items;
        ListaCircularDoble& lista = sistema.obtenerSolicitudes();
        if (lista.cabeza != nullptr) {
            NodoSolicitud* actual = lista.cabeza;
            do {
                items.emplace_back(crow::json::wvalue({{"id", actual->solicitud->idSolicitud},
                    {"cliente", actual->solicitud->nombreCliente},
                    {"descripcion", actual->solicitud->descripcionEvento},
                    {"estado", actual->solicitud->estado}}));
                actual = actual->siguiente;
            } while (actual != lista.cabeza);
        }
        body["message"] = "Solicitudes obtenidas";
        body["data"] = std::move(items);
        return respuestaJson(body);
    });

    CROW_ROUTE(app, "/api/solicitudes")
    .methods(crow::HTTPMethod::Post)
    ([&sistema](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !tieneTexto(body, "cliente") || !tieneTexto(body, "descripcion")) {
            return respuestaJson(400, false, "Cliente y descripcion son obligatorios.");
        }
        int id = sistema.obtenerSolicitudes().contarSolicitudes() + 1;
        while (sistema.obtenerSolicitudes().buscarSolicitud(id) != nullptr) ++id;
        sistema.obtenerSolicitudes().insertarSolicitud(new Solicitud(id, body["cliente"].s(), body["descripcion"].s()));
        return respuestaJson(201, true, "Solicitud enviada correctamente.");
    });

    CROW_ROUTE(app, "/api/promociones")
    ([&sistema] {
        crow::json::wvalue body;
        body["ok"] = true;
        crow::json::wvalue::list items;
        ListaCircularPromociones& lista = sistema.obtenerPromociones();
        if (lista.cabeza != nullptr) {
            NodoPromocion* actual = lista.cabeza;
            do {
                crow::json::wvalue item({{"id", actual->promocion->idPromocion},
                    {"nombre", actual->promocion->nombre},
                    {"descripcion", actual->promocion->descripcion}});
                crow::json::wvalue::list beneficios;
                for (NodoBeneficio* beneficio = actual->listaBeneficios->cabeza;
                     beneficio != nullptr; beneficio = beneficio->siguiente) {
                    beneficios.emplace_back(crow::json::wvalue({
                        {"id", beneficio->beneficio->idBeneficio},
                        {"descripcion", beneficio->beneficio->descripcion},
                        {"descuento", beneficio->beneficio->porcentajeDescuento}}));
                }
                item["beneficios"] = std::move(beneficios);
                items.emplace_back(std::move(item));
                actual = actual->siguiente;
            } while (actual != lista.cabeza);
        }
        body["message"] = "Promociones obtenidas";
        body["data"] = std::move(items);
        return respuestaJson(body);
    });

    CROW_ROUTE(app, "/api/solicitudes/<int>")
    .methods(crow::HTTPMethod::Put)
    ([&sistema](const crow::request& req, int idSolicitud) {
        auto body = crow::json::load(req.body);
        if (!body || !tieneTexto(body, "estado")) return respuestaJson(400, false, "El estado es obligatorio.");
        bool actualizado = sistema.obtenerSolicitudes().cambiarEstadoSolicitud(idSolicitud, body["estado"].s());
        return respuestaJson(actualizado ? 200 : 404, actualizado,
                             actualizado ? "Solicitud actualizada." : "Solicitud no encontrada.");
    });

    CROW_ROUTE(app, "/api/sedes")
    ([&sistema] {
        crow::json::wvalue body; body["ok"]=true; body["message"]="Sedes obtenidas";
        crow::json::wvalue::list items;
        for (const auto& par : sistema.obtenerGrafoSedes().obtenerSedes()) {
            items.emplace_back(crow::json::wvalue({{"codigo",par.second.codigo},{"nombre",par.second.nombre},{"direccion",par.second.direccion},{"cantidad_salas",par.second.cantidadSalas},{"telefono",par.second.telefono}}));
        }
        body["data"]=std::move(items); return respuestaJson(body);
    });

    CROW_ROUTE(app, "/api/sedes")
    .methods(crow::HTTPMethod::Post)
    ([&sistema](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !tieneTexto(body, "codigo") || !tieneTexto(body, "nombre") ||
            !tieneTexto(body, "direccion") || !tieneTexto(body, "telefono") || !body["cantidad_salas"] ||
            body["cantidad_salas"].t() != crow::json::type::Number || body["cantidad_salas"].i() <= 0 ||
            body["cantidad_salas"].i() > std::numeric_limits<int>::max())
            return respuestaJson(400, false, "Codigo, nombre, direccion, telefono y una cantidad_salas positiva son obligatorios.");
        std::string mensaje;
        bool creado = sistema.crearSede({body["codigo"].s(), body["nombre"].s(), body["direccion"].s(), static_cast<int>(body["cantidad_salas"].i()), body["telefono"].s()}, mensaje);
        return respuestaJson(creado ? 201 : 400, creado, mensaje);
    });

    CROW_ROUTE(app, "/api/sedes/<string>/peliculas")
    ([&sistema](const std::string& codigo) {
        auto& grafo = sistema.obtenerGrafoSedes();
        if (!grafo.existeSede(codigo)) return respuestaJson(404, false, "Sede no encontrada.");
        crow::json::wvalue::list peliculas;
        std::set<std::string> agregadas;
        const auto& todasLasFunciones = sistema.obtenerFunciones();
        for (const auto& funcion : todasLasFunciones) {
            if (funcion.sede != codigo || !funcion.pelicula || !agregadas.insert(funcion.pelicula->getCodigo()).second) continue;
            crow::json::wvalue item = peliculaJson(*funcion.pelicula);
            crow::json::wvalue::list funciones;
            for (const auto& funcionSede : todasLasFunciones) if (funcionSede.sede == codigo && funcionSede.pelicula && funcionSede.pelicula->getCodigo() == funcion.pelicula->getCodigo())
                funciones.emplace_back(crow::json::wvalue({{"horario",funcionSede.horario},{"sala",funcionSede.sala},{"funcion",funcionSede.codigoFuncion}}));
            item["funciones"] = std::move(funciones);
            peliculas.emplace_back(std::move(item));
        }
        crow::json::wvalue salida; salida["ok"] = true; salida["message"] = "Peliculas de la sede obtenidas"; salida["data"] = std::move(peliculas); return respuestaJson(salida);
    });

    CROW_ROUTE(app, "/api/sedes/camino")
    ([&sistema](const crow::request& req) {
        const char* origen = req.url_params.get("origen"); const char* destino = req.url_params.get("destino");
        if (!origen || !destino) return respuestaJson(400, false, "Indique origen y destino.");
        auto& grafo = sistema.obtenerGrafoSedes();
        if (!grafo.existeSede(origen) || !grafo.existeSede(destino)) return respuestaJson(404, false, "Una de las sedes no existe.");
        auto camino = grafo.caminoBFS(origen, destino);
        crow::json::wvalue salida; salida["ok"] = true; salida["conectadas"] = !camino.empty();
        salida["message"] = camino.empty() ? "No existe una ruta entre estas sedes." : "Ruta con menor cantidad de conexiones encontrada por BFS.";
        crow::json::wvalue::list codigos; for (const auto& c : camino) codigos.emplace_back(c); salida["data"] = std::move(codigos);
        salida["intermedias"] = camino.size() < 3 ? 0 : static_cast<int>(camino.size()) - 2; return respuestaJson(salida);
    });

    CROW_ROUTE(app, "/api/sedes/ranking")
    ([&sistema] {
        crow::json::wvalue salida; salida["ok"] = true; salida["message"] = "Relaciones ordenadas por peliculas compartidas";
        crow::json::wvalue::list items; auto& grafo = sistema.obtenerGrafoSedes();
        for (const auto& relacion : grafo.obtenerRelacionesOrdenadas()) {
            const auto& sedes = grafo.obtenerSedes();
            items.emplace_back(crow::json::wvalue({{"origen",relacion.origen},{"nombre_origen",sedes.at(relacion.origen).nombre},{"destino",relacion.destino},{"nombre_destino",sedes.at(relacion.destino).nombre},{"peso",relacion.peso}}));
        }
        salida["data"] = std::move(items); return respuestaJson(salida);
    });

    CROW_ROUTE(app, "/api/carga/peliculas-funciones")
    .methods(crow::HTTPMethod::Post)
    ([&sistema](const crow::request& req) {
        auto body=crow::json::load(req.body);
        if(!body || !tieneTexto(body,"ruta")) return respuestaJson(400,false,"Indique la ruta del archivo JSON.");
        int total=cargarPeliculasYFuncionesJSON(body["ruta"].s(),sistema.obtenerCartelera(),sistema.obtenerFunciones(),sistema.obtenerArbolFunciones(),&sistema.obtenerGrafoSedes());
        sistema.actualizarGrafoSedes();
        crow::json::wvalue out; out["ok"]=total>0; out["message"]=obtenerUltimoMensajeCargaFase2(); out["total"]=total; return respuestaJson(total>0?200:400,out);
    });
    CROW_ROUTE(app, "/api/peliculas")
    ([&sistema](const crow::request& req) {
        crow::json::wvalue body;
        body["ok"] = true;
        body["message"] = "Cartelera obtenida";

        std::string recorrido = "inorden";
        if (req.url_params.get("recorrido") != nullptr) {
            recorrido = req.url_params.get("recorrido");
        }
        if (recorrido != "inorden" && recorrido != "preorden" && recorrido != "postorden") {
            return respuestaJson(400, false, "El recorrido debe ser inorden, preorden o postorden.");
        }

        crow::json::wvalue::list peliculas;
        for (Pelicula* pelicula : sistema.obtenerCartelera().obtenerPeliculasRecorrido(recorrido)) {
            peliculas.emplace_back(peliculaJson(*pelicula));
        }
        body["data"] = std::move(peliculas);
        return respuestaJson(body);
    });

    CROW_ROUTE(app, "/api/peliculas/<string>")
    ([&sistema](const std::string& codigo) {
        Pelicula* pelicula = sistema.obtenerCartelera().buscarPorCodigo(codigo);
        if (pelicula == nullptr) {
            return respuestaJson(404, false, "Pelicula no encontrada.");
        }

        crow::json::wvalue body;
        body["ok"] = true;
        body["message"] = "Pelicula encontrada";
        body["data"] = peliculaJson(*pelicula);
        return respuestaJson(body);
    });

    CROW_ROUTE(app, "/api/peliculas")
    .methods(crow::HTTPMethod::Post)
    ([&sistema](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !body["id"] || !body["duracion"] ||
            !tieneTexto(body, "titulo") || !tieneTexto(body, "genero") ||
            !tieneTexto(body, "clasificacion") || !tieneTexto(body, "idioma") ||
            !tieneTexto(body, "fecha_estreno") || !tieneTexto(body, "fecha_fin") ||
            !tieneTexto(body, "codigo")) {
            return respuestaJson(400, false, "Faltan campos obligatorios de la pelicula.");
        }

        std::string mensaje;
        bool creada = sistema.crearPelicula(
            body["id"].i(),
            body["titulo"].s(),
            body["genero"].s(),
            body["duracion"].i(),
            body["clasificacion"].s(),
            body["fecha_fin"].s(),
            body["codigo"].s(),
            body["idioma"].s(),
            body["fecha_estreno"].s(),
            mensaje
        );
        return respuestaJson(creada ? 201 : 400, creada, mensaje);
    });

    CROW_ROUTE(app, "/api/funciones")
    .methods(crow::HTTPMethod::Get, crow::HTTPMethod::Post)
    ([&sistema](const crow::request& req) {
        if (req.method == crow::HTTPMethod::Post) {
            auto body = crow::json::load(req.body);
            if (!body || !tieneTexto(body, "pelicula") || !body["filas"] ||
                !body["columnas"] || !tieneTexto(body, "horario") ||
                !tieneTexto(body, "sala") || !tieneTexto(body, "sede")) {
                return respuestaJson(400, false, "Faltan pelicula, sede, horario, sala, filas o columnas.");
            }

            Pelicula* pelicula = sistema.obtenerCartelera().buscarPorCodigo(body["pelicula"].s());
            std::string codigoCreado;
            std::string mensaje;
            bool creada = sistema.crearFuncion(pelicula, body["horario"].s(), body["sala"].s(),
                                               body["sede"].s(), body["filas"].i(), body["columnas"].i(),
                                               codigoCreado, mensaje);
            crow::json::wvalue respuesta;
            respuesta["ok"] = creada;
            respuesta["message"] = mensaje;
            if (creada) respuesta["codigo_funcion"] = codigoCreado;
            return respuestaJson(creada ? 201 : 400, respuesta);
        }

        crow::json::wvalue body;
        body["ok"] = true;
        body["message"] = "Funciones obtenidas";

        std::string recorrido = "inorden";
        if (req.url_params.get("recorrido") != nullptr) {
            recorrido = req.url_params.get("recorrido");
        }
        if (recorrido != "inorden" && recorrido != "preorden" && recorrido != "postorden") {
            return respuestaJson(400, false, "El recorrido debe ser inorden, preorden o postorden.");
        }

        crow::json::wvalue::list funciones;
        for (const FuncionCine& funcion : sistema.obtenerArbolFunciones().obtenerFunciones(recorrido)) {
            funciones.emplace_back(funcionJson(funcion));
        }
        body["data"] = std::move(funciones);
        return respuestaJson(body);
    });

    CROW_ROUTE(app, "/api/funciones/<string>")
    ([&sistema](const std::string& codigoFuncion) {
        FuncionCine* funcion = sistema.buscarFuncion(codigoFuncion);
        if (funcion == nullptr) {
            return respuestaJson(404, false, "Funcion no encontrada.");
        }

        crow::json::wvalue body;
        body["ok"] = true;
        body["message"] = "Funcion encontrada";
        body["data"] = funcionJson(*funcion);
        return respuestaJson(body);
    });

    CROW_ROUTE(app, "/api/funciones/<string>")
    .methods(crow::HTTPMethod::Put)
    ([&sistema](const crow::request& req, const std::string& codigoFuncion) {
        auto body = crow::json::load(req.body);
        if (!body) {
            return respuestaJson(400, false, "El cuerpo debe ser JSON valido.");
        }

        std::string horario;
        std::string sala;
        int filas = 0;
        int columnas = 0;
        if (!leerCamposFuncion(body, horario, sala, filas, columnas)) {
            return respuestaJson(400, false, "Faltan horario, sala, filas o columnas.");
        }

        std::string mensaje;
        bool editada = sistema.editarFuncion(codigoFuncion, horario, sala, filas, columnas, mensaje);
        return respuestaJson(editada ? 200 : 400, editada, mensaje);
    });

    CROW_ROUTE(app, "/api/funciones/<string>/asientos")
    ([&sistema](const std::string& codigoFuncion) {
        FuncionCine* funcion = sistema.buscarFuncion(codigoFuncion);
        if (funcion == nullptr) {
            return respuestaJson(404, false, "Funcion no encontrada.");
        }

        cargarAsientosFuncion(*funcion);

        crow::json::wvalue body;
        body["ok"] = true;
        body["message"] = "Mapa de asientos obtenido";
        body["codigo_funcion"] = codigoFuncion;
        body["filas"] = funcion->asientos.obtenerFilas();
        body["columnas"] = funcion->asientos.obtenerColumnas();

        crow::json::wvalue::list asientos;
        for (int fila = 1; fila <= funcion->asientos.obtenerFilas(); ++fila) {
            for (int columna = 1; columna <= funcion->asientos.obtenerColumnas(); ++columna) {
                NodoMatriz* asiento = funcion->asientos.obtenerAsiento(fila, columna);
                crow::json::wvalue item;
                item["fila"] = fila;
                item["columna"] = columna;
                item["ocupado"] = asiento != nullptr;
                item["reserva"] = asiento != nullptr ? asiento->titular : "";
                asientos.emplace_back(std::move(item));
            }
        }
        body["data"] = std::move(asientos);
        return respuestaJson(body);
    });

    CROW_ROUTE(app, "/api/carga/clientes-reservas")
    .methods(crow::HTTPMethod::Post)
    ([&sistema](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !tieneTexto(body, "ruta")) {
            return respuestaJson(400, false, "Indique la ruta del archivo JSON de clientes y reservas.");
        }

        int total = cargarClientesYReservasJSON(
            body["ruta"].s(),
            sistema.obtenerClientes(),
            sistema.obtenerReservas(),
            sistema.obtenerFunciones()
        );

        crow::json::wvalue salida;
        salida["ok"] = total > 0;
        salida["message"] = obtenerUltimoMensajeCargaClientesReservas();
        salida["total"] = total;
        return respuestaJson(total > 0 ? 200 : 400, salida);
    });
    CROW_ROUTE(app, "/api/reservas")
    .methods(crow::HTTPMethod::Post)
    ([&sistema](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !tieneTexto(body, "cliente") || !tieneTexto(body, "funcion") ||
            !body["fila"] || !body["columna"]) {
            return respuestaJson(400, false, "Faltan cliente, funcion, fila o columna.");
        }

        std::string codigoReserva;
        std::string mensaje;
        bool creada = sistema.reservarAsiento(body["cliente"].s(), body["funcion"].s(),
                                              body["fila"].i(), body["columna"].i(),
                                              codigoReserva, mensaje);
        crow::json::wvalue respuesta;
        respuesta["ok"] = creada;
        respuesta["message"] = mensaje;
        if (creada) respuesta["codigo_reserva"] = codigoReserva;
        return respuestaJson(creada ? 201 : 400, respuesta);
    });

    CROW_ROUTE(app, "/api/clientes/<string>/reservas/<string>")
    .methods(crow::HTTPMethod::Delete)
    ([&sistema](const std::string& idCliente, const std::string& codigoReserva) {
        std::string mensaje;
        bool cancelada = sistema.cancelarReserva(idCliente, codigoReserva, mensaje);
        return respuestaJson(cancelada ? 200 : 400, cancelada, mensaje);
    });

    CROW_ROUTE(app, "/api/login")
    .methods(crow::HTTPMethod::Post)
    ([&sistema](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !tieneTexto(body, "correo") || !tieneTexto(body, "password")) {
            return respuestaJson(400, false, "Correo y password son obligatorios.");
        }

        if (body["correo"].s() == "AdminCine@gmail.com" &&
            body["password"].s() == "admin123Pass") {
            crow::json::wvalue respuesta;
            respuesta["ok"] = true;
            respuesta["message"] = "Inicio de sesion correcto";
            respuesta["data"] = crow::json::wvalue({
                {"id", "ADMIN"},
                {"nombre", "Administrador"},
                {"correo", "AdminCine@gmail.com"},
                {"tipo", "admin"}
            });
            return respuestaJson(respuesta);
        }

        Cliente* cliente = nullptr;
        if (!sistema.obtenerClientes().autenticar(body["correo"].s(), body["password"].s(), cliente)) {
            return respuestaJson(401, false, "Correo o password incorrectos.");
        }

        crow::json::wvalue respuesta;
        respuesta["ok"] = true;
        respuesta["message"] = "Inicio de sesion correcto";
        respuesta["data"] = crow::json::wvalue({
            {"id", cliente->id},
            {"nombre", cliente->nombre},
            {"correo", cliente->correo},
            {"tipo", cliente->tipo}
        });
        return respuestaJson(respuesta);
    });
}
