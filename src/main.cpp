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

int main()
{
    // 1. INSTANCIAR TODAS LAS ESTRUCTURAS (Una sola vez)
    ListaCircularPromociones misPromociones;
    ArbolBinario miCartelera;
    std::vector<FuncionCine> misFunciones;
    ListaCircularDoble misSolicitudes;
    PanelAdmin panel;
    PanelCliente panelCliente;

    // 2. INSERTAR DATOS DE PRUEBA
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

    

    Pelicula *p1 = new Pelicula(50, "Inception", "Nolan", 148, "Ciencia Ficcion", "24/08/2026");
    Pelicula *p2 = new Pelicula(25, "Interstellar", "Nolan", 169, "Ciencia Ficcion", "30/09/2026");
    Pelicula *p3 = new Pelicula(75, "Dune", "Villeneuve", 155, "Aventura", "15/10/2026");
    Pelicula *p4 = new Pelicula(10, "Matrix", "Wachowski", 136, "Accion", "20/08/2026");
    miCartelera.insertar(p1);
    miCartelera.insertar(p2);
    miCartelera.insertar(p3);
    miCartelera.insertar(p4);

    misFunciones.emplace_back(1, p1, "14:00", "Sala 1", 7, 10);
    misFunciones.emplace_back(2, p1, "20:00", "Sala 2", 8, 12);
    misFunciones.emplace_back(3, p2, "17:00", "Sala 1", 6, 8);
    misFunciones[0].asientos.reservarAsiento(3, 5, "Juan Perez");
    misFunciones[0].asientos.reservarAsiento(1, 2, "Maria Lopez");
    misFunciones[2].asientos.reservarAsiento(3, 2, "Carlos Ruiz");

    // Pruebas en consola (Opcional)
    miCartelera.generarReporteGraphviz();
    miCartelera.imprimirInorden();

    // 3. CONFIGURACIÓN DE GLFW Y DEAR IMGUI
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

    // 4. CICLO PRINCIPAL DE RENDERIZADO
    // 4. CICLO PRINCIPAL DE RENDERIZADO
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Iniciar nuevo frame de ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // =========================================================
        // CONTROL DE ACCESO: LOGIN VS PANEL ADMINISTRATIVO / CLIENTE
        // =========================================================
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

        // Renderizado estándar de Dear ImGui y OpenGL
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
    // 5. LIMPIEZA FINAL
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
