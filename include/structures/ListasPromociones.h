#pragma once
#include "../models/Promocion.h"
#include "../models/Beneficio.h"

class NodoBeneficio {
public:
    Beneficio* beneficio;
    NodoBeneficio* siguiente;
    NodoBeneficio* anterior;

    NodoBeneficio(Beneficio* b) : beneficio(b), siguiente(nullptr), anterior(nullptr) {}
};

class ListaDobleBeneficios {
public:
    NodoBeneficio* cabeza;
    NodoBeneficio* cola;

    ListaDobleBeneficios();
    void insertarBeneficio(Beneficio* b);
};

class NodoPromocion {
public:
    Promocion* promocion;
    ListaDobleBeneficios* listaBeneficios; // Enlace a la sub-estructura
    NodoPromocion* siguiente;

    NodoPromocion(Promocion* p) : promocion(p), listaBeneficios(new ListaDobleBeneficios()), siguiente(nullptr) {}
};

class ListaCircularPromociones {
public:
    NodoPromocion* cabeza;
    NodoPromocion* cola;

    ListaCircularPromociones();
    void insertarPromocion(Promocion* p);
    bool agregarBeneficioAPromocion(int idPromocion, Beneficio* b);
    NodoPromocion* buscarPromocion(int idPromocion);
    bool existePromocion(int idPromocion);
    int contarPromociones();
    int contarBeneficios(NodoPromocion* nodoPromocion);
    
    void generarReporteGraphviz();
};
