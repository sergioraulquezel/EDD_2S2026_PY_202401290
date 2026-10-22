#include "structures/ArbolBClientes.h"
#include <cstdlib>
#include <fstream>
#include <iostream>

static std::string escaparDotCliente(const std::string& texto)
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

NodoBClientes::NodoBClientes(bool esHoja) : hoja(esHoja)
{
}

ArbolBClientes::ArbolBClientes() : raiz(nullptr)
{
}

ArbolBClientes::~ArbolBClientes()
{
    liberar(raiz);
}

void ArbolBClientes::liberar(NodoBClientes* nodo)
{
    if (nodo == nullptr) return;
    for (NodoBClientes* hijo : nodo->hijos) {
        liberar(hijo);
    }
    delete nodo;
}

bool ArbolBClientes::insertar(const Cliente& cliente)
{
    if (cliente.id.empty() || cliente.correo.empty() || cliente.password.empty()) {
        std::cout << "[ARBOL B] Cliente invalido: id, correo y password son obligatorios.\n";
        return false;
    }
    if (buscarPorId(cliente.id) != nullptr) {
        std::cout << "[ARBOL B] Ya existe el cliente con ID " << cliente.id << ".\n";
        return false;
    }
    if (buscarPorCorreo(cliente.correo) != nullptr) {
        std::cout << "[ARBOL B] Ya existe un cliente con correo " << cliente.correo << ".\n";
        return false;
    }

    if (raiz == nullptr) {
        raiz = new NodoBClientes(true);
        raiz->claves.push_back(cliente);
        return true;
    }

    if ((int)raiz->claves.size() == 2 * GRADO_MINIMO - 1) {
        NodoBClientes* nuevaRaiz = new NodoBClientes(false);
        nuevaRaiz->hijos.push_back(raiz);
        dividirHijo(nuevaRaiz, 0);
        raiz = nuevaRaiz;
    }

    insertarNoLleno(raiz, cliente);
    return true;
}

void ArbolBClientes::dividirHijo(NodoBClientes* padre, int indice)
{
    NodoBClientes* lleno = padre->hijos[indice];
    NodoBClientes* nuevo = new NodoBClientes(lleno->hoja);
    Cliente medio = lleno->claves[GRADO_MINIMO - 1];

    for (int j = 0; j < GRADO_MINIMO - 1; ++j) {
        nuevo->claves.push_back(lleno->claves[j + GRADO_MINIMO]);
    }

    if (!lleno->hoja) {
        for (int j = 0; j < GRADO_MINIMO; ++j) {
            nuevo->hijos.push_back(lleno->hijos[j + GRADO_MINIMO]);
        }
        lleno->hijos.resize(GRADO_MINIMO);
    }

    lleno->claves.resize(GRADO_MINIMO - 1);
    padre->hijos.insert(padre->hijos.begin() + indice + 1, nuevo);
    padre->claves.insert(padre->claves.begin() + indice, medio);
}

void ArbolBClientes::insertarNoLleno(NodoBClientes* nodo, const Cliente& cliente)
{
    int i = (int)nodo->claves.size() - 1;

    if (nodo->hoja) {
        nodo->claves.push_back(cliente);
        while (i >= 0 && cliente.id < nodo->claves[i].id) {
            nodo->claves[i + 1] = nodo->claves[i];
            --i;
        }
        nodo->claves[i + 1] = cliente;
        return;
    }

    while (i >= 0 && cliente.id < nodo->claves[i].id) {
        --i;
    }
    ++i;

    if ((int)nodo->hijos[i]->claves.size() == 2 * GRADO_MINIMO - 1) {
        dividirHijo(nodo, i);
        if (cliente.id > nodo->claves[i].id) {
            ++i;
        }
    }
    insertarNoLleno(nodo->hijos[i], cliente);
}

Cliente* ArbolBClientes::buscarPorId(const std::string& id)
{
    return buscarRecursivo(raiz, id);
}

Cliente* ArbolBClientes::buscarRecursivo(NodoBClientes* nodo, const std::string& id)
{
    if (nodo == nullptr) return nullptr;
    int i = 0;
    while (i < (int)nodo->claves.size() && id > nodo->claves[i].id) {
        ++i;
    }
    if (i < (int)nodo->claves.size() && id == nodo->claves[i].id) {
        return &nodo->claves[i];
    }
    if (nodo->hoja) return nullptr;
    return buscarRecursivo(nodo->hijos[i], id);
}

Cliente* ArbolBClientes::buscarPorCorreo(const std::string& correo)
{
    return buscarPorCorreoRecursivo(raiz, correo);
}

Cliente* ArbolBClientes::buscarPorCorreoRecursivo(NodoBClientes* nodo, const std::string& correo)
{
    if (nodo == nullptr) return nullptr;
    for (Cliente& cliente : nodo->claves) {
        if (cliente.correo == correo) return &cliente;
    }
    for (NodoBClientes* hijo : nodo->hijos) {
        Cliente* encontrado = buscarPorCorreoRecursivo(hijo, correo);
        if (encontrado != nullptr) return encontrado;
    }
    return nullptr;
}

bool ArbolBClientes::autenticar(const std::string& correo, const std::string& password, Cliente*& cliente)
{
    cliente = buscarPorCorreo(correo);
    return cliente != nullptr && cliente->password == password;
}

std::vector<Cliente> ArbolBClientes::listarOrdenado() const
{
    std::vector<Cliente> salida;
    listarRecursivo(raiz, salida);
    return salida;
}

void ArbolBClientes::listarRecursivo(NodoBClientes* nodo, std::vector<Cliente>& salida) const
{
    if (nodo == nullptr) return;
    for (int i = 0; i < (int)nodo->claves.size(); ++i) {
        if (!nodo->hoja) listarRecursivo(nodo->hijos[i], salida);
        salida.push_back(nodo->claves[i]);
    }
    if (!nodo->hoja) listarRecursivo(nodo->hijos[(int)nodo->claves.size()], salida);
}

int ArbolBClientes::contar() const
{
    return (int)listarOrdenado().size();
}

bool ArbolBClientes::eliminar(const std::string& id)
{
    std::vector<Cliente> clientes = listarOrdenado();
    bool encontrado = false;
    liberar(raiz);
    raiz = nullptr;

    for (const Cliente& cliente : clientes) {
        if (cliente.id == id) {
            encontrado = true;
        } else {
            insertar(cliente);
        }
    }
    return encontrado;
}

void ArbolBClientes::generarReporteGraphviz()
{
    std::ofstream archivo("reporte_arbol_b_clientes.dot");
    if (!archivo.is_open()) {
        std::cout << "[Graphviz] No se pudo crear reporte_arbol_b_clientes.dot.\n";
        return;
    }

    archivo << "digraph ArbolBClientes {\n";
    archivo << "  graph [rankdir=TB, bgcolor=\"white\", labelloc=\"t\", fontname=\"Arial\", fontsize=18, label=\"Reporte 3: Arbol B de Orden 4 de Clientes\"];\n";
    archivo << "  node [shape=record, style=\"rounded,filled\", fontname=\"Arial\", fontsize=10, color=\"#37474f\"];\n";
    archivo << "  edge [color=\"#37474f\", arrowsize=0.8];\n";

    int contador = 0;
    if (raiz == nullptr) {
        archivo << "  vacio [label=\"Sin clientes\", fillcolor=\"#eeeeee\"];\n";
    } else {
        escribirDot(raiz, archivo, contador);
    }

    archivo << "}\n";
    archivo.close();

    int resultado = std::system("dot -Tpng reporte_arbol_b_clientes.dot -o reporte_arbol_b_clientes.png");
    if (resultado == 0) {
        std::cout << "[Graphviz] Reporte generado: reporte_arbol_b_clientes.png\n";
    } else {
        std::cout << "[Graphviz] DOT generado: reporte_arbol_b_clientes.dot.\n";
    }
}

void ArbolBClientes::escribirDot(NodoBClientes* nodo, std::ofstream& archivo, int& contador)
{
    if (nodo == nullptr) return;
    int idNodo = contador++;
    std::string color = (idNodo == 0) ? "#ffdd75" : (nodo->hoja ? "#c8e6c9" : "#bbdefb");

    archivo << "  nodo" << idNodo << " [fillcolor=\"" << color << "\", label=\"";
    for (int i = 0; i < (int)nodo->claves.size(); ++i) {
        if (i > 0) archivo << "|";
        archivo << "<k" << i << "> " << escaparDotCliente(nodo->claves[i].id)
                << "\\n" << escaparDotCliente(nodo->claves[i].nombre)
                << "\\n" << escaparDotCliente(nodo->claves[i].correo);
    }
    archivo << "\"];\n";

    for (NodoBClientes* hijo : nodo->hijos) {
        int idHijo = contador;
        escribirDot(hijo, archivo, contador);
        archivo << "  nodo" << idNodo << " -> nodo" << idHijo << ";\n";
    }
}
