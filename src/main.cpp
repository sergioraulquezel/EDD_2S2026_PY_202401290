#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "structures/ArbolBinario.h"
#include "structures/ListasPromociones.h"
#include "structures/MatrizDispersa.h"

// Función para manejar errores de GLFW
static void glfw_error_callback(int error, const char *description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

int main()
{

    ListaCircularPromociones cicloPromociones;

    Promocion *promo1 = new Promocion(1, "Lunes Estudiantil", "Descuentos con carnet universitario");
    Promocion *promo2 = new Promocion(2, "Miercoles 2x1", "Lleva a un acompaniante gratis");

    cicloPromociones.insertarPromocion(promo1);
    cicloPromociones.insertarPromocion(promo2);

    Beneficio *b1 = new Beneficio(101, "Descuento en boleto", 50);
    Beneficio *b2 = new Beneficio(102, "Descuento en pipocas grandes", 20);
    Beneficio *b3 = new Beneficio(103, "Boleto extra gratis", 100);

    cicloPromociones.agregarBeneficioAPromocion(1, b1);
    cicloPromociones.agregarBeneficioAPromocion(1, b2);
    cicloPromociones.agregarBeneficioAPromocion(2, b3);

    ArbolBinario miCartelera;

    Pelicula *p1 = new Pelicula(50, "Inception", "Nolan", 148, "Ciencia Ficcion");
    Pelicula *p2 = new Pelicula(25, "Interstellar", "Nolan", 169, "Ciencia Ficcion");
    Pelicula *p3 = new Pelicula(75, "Dune", "Villeneuve", 155, "Aventura");
    Pelicula *p4 = new Pelicula(10, "Matrix", "Wachowski", 136, "Acción");

    miCartelera.insertar(p1);
    miCartelera.insertar(p2);
    miCartelera.insertar(p3);
    miCartelera.insertar(p4);

    MatrizDispersa salaCine;

    salaCine.reservarAsiento(3, 5, "Juan Perez");
    salaCine.reservarAsiento(1, 2, "Maria Lopez");
    salaCine.reservarAsiento(3, 2, "Carlos Ruiz");

    miCartelera.generarReporteGraphviz();

    miCartelera.imprimirInorden();
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

    // Ciclo principal de renderizado (Render Loop)
    while (!glfwWindowShouldClose(window))
    {
        // Procesar eventos (teclado, ratón)
        glfwPollEvents();

        // Iniciar un nuevo frame de ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- AQUÍ CONSTRUIREMOS LA INTERFAZ ---

        // Ventana de prueba básica
        ImGui::Begin("Menú Principal");
        ImGui::Text("Bienvenido al Sistema de Gestión de Cine");
        if (ImGui::Button("Generar Reporte Graphviz"))
        {
            std::cout << "Aquí ejecutaremos el comando system(\"dot -Tpng...\")" << std::endl;
        }
        ImGui::End();

        // Renderizar todo
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        // Color de fondo de la ventana contenedora (Gris muy oscuro)
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    //Limpieza final al cerrar
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}