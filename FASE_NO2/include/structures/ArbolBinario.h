#pragma once
#include "models/Pelicula.h"
#include <fstream>
#include <string>
#include <vector>
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
    NodoBST* eliminarRecursivo(NodoBST* nodo, int id, bool& eliminado, bool liberarMemoria);
    NodoBST* minimo(NodoBST* nodo);
    void inordenRecursivo(NodoBST* nodo);
    void listarPreordenRecursivo(NodoBST* nodo, std::vector<Pelicula*>& salida);
    void listarInordenRecursivo(NodoBST* nodo, std::vector<Pelicula*>& salida);
    void listarPostordenRecursivo(NodoBST* nodo, std::vector<Pelicula*>& salida);
    void escribirNodosDot(NodoBST* nodo, std::ofstream& archivo, int& contadorNulos);
    void obtenerPeliculasRecursivo(NodoBST* nodo, Pelicula* arreglo[], int& contador);
    bool existeIdRecursivo(NodoBST* nodo, int id);
    Pelicula* buscarIdRecursivo(NodoBST* nodo, int id);
    bool validarBSTRecursivo(NodoBST* nodo, bool& tieneUltimoId, int& ultimoId, int& totalNodos);

public:
    ArbolBinario();
    void insertar(Pelicula* pelicula);
    bool eliminarPorId(int id, bool liberarMemoria = true);
    bool eliminarPorCodigo(std::string codigo, bool liberarMemoria = true);
    void imprimirInorden();
    bool validarBST();
    void generarReporteGraphviz();
    int cargarPeliculasCSV(std::string rutaArchivo);
    Pelicula* buscarPorId(int id);
    Pelicula* buscarPorCodigo(std::string codigo);
    int obtenerDiasRestantes(Pelicula* pelicula);
    std::vector<Pelicula*> obtenerPeliculasProximasAFinalizar(int limiteDias = 7);
    std::vector<Pelicula*> obtenerPeliculasRecorrido(const std::string& recorrido);
    void obtenerPeliculas(Pelicula* arreglo[], int& contador);
};
