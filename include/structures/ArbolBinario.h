#pragma once
#include "models/Pelicula.h"
struct NodoBST {
    Pelicula* pelicula;
    NodoBST* izquierdo;
    NodoBST* derecho;
    
    NodoBST(Pelicula* p) {
        pelicula = p;
        izquierdo = nullptr;
        derecho = nullptr;
    }
};

class ArbolBinario {
private:
    NodoBST* raiz;
    
    // Funciones auxiliares recursivas
    NodoBST* insertarRecursivo(NodoBST* nodo, Pelicula* p);
    void inordenRecursivo(NodoBST* nodo);
    void escribirNodosDot(NodoBST* nodo, std::ofstream& archivo);
    void obtenerPeliculasRecursivo(NodoBST* nodo, Pelicula* arreglo[], int& contador);

public:
    ArbolBinario();
    void insertar(Pelicula* pelicula);
    void imprimirInorden();
    void generarReporteGraphviz();
    void obtenerPeliculas(Pelicula* arreglo[], int& contador);
};