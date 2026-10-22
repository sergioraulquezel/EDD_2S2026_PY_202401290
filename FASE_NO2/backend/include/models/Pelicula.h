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
    void actualizar(std::string _titulo, std::string _genero, int _duracion, std::string _clasificacion, std::string _fechaFin = "", std::string _codigo = "", std::string _idioma = "", std::string _fechaEstreno = "");

    // Getters
    int getId() const { return id; }
    std::string getTitulo() const { return titulo; }
    std::string getGenero() const { return genero; }
    int getDuracion() const { return duracion; }
    std::string getClasificacion() const { return clasificacion; }
    std::string getCodigo() const { return codigo.empty() ? "CO-" + std::to_string(id) : codigo; }
    std::string getIdioma() const { return idioma; }
    std::string getFechaEstreno() const { return fechaEstreno; }
    std::string getFechaFin() const { return fechaFin; }
   
};

#endif
