#pragma once
#include <string>

class Beneficio {
public:
    int idBeneficio;
    std::string descripcion;
    int porcentajeDescuento;

    Beneficio(int id, std::string desc, int descuento);
};