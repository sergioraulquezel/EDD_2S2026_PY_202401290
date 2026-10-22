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
        .headers("Content-Type", "Accept")
        .methods(crow::HTTPMethod::Get, crow::HTTPMethod::Post,
                 crow::HTTPMethod::Put, crow::HTTPMethod::Delete,
                 crow::HTTPMethod::Options);

    configurarRutasCinema(app, sistema);

    const char* puertoTexto = std::getenv("CINEMA_PORT");
    int puerto = puertoTexto != nullptr ? std::atoi(puertoTexto) : 18080;
    if (puerto <= 0 || puerto > 65535) puerto = 18080;

    std::cout << "Cinema API escuchando en http://localhost:" << puerto << "\n";
    std::cout << "Prueba de salud: http://localhost:" << puerto << "/api/health\n";
    app.port(puerto).multithreaded().run();
    return 0;
}
