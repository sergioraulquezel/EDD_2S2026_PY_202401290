#include "structures/ArbolBinario.h"
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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

static std::vector<std::string> separarCSV(const std::string& linea) {
    std::vector<std::string> campos;
    std::string campo;
    bool dentroComillas = false;

    for (size_t i = 0; i < linea.size(); ++i) {
        char ch = linea[i];
        if (ch == '"') {
            if (dentroComillas && i + 1 < linea.size() && linea[i + 1] == '"') {
                campo += '"';
                ++i;
            } else {
                dentroComillas = !dentroComillas;
            }
        } else if (ch == ',' && !dentroComillas) {
            campos.push_back(campo);
            campo.clear();
        } else {
            campo += ch;
        }
    }

    campos.push_back(campo);
    return campos;
}

static int extraerIdDesdeCodigo(const std::string& codigo) {
    std::string digitos;
    for (char ch : codigo) {
        if (ch >= '0' && ch <= '9') {
            digitos += ch;
        }
    }

    if (digitos.empty()) return 0;
    return std::stoi(digitos);
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

int ArbolBinario::cargarPeliculasCSV(std::string rutaArchivo) {
    std::ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        std::cout << "[ERROR] No se pudo abrir el archivo CSV: " << rutaArchivo << "\n";
        return 0;
    }

    std::string linea;
    int insertadas = 0;
    int lineaActual = 0;

    while (std::getline(archivo, linea)) {
        lineaActual++;
        if (!linea.empty() && linea.back() == '\r') {
            linea.pop_back();
        }

        if (linea.empty()) continue;
        if (lineaActual == 1 && linea.find("codigo") != std::string::npos && linea.find("titulo") != std::string::npos) {
            continue;
        }

        std::vector<std::string> campos = separarCSV(linea);
        if (campos.size() < 8) {
            std::cout << "[CSV] Linea " << lineaActual << " ignorada: se esperaban 8 columnas.\n";
            continue;
        }

        std::string codigo = campos[0];
        int id = extraerIdDesdeCodigo(codigo);
        if (id <= 0) {
            std::cout << "[CSV] Linea " << lineaActual << " ignorada: codigo invalido '" << codigo << "'.\n";
            continue;
        }

        if (existeIdRecursivo(this->raiz, id)) {
            std::cout << "[CSV] Linea " << lineaActual << " ignorada: codigo duplicado '" << codigo << "'.\n";
            continue;
        }

        int duracion = 0;
        try {
            duracion = std::stoi(campos[3]);
        } catch (...) {
            std::cout << "[CSV] Linea " << lineaActual << " ignorada: duracion invalida.\n";
            continue;
        }

        Pelicula* pelicula = new Pelicula(
            id,
            campos[1],
            campos[2],
            duracion,
            campos[4],
            campos[7],
            codigo,
            campos[5],
            campos[6]
        );

        insertar(pelicula);
        insertadas++;
    }

    std::cout << "[CSV] Peliculas cargadas correctamente: " << insertadas << "\n";
    return insertadas;
}

void ArbolBinario::escribirNodosDot(NodoBST* nodo, std::ofstream& archivo) {
    if (nodo != nullptr) {
        std::string color = estaProximaARetirarBST(nodo->pelicula) ? "#ffe082" : "#b7e1a1";
        std::string fechaFin = nodo->pelicula->getFechaFin().empty() ? "Sin fecha fin" : nodo->pelicula->getFechaFin();

        archivo << "    nodo" << nodo->pelicula->getId()
                << " [fillcolor=\"" << color << "\", label=\"Codigo: " << escaparDotBST(nodo->pelicula->getCodigo())
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

bool ArbolBinario::existeIdRecursivo(NodoBST* nodo, int id) {
    if (nodo == nullptr) return false;
    if (nodo->pelicula->getId() == id) return true;
    if (id < nodo->pelicula->getId()) return existeIdRecursivo(nodo->izquierdo, id);
    return existeIdRecursivo(nodo->derecho, id);
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
