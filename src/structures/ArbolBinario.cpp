#include "structures/ArbolBinario.h"
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

static std::string escaparDotBST(const std::string& texto) {
    std::string salida;
    for (char ch : texto) {
        if (ch == '"') salida += "\\\"";
        else if (ch == '\\') salida += "\\\\";
        else if (ch == '\n' || ch == '\r') salida += " ";
        else salida += ch;
    }
    return salida;
}

static bool parsearFechaFinBST(const std::string& fecha, std::tm& salida) {
    int dia = 0;
    int mes = 0;
    int anio = 0;

    if (std::sscanf(fecha.c_str(), "%d/%d/%d", &dia, &mes, &anio) != 3 &&
        std::sscanf(fecha.c_str(), "%d-%d-%d", &anio, &mes, &dia) != 3) {
        return false;
    }

    if (dia < 1 || dia > 31 || mes < 1 || mes > 12 || anio < 1900) {
        return false;
    }

    salida = {};
    salida.tm_mday = dia;
    salida.tm_mon = mes - 1;
    salida.tm_year = anio - 1900;
    salida.tm_hour = 0;
    salida.tm_min = 0;
    salida.tm_sec = 0;
    salida.tm_isdst = -1;
    return true;
}

static bool estaProximaARetirarBST(Pelicula* pelicula) {
    if (pelicula == nullptr || pelicula->getFechaFin().empty()) {
        return false;
    }

    std::tm fechaFin = {};
    if (!parsearFechaFinBST(pelicula->getFechaFin(), fechaFin)) {
        return false;
    }

    std::time_t ahora = std::time(nullptr);
    std::tm hoy = *std::localtime(&ahora);
    hoy.tm_hour = 0;
    hoy.tm_min = 0;
    hoy.tm_sec = 0;
    hoy.tm_isdst = -1;

    std::time_t tiempoHoy = std::mktime(&hoy);
    std::time_t tiempoFin = std::mktime(&fechaFin);
    if (tiempoHoy == (std::time_t)-1 || tiempoFin == (std::time_t)-1) {
        return false;
    }

    double diasRestantes = std::difftime(tiempoFin, tiempoHoy) / (60 * 60 * 24);
    return diasRestantes >= 0 && diasRestantes < 7;
}

ArbolBinario::ArbolBinario() {
    this->raiz = nullptr;
}

void ArbolBinario::insertar(Pelicula* p) {
    this->raiz = insertarRecursivo(this->raiz, p);
}

void ArbolBinario::imprimirInorden() {
    std::cout << "--- Cartelera de Peliculas (Recorrido Inorden) ---\n";
    if (this->raiz == nullptr) {
        std::cout << "El arbol esta vacio. No hay peliculas en cartelera.\n";
    } else {
        inordenRecursivo(this->raiz);
    }
    std::cout << "--------------------------------------------------\n";
}

void ArbolBinario::generarReporteGraphviz() {
    if (this->raiz == nullptr) {
        std::cout << "El arbol esta vacio. No se puede generar reporte.\n";
        return;
    }

    std::ofstream archivo("reporte_bst.dot");
    if (!archivo.is_open()) {
        std::cout << "[ERROR] No se pudo crear reporte_bst.dot.\n";
        return;
    }

    archivo << "digraph ArbolPeliculas {\n";
    archivo << "    rankdir=TB;\n";
    archivo << "    graph [bgcolor=\"white\", labelloc=\"t\", label=\"Arbol binario de busqueda (Cartelera de Peliculas)\"];\n";
    archivo << "    node [shape=box, style=\"rounded,filled\", fontname=\"Arial\", fontsize=10, color=\"#37474f\", penwidth=1.4];\n";
    archivo << "    edge [color=\"#37474f\", arrowsize=0.8];\n";
    archivo << "    leyenda_verde [label=\"En cartelera\", fillcolor=\"#b7e1a1\"];\n";
    archivo << "    leyenda_amarillo [label=\"Proximo a retirar (< 7 dias)\", fillcolor=\"#ffe082\"];\n";
    archivo << "    { rank=same; leyenda_verde; leyenda_amarillo; }\n";

    escribirNodosDot(this->raiz, archivo);

    archivo << "}\n";
    archivo.close();

    std::cout << "Generando imagen del arbol...\n";
    int resultado = std::system("dot -Tpng reporte_bst.dot -o reporte_bst.png");
    if (resultado == 0) {
        std::cout << "Reporte generado: reporte_bst.png\n";
    } else {
        std::cout << "[Graphviz] DOT generado: reporte_bst.dot. No se pudo generar PNG automaticamente.\n";
    }
}

void ArbolBinario::escribirNodosDot(NodoBST* nodo, std::ofstream& archivo) {
    if (nodo != nullptr) {
        std::string color = estaProximaARetirarBST(nodo->pelicula) ? "#ffe082" : "#b7e1a1";
        std::string fechaFin = nodo->pelicula->getFechaFin().empty() ? "Sin fecha fin" : nodo->pelicula->getFechaFin();

        archivo << "    nodo" << nodo->pelicula->getId()
                << " [fillcolor=\"" << color << "\", label=\"Codigo: CO-" << nodo->pelicula->getId()
                << "\\nTitulo: " << escaparDotBST(nodo->pelicula->getTitulo())
                << "\\nDuracion: " << nodo->pelicula->getDuracion() << " min"
                << "\\nClasificacion: " << escaparDotBST(nodo->pelicula->getClasificacion())
                << "\\nFecha fin: " << escaparDotBST(fechaFin)
                << "\"];\n";

        if (nodo->izquierdo != nullptr) {
            archivo << "    nodo" << nodo->pelicula->getId()
                    << " -> nodo" << nodo->izquierdo->pelicula->getId() << ";\n";
            escribirNodosDot(nodo->izquierdo, archivo);
        }

        if (nodo->derecho != nullptr) {
            archivo << "    nodo" << nodo->pelicula->getId()
                    << " -> nodo" << nodo->derecho->pelicula->getId() << ";\n";
            escribirNodosDot(nodo->derecho, archivo);
        }
    }
}

NodoBST* ArbolBinario::insertarRecursivo(NodoBST* nodo, Pelicula* p) {
    if (nodo == nullptr) {
        return new NodoBST(p);
    }

    if (p->getId() < nodo->pelicula->getId()) {
        nodo->izquierdo = insertarRecursivo(nodo->izquierdo, p);
    } else if (p->getId() > nodo->pelicula->getId()) {
        nodo->derecho = insertarRecursivo(nodo->derecho, p);
    } else {
        std::cout << "Aviso: La pelicula con ID " << p->getId()
                  << " (" << p->getTitulo() << ") ya existe en la cartelera.\n";
    }

    return nodo;
}

void ArbolBinario::inordenRecursivo(NodoBST* nodo) {
    if (nodo != nullptr) {
        inordenRecursivo(nodo->izquierdo);
        std::cout << "ID: " << nodo->pelicula->getId()
                  << " | Titulo: " << nodo->pelicula->getTitulo()
                  << " | Genero: " << nodo->pelicula->getGenero()
                  << " | Fecha fin: " << nodo->pelicula->getFechaFin() << "\n";
        inordenRecursivo(nodo->derecho);
    }
}

void ArbolBinario::obtenerPeliculas(Pelicula* arreglo[], int& contador) {
    obtenerPeliculasRecursivo(this->raiz, arreglo, contador);
}

void ArbolBinario::obtenerPeliculasRecursivo(NodoBST* nodo, Pelicula* arreglo[], int& contador) {
    if (nodo == nullptr) {
        return;
    }

    obtenerPeliculasRecursivo(nodo->izquierdo, arreglo, contador);
    arreglo[contador] = nodo->pelicula;
    contador++;
    obtenerPeliculasRecursivo(nodo->derecho, arreglo, contador);
}
