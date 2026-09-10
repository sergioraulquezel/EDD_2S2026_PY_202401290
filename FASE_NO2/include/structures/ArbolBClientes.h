#pragma once

#include "models/Cliente.h"
#include <string>
#include <vector>

class NodoBClientes {
public:
    bool hoja;
    std::vector<Cliente> claves;
    std::vector<NodoBClientes*> hijos;

    explicit NodoBClientes(bool esHoja);
};

class ArbolBClientes {
private:
    NodoBClientes* raiz;
    static const int GRADO_MINIMO = 2;

    void dividirHijo(NodoBClientes* padre, int indice);
    void insertarNoLleno(NodoBClientes* nodo, const Cliente& cliente);
    Cliente* buscarRecursivo(NodoBClientes* nodo, const std::string& id);
    Cliente* buscarPorCorreoRecursivo(NodoBClientes* nodo, const std::string& correo);
    void listarRecursivo(NodoBClientes* nodo, std::vector<Cliente>& salida) const;
    void liberar(NodoBClientes* nodo);
    void escribirDot(NodoBClientes* nodo, std::ofstream& archivo, int& contador);

public:
    ArbolBClientes();
    ~ArbolBClientes();

    bool insertar(const Cliente& cliente);
    Cliente* buscarPorId(const std::string& id);
    Cliente* buscarPorCorreo(const std::string& correo);
    bool autenticar(const std::string& correo, const std::string& password, Cliente*& cliente);
    bool eliminar(const std::string& id);
    std::vector<Cliente> listarOrdenado() const;
    int contar() const;
    void generarReporteGraphviz();
};
