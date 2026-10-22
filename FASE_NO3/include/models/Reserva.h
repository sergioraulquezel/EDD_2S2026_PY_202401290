#pragma once

#include <string>

struct Reserva {
    std::string codigoReserva;
    std::string idCliente;
    std::string codigoFuncion;
    int fila;
    int columna;
    std::string fechaReserva;
    std::string estado;

    Reserva();
    Reserva(const std::string& codigo, const std::string& cliente, const std::string& funcion,
            int filaReserva, int columnaReserva, const std::string& fecha);
};
