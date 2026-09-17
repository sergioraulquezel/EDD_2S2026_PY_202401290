#include "structures/ArbolAVLFunciones.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>

static std::string escaparDotAVL(const std::string& texto)
{
    std::string salida;
    for (char ch : texto) {
        if (ch == '"') salida += "\\\"";
        else if (ch == '\\') salida += "\\\\";
        else if (ch == '\n' || ch == '\r') salida += " ";
        else salida += ch;
    }
    return salida;
}

static bool funcionRealizada(const FuncionCine& funcion)
{
    return false;
}

ArbolAVLFunciones::ArbolAVLFunciones() : raiz(nullptr) {}

int ArbolAVLFunciones::altura(NodoAVLFuncion* nodo)
{
    return nodo == nullptr ? 0 : nodo->altura;
}

int ArbolAVLFunciones::balance(NodoAVLFuncion* nodo)
{
    return nodo == nullptr ? 0 : altura(nodo->izquierdo) - altura(nodo->derecho);
}

NodoAVLFuncion* ArbolAVLFunciones::rotarDerecha(NodoAVLFuncion* y)
{
    NodoAVLFuncion* x = y->izquierdo;
    NodoAVLFuncion* t2 = x->derecho;

    x->derecho = y;
    y->izquierdo = t2;

    y->altura = 1 + std::max(altura(y->izquierdo), altura(y->derecho));
    x->altura = 1 + std::max(altura(x->izquierdo), altura(x->derecho));

    return x;
}

NodoAVLFuncion* ArbolAVLFunciones::rotarIzquierda(NodoAVLFuncion* x)
{
    NodoAVLFuncion* y = x->derecho;
    NodoAVLFuncion* t2 = y->izquierdo;

    y->izquierdo = x;
    x->derecho = t2;

    x->altura = 1 + std::max(altura(x->izquierdo), altura(x->derecho));
    y->altura = 1 + std::max(altura(y->izquierdo), altura(y->derecho));

    return y;
}

void ArbolAVLFunciones::insertar(const FuncionCine& funcion)
{
    raiz = insertarRecursivo(raiz, funcion);
}

bool ArbolAVLFunciones::eliminar(const std::string& codigoFuncion)
{
    bool eliminado = false;
    raiz = eliminarRecursivo(raiz, codigoFuncion, eliminado);
    return eliminado;
}

NodoAVLFuncion* ArbolAVLFunciones::insertarRecursivo(NodoAVLFuncion* nodo, const FuncionCine& funcion)
{
    if (nodo == nullptr) {
        return new NodoAVLFuncion(funcion);
    }

    if (funcion.codigoFuncion < nodo->funcion.codigoFuncion) {
        nodo->izquierdo = insertarRecursivo(nodo->izquierdo, funcion);
    } else if (funcion.codigoFuncion > nodo->funcion.codigoFuncion) {
        nodo->derecho = insertarRecursivo(nodo->derecho, funcion);
    } else {
        nodo->funcion = funcion;
        return nodo;
    }

    nodo->altura = 1 + std::max(altura(nodo->izquierdo), altura(nodo->derecho));
    int factor = balance(nodo);

    if (factor > 1 && funcion.codigoFuncion < nodo->izquierdo->funcion.codigoFuncion) {
        return rotarDerecha(nodo);
    }
    if (factor < -1 && funcion.codigoFuncion > nodo->derecho->funcion.codigoFuncion) {
        return rotarIzquierda(nodo);
    }
    if (factor > 1 && funcion.codigoFuncion > nodo->izquierdo->funcion.codigoFuncion) {
        nodo->izquierdo = rotarIzquierda(nodo->izquierdo);
        return rotarDerecha(nodo);
    }
    if (factor < -1 && funcion.codigoFuncion < nodo->derecho->funcion.codigoFuncion) {
        nodo->derecho = rotarDerecha(nodo->derecho);
        return rotarIzquierda(nodo);
    }

    return nodo;
}

NodoAVLFuncion* ArbolAVLFunciones::minimo(NodoAVLFuncion* nodo)
{
    NodoAVLFuncion* actual = nodo;
    while (actual != nullptr && actual->izquierdo != nullptr) {
        actual = actual->izquierdo;
    }
    return actual;
}

NodoAVLFuncion* ArbolAVLFunciones::eliminarRecursivo(NodoAVLFuncion* nodo, const std::string& codigoFuncion, bool& eliminado)
{
    if (nodo == nullptr) return nullptr;

    if (codigoFuncion < nodo->funcion.codigoFuncion) {
        nodo->izquierdo = eliminarRecursivo(nodo->izquierdo, codigoFuncion, eliminado);
    } else if (codigoFuncion > nodo->funcion.codigoFuncion) {
        nodo->derecho = eliminarRecursivo(nodo->derecho, codigoFuncion, eliminado);
    } else {
        eliminado = true;

        if (nodo->izquierdo == nullptr || nodo->derecho == nullptr) {
            NodoAVLFuncion* hijo = nodo->izquierdo != nullptr ? nodo->izquierdo : nodo->derecho;
            delete nodo;
            return hijo;
        }

        NodoAVLFuncion* sucesor = minimo(nodo->derecho);
        nodo->funcion = sucesor->funcion;
        bool eliminadoSucesor = false;
        nodo->derecho = eliminarRecursivo(nodo->derecho, sucesor->funcion.codigoFuncion, eliminadoSucesor);
    }

    nodo->altura = 1 + std::max(altura(nodo->izquierdo), altura(nodo->derecho));
    int factor = balance(nodo);

    if (factor > 1 && balance(nodo->izquierdo) >= 0) {
        return rotarDerecha(nodo);
    }
    if (factor > 1 && balance(nodo->izquierdo) < 0) {
        nodo->izquierdo = rotarIzquierda(nodo->izquierdo);
        return rotarDerecha(nodo);
    }
    if (factor < -1 && balance(nodo->derecho) <= 0) {
        return rotarIzquierda(nodo);
    }
    if (factor < -1 && balance(nodo->derecho) > 0) {
        nodo->derecho = rotarDerecha(nodo->derecho);
        return rotarIzquierda(nodo);
    }

    return nodo;
}

FuncionCine* ArbolAVLFunciones::buscar(const std::string& codigoFuncion)
{
    NodoAVLFuncion* nodo = buscarRecursivo(raiz, codigoFuncion);
    return nodo == nullptr ? nullptr : &nodo->funcion;
}

NodoAVLFuncion* ArbolAVLFunciones::buscarRecursivo(NodoAVLFuncion* nodo, const std::string& codigoFuncion)
{
    if (nodo == nullptr || nodo->funcion.codigoFuncion == codigoFuncion) {
        return nodo;
    }
    if (codigoFuncion < nodo->funcion.codigoFuncion) {
        return buscarRecursivo(nodo->izquierdo, codigoFuncion);
    }
    return buscarRecursivo(nodo->derecho, codigoFuncion);
}

std::vector<FuncionCine> ArbolAVLFunciones::obtenerFunciones(const std::string& recorrido)
{
    std::vector<FuncionCine> salida;
    if (recorrido == "preorden") listarPreorden(raiz, salida);
    else if (recorrido == "postorden") listarPostorden(raiz, salida);
    else listarInorden(raiz, salida);
    return salida;
}

void ArbolAVLFunciones::listarInorden(NodoAVLFuncion* nodo, std::vector<FuncionCine>& salida)
{
    if (nodo == nullptr) return;
    listarInorden(nodo->izquierdo, salida);
    salida.push_back(nodo->funcion);
    listarInorden(nodo->derecho, salida);
}

void ArbolAVLFunciones::listarPreorden(NodoAVLFuncion* nodo, std::vector<FuncionCine>& salida)
{
    if (nodo == nullptr) return;
    salida.push_back(nodo->funcion);
    listarPreorden(nodo->izquierdo, salida);
    listarPreorden(nodo->derecho, salida);
}

void ArbolAVLFunciones::listarPostorden(NodoAVLFuncion* nodo, std::vector<FuncionCine>& salida)
{
    if (nodo == nullptr) return;
    listarPostorden(nodo->izquierdo, salida);
    listarPostorden(nodo->derecho, salida);
    salida.push_back(nodo->funcion);
}

void ArbolAVLFunciones::generarReporteGraphviz()
{
    std::ofstream archivo("reporte_avl_funciones.dot");
    if (!archivo.is_open()) {
        std::cout << "[ERROR] No se pudo crear reporte_avl_funciones.dot.\n";
        return;
    }

    archivo << "digraph AVLFunciones {\n";
    archivo << "  rankdir=TB;\n";
    archivo << "  graph [bgcolor=\"white\", labelloc=\"t\", fontname=\"Arial\", fontsize=18, label=\"Arbol AVL de Funciones\"];\n";
    archivo << "  node [shape=circle, style=\"filled\", fontname=\"Arial\", fontsize=9, color=\"#37474f\", penwidth=1.4, width=1.25, fixedsize=false];\n";
    archivo << "  edge [color=\"#37474f\", arrowsize=0.8];\n";
    archivo << "  leyenda_programada [label=\"Programada\", fillcolor=\"#b7e1a1\"];\n";
    archivo << "  leyenda_realizada [label=\"Realizada\", fillcolor=\"#ff8a80\"];\n";
    archivo << "  { rank=same; leyenda_programada; leyenda_realizada; }\n";

    if (raiz == nullptr) {
        archivo << "  vacio [label=\"Sin funciones\", shape=box, fillcolor=\"#eeeeee\"];\n";
    } else {
        escribirDot(raiz, archivo);
    }

    archivo << "}\n";
    archivo.close();

    int resultado = std::system("dot -Tpng reporte_avl_funciones.dot -o reporte_avl_funciones.png");
    if (resultado == 0) {
        std::cout << "[Graphviz] Reporte generado: reporte_avl_funciones.png\n";
    } else {
        std::cout << "[Graphviz] DOT generado: reporte_avl_funciones.dot. No se pudo generar PNG automaticamente.\n";
    }
}

void ArbolAVLFunciones::escribirDot(NodoAVLFuncion* nodo, std::ofstream& archivo)
{
    if (nodo == nullptr) return;

    std::string titulo = nodo->funcion.pelicula != nullptr ? nodo->funcion.pelicula->getTitulo() : "Sin pelicula";
    std::string color = funcionRealizada(nodo->funcion) ? "#ff8a80" : "#b7e1a1";

    archivo << "  \"" << escaparDotAVL(nodo->funcion.codigoFuncion) << "\""
            << " [fillcolor=\"" << color << "\", label=\""
            << escaparDotAVL(nodo->funcion.codigoFuncion)
            << "\\n" << escaparDotAVL(titulo)
            << "\\n" << escaparDotAVL(nodo->funcion.horario)
            << "\\n" << escaparDotAVL(nodo->funcion.sala)
            << "\\nAltura: " << nodo->altura << "\"];\n";

    if (nodo->izquierdo != nullptr) {
        archivo << "  \"" << escaparDotAVL(nodo->funcion.codigoFuncion) << "\" -> \""
                << escaparDotAVL(nodo->izquierdo->funcion.codigoFuncion) << "\";\n";
        escribirDot(nodo->izquierdo, archivo);
    }

    if (nodo->derecho != nullptr) {
        archivo << "  \"" << escaparDotAVL(nodo->funcion.codigoFuncion) << "\" -> \""
                << escaparDotAVL(nodo->derecho->funcion.codigoFuncion) << "\";\n";
        escribirDot(nodo->derecho, archivo);
    }
}
