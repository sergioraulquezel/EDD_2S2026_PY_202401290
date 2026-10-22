#include "models/Beneficio.h"

Beneficio::Beneficio(int id, std::string desc, int descuento) {
    this->idBeneficio = id;
    this->descripcion = desc;
    this->porcentajeDescuento = descuento;
}