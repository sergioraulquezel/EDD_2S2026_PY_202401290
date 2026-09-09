#ifndef FUNCIONCINE_H
#define FUNCIONCINE_H

#include "models/Pelicula.h"
#include "structures/MatrizDispersa.h"
#include <string>

struct FuncionCine {
    int id;
    std::string codigoFuncion;
    Pelicula* pelicula;
    std::string horario;
    std::string sala;
    std::string archivoAsientos;
    MatrizDispersa asientos;

    FuncionCine(int _id, Pelicula* _pelicula, const std::string& _horario, const std::string& _sala, int filas, int columnas)
        : id(_id), codigoFuncion("F" + std::to_string(_id)), pelicula(_pelicula), horario(_horario), sala(_sala),
          archivoAsientos(codigoFuncion + "_funcion.json") {
        asientos.configurarSala(filas, columnas);
    }

    FuncionCine(int _id, const std::string& _codigoFuncion, Pelicula* _pelicula, const std::string& _horario,
                const std::string& _sala, int filas, int columnas, const std::string& _archivoAsientos = "")
        : id(_id), codigoFuncion(_codigoFuncion), pelicula(_pelicula), horario(_horario), sala(_sala),
          archivoAsientos(_archivoAsientos.empty() ? _codigoFuncion + "_funcion.json" : _archivoAsientos) {
        asientos.configurarSala(filas, columnas);
    }

    std::string obtenerEtiqueta() const {
        std::string titulo = pelicula != nullptr ? pelicula->getTitulo() : "Sin pelicula";
        return codigoFuncion + " | " + titulo + " | " + horario + " | " + sala;
    }
};

#endif
