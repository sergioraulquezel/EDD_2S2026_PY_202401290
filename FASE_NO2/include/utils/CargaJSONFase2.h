#pragma once

#include "structures/ArbolAVLFunciones.h"
#include "structures/ArbolBinario.h"
#include <string>
#include <vector>

int cargarPeliculasYFuncionesJSON(const std::string& rutaArchivo,
                                  ArbolBinario& cartelera,
                                  std::vector<FuncionCine>& funciones,
                                  ArbolAVLFunciones& arbolFunciones);
