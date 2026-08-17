#include "ui/PanelLogin.h"
#include "imgui.h"
#include <iostream>
#include <string>

// Estado de autenticación para controlar qué vista mostrar en tu bucle principal (main)
bool g_estaAutenticado = false;
std::string g_rolUsuarioActual = ""; // "admin" o "cliente"

void dibujarPanelLogin()
{
    // Datos precargados por defecto para la simulación
    static char inputCorreo[128] = "admin@cine.com";
    static char inputPassword[128] = "123456";
    static std::string mensajeError = "";

    // Configuración de la ventana principal de login
    ImGui::Begin("PANEL GENERAL", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    
    // Centrar o dar un diseño limpio dentro de la ventana de ImGui
    ImGui::SetWindowSize(ImVec2(450, 400));

    ImGui::Spacing();
    ImGui::Spacing();

    // Título / Botón estético superior "LOGIN" tal como aparece en el diseño
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 300) * 0.5f);
    ImGui::Button("LOGIN", ImVec2(300, 40));

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    // Campos de texto para Correo y Contraseña
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 300) * 0.5f);
    ImGui::Text("Correo Electrónico:");
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 300) * 0.5f);
    ImGui::InputText("##correo", inputCorreo, IM_ARRAYSIZE(inputCorreo));

    ImGui::Spacing();

    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 300) * 0.5f);
    ImGui::Text("Contraseña:");
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 300) * 0.5f);
    ImGui::InputText("##password", inputPassword, IM_ARRAYSIZE(inputPassword), ImGuiInputTextFlags_Password);

    ImGui::Spacing();
    ImGui::Spacing();

    // Mostrar mensaje de error si las credenciales fallan
    if (!mensajeError.empty())
    {
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 300) * 0.5f);
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", mensajeError.c_str());
        ImGui::Spacing();
    }

    // Botón inferior verde "INICIAR SESION"
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 300) * 0.5f);
    
    // Cambiar color temporalmente para que el botón sea verde como en tu imagen
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));

    if (ImGui::Button("INICIAR SESION", ImVec2(300, 40)))
    {
        std::string correo(inputCorreo);
        std::string password(inputPassword);

        // Simulación de validación de usuarios predeterminados
        if (correo == "admin@cine.com" && password == "123456")
        {
            g_estaAutenticado = true;
            g_rolUsuarioActual = "admin";
            mensajeError = "";
            std::cout << "[LOGIN] Sesión iniciada como Administrador.\n";
        }
        else if (correo == "cliente@cine.com" && password == "123456")
        {
            g_estaAutenticado = true;
            g_rolUsuarioActual = "cliente";
            mensajeError = "";
            std::cout << "[LOGIN] Sesión iniciada como Cliente.\n";
        }
        else
        {
            mensajeError = "Correo o contraseña incorrectos.";
        }
    }

    ImGui::PopStyleColor(3);
    ImGui::End();
}