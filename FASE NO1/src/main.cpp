#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "ui/PanelLogin.h"
#include <iostream>
#include <vector>

// (Asegúrate de que la extensión coincida con tus archivos, .hpp o .h)
#include "structures/ArbolBinario.h"
#include "structures/ListasPromociones.h"
#include "structures/MatrizDispersa.h"
#include "structures/ListaSolicitudes.h" // <-- FALTABA ESTE INCLUDE
#include "models/FuncionCine.h"
#include "ui/PanelAdmin.h"
#include "ui/PanelLogin.h"
#include "ui/PanelCliente.h"

// Función para manejar errores de GLFW
static void glfw_error_callback(int error, const char *description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

static void aplicarTemaCine()
{
    aplicarTemaCine();
    ImGuiStyle &style = ImGui::GetStyle();

    style.WindowPadding = ImVec2(18, 16);
    style.FramePadding = ImVec2(12, 7);
    style.ItemSpacing = ImVec2(10, 9);
    style.ItemInnerSpacing = ImVec2(8, 6);
    style.ScrollbarSize = 14.0f;
    style.WindowRounding = 8.0f;
    style.ChildRounding = 7.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 7.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabRounding = 6.0f;
    style.TabRounding = 6.0f;
    style.WindowBorderSize = 1.0f;

    ImVec4 *colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.92f, 0.95f, 0.96f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.55f, 0.62f, 0.66f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.09f, 0.11f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.13f, 0.16f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.10f, 0.12f, 0.98f);
    colors[ImGuiCol_Border] = ImVec4(0.20f, 0.27f, 0.31f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.16f, 0.19f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.17f, 0.23f, 0.27f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.29f, 0.33f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.05f, 0.07f, 0.09f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.13f, 0.16f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.12f, 0.42f, 0.48f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.15f, 0.54f, 0.62f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.10f, 0.34f, 0.40f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.13f, 0.33f, 0.38f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.16f, 0.45f, 0.52f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.12f, 0.38f, 0.45f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.16f, 0.19f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.16f, 0.48f, 0.55f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.12f, 0.36f, 0.42f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.12f, 0.26f, 0.30f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.11f, 0.15f, 0.18f, 1.00f);
}

int main()
{
    ListaCircularPromociones misPromociones;
    ArbolBinario miCartelera;
    std::vector<FuncionCine> misFunciones;
    ListaCircularDoble misSolicitudes;
    PanelAdmin panel;
    PanelCliente panelCliente;

    Promocion *promo1 = new Promocion(1, "Lunes Estudiantil", "Descuentos con carnet universitario");
    Promocion *promo2 = new Promocion(2, "Miercoles 2x1", "Lleva a un acompaniante gratis");
    misPromociones.insertarPromocion(promo1);
    misPromociones.insertarPromocion(promo2);

    Beneficio *b1 = new Beneficio(101, "Descuento en boleto", 50);
    Beneficio *b2 = new Beneficio(102, "Descuento en pipocas grandes", 20);
    Beneficio *b3 = new Beneficio(103, "Boleto extra gratis", 100);
    misPromociones.agregarBeneficioAPromocion(1, b1);
    misPromociones.agregarBeneficioAPromocion(1, b2);
    misPromociones.agregarBeneficioAPromocion(2, b3);

    // La cartelera inicia vacia para que el BST refleje exactamente el orden
    // de carga del administrador o del archivo CSV.

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "Panel Administrativo - Cine", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Activar V-Sync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Iniciar nuevo frame de ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // CONTROL DE ACCESO: LOGIN VS PANEL ADMINISTRATIVO / CLIENTE
        if (!g_estaAutenticado)
        {
            dibujarPanelLogin(); // Muestra la ventana de inicio de sesión
        }
        else
        {
            if (g_rolUsuarioActual == "admin")
            {
                // Muestra el panel administrativo si entra el admin
                panel.dibujar(miCartelera, misFunciones, misPromociones, misSolicitudes);
            }
            else if (g_rolUsuarioActual == "cliente")
            {
                panelCliente.dibujar(miCartelera, misFunciones, misPromociones, misSolicitudes);
                if (false) {
                // Aquí puedes colocar la vista destinada al usuario cliente simulado
                ImGui::Begin("Panel de Cliente", nullptr, ImGuiWindowFlags_NoCollapse);
                ImGui::Text("¡Bienvenido al sistema de cine, cliente!");
                if (ImGui::Button("Cerrar Sesión"))
                { // <-- Corregido con ::
                    g_estaAutenticado = false;
                }
                ImGui::End();
                }
            }
        }

        // Renderizado estándar de Dear ImGui y OpenGL
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.04f, 0.06f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
