#include "structures/ArbolBinario.h"
#include <iostream>

// --- Constructor ---
ArbolBinario::ArbolBinario() {
    this->raiz = nullptr; // Inicializamos el árbol completamente vacío
}

// --- Métodos Públicos ---

void ArbolBinario::insertar(Pelicula* p) {
    // Llamamos a la función recursiva desde la raíz
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
    std::cout << "[Graphviz] Preparando generación de reporte del BST...\n";
}


// --- Métodos Privados (Lógica Recursiva) ---

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