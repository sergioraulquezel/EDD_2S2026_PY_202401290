#include "models/Pelicula.h"

Pelicula::Pelicula(int _id, std::string _titulo, std::string _genero, int _duracion, std::string _clasificacion) {
    this->id = _id;
    this->titulo = _titulo;
    this->genero = _genero;
    this->duracion = _duracion;
    this->clasificacion = _clasificacion;
}