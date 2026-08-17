#include "structures/MatrizDispersa.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

static std::string escaparHtml(const std::string& texto) {
    std::string salida;
    for (char ch : texto) {
        if (ch == '&') salida += "&amp;";
        else if (ch == '<') salida += "&lt;";
        else if (ch == '>') salida += "&gt;";
        else if (ch == '"') salida += "&quot;";
        else salida += ch;
    }
    return salida;
}

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

MatrizDispersa::MatrizDispersa() : raiz(new NodoMatriz(-1, -1, "RAIZ")), totalFilas(0), totalColumnas(0) {}

MatrizDispersa::MatrizDispersa(const MatrizDispersa& otra) : raiz(new NodoMatriz(-1, -1, "RAIZ")), totalFilas(0), totalColumnas(0) {
    copiarDesde(otra);
}

MatrizDispersa& MatrizDispersa::operator=(const MatrizDispersa& otra) {
    if (this != &otra) {
        limpiar();
        copiarDesde(otra);
    }
    return *this;
}

MatrizDispersa::~MatrizDispersa() {
    limpiar();
    delete raiz;
}

void MatrizDispersa::limpiar() {
    NodoMatriz* fila = raiz->abajo;
    while (fila != nullptr) {
        NodoMatriz* siguienteFila = fila->abajo;
        NodoMatriz* asiento = fila->derecha;
        while (asiento != nullptr) {
            NodoMatriz* siguienteAsiento = asiento->derecha;
            delete asiento;
            asiento = siguienteAsiento;
        }
        delete fila;
        fila = siguienteFila;
    }

    NodoMatriz* columna = raiz->derecha;
    while (columna != nullptr) {
        NodoMatriz* siguienteColumna = columna->derecha;
        delete columna;
        columna = siguienteColumna;
    }

    raiz->arriba = nullptr;
    raiz->abajo = nullptr;
    raiz->izquierda = nullptr;
    raiz->derecha = nullptr;
    totalFilas = 0;
    totalColumnas = 0;
}

void MatrizDispersa::copiarDesde(const MatrizDispersa& otra) {
    totalFilas = otra.totalFilas;
    totalColumnas = otra.totalColumnas;

    NodoMatriz* fila = otra.raiz->abajo;
    while (fila != nullptr) {
        NodoMatriz* asiento = fila->derecha;
        while (asiento != nullptr) {
            reservarAsiento(asiento->fila, asiento->columna, asiento->titular);
            asiento = asiento->derecha;
        }
        fila = fila->abajo;
    }
}

NodoMatriz* MatrizDispersa::buscarFila(int fila) {
    NodoMatriz* actual = raiz->abajo;
    while (actual != nullptr) {
        if (actual->fila == fila) return actual;
        actual = actual->abajo;
    }
    return nullptr;
}

NodoMatriz* MatrizDispersa::buscarColumna(int columna) {
    NodoMatriz* actual = raiz->derecha;
    while (actual != nullptr) {
        if (actual->columna == columna) return actual;
        actual = actual->derecha;
    }
    return nullptr;
}

NodoMatriz* MatrizDispersa::crearCabeceraFila(int fila) {
    NodoMatriz* nuevaFila = new NodoMatriz(fila, -1, "CABECERA_FILA");
    NodoMatriz* actual = raiz;

    while (actual->abajo != nullptr && actual->abajo->fila < fila) {
        actual = actual->abajo;
    }

    nuevaFila->abajo = actual->abajo;
    if (actual->abajo != nullptr) actual->abajo->arriba = nuevaFila;
    actual->abajo = nuevaFila;
    nuevaFila->arriba = actual;

    return nuevaFila;
}

NodoMatriz* MatrizDispersa::crearCabeceraColumna(int columna) {
    NodoMatriz* nuevaCol = new NodoMatriz(-1, columna, "CABECERA_COL");
    NodoMatriz* actual = raiz;

    while (actual->derecha != nullptr && actual->derecha->columna < columna) {
        actual = actual->derecha;
    }

    nuevaCol->derecha = actual->derecha;
    if (actual->derecha != nullptr) actual->derecha->izquierda = nuevaCol;
    actual->derecha = nuevaCol;
    nuevaCol->izquierda = actual;

    return nuevaCol;
}

void MatrizDispersa::configurarSala(int filas, int columnas) {
    limpiar();
    totalFilas = filas;
    totalColumnas = columnas;
    std::cout << "Matriz configurada para una sala de " << filas << " filas por " << columnas << " columnas.\n";
}

bool MatrizDispersa::reservarAsiento(int fila, int columna, std::string cliente) {
    if (!asientoValido(fila, columna)) {
        std::cout << "[ERROR] Asiento (" << fila << "," << columna << ") fuera de los limites de la sala.\n";
        return false;
    }

    if (obtenerAsiento(fila, columna) != nullptr) {
        std::cout << "[AVISO] El asiento (" << fila << "," << columna << ") ya esta reservado.\n";
        return false;
    }

    NodoMatriz* nodoFila = buscarFila(fila);
    if (nodoFila == nullptr) nodoFila = crearCabeceraFila(fila);

    NodoMatriz* nodoCol = buscarColumna(columna);
    if (nodoCol == nullptr) nodoCol = crearCabeceraColumna(columna);

    NodoMatriz* nuevoAsiento = new NodoMatriz(fila, columna, "Reservado");
    nuevoAsiento->titular = cliente.empty() ? "Sin titular" : cliente;

    NodoMatriz* actualH = nodoFila;
    while (actualH->derecha != nullptr && actualH->derecha->columna < columna) {
        actualH = actualH->derecha;
    }
    nuevoAsiento->derecha = actualH->derecha;
    if (actualH->derecha != nullptr) actualH->derecha->izquierda = nuevoAsiento;
    actualH->derecha = nuevoAsiento;
    nuevoAsiento->izquierda = actualH;

    NodoMatriz* actualV = nodoCol;
    while (actualV->abajo != nullptr && actualV->abajo->fila < fila) {
        actualV = actualV->abajo;
    }
    nuevoAsiento->abajo = actualV->abajo;
    if (actualV->abajo != nullptr) actualV->abajo->arriba = nuevoAsiento;
    actualV->abajo = nuevoAsiento;
    nuevoAsiento->arriba = actualV;

    std::cout << "Asiento (" << fila << "," << columna << ") reservado exitosamente para " << nuevoAsiento->titular << ".\n";
    return true;
}

bool MatrizDispersa::cancelarReserva(int fila, int columna) {
    NodoMatriz* asiento = obtenerAsiento(fila, columna);
    if (asiento == nullptr) {
        std::cout << "[AVISO] No existe reserva en el asiento (" << fila << "," << columna << ").\n";
        return false;
    }

    if (asiento->izquierda != nullptr) asiento->izquierda->derecha = asiento->derecha;
    if (asiento->derecha != nullptr) asiento->derecha->izquierda = asiento->izquierda;
    if (asiento->arriba != nullptr) asiento->arriba->abajo = asiento->abajo;
    if (asiento->abajo != nullptr) asiento->abajo->arriba = asiento->arriba;

    delete asiento;
    std::cout << "Reserva cancelada en asiento (" << fila << "," << columna << ").\n";
    return true;
}

void MatrizDispersa::generarReporteGraphviz() {
    generarReporteGraphviz("Funcion: Sin funcion seleccionada");
}

void MatrizDispersa::generarReporteGraphviz(std::string tituloFuncion) {
    std::ofstream archivo("reporte_matriz_asientos.dot");
    if (!archivo.is_open()) {
        std::cout << "[ERROR] No se pudo crear reporte_matriz_asientos.dot.\n";
        return;
    }

    int reservados = contarReservados();
    int total = totalFilas * totalColumnas;
    int libres = total - reservados;

    archivo << "digraph MatrizAsientos {\n";
    archivo << "  graph [rankdir=TB, splines=ortho, nodesep=0.55, ranksep=0.65, bgcolor=\"white\", labelloc=\"t\", label=\""
            << escaparDot(tituloFuncion) << "\\nTotal: " << total
            << " | Reservados: " << reservados
            << " | Libres: " << libres << "\"];\n";
    archivo << "  node [fontname=\"Arial\", fontsize=11, style=\"filled\", margin=\"0.08,0.06\"];\n";
    archivo << "  edge [color=\"#37474f\", arrowsize=0.75];\n";
    archivo << "  funcion [label=\"Funcion\", shape=diamond, fillcolor=\"#ffffff\", color=\"#37474f\", penwidth=1.6];\n";
    archivo << "  stats [label=\"Total: " << total << "\\nReservados: " << reservados << "\\nLibres: " << libres
            << "\", shape=note, fillcolor=\"#e8f5e9\", color=\"#2e7d32\"];\n";

    for (int f = 1; f <= totalFilas; ++f) {
        archivo << "  fila_" << f << " [label=\"F" << f
                << "\", shape=box, fillcolor=\"#ffca28\", color=\"#37474f\", penwidth=1.4];\n";
    }

    for (int c = 1; c <= totalColumnas; ++c) {
        archivo << "  col_" << c << " [label=\"C" << c
                << "\", shape=box, fillcolor=\"#ffca28\", color=\"#37474f\", penwidth=1.4];\n";
    }

    for (int f = 1; f <= totalFilas; ++f) {
        for (int c = 1; c <= totalColumnas; ++c) {
            NodoMatriz* asiento = obtenerAsiento(f, c);
            archivo << "  asiento_" << f << "_" << c;
            if (asiento != nullptr) {
                archivo << " [label=\"" << escaparDot(asiento->titular)
                        << "\\nF" << f << "-C" << c
                        << "\", shape=circle, width=0.9, fixedsize=false, fillcolor=\"#ff6b6b\", color=\"#37474f\", penwidth=1.4];\n";
            } else {
                archivo << " [label=\"F" << f << "-C" << c
                        << "\\nLibre\", shape=box, style=\"rounded,dashed,filled\", fillcolor=\"#fff1f1\", color=\"#cfcfcf\", fontcolor=\"#777777\"];\n";
            }
        }
    }

    archivo << "  { rank=same; funcion; ";
    for (int c = 1; c <= totalColumnas; ++c) {
        archivo << "col_" << c << "; ";
    }
    archivo << "stats; }\n";

    for (int f = 1; f <= totalFilas; ++f) {
        archivo << "  { rank=same; fila_" << f << "; ";
        for (int c = 1; c <= totalColumnas; ++c) {
            archivo << "asiento_" << f << "_" << c << "; ";
        }
        archivo << "}\n";
    }

    archivo << "  funcion -> col_1;\n";
    archivo << "  funcion -> fila_1;\n";

    for (int c = 1; c < totalColumnas; ++c) {
        archivo << "  col_" << c << " -> col_" << (c + 1) << ";\n";
    }

    for (int f = 1; f < totalFilas; ++f) {
        archivo << "  fila_" << f << " -> fila_" << (f + 1) << ";\n";
    }

    for (int f = 1; f <= totalFilas; ++f) {
        if (totalColumnas > 0) {
            archivo << "  fila_" << f << " -> asiento_" << f << "_1 [dir=both];\n";
        }
        for (int c = 1; c < totalColumnas; ++c) {
            archivo << "  asiento_" << f << "_" << c << " -> asiento_" << f << "_" << (c + 1)
                    << " [dir=both];\n";
        }
    }

    for (int c = 1; c <= totalColumnas; ++c) {
        if (totalFilas > 0) {
            archivo << "  col_" << c << " -> asiento_1_" << c << " [dir=both];\n";
        }
        for (int f = 1; f < totalFilas; ++f) {
            archivo << "  asiento_" << f << "_" << c << " -> asiento_" << (f + 1) << "_" << c
                    << " [dir=both];\n";
        }
    }

    archivo << "}\n";
    archivo.close();

    int resultado = std::system("dot -Tpng reporte_matriz_asientos.dot -o reporte_matriz_asientos.png");
    if (resultado == 0) {
        std::cout << "[Graphviz] Reporte generado: reporte_matriz_asientos.png\n";
    } else {
        std::cout << "[Graphviz] DOT generado: reporte_matriz_asientos.dot. Instale Graphviz o ejecute dot manualmente para crear PNG.\n";
    }
}

bool MatrizDispersa::estaConfigurada() const {
    return totalFilas > 0 && totalColumnas > 0;
}

bool MatrizDispersa::asientoValido(int fila, int columna) const {
    return estaConfigurada() && fila >= 1 && fila <= totalFilas && columna >= 1 && columna <= totalColumnas;
}

int MatrizDispersa::obtenerFilas() const {
    return totalFilas;
}

int MatrizDispersa::obtenerColumnas() const {
    return totalColumnas;
}

int MatrizDispersa::contarReservados() {
    int total = 0;
    NodoMatriz* fila = raiz->abajo;
    while (fila != nullptr) {
        NodoMatriz* actual = fila->derecha;
        while (actual != nullptr) {
            ++total;
            actual = actual->derecha;
        }
        fila = fila->abajo;
    }
    return total;
}

NodoMatriz* MatrizDispersa::obtenerAsiento(int fila, int columna) {
    NodoMatriz* auxFila = buscarFila(fila);
    if (auxFila == nullptr) return nullptr;

    NodoMatriz* actual = auxFila->derecha;
    while (actual != nullptr) {
        if (actual->columna == columna) {
            return actual;
        }
        actual = actual->derecha;
    }
    return nullptr;
}
