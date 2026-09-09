#include "ui/PanelLogin.h"
#include "imgui.h"
#include <cstdio>
#include <iostream>
#include <string>

bool g_estaAutenticado = false;
std::string g_rolUsuarioActual = "";
std::string g_correoUsuarioActual = "";
std::string g_nombreUsuarioActual = "";

struct UsuarioDemo
{
    const char* correo;
    const char* password;
    const char* rol;
    const char* nombre;
};

static const UsuarioDemo USUARIOS_DEMO[] = {
    {"admin@cine.com", "123456", "admin", "Administrador"},
    {"cliente1@cine.com", "123456", "cliente", "Cliente 1"},
    {"cliente2@cine.com", "123456", "cliente", "Cliente 2"},
    {"cliente3@cine.com", "123456", "cliente", "Cliente 3"},
    {"cliente4@cine.com", "123456", "cliente", "Cliente 4"},
    {"cliente5@cine.com", "123456", "cliente", "Cliente 5"},
};

static const UsuarioDemo* buscarUsuario(const std::string& correo, const std::string& password)
{
    for (const UsuarioDemo& usuario : USUARIOS_DEMO)
    {
        if (correo == usuario.correo && password == usuario.password)
        {
            return &usuario;
        }
    }
    return nullptr;
}

static void botonCredencial(const char* texto, const UsuarioDemo& usuario, char inputCorreo[], char inputPassword[])
{
    if (ImGui::Button(texto, ImVec2(155, 32)))
    {
        std::snprintf(inputCorreo, 128, "%s", usuario.correo);
        std::snprintf(inputPassword, 128, "%s", usuario.password);
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
    botonCredencial("Admin", USUARIOS_DEMO[0], inputCorreo, inputPassword);
    for (int i = 1; i <= 5; ++i)
    {
        if (i % 2 == 1)
        {
            ImGui::SameLine();
        }
        else
        {
            ImGui::Spacing();
        }

        char etiqueta[32];
        std::snprintf(etiqueta, sizeof(etiqueta), "Cliente %d", i);
        botonCredencial(etiqueta, USUARIOS_DEMO[i], inputCorreo, inputPassword);
    }

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
        const UsuarioDemo* usuario = buscarUsuario(correo, password);

        if (usuario != nullptr)
        {
            g_estaAutenticado = true;
            g_rolUsuarioActual = usuario->rol;
            g_correoUsuarioActual = usuario->correo;
            g_nombreUsuarioActual = usuario->nombre;
            mensajeError = "";
            std::cout << "[LOGIN] Sesion iniciada como " << usuario->nombre << ".\n";
        }
        else
        {
            mensajeError = "Correo o contrasena incorrectos.";
        }
    }
    ImGui::PopStyleColor(3);

    ImGui::Spacing();
    ImGui::TextDisabled("Admin: admin@cine.com | pass: 123456");
    ImGui::TextDisabled("Clientes: cliente1@cine.com, cliente2@cine.com, cliente3@cine.com");
    ImGui::TextDisabled("          cliente4@cine.com, cliente5@cine.com | pass: 123456");
    ImGui::End();
}
