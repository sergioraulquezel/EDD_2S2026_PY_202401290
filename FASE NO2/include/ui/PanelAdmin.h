#pragma once
#include "structures/ArbolBinario.h"
#include "structures/MatrizDispersa.h"
#include "structures/ListasPromociones.h"
#include "structures/ListaSolicitudes.h"
#include "models/FuncionCine.h"
#include <vector>

class PanelAdmin {
private:
    // Variables temporales para los inputs de texto en ImGui
    int inputId = 0;
    char inputTitulo[100] = "";
    char inputDirector[100] = "";
    int inputDuracion = 0;
    char inputGenero[50] = "";
    int inputFila = 0;
    int inputColumna = 0;
    char inputCliente[100] = "";

public:
    PanelAdmin();
    // Le pasamos las estructuras por referencia para que pueda modificarlas
    void dibujar(ArbolBinario& cartelera, std::vector<FuncionCine>& funciones, ListaCircularPromociones& promociones, ListaCircularDoble& solicitudes);
};
