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
    NodoMatriz* raiz; // Nodo origen (-1,-1)
    int totalFilas;
    int totalColumnas;
    
    NodoMatriz* buscarFila(int fila);
    NodoMatriz* buscarColumna(int columna);
    NodoMatriz* crearCabeceraFila(int fila);
    NodoMatriz* crearCabeceraColumna(int columna);
    void limpiar();
    void copiarDesde(const MatrizDispersa& otra);

public:
    MatrizDispersa();
    MatrizDispersa(const MatrizDispersa& otra);
    MatrizDispersa& operator=(const MatrizDispersa& otra);
    ~MatrizDispersa();
    
    void configurarSala(int totalFilas, int totalColumnas);
    bool reservarAsiento(int fila, int columna, std::string cliente);
    bool cancelarReserva(int fila, int columna);
    
    void generarReporteGraphviz();
    void generarReporteGraphviz(std::string tituloFuncion);

    bool estaConfigurada() const;
    bool asientoValido(int fila, int columna) const;
    int obtenerFilas() const;
    int obtenerColumnas() const;
    int contarReservados();
    NodoMatriz* obtenerAsiento(int fila, int columna);
};
