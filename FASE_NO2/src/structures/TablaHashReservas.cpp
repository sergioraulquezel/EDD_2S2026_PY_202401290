#include "structures/TablaHashReservas.h"
#include <cstdlib>
#include <fstream>
#include <iostream>

static std::string escaparDotReserva(const std::string& texto)
{
    std::string salida;
    for (char ch : texto) {
        if (ch == '"') salida += "\\\"";
        else if (ch == '\\') salida += "\\\\";
        else if (ch == '\n' || ch == '\r') salida += " ";
        else salida += ch;
    }
    return salida;
}

NodoHashReserva::NodoHashReserva(const Reserva& reservaNueva)
    : reserva(reservaNueva), siguiente(nullptr)
{
}

TablaHashReservas::TablaHashReservas(int tamano) : buckets(tamano, nullptr), totalReservas(0)
{
}

TablaHashReservas::~TablaHashReservas()
{
    liberar();
}

void TablaHashReservas::liberar()
{
    for (NodoHashReserva*& cabeza : buckets) {
        while (cabeza != nullptr) {
            NodoHashReserva* siguiente = cabeza->siguiente;
            delete cabeza;
            cabeza = siguiente;
        }
    }
    totalReservas = 0;
}

int TablaHashReservas::hash(const std::string& codigoReserva) const
{
    unsigned int valor = 0;
    for (char ch : codigoReserva) {
        valor = valor * 31 + static_cast<unsigned char>(ch);
    }
    return (int)(valor % buckets.size());
}

bool TablaHashReservas::insertar(const Reserva& reserva)
{
    if (reserva.codigoReserva.empty() || reserva.idCliente.empty() || reserva.codigoFuncion.empty()) {
        std::cout << "[HASH] Reserva invalida: codigo, cliente y funcion son obligatorios.\n";
        return false;
    }
    if (buscar(reserva.codigoReserva) != nullptr) {
        std::cout << "[HASH] Ya existe la reserva " << reserva.codigoReserva << ".\n";
        return false;
    }

    int indice = hash(reserva.codigoReserva);
    NodoHashReserva* nuevo = new NodoHashReserva(reserva);
    nuevo->siguiente = buckets[indice];
    buckets[indice] = nuevo;
    ++totalReservas;
    return true;
}

Reserva* TablaHashReservas::buscar(const std::string& codigoReserva)
{
    int indice = hash(codigoReserva);
    NodoHashReserva* actual = buckets[indice];
    while (actual != nullptr) {
        if (actual->reserva.codigoReserva == codigoReserva) {
            return &actual->reserva;
        }
        actual = actual->siguiente;
    }
    return nullptr;
}

bool TablaHashReservas::eliminar(const std::string& codigoReserva)
{
    int indice = hash(codigoReserva);
    NodoHashReserva* actual = buckets[indice];
    NodoHashReserva* anterior = nullptr;

    while (actual != nullptr) {
        if (actual->reserva.codigoReserva == codigoReserva) {
            if (anterior == nullptr) buckets[indice] = actual->siguiente;
            else anterior->siguiente = actual->siguiente;
            delete actual;
            --totalReservas;
            return true;
        }
        anterior = actual;
        actual = actual->siguiente;
    }
    return false;
}

std::vector<Reserva> TablaHashReservas::listarTodas() const
{
    std::vector<Reserva> salida;
    for (NodoHashReserva* cabeza : buckets) {
        NodoHashReserva* actual = cabeza;
        while (actual != nullptr) {
            salida.push_back(actual->reserva);
            actual = actual->siguiente;
        }
    }
    return salida;
}

std::vector<Reserva> TablaHashReservas::listarPorCliente(const std::string& idCliente) const
{
    std::vector<Reserva> salida;
    for (NodoHashReserva* cabeza : buckets) {
        NodoHashReserva* actual = cabeza;
        while (actual != nullptr) {
            if (actual->reserva.idCliente == idCliente) salida.push_back(actual->reserva);
            actual = actual->siguiente;
        }
    }
    return salida;
}

int TablaHashReservas::contar() const
{
    return totalReservas;
}

int TablaHashReservas::tamano() const
{
    return (int)buckets.size();
}

int TablaHashReservas::bucketsOcupados() const
{
    int ocupados = 0;
    for (NodoHashReserva* cabeza : buckets) {
        if (cabeza != nullptr) ++ocupados;
    }
    return ocupados;
}

int TablaHashReservas::contarColisiones() const
{
    int colisiones = 0;
    for (NodoHashReserva* cabeza : buckets) {
        int elementos = 0;
        NodoHashReserva* actual = cabeza;
        while (actual != nullptr) {
            ++elementos;
            actual = actual->siguiente;
        }
        if (elementos > 1) colisiones += elementos - 1;
    }
    return colisiones;
}

void TablaHashReservas::generarReporteGraphviz()
{
    std::ofstream archivo("reporte_tabla_hash_reservas.dot");
    if (!archivo.is_open()) {
        std::cout << "[Graphviz] No se pudo crear reporte_tabla_hash_reservas.dot.\n";
        return;
    }

    archivo << "digraph TablaHashReservas {\n";
    archivo << "  graph [rankdir=LR, bgcolor=\"white\", labelloc=\"t\", fontname=\"Arial\", fontsize=18, label=\"Reporte 4: Tabla Hash de Reservas\\nTamano: "
            << tamano() << " | Reservas: " << contar() << " | Buckets ocupados: " << bucketsOcupados()
            << " | Colisiones: " << contarColisiones() << "\"];\n";
    archivo << "  node [shape=box, style=\"rounded,filled\", fontname=\"Arial\", fontsize=10, color=\"#37474f\"];\n";
    archivo << "  edge [color=\"#37474f\", arrowsize=0.8];\n";

    for (int i = 0; i < (int)buckets.size(); ++i) {
        bool vacio = buckets[i] == nullptr;
        archivo << "  bucket" << i << " [label=\"Bucket " << i << "\", fillcolor=\""
                << (vacio ? "#eeeeee" : "#ffdd75") << "\"];\n";

        NodoHashReserva* actual = buckets[i];
        int pos = 0;
        while (actual != nullptr) {
            std::string nombreNodo = "res_" + std::to_string(i) + "_" + std::to_string(pos);
            archivo << "  " << nombreNodo << " [label=\""
                    << escaparDotReserva(actual->reserva.codigoReserva)
                    << "\\nCliente: " << escaparDotReserva(actual->reserva.idCliente)
                    << "\\nFuncion: " << escaparDotReserva(actual->reserva.codigoFuncion)
                    << "\\nF" << actual->reserva.fila << "-C" << actual->reserva.columna
                    << "\", fillcolor=\"" << (pos == 0 ? "#c8e6c9" : "#ffcdd2") << "\"];\n";
            if (pos == 0) archivo << "  bucket" << i << " -> " << nombreNodo << ";\n";
            else archivo << "  res_" << i << "_" << (pos - 1) << " -> " << nombreNodo << ";\n";
            actual = actual->siguiente;
            ++pos;
        }
    }

    archivo << "}\n";
    archivo.close();

    int resultado = std::system("dot -Tpng reporte_tabla_hash_reservas.dot -o reporte_tabla_hash_reservas.png");
    if (resultado == 0) {
        std::cout << "[Graphviz] Reporte generado: reporte_tabla_hash_reservas.png\n";
    } else {
        std::cout << "[Graphviz] DOT generado: reporte_tabla_hash_reservas.dot.\n";
    }
}
