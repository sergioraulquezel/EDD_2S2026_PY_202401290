#ifndef PANELDLOGIN_H
#define PANELDLOGIN_H

#include <string>

// Variables globales externas para el control de sesión compartidas con el main
extern bool g_estaAutenticado;
extern std::string g_rolUsuarioActual;

// Declaración de la función que dibuja el panel de login con Dear ImGui
void dibujarPanelLogin();

#endif // PANELDLOGIN_H