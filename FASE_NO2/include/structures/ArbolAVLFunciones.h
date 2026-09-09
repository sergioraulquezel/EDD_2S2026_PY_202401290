#pragma once

#include "models/FuncionCine.h"
#include <fstream>
#include <string>
#include <vector>

struct NodoAVLFuncion {
    FuncionCine funcion;
    NodoAVLFuncion* izquierdo;
    NodoAVLFuncion* derecho;
    int altura;

    NodoAVLFuncion(const FuncionCine& f)
        : funcion(f), izquierdo(nullptr), derecho(nullptr), altura(1) {}
};

class ArbolAVLFunciones {
private:
    NodoAVLFuncion* raiz;

    int altura(NodoAVLFuncion* nodo);
    int balance(NodoAVLFuncion* nodo);
    NodoAVLFuncion* rotarDerecha(NodoAVLFuncion* y);
    NodoAVLFuncion* rotarIzquierda(NodoAVLFuncion* x);
    NodoAVLFuncion* insertarRecursivo(NodoAVLFuncion* nodo, const FuncionCine& funcion);
    NodoAVLFuncion* buscarRecursivo(NodoAVLFuncion* nodo, const std::string& codigoFuncion);
    void listarInorden(NodoAVLFuncion* nodo, std::vector<FuncionCine>& salida);
    void listarPreorden(NodoAVLFuncion* nodo, std::vector<FuncionCine>& salida);
    void listarPostorden(NodoAVLFuncion* nodo, std::vector<FuncionCine>& salida);
    void escribirDot(NodoAVLFuncion* nodo, std::ofstream& archivo);

public:
    ArbolAVLFunciones();

    void insertar(const FuncionCine& funcion);
    FuncionCine* buscar(const std::string& codigoFuncion);
    std::vector<FuncionCine> obtenerFunciones(const std::string& recorrido);
    void generarReporteGraphviz();
};
