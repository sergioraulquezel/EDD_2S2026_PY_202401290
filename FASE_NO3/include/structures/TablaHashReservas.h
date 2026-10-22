#pragma once

#include "models/Reserva.h"
#include <string>
#include <vector>

class NodoHashReserva {
public:
    Reserva reserva;
    NodoHashReserva* siguiente;

    explicit NodoHashReserva(const Reserva& reservaNueva);
};

class TablaHashReservas {
private:
    std::vector<NodoHashReserva*> buckets;
    int totalReservas;

    int hash(const std::string& codigoReserva) const;
    void liberar();

public:
    explicit TablaHashReservas(int tamano = 23);
    ~TablaHashReservas();

    bool insertar(const Reserva& reserva);
    Reserva* buscar(const std::string& codigoReserva);
    bool eliminar(const std::string& codigoReserva);
    std::vector<Reserva> listarTodas() const;
    std::vector<Reserva> listarPorCliente(const std::string& idCliente) const;
    int contar() const;
    int tamano() const;
    int bucketsOcupados() const;
    int contarColisiones() const;
    void generarReporteGraphviz();
};
