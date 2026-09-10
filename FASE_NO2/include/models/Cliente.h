#pragma once

#include <string>
#include <vector>

struct Cliente {
    std::string id;
    std::string nombre;
    std::string correo;
    std::string telefono;
    std::string password;
    std::string tipo;
    std::vector<std::string> codigosReserva;

    Cliente();
    Cliente(const std::string& idCliente, const std::string& nombreCliente, const std::string& correoCliente,
            const std::string& telefonoCliente, const std::string& passwordCliente,
            const std::string& tipoCliente = "cliente");

    void agregarReserva(const std::string& codigoReserva);
    bool eliminarReserva(const std::string& codigoReserva);
    bool tieneReserva(const std::string& codigoReserva) const;
};
