#pragma once
#include "models/FuncionCine.h"
#include "models/Sede.h"
#include <map>
#include <string>
#include <vector>
struct AristaSede { std::string destino; int peso=0; };
struct RelacionSedes { std::string origen; std::string destino; int peso=0; };
class GrafoSedes {
 std::map<std::string,Sede> sedes; std::map<std::string,std::map<std::string,int>> adyacencia;
public:
 bool agregarSede(const Sede&); bool existeSede(const std::string&) const;
 const std::map<std::string,Sede>& obtenerSedes() const { return sedes; }
 void reconstruir(const std::vector<FuncionCine>&);
 int peso(const std::string&,const std::string&) const;
 std::vector<AristaSede> vecinos(const std::string&) const;
 std::vector<std::string> caminoBFS(const std::string&, const std::string&) const;
 std::vector<RelacionSedes> obtenerRelacionesOrdenadas() const;
 void generarReporteGraphviz(const std::string& dot="reporte_grafo_sedes.dot", const std::string& png="reporte_grafo_sedes.png") const;
};
