#include "structures/ListasPromociones.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

static std::string escaparDot(const std::string& texto) {
    std::string salida;
    for (char ch : texto) {
        if (ch == '"') salida += "\\\"";
        else if (ch == '\\') salida += "\\\\";
        else if (ch == '\n' || ch == '\r') salida += " ";
        else salida += ch;
    }
    return salida;
}

ListaDobleBeneficios::ListaDobleBeneficios() {
    this->cabeza = nullptr;
    this->cola = nullptr;
}

void ListaDobleBeneficios::insertarBeneficio(Beneficio* b) {
    if (b == nullptr) return;

    NodoBeneficio* nuevo = new NodoBeneficio(b);

    if (this->cabeza == nullptr) {
        this->cabeza = nuevo;
        this->cola = nuevo;
    } else {
        this->cola->siguiente = nuevo;
        nuevo->anterior = this->cola;
        this->cola = nuevo;
    }
}

ListaCircularPromociones::ListaCircularPromociones() {
    this->cabeza = nullptr;
    this->cola = nullptr;
}

void ListaCircularPromociones::insertarPromocion(Promocion* p) {
    if (p == nullptr) {
        std::cout << "Error: promocion invalida.\n";
        return;
    }

    if (existePromocion(p->idPromocion)) {
        std::cout << "Error: ya existe una promocion con ID " << p->idPromocion << ".\n";
        return;
    }

    NodoPromocion* nuevo = new NodoPromocion(p);

    if (this->cabeza == nullptr) {
        this->cabeza = nuevo;
        this->cola = nuevo;
        this->cola->siguiente = this->cabeza;
    } else {
        this->cola->siguiente = nuevo;
        this->cola = nuevo;
        this->cola->siguiente = this->cabeza;
    }

    std::cout << "Promocion '" << p->nombre << "' agregada al ciclo.\n";
}

bool ListaCircularPromociones::agregarBeneficioAPromocion(int idPromocion, Beneficio* b) {
    if (b == nullptr) {
        std::cout << "Error: beneficio invalido.\n";
        return false;
    }

    NodoPromocion* actual = buscarPromocion(idPromocion);
    if (actual == nullptr) {
        std::cout << "Error: Promocion con ID " << idPromocion << " no encontrada.\n";
        return false;
    }

    actual->listaBeneficios->insertarBeneficio(b);
    std::cout << "  -> Beneficio de " << b->porcentajeDescuento
              << "% agregado a la promocion '" << actual->promocion->nombre << "'.\n";
    return true;
}

NodoPromocion* ListaCircularPromociones::buscarPromocion(int idPromocion) {
    if (this->cabeza == nullptr) return nullptr;

    NodoPromocion* actual = this->cabeza;
    do {
        if (actual->promocion != nullptr && actual->promocion->idPromocion == idPromocion) {
            return actual;
        }
        actual = actual->siguiente;
    } while (actual != this->cabeza);

    return nullptr;
}

bool ListaCircularPromociones::existePromocion(int idPromocion) {
    return buscarPromocion(idPromocion) != nullptr;
}

int ListaCircularPromociones::contarPromociones() {
    if (this->cabeza == nullptr) return 0;

    int total = 0;
    NodoPromocion* actual = this->cabeza;
    do {
        total++;
        actual = actual->siguiente;
    } while (actual != this->cabeza);

    return total;
}

int ListaCircularPromociones::contarBeneficios(NodoPromocion* nodoPromocion) {
    if (nodoPromocion == nullptr || nodoPromocion->listaBeneficios == nullptr) return 0;

    int total = 0;
    NodoBeneficio* actual = nodoPromocion->listaBeneficios->cabeza;
    while (actual != nullptr) {
        total++;
        actual = actual->siguiente;
    }

    return total;
}

void ListaCircularPromociones::generarReporteGraphviz() {
    std::ofstream archivo("reporte_promociones.dot");
    if (!archivo.is_open()) {
        std::cout << "[ERROR] No se pudo crear reporte_promociones.dot.\n";
        return;
    }

    archivo << "digraph Promociones {\n";
    archivo << "  graph [rankdir=TB, splines=true, nodesep=0.75, ranksep=0.65, bgcolor=\"white\", labelloc=\"t\", label=\"Reporte 3: Lista Circular de Listas (Promociones y Beneficios)\"];\n";
    archivo << "  node [shape=box, style=\"rounded,filled\", fontname=\"Arial\", fontsize=10, margin=\"0.08,0.06\"];\n";
    archivo << "  edge [fontname=\"Arial\", fontsize=9, color=\"#37474f\", arrowsize=0.8];\n";

    if (this->cabeza == nullptr) {
        archivo << "  vacio [label=\"No hay promociones\", fillcolor=\"#fce8b2\"];\n";
    } else {
        archivo << "  { rank=same; ";
        NodoPromocion* actual = this->cabeza;
        do {
            archivo << "promo_" << actual->promocion->idPromocion << "; ";
            actual = actual->siguiente;
        } while (actual != this->cabeza);
        archivo << "}\n";

        actual = this->cabeza;
        do {
            archivo << "  promo_" << actual->promocion->idPromocion
                    << " [label=\"Codigo: " << actual->promocion->idPromocion << "\\n"
                    << escaparDot(actual->promocion->nombre) << "\\n"
                    << escaparDot(actual->promocion->descripcion)
                    << "\", fillcolor=\"#dceff1\", color=\"#37474f\", penwidth=1.5];\n";

            archivo << "  promo_" << actual->promocion->idPromocion << " -> promo_"
                    << actual->siguiente->promocion->idPromocion;
            if (actual->siguiente == this->cabeza) {
                archivo << " [constraint=false, minlen=2, label=\"circular\"];\n";
            } else {
                archivo << " [constraint=false];\n";
            }

            std::string nodoPadre = "promo_" + std::to_string(actual->promocion->idPromocion);
            NodoBeneficio* beneficio = actual->listaBeneficios->cabeza;
            int indiceBeneficio = 0;
            while (beneficio != nullptr) {
                std::string nodoBeneficio = "ben_" + std::to_string(actual->promocion->idPromocion) + "_" + std::to_string(indiceBeneficio);
                archivo << "  " << nodoBeneficio
                        << " [label=\"" << escaparDot(beneficio->beneficio->descripcion) << "\\n"
                        << beneficio->beneficio->porcentajeDescuento << "%\", fillcolor=\"#fff9c4\", color=\"#9e9d24\"];\n";
                archivo << "  " << nodoPadre << " -> " << nodoBeneficio
                        << " [dir=both, color=\"#546e7a\"];\n";
                nodoPadre = nodoBeneficio;
                beneficio = beneficio->siguiente;
                indiceBeneficio++;
            }

            std::string nodoNull = "null_" + std::to_string(actual->promocion->idPromocion);
            archivo << "  " << nodoNull << " [label=\"null\", width=0.45, height=0.2, fontsize=8, fillcolor=\"#fff9c4\", color=\"#9e9d24\"];\n";
            archivo << "  " << nodoPadre << " -> " << nodoNull << " [style=dashed, color=\"#546e7a\"];\n";

            actual = actual->siguiente;
        } while (actual != this->cabeza);
    }

    archivo << "}\n";
    archivo.close();

    int resultado = std::system("dot -Tpng reporte_promociones.dot -o reporte_promociones.png");
    if (resultado == 0) {
        std::cout << "[Graphviz] Reporte generado: reporte_promociones.png\n";
    } else {
        std::cout << "[Graphviz] DOT generado: reporte_promociones.dot. Instale Graphviz o ejecute dot manualmente para crear PNG.\n";
    }
}
