#pragma once

#include "structures/ArbolAVLFunciones.h"
#include "structures/ArbolBinario.h"
#include "structures/GrafoSedes.h"
#include <string>
#include <vector>

int cargarPeliculasYFuncionesJSON(const std::string& rutaArchivo,
                                  ArbolBinario& cartelera,
                                  std::vector<FuncionCine>& funciones,
                                  ArbolAVLFunciones& arbolFunciones, GrafoSedes* grafoSedes = nullptr);
std::string obtenerUltimoMensajeCargaFase2();
