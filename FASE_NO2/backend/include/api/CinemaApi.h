#pragma once

#include "core/CinemaSystem.h"
#include "crow.h"
#include "crow/middlewares/cors.h"

// Registra las rutas HTTP sobre el estado central del cine.
void configurarRutasCinema(crow::App<crow::CORSHandler>& app, CinemaSystem& sistema);
