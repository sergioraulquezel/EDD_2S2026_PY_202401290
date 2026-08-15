#include "models/Pelicula.h"

// Implementación del constructor
Pelicula::Pelicula(int id, std::string t, std::string d, int dur, std::string g) {
    this->idPelicula = id;
    this->titulo = t;
    this->director = d;
    this->duracionMinutos = dur;
    this->genero = g;
}