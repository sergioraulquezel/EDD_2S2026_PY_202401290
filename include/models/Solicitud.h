#pragma once
#include <string>

class Solicitud {
public:
    int idSolicitud;
    std::string nombreCliente;
    std::string descripcionEvento; // Ej: Cumpleaños, Evento corporativo
    std::string estado; // "Pendiente", "Aprobada", "Rechazada"

    Solicitud(int id, std::string cliente, std::string desc);
};