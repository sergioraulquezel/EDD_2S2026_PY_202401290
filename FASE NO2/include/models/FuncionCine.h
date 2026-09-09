#ifndef FUNCIONCINE_H
#define FUNCIONCINE_H

#include "models/Pelicula.h"
#include "structures/MatrizDispersa.h"
#include <string>

struct FuncionCine {
    int id;
    Pelicula* pelicula;
    std::string horario;
    std::string sala;
    MatrizDispersa asientos;

    FuncionCine(int _id, Pelicula* _pelicula, const std::string& _horario, const std::string& _sala, int filas, int columnas)
        : id(_id), pelicula(_pelicula), horario(_horario), sala(_sala) {
        asientos.configurarSala(filas, columnas);
    }

    std::string obtenerEtiqueta() const {
        std::string titulo = pelicula != nullptr ? pelicula->getTitulo() : "Sin pelicula";
        return titulo + " | " + horario + " | " + sala;
    }
};

#endif
