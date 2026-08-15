// include/structures/ListaSolicitudes.hpp
#pragma once
#include "../models/Solicitud.h"

class NodoSolicitud {
public:
    Solicitud* solicitud;
    NodoSolicitud* siguiente;
    NodoSolicitud* anterior;

    NodoSolicitud(Solicitud* s) : solicitud(s), siguiente(nullptr), anterior(nullptr) {}
};

class ListaCircularDoble {
public:
    NodoSolicitud* cabeza;

    ListaCircularDoble();
    void insertarSolicitud(Solicitud* s);
    void cambiarEstadoSolicitud(int idSolicitud, std::string nuevoEstado);
    
    void generarReporteGraphviz();
};