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
    bool cambiarEstadoSolicitud(int idSolicitud, std::string nuevoEstado);
    NodoSolicitud* buscarSolicitud(int idSolicitud);
    int contarSolicitudes();
    int contarPorEstado(std::string estado);
    
    void generarReporteGraphviz();
};
