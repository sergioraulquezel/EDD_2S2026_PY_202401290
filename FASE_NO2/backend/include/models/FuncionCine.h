#ifndef FUNCIONCINE_H
#define FUNCIONCINE_H
#include "models/Pelicula.h"
#include "structures/MatrizDispersa.h"
#include <string>
struct FuncionCine {
 int id; std::string codigoFuncion; Pelicula* pelicula; std::string horario; std::string sala; std::string sede; std::string archivoAsientos; MatrizDispersa asientos;
 FuncionCine(int i,Pelicula* p,const std::string& h,const std::string& s,int f,int c,const std::string& se=""):id(i),codigoFuncion("F"+std::to_string(i)),pelicula(p),horario(h),sala(s),sede(se),archivoAsientos(codigoFuncion+"_funcion.json"){asientos.configurarSala(f,c);}
 FuncionCine(int i,const std::string& cf,Pelicula* p,const std::string& h,const std::string& s,int f,int c,const std::string& a="",const std::string& se=""):id(i),codigoFuncion(cf),pelicula(p),horario(h),sala(s),sede(se),archivoAsientos(a.empty()?cf+"_funcion.json":a){asientos.configurarSala(f,c);}
 std::string obtenerEtiqueta()const{std::string t=pelicula?pelicula->getTitulo():"Sin pelicula";return codigoFuncion+" | "+t+" | "+horario+" | "+sala;}
};
#endif