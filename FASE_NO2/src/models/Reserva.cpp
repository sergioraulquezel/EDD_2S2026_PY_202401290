#include "models/Reserva.h"

Reserva::Reserva()
    : fila(0), columna(0), estado("VALIDA")
{
}

Reserva::Reserva(const std::string& codigo, const std::string& cliente, const std::string& funcion,
                 int filaReserva, int columnaReserva, const std::string& fecha)
    : codigoReserva(codigo), idCliente(cliente), codigoFuncion(funcion),
      fila(filaReserva), columna(columnaReserva), fechaReserva(fecha), estado("VALIDA")
{
}
