#include "ui/PanelLogin.h"
#include "utils/Validaciones.h"
#include "imgui.h"
#include "models/Cliente.h"
#include <cstdio>
#include <iostream>
#include <string>

bool g_estaAutenticado = false;
std::string g_rolUsuarioActual = "";
std::string g_correoUsuarioActual = "";
std::string g_nombreUsuarioActual = "";
std::string g_idClienteActual = "";

struct UsuarioDemo
{
    const char* correo;
    const char* password;
    const char* rol;
    const char* nombre;
};

static const UsuarioDemo USUARIOS_DEMO[] = {
    {"AdminCine@gmail.com", "admin123Pass", "admin", "Administrador"},
    {"cliente1@cine.com", "123456", "cliente", "Cliente 1"},
    {"cliente2@cine.com", "123456", "cliente", "Cliente 2"},
    {"cliente3@cine.com", "123456", "cliente", "Cliente 3"},
    {"cliente4@cine.com", "123456", "cliente", "Cliente 4"},
    {"cliente5@cine.com", "123456", "cliente", "Cliente 5"},
};

static void botonCredencial(const char* texto, const UsuarioDemo& usuario, char inputCorreo[], char inputPassword[])
{
    if (ImGui::Button(texto, ImVec2(155, 32)))
    {
        std::snprintf(inputCorreo, 128, "%s", usuario.correo);
        std::snprintf(inputPassword, 128, "%s", usuario.password);
    }
}

void dibujarPanelLogin(ArbolBClientes& clientes)
{
    static char inputCorreo[128] = "AdminCine@gmail.com";
    static char inputPassword[128] = "admin123Pass";
    static char regId[32] = "U006";
    static char regNombre[100] = "Cliente Nuevo";
    static char regCorreo[128] = "cliente6@cine.com";
    static char regTelefono[32] = "55550006";
    static char regPassword[128] = "123456";
    static std::string mensajeError = "";
    static std::string mensajeRegistro = "";

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
        if (correo == USUARIOS_DEMO[0].correo && password == USUARIOS_DEMO[0].password)
        {
            g_estaAutenticado = true;
            g_rolUsuarioActual = "admin";
            g_correoUsuarioActual = correo;
            g_nombreUsuarioActual = "Administrador";
            g_idClienteActual.clear();
            mensajeError = "";
            std::cout << "[LOGIN] Sesion iniciada como Administrador.\n";
        }
        else
        {
            Cliente* cliente = nullptr;
            if (clientes.autenticar(correo, password, cliente))
            {
                g_estaAutenticado = true;
                g_rolUsuarioActual = "cliente";
                g_correoUsuarioActual = cliente->correo;
                g_nombreUsuarioActual = cliente->nombre;
                g_idClienteActual = cliente->id;
                mensajeError = "";
                std::cout << "[LOGIN] Sesion iniciada como " << cliente->nombre << ".\n";
            }
            else
            {
                mensajeError = "Correo o contrasena incorrectos.";
            }
        }
    }
    ImGui::PopStyleColor(3);

    ImGui::Spacing();
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Registro de Cliente"))
    {
        ImGui::InputText("ID Cliente", regId, IM_ARRAYSIZE(regId));
        ImGui::InputText("Nombre", regNombre, IM_ARRAYSIZE(regNombre));
        ImGui::InputText("Correo registro", regCorreo, IM_ARRAYSIZE(regCorreo));
        ImGui::InputText("Telefono", regTelefono, IM_ARRAYSIZE(regTelefono));
        ImGui::InputText("Password registro", regPassword, IM_ARRAYSIZE(regPassword), ImGuiInputTextFlags_Password);

        if (ImGui::Button("Crear cuenta de cliente", ImVec2(-1, 36)))
        {
            if (campoVacio(regId) || campoVacio(regNombre) || campoVacio(regCorreo) ||
                campoVacio(regTelefono) || campoVacio(regPassword))
            {
                mensajeRegistro = "No se pudo registrar: todos los campos son obligatorios.";
            }
            else if (!correoValido(regCorreo))
            {
                mensajeRegistro = "No se pudo registrar: correo invalido.";
            }
            else if (clientes.buscarPorId(regId) != nullptr)
            {
                mensajeRegistro = "No se pudo registrar: ID duplicado.";
            }
            else if (clientes.buscarPorCorreo(regCorreo) != nullptr)
            {
                mensajeRegistro = "No se pudo registrar: correo duplicado.";
            }
            else
            {
                Cliente nuevo(regId, regNombre, regCorreo, regTelefono, regPassword);
                if (clientes.insertar(nuevo))
                {
                    mensajeRegistro = "Cliente registrado en Arbol B: " + nuevo.id;
                    mensajeError = "";
                }
                else
                {
                    mensajeRegistro = "No se pudo registrar: revise los datos ingresados.";
                }
            }
        }

        if (!mensajeRegistro.empty())
        {
            ImGui::TextWrapped("%s", mensajeRegistro.c_str());
        }
    }

    ImGui::Spacing();
    ImGui::TextDisabled("Admin: AdminCine@gmail.com | pass: admin123Pass");
    ImGui::TextDisabled("Clientes: cliente1@cine.com, cliente2@cine.com, cliente3@cine.com");
    ImGui::TextDisabled("          cliente4@cine.com, cliente5@cine.com | pass: 123456");
    ImGui::End();
}
