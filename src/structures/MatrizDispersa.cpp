#include "structures/MatrizDispersa.h"
#include <iostream>

MatrizDispersa::MatrizDispersa() {
    // El nodo raíz maestro de la matriz, usualmente en posición (-1, -1) o (0,0)
    this->raiz = new NodoMatriz(-1, -1, "RAIZ");
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

// --- Métodos de Creación de Cabeceras (Insertando en orden) ---

NodoMatriz* MatrizDispersa::crearCabeceraFila(int fila) {
    NodoMatriz* nuevaFila = new NodoMatriz(fila, -1, "CABECERA_FILA");
    NodoMatriz* actual = raiz;
    
    // Buscar la posición correcta para mantener el orden ascendente
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


void MatrizDispersa::reservarAsiento(int fila, int columna, std::string cliente) {
    NodoMatriz* nodoFila = buscarFila(fila);
    if (nodoFila == nullptr) nodoFila = crearCabeceraFila(fila);
    
    NodoMatriz* nodoCol = buscarColumna(columna);
    if (nodoCol == nullptr) nodoCol = crearCabeceraColumna(columna);
    
    NodoMatriz* nuevoAsiento = new NodoMatriz(fila, columna, "Reservado");
    nuevoAsiento->titular = cliente;
    
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
    
    std::cout << "Asiento (" << fila << "," << columna << ") reservado exitosamente para " << cliente << ".\n";
}

void MatrizDispersa::generarReporteGraphviz() {
    std::cout << "[Graphviz] Preparando generación de reporte de la Matriz...\n";
}