#pragma once
#include <string>

class NodoMatriz {
public:
    int fila;
    int columna;
    std::string estado; 
    std::string titular; 

    NodoMatriz* arriba;
    NodoMatriz* abajo;
    NodoMatriz* izquierda;
    NodoMatriz* derecha;

    NodoMatriz(int f, int c, std::string est = "Libre") 
        : fila(f), columna(c), estado(est), titular(""), 
          arriba(nullptr), abajo(nullptr), izquierda(nullptr), derecha(nullptr) {}
};

class MatrizDispersa {
private:
    NodoMatriz* raiz; // Nodo origen (0,0)
    
    NodoMatriz* buscarFila(int fila);
    NodoMatriz* buscarColumna(int columna);
    NodoMatriz* crearCabeceraFila(int fila);
    NodoMatriz* crearCabeceraColumna(int columna);

public:
    MatrizDispersa();
    
    void configurarSala(int totalFilas, int totalColumnas);
    void reservarAsiento(int fila, int columna, std::string cliente);
    void cancelarReserva(int fila, int columna);
    
    void generarReporteGraphviz();
};