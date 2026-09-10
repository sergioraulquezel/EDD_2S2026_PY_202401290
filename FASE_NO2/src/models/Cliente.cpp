#include "models/Cliente.h"

Cliente::Cliente() : tipo("cliente")
{
}

Cliente::Cliente(const std::string& idCliente, const std::string& nombreCliente, const std::string& correoCliente,
                 const std::string& telefonoCliente, const std::string& passwordCliente,
                 const std::string& tipoCliente)
    : id(idCliente), nombre(nombreCliente), correo(correoCliente), telefono(telefonoCliente),
      password(passwordCliente), tipo(tipoCliente)
{
}

void Cliente::agregarReserva(const std::string& codigoReserva)
{
    if (!tieneReserva(codigoReserva)) {
        codigosReserva.push_back(codigoReserva);
    }
}

bool Cliente::eliminarReserva(const std::string& codigoReserva)
{
    for (auto it = codigosReserva.begin(); it != codigosReserva.end(); ++it) {
        if (*it == codigoReserva) {
            codigosReserva.erase(it);
            return true;
        }
    }
    return false;
}

bool Cliente::tieneReserva(const std::string& codigoReserva) const
{
    for (const std::string& codigo : codigosReserva) {
        if (codigo == codigoReserva) return true;
    }
    return false;
}
