#include "structures/ListaSolicitudes.h"
#include <iostream>

ListaCircularDoble::ListaCircularDoble() {
    this->cabeza = nullptr;
}

void ListaCircularDoble::insertarSolicitud(Solicitud* s) {
    NodoSolicitud* nuevo = new NodoSolicitud(s);

    if (this->cabeza == nullptr) {
        // Si está vacía, el nodo se apunta a sí mismo en ambas direcciones
        this->cabeza = nuevo;
        this->cabeza->siguiente = this->cabeza;
        this->cabeza->anterior = this->cabeza;
    } else {
        // Encontrar el último nodo (que es el anterior a la cabeza)
        NodoSolicitud* ultimo = this->cabeza->anterior;

        // Conectar el nuevo nodo al final
        ultimo->siguiente = nuevo;
        nuevo->anterior = ultimo;

        // Cerrar el círculo con la cabeza
        nuevo->siguiente = this->cabeza;
        this->cabeza->anterior = nuevo;
    }
    
    std::cout << "Solicitud #" << s->idSolicitud << " de " << s->nombreCliente << " registrada.\n";
}

void ListaCircularDoble::cambiarEstadoSolicitud(int idSolicitud, std::string nuevoEstado) {
    if (this->cabeza == nullptr) return;

    NodoSolicitud* actual = this->cabeza;
    bool encontrada = false;

    do {
        if (actual->solicitud->idSolicitud == idSolicitud) {
            actual->solicitud->estado = nuevoEstado;
            std::cout << "Estado de solicitud #" << idSolicitud << " actualizado a: " << nuevoEstado << "\n";
            encontrada = true;
            break;
        }
        actual = actual->siguiente;
    } while (actual != this->cabeza);

    if (!encontrada) {
        std::cout << "Solicitud #" << idSolicitud << " no encontrada.\n";
    }
}

void ListaCircularDoble::generarReporteGraphviz() {
    std::cout << "[Graphviz] Preparando generación de reporte de Solicitudes...\n";
}