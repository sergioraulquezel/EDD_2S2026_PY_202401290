#pragma once
#include <string>

class Pelicula {
public:
    int idPelicula; // Clave para ordenar el Árbol Binario 
    std::string titulo;
    std::string director;
    int duracionMinutos;
    std::string genero;

    Pelicula(int id, std::string t, std::string d, int dur, std::string g);
};