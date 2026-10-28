#include "api/CinemaApi.h"
#include "crow/middlewares/cors.h"
#include <cstdlib>
#include <iostream>

int main()
{
    CinemaSystem sistema;
    crow::App<crow::CORSHandler> app;

    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors.global()
        .origin("*")
        .headers("Content-Type", "Accept", "Origin")
        .methods(crow::HTTPMethod::Get, crow::HTTPMethod::Post,
                 crow::HTTPMethod::Put, crow::HTTPMethod::Delete,
                 crow::HTTPMethod::Options);

    // Registrar primero las rutas concretas para que tengan prioridad sobre el comodin.
    configurarRutasCinema(app, sistema);

    // Responder al preflight del navegador y devolver JSON para rutas API inexistentes.
    CROW_ROUTE(app, "/api/<path>")
        .methods(crow::HTTPMethod::Options)
        ([](const crow::request&, crow::response& respuesta, const std::string&) {
            respuesta.code = 204;
        });

    CROW_ROUTE(app, "/api/<path>")
        .methods(crow::HTTPMethod::Get, crow::HTTPMethod::Post,
                 crow::HTTPMethod::Put, crow::HTTPMethod::Delete)
        ([](const crow::request&, const std::string& ruta) {
            crow::json::wvalue body;
            body["ok"] = false;
            body["message"] = "Ruta API no encontrada: /api/" + ruta;
            crow::response respuesta(404);
            respuesta.set_header("Content-Type", "application/json");
            respuesta.write(body.dump());
            return respuesta;
        });

    const char* puertoTexto = std::getenv("CINEMA_PORT");
    int puerto = puertoTexto != nullptr ? std::atoi(puertoTexto) : 18080;
    if (puerto <= 0 || puerto > 65535) puerto = 18080;

    std::cout << "Cinema API escuchando en http://localhost:" << puerto << "\n";
    std::cout << "Prueba de salud: http://localhost:" << puerto << "/api/health\n";
    app.port(puerto).multithreaded().run();
    return 0;
}
