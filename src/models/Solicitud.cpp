#include "models/Solicitud.h"

Solicitud::Solicitud(int id, std::string cliente, std::string desc) {
    this->idSolicitud = id;
    this->nombreCliente = cliente;
    this->descripcionEvento = desc;
    this->estado = "Pendiente"; // Por defecto, toda solicitud nueva está pendiente
}