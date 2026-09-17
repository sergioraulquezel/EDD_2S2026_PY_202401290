#pragma once

#include "models/FuncionCine.h"
#include "structures/ArbolBClientes.h"
#include "structures/TablaHashReservas.h"
#include <string>
#include <vector>

int cargarClientesYReservasJSON(const std::string& rutaArchivo,
                                ArbolBClientes& clientes,
                                TablaHashReservas& reservas,
                                std::vector<FuncionCine>& funciones);
