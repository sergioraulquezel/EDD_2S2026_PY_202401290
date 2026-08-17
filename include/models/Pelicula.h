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
    std::string codigo;
    std::string idioma;
    std::string fechaEstreno;
    std::string fechaFin;

public:
    // Solo la declaración (sin llaves ni código aquí)
    Pelicula(int _id, std::string _titulo, std::string _genero, int _duracion, std::string _clasificacion, std::string _fechaFin = "", std::string _codigo = "", std::string _idioma = "", std::string _fechaEstreno = "");

    // Getters
    int getId() { return id; }
    std::string getTitulo() { return titulo; }
    std::string getGenero() { return genero; }
    int getDuracion() { return duracion; }
    std::string getClasificacion() { return clasificacion; }
    std::string getCodigo() { return codigo.empty() ? "CO-" + std::to_string(id) : codigo; }
    std::string getIdioma() { return idioma; }
    std::string getFechaEstreno() { return fechaEstreno; }
    std::string getFechaFin() { return fechaFin; }
   
};

#endif
