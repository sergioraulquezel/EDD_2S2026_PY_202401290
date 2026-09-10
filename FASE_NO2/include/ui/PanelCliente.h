#ifndef PANELCLIENTE_H
#define PANELCLIENTE_H

#include "structures/ArbolBinario.h"
#include "structures/MatrizDispersa.h"
#include "structures/ListasPromociones.h"
#include "structures/ListaSolicitudes.h"
#include "structures/ArbolBClientes.h"
#include "structures/TablaHashReservas.h"
#include "models/FuncionCine.h"
#include <vector>

class PanelCliente {
public:
    PanelCliente();
    // Debe aceptar ambos argumentos tal como los llamamos en el main
    void dibujar(ArbolBinario &cartelera, std::vector<FuncionCine> &funciones, ListaCircularPromociones &promociones,
                 ListaCircularDoble &solicitudes, ArbolBClientes& clientes, TablaHashReservas& reservas);
};

#endif // PANELCLIENTE_H
