#include "ui/PanelLogin.h"
#include "imgui.h"
#include <cstdio>
#include <iostream>
#include <string>

bool g_estaAutenticado = false;
std::string g_rolUsuarioActual = "";

static void botonCredencial(const char* texto, const char* correo, char inputCorreo[], char inputPassword[])
{
    if (ImGui::Button(texto, ImVec2(155, 32)))
    {
        std::snprintf(inputCorreo, 128, "%s", correo);
        std::snprintf(inputPassword, 128, "%s", "123456");
    }
}

void dibujarPanelLogin()
{
    static char inputCorreo[128] = "admin@cine.com";
    static char inputPassword[128] = "123456";
    static std::string mensajeError = "";

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 tamVentana(460, 430);
    ImGui::SetNextWindowSize(tamVentana, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2((io.DisplaySize.x - tamVentana.x) * 0.5f, (io.DisplaySize.y - tamVentana.y) * 0.5f), ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    ImGui::Begin("Acceso al Sistema de Cine", nullptr, flags);

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.34f, 0.86f, 0.92f, 1.0f), "Cinema EDD");
    ImGui::Text("Panel de acceso");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextDisabled("Credenciales rapidas");
    botonCredencial("Admin", "admin@cine.com", inputCorreo, inputPassword);
    ImGui::SameLine();
    botonCredencial("Cliente", "cliente@cine.com", inputCorreo, inputPassword);

    ImGui::Spacing();
    ImGui::Text("Correo electronico");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText("##correo", inputCorreo, IM_ARRAYSIZE(inputCorreo));

    ImGui::Text("Contrasena");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText("##password", inputPassword, IM_ARRAYSIZE(inputPassword), ImGuiInputTextFlags_Password);

    if (!mensajeError.empty())
    {
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "%s", mensajeError.c_str());
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.13f, 0.62f, 0.50f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.16f, 0.74f, 0.60f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.10f, 0.48f, 0.40f, 1.0f));
    if (ImGui::Button("Iniciar sesion", ImVec2(-1, 42)))
    {
        std::string correo(inputCorreo);
        std::string password(inputPassword);

        if (correo == "admin@cine.com" && password == "123456")
        {
            g_estaAutenticado = true;
            g_rolUsuarioActual = "admin";
            mensajeError = "";
            std::cout << "[LOGIN] Sesion iniciada como Administrador.\n";
        }
        else if (correo == "cliente@cine.com" && password == "123456")
        {
            g_estaAutenticado = true;
            g_rolUsuarioActual = "cliente";
            mensajeError = "";
            std::cout << "[LOGIN] Sesion iniciada como Cliente.\n";
        }
        else
        {
            mensajeError = "Correo o contrasena incorrectos.";
        }
    }
    ImGui::PopStyleColor(3);

    ImGui::Spacing();
    ImGui::TextDisabled("Usuarios: admin@cine.com / cliente@cine.com");
    ImGui::End();
}
