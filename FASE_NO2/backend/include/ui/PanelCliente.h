#ifndef PANELCLIENTE_H
#define PANELCLIENTE_H

#include "core/CinemaSystem.h"

class PanelCliente {
public:
    PanelCliente();
    // Debe aceptar ambos argumentos tal como los llamamos en el main
    void dibujar(CinemaSystem& sistema);
};

#endif // PANELCLIENTE_H
