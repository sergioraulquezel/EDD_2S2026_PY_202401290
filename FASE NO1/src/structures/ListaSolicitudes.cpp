#include "structures/ListaSolicitudes.h"
#include <cstdlib>
#include <fstream>
#include <iostream>

ListaCircularDoble::ListaCircularDoble() {
    this->cabeza = nullptr;
}

void ListaCircularDoble::insertarSolicitud(Solicitud* s) {
    if (s == nullptr) {
        std::cout << "Error: solicitud invalida.\n";
        return;
    }

    NodoSolicitud* nuevo = new NodoSolicitud(s);

    if (this->cabeza == nullptr) {
        this->cabeza = nuevo;
        this->cabeza->siguiente = this->cabeza;
        this->cabeza->anterior = this->cabeza;
    } else {
        NodoSolicitud* ultimo = this->cabeza->anterior;
        ultimo->siguiente = nuevo;
        nuevo->anterior = ultimo;
        nuevo->siguiente = this->cabeza;
        this->cabeza->anterior = nuevo;
    }

    std::cout << "Solicitud #" << s->idSolicitud << " de " << s->nombreCliente << " registrada.\n";
}

bool ListaCircularDoble::cambiarEstadoSolicitud(int idSolicitud, std::string nuevoEstado) {
    NodoSolicitud* actual = buscarSolicitud(idSolicitud);
    if (actual == nullptr) {
        std::cout << "Solicitud #" << idSolicitud << " no encontrada.\n";
        return false;
    }

    actual->solicitud->estado = nuevoEstado;
    std::cout << "Estado de solicitud #" << idSolicitud << " actualizado a: " << nuevoEstado << "\n";
    return true;
}

NodoSolicitud* ListaCircularDoble::buscarSolicitud(int idSolicitud) {
    if (this->cabeza == nullptr) return nullptr;

    NodoSolicitud* actual = this->cabeza;
    do {
        if (actual->solicitud != nullptr && actual->solicitud->idSolicitud == idSolicitud) {
            return actual;
        }
        actual = actual->siguiente;
    } while (actual != this->cabeza);

    return nullptr;
}

int ListaCircularDoble::contarSolicitudes() {
    if (this->cabeza == nullptr) return 0;

    int total = 0;
    NodoSolicitud* actual = this->cabeza;
    do {
        total++;
        actual = actual->siguiente;
    } while (actual != this->cabeza);

    return total;
}

int ListaCircularDoble::contarPorEstado(std::string estado) {
    if (this->cabeza == nullptr) return 0;

    int total = 0;
    NodoSolicitud* actual = this->cabeza;
    do {
        if (actual->solicitud != nullptr && actual->solicitud->estado == estado) {
            total++;
        }
        actual = actual->siguiente;
    } while (actual != this->cabeza);

    return total;
}

void ListaCircularDoble::generarReporteGraphviz() {
    std::ofstream archivo("reporte_solicitudes.dot");
    if (!archivo.is_open()) {
        std::cout << "[ERROR] No se pudo crear reporte_solicitudes.dot.\n";
        return;
    }

    archivo << "digraph Solicitudes {\n";
    archivo << "  rankdir=LR;\n";
    archivo << "  node [shape=box, style=filled, fontname=\"Arial\"];\n";

    if (this->cabeza == nullptr) {
        archivo << "  vacio [label=\"No hay solicitudes\", fillcolor=\"#fce8b2\"];\n";
    } else {
        NodoSolicitud* actual = this->cabeza;
        do {
            std::string color = "#fce8b2";
            if (actual->solicitud->estado == "Aprobada") color = "#d7ead1";
            if (actual->solicitud->estado == "Rechazada") color = "#f4c7c3";

            archivo << "  sol_" << actual->solicitud->idSolicitud
                    << " [label=\"Solicitud " << actual->solicitud->idSolicitud << "\\n"
                    << actual->solicitud->nombreCliente << "\\n"
                    << actual->solicitud->descripcionEvento << "\\n"
                    << actual->solicitud->estado << "\", fillcolor=\"" << color << "\"];\n";

            archivo << "  sol_" << actual->solicitud->idSolicitud << " -> sol_"
                    << actual->siguiente->solicitud->idSolicitud
                    << " [label=\"sig\"];\n";
            archivo << "  sol_" << actual->solicitud->idSolicitud << " -> sol_"
                    << actual->anterior->solicitud->idSolicitud
                    << " [label=\"ant\", color=\"#888888\"];\n";

            actual = actual->siguiente;
        } while (actual != this->cabeza);
    }

    archivo << "}\n";
    archivo.close();

    int resultado = std::system("dot -Tpng reporte_solicitudes.dot -o reporte_solicitudes.png");
    if (resultado == 0) {
        std::cout << "[Graphviz] Reporte generado: reporte_solicitudes.png\n";
    } else {
        std::cout << "[Graphviz] DOT generado: reporte_solicitudes.dot. Instale Graphviz o ejecute dot manualmente para crear PNG.\n";
    }
}
