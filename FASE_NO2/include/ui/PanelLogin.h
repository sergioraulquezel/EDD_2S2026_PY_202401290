#ifndef PANELDLOGIN_H
#define PANELDLOGIN_H

#include <string>
#include "structures/ArbolBClientes.h"

// Variables globales externas para el control de sesión compartidas con el main
extern bool g_estaAutenticado;
extern std::string g_rolUsuarioActual;
extern std::string g_correoUsuarioActual;
extern std::string g_nombreUsuarioActual;
extern std::string g_idClienteActual;

// Declaración de la función que dibuja el panel de login con Dear ImGui
void dibujarPanelLogin(ArbolBClientes& clientes);

#endif // PANELDLOGIN_H
