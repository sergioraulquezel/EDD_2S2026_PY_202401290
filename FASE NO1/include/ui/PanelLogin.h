#ifndef PANELDLOGIN_H
#define PANELDLOGIN_H

#include <string>

// Variables globales externas para el control de sesión compartidas con el main
extern bool g_estaAutenticado;
extern std::string g_rolUsuarioActual;
extern std::string g_correoUsuarioActual;
extern std::string g_nombreUsuarioActual;

// Declaración de la función que dibuja el panel de login con Dear ImGui
void dibujarPanelLogin();

#endif // PANELDLOGIN_H
