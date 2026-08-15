#include "structures/ListasPromociones.h"
#include <iostream>


ListaDobleBeneficios::ListaDobleBeneficios() {
    this->cabeza = nullptr;
    this->cola = nullptr;
}

void ListaDobleBeneficios::insertarBeneficio(Beneficio* b) {
    NodoBeneficio* nuevo = new NodoBeneficio(b);
    
    if (this->cabeza == nullptr) {
        // Si la lista está vacía
        this->cabeza = nuevo;
        this->cola = nuevo;
    } else {
        // Insertar al final
        this->cola->siguiente = nuevo;
        nuevo->anterior = this->cola;
        this->cola = nuevo;
    }
}


ListaCircularPromociones::ListaCircularPromociones() {
    this->cabeza = nullptr;
    this->cola = nullptr;
}

void ListaCircularPromociones::insertarPromocion(Promocion* p) {
    NodoPromocion* nuevo = new NodoPromocion(p);
    
    if (this->cabeza == nullptr) {
        // Si la lista está vacía, el nodo se apunta a sí mismo
        this->cabeza = nuevo;
        this->cola = nuevo;
        this->cola->siguiente = this->cabeza;
    } else {
        // Insertar al final y mantener la circularidad
        this->cola->siguiente = nuevo;
        this->cola = nuevo;
        this->cola->siguiente = this->cabeza; // La cola siempre apunta a la cabeza
    }
    std::cout << "Promocion '" << p->nombre << "' agregada al ciclo.\n";
}

void ListaCircularPromociones::agregarBeneficioAPromocion(int idPromocion, Beneficio* b) {
    if (this->cabeza == nullptr) {
        std::cout << "No hay promociones registradas.\n";
        return;
    }

    NodoPromocion* actual = this->cabeza;
    bool encontrada = false;

    // Recorrer la lista circular (usamos do-while para asegurar que entramos al menos una vez)
    do {
        if (actual->promocion->idPromocion == idPromocion) {
            // Si encontramos la promoción, insertamos el beneficio en SU lista doble
            actual->listaBeneficios->insertarBeneficio(b);
            std::cout << "  -> Beneficio de " << b->porcentajeDescuento 
                      << "% agregado a la promocion '" << actual->promocion->nombre << "'.\n";
            encontrada = true;
            break;
        }
        actual = actual->siguiente;
    } while (actual != this->cabeza); // Detenerse al dar una vuelta completa

    if (!encontrada) {
        std::cout << "Error: Promocion con ID " << idPromocion << " no encontrada.\n";
    }
}

void ListaCircularPromociones::generarReporteGraphviz() {
    std::cout << "[Graphviz] Preparando generación de reporte de Promociones y Beneficios...\n";
}