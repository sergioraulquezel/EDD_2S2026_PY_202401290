#ifndef PELICULA_H
#define PELICULA_H

#include <string>

class Pelicula {
private:
    int id;
    std::string titulo;
    std::string genero;
    int duracion;
    std::string clasificacion;
    std::string fechaFin;

public:
    // Solo la declaración (sin llaves ni código aquí)
    Pelicula(int _id, std::string _titulo, std::string _genero, int _duracion, std::string _clasificacion, std::string _fechaFin = "");

    // Getters
    int getId() { return id; }
    std::string getTitulo() { return titulo; }
    std::string getGenero() { return genero; }
    int getDuracion() { return duracion; }
    std::string getClasificacion() { return clasificacion; }
    std::string getFechaFin() { return fechaFin; }
   
};

#endif
