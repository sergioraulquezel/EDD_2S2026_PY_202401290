#include "structures/ArbolBinario.h"
#include <iostream>

#include <fstream>
#include <cstdlib> // Para la función system()

// --- Constructor ---
ArbolBinario::ArbolBinario() {
    this->raiz = nullptr; // Inicializamos el árbol completamente vacío
}


void ArbolBinario::insertar(Pelicula* p) {
    this->raiz = insertarRecursivo(this->raiz, p);
}

void ArbolBinario::imprimirInorden() {
    std::cout << "--- Cartelera de Películas (Recorrido Inorden) ---\n";
    if (this->raiz == nullptr) {
        std::cout << "El árbol está vacío. No hay películas en cartelera.\n";
    } else {
        inordenRecursivo(this->raiz);
    }
    std::cout << "--------------------------------------------------\n";
}

void ArbolBinario::generarReporteGraphviz() {
    if (this->raiz == nullptr) {
        std::cout << "El árbol está vacío. No se puede generar reporte.\n";
        return;
    }

    std::ofstream archivo("reporte_bst.dot");

    archivo << "digraph ArbolPeliculas {\n";
    archivo << "    rankdir=TB;\n"; // Top-Bottom (De arriba hacia abajo)
    archivo << "    node [shape=record, style=filled, fillcolor=lightcyan, fontname=\"Arial\"];\n";

    escribirNodosDot(this->raiz, archivo);

    archivo << "}\n";
    archivo.close();

    std::cout << "Generando imagen del arbol...\n";
    system("dot -Tpng reporte_bst.dot -o reporte_bst.png");
    std::cout << "Reporte generado: reporte_bst.png\n";
}

void ArbolBinario::escribirNodosDot(NodoBST* nodo, std::ofstream& archivo) {
    if (nodo != nullptr) {
        archivo << "    nodo" << nodo->pelicula->idPelicula
                << " [label=\"{ID: " << nodo->pelicula->idPelicula 
                << " | " << nodo->pelicula->titulo << "}\"];\n";

        // Si tiene hijo izquierdo, dibujar flecha hacia él
        if (nodo->izquierdo != nullptr) {
            archivo << "    nodo" << nodo->pelicula->idPelicula 
                    << " -> nodo" << nodo->izquierdo->pelicula->idPelicula << ";\n";
            escribirNodosDot(nodo->izquierdo, archivo); // Visitar hijo izquierdo
        }

        // Si tiene hijo derecho, dibujar flecha hacia él
        if (nodo->derecho != nullptr) {
            archivo << "    nodo" << nodo->pelicula->idPelicula 
                    << " -> nodo" << nodo->derecho->pelicula->idPelicula << ";\n";
            escribirNodosDot(nodo->derecho, archivo); // Visitar hijo derecho
        }
    }
}

NodoBST* ArbolBinario::insertarRecursivo(NodoBST* nodo, Pelicula* p) {
    if (nodo == nullptr) {
        return new NodoBST(p);
    }

    // Regla de inserción del BST comparando los IDs
    if (p->idPelicula < nodo->pelicula->idPelicula) {
        // Si el ID es menor, viajamos por la rama izquierda
        nodo->izquierdo = insertarRecursivo(nodo->izquierdo, p);
    } 
    else if (p->idPelicula > nodo->pelicula->idPelicula) {
        // Si el ID es mayor, viajamos por la rama derecha
        nodo->derecho = insertarRecursivo(nodo->derecho, p);
    } 
    else {
        // Si el ID es igual, decidimos no insertarlo para evitar películas duplicadas
        std::cout << "Aviso: La película con ID " << p->idPelicula 
                  << " (" << p->titulo << ") ya existe en la cartelera.\n";
    }

    // Retornamos el nodo actual 
    return nodo;
}

void ArbolBinario::inordenRecursivo(NodoBST* nodo) {
    // El recorrido Inorden garantiza que los IDs se impriman de menor a mayor
    if (nodo != nullptr) {
        inordenRecursivo(nodo->izquierdo);
        
        std::cout << "ID: " << nodo->pelicula->idPelicula 
                  << " | Titulo: " << nodo->pelicula->titulo 
                  << " | Genero: " << nodo->pelicula->genero << "\n";
        
        inordenRecursivo(nodo->derecho);
    }
}