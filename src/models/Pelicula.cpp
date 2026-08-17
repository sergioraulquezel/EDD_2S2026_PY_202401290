#include "models/Pelicula.h"

Pelicula::Pelicula(int _id, std::string _titulo, std::string _genero, int _duracion, std::string _clasificacion, std::string _fechaFin, std::string _codigo, std::string _idioma, std::string _fechaEstreno) {
    this->id = _id;
    this->titulo = _titulo;
    this->genero = _genero;
    this->duracion = _duracion;
    this->clasificacion = _clasificacion;
    this->codigo = _codigo;
    this->idioma = _idioma;
    this->fechaEstreno = _fechaEstreno;
    this->fechaFin = _fechaFin;
}
