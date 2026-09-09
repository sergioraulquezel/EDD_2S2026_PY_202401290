#pragma once
#include <string>

class Promocion {
public:
    int idPromocion;
    std::string nombre;
    std::string descripcion;
    // La lista doblemente enlazada de Beneficios irá en la estructura, no aquí.

    Promocion(int id, std::string nom, std::string desc);
};