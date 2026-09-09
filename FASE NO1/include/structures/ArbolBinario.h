#pragma once
#include "models/Pelicula.h"
#include <fstream>
#include <string>
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
    void escribirNodosDot(NodoBST* nodo, std::ofstream& archivo, int& contadorNulos);
    void obtenerPeliculasRecursivo(NodoBST* nodo, Pelicula* arreglo[], int& contador);
    bool existeIdRecursivo(NodoBST* nodo, int id);
    bool validarBSTRecursivo(NodoBST* nodo, bool& tieneUltimoId, int& ultimoId, int& totalNodos);

public:
    ArbolBinario();
    void insertar(Pelicula* pelicula);
    void imprimirInorden();
    bool validarBST();
    void generarReporteGraphviz();
    int cargarPeliculasCSV(std::string rutaArchivo);
    void obtenerPeliculas(Pelicula* arreglo[], int& contador);
};
