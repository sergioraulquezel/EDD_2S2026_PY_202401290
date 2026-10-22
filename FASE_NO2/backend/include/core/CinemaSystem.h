#pragma once

#include "models/FuncionCine.h"
#include "structures/ArbolAVLFunciones.h"
#include "structures/ArbolBClientes.h"
#include "structures/ArbolBinario.h"
#include "structures/ListaSolicitudes.h"
#include "structures/ListasPromociones.h"
#include "structures/TablaHashReservas.h"
#include <string>
#include <vector>

// Estado y estructuras principales del sistema. Esta clase no conoce ImGui,
// por lo que luego puede reutilizarse desde una API web.
class CinemaSystem {
private:
    ArbolBinario cartelera;
    std::vector<FuncionCine> funciones;
    ArbolAVLFunciones arbolFunciones;
    ArbolBClientes clientes;
    TablaHashReservas reservas;
    ListaCircularPromociones promociones;
    ListaCircularDoble solicitudes;

    void cargarDatosIniciales();
    FuncionCine* buscarFuncionEnVector(const std::string& codigoFuncion);

public:
    CinemaSystem();

    ArbolBinario& obtenerCartelera();
    std::vector<FuncionCine>& obtenerFunciones();
    ArbolAVLFunciones& obtenerArbolFunciones();
    FuncionCine* buscarFuncion(const std::string& codigoFuncion);
    ArbolBClientes& obtenerClientes();
    TablaHashReservas& obtenerReservas();
    ListaCircularPromociones& obtenerPromociones();
    ListaCircularDoble& obtenerSolicitudes();

    bool crearPelicula(int id, const std::string& titulo, const std::string& genero, int duracion,
                       const std::string& clasificacion, const std::string& fechaFin,
                       const std::string& codigo, const std::string& idioma,
                       const std::string& fechaEstreno, std::string& mensaje);

    bool crearFuncion(Pelicula* pelicula, const std::string& horario, const std::string& sala,
                      int filas, int columnas, std::string& codigoCreado,
                      std::string& mensaje);

    bool editarFuncion(const std::string& codigoFuncion, const std::string& horario,
                       const std::string& sala, int filas, int columnas,
                       std::string& mensaje);

    bool reservarAsiento(const std::string& idCliente, const std::string& codigoFuncion,
                         int fila, int columna, std::string& codigoReserva,
                         std::string& mensaje);

    bool cancelarReserva(const std::string& idCliente, const std::string& codigoReserva,
                         std::string& mensaje);

    bool eliminarCliente(const std::string& idCliente, std::string& mensaje);
};
