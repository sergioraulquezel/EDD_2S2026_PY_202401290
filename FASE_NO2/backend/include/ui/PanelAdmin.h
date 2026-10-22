#pragma once
#include "core/CinemaSystem.h"

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
    void dibujar(CinemaSystem& sistema);
};
