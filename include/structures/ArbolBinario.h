#pragma once
#include <fstream>
#include "../models/Pelicula.h"

class NodoBST {
public:
    Pelicula* pelicula;
    NodoBST* izquierdo;
    NodoBST* derecho;

    NodoBST(Pelicula* p) : pelicula(p), izquierdo(nullptr), derecho(nullptr) {}
};

class ArbolBinario {
private:
    NodoBST* raiz;
    
    NodoBST* insertarRecursivo(NodoBST* nodo, Pelicula* p);
    void inordenRecursivo(NodoBST* nodo);

    void escribirNodosDot(NodoBST* nodo, std::ofstream& archivo);

public:
    ArbolBinario();
    
    void insertar(Pelicula* p);
    void imprimirInorden();
    
    void generarReporteGraphviz(); 
};