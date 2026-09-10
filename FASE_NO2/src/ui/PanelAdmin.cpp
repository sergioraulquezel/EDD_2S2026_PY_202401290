#include "ui/PanelAdmin.h"
#include "ui/PanelLogin.h"
#include "utils/CargaJSONFase2.h"
#include "imgui.h"
#include "models/Cliente.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

PanelAdmin::PanelAdmin() {}

static std::string generarCodigoFuncionAdmin(int id)
{
    char buffer[16];
    std::snprintf(buffer, sizeof(buffer), "F%03d", id);
    return buffer;
}

static void crearArchivoAsientosAdmin(const std::string& archivoAsientos, const std::string& codigoFuncion)
{
    std::ofstream archivo(archivoAsientos);
    if (!archivo.is_open()) {
        std::cout << "[ERROR] No se pudo crear archivo de asientos: " << archivoAsientos << "\n";
        return;
    }

    archivo << "{\n";
    archivo << "  \"codigo_funcion\": \"" << codigoFuncion << "\",\n";
    archivo << "  \"asientos_ocupados\": []\n";
    archivo << "}\n";
}

void PanelAdmin::dibujar(ArbolBinario &cartelera, std::vector<FuncionCine> &funciones, ArbolAVLFunciones &arbolFunciones,
                         ListaCircularPromociones &promociones, ListaCircularDoble &solicitudes,
                         ArbolBClientes& clientes, TablaHashReservas& reservas)
{
    static char inputHorario[32] = "14:00";
    static char inputSala[32] = "Sala 1";
    static int inputFilas = 10;
    static int inputColumnas = 20;
    static Pelicula *peliculaSeleccionadaParaFuncion = nullptr;
    static int siguienteIdFuncion = 1;
    static int funcionSeleccionadaIndex = -1;

    auto obtenerFuncionActual = [&]() -> FuncionCine* {
        if (funcionSeleccionadaIndex >= 0 && funcionSeleccionadaIndex < (int)funciones.size()) {
            return &funciones[funcionSeleccionadaIndex];
        }
        return nullptr;
    };

    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(16, 16), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - 32, io.DisplaySize.y - 32), ImGuiCond_Always);
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar;
    ImGui::Begin("Panel Administrativo - Cine", nullptr, windowFlags);

    if (ImGui::BeginMenuBar())
    {
        ImGui::Text("Cinema EDD | Administrador");
        ImGui::EndMenuBar();
    }

    ImGui::TextColored(ImVec4(0.34f, 0.86f, 0.92f, 1.0f), "Gestion del sistema de cine");
    ImGui::TextDisabled("Cartelera, funciones, promociones, solicitudes y reportes");
    ImGui::Separator();

    if (ImGui::BeginTabBar("MenuPrincipal"))
    {
        // =========================================================
        // PESTAÑA 1: CARTELERA (Formulario + Reporte + Tabla unificados)
        // =========================================================
        if (ImGui::BeginTabItem("Cartelera"))
        {
            ImGui::Spacing();
            ImGui::Text("Registrar Nueva Pelicula en el Arbol (BST):");
            ImGui::Separator();

            static int inputIdNum = 1;
            static char inputId[50] = "CO-001";
            static char inputTitulo[100] = "";
            static char inputGenero[50] = "";
            static int inputDuracion = 120;
            static char inputClasificacion[50] = "";
            static char inputIdioma[50] = "";
            static char inputEstreno[50] = "";
            static char inputFin[50] = "";
            static char rutaCSV[260] = "C:\\Users\\EQUIPO\\Downloads\\";
            static char archivoCSV[120] = "peliculas_carga_masiva_40.csv";
            static char archivoJSONFase2[120] = "peliculas_funciones.json";
            static std::string mensajeCargaCSV = "";
            static std::string mensajeCargaJSON = "";

            ImGui::TextColored(ImVec4(0.34f, 0.86f, 0.92f, 1.0f), "Carga Masiva Fase 2 desde JSON:");
            ImGui::InputText("Carpeta JSON", rutaCSV, IM_ARRAYSIZE(rutaCSV));
            ImGui::InputText("Archivo JSON Fase 2", archivoJSONFase2, IM_ARRAYSIZE(archivoJSONFase2));
            if (ImGui::Button("Cargar JSON Peliculas y Funciones", ImVec2(290, 30)))
            {
                std::string rutaJSON = rutaCSV;
                if (!rutaJSON.empty() && rutaJSON.back() != '\\' && rutaJSON.back() != '/')
                {
                    rutaJSON += "\\";
                }
                rutaJSON += archivoJSONFase2;

                int totalCargado = cargarPeliculasYFuncionesJSON(rutaJSON, cartelera, funciones, arbolFunciones);
                mensajeCargaJSON = "JSON procesado: " + std::to_string(totalCargado) +
                                   " registros agregados | Funciones en memoria: " + std::to_string(funciones.size()) +
                                   " | Ruta: " + rutaJSON;

                cartelera.generarReporteGraphviz();
                arbolFunciones.generarReporteGraphviz();
            }
            ImGui::SameLine();
            if (ImGui::Button("Reporte AVL Funciones", ImVec2(190, 30)))
            {
                arbolFunciones.generarReporteGraphviz();
            }
            if (!mensajeCargaJSON.empty())
            {
                ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.3f, 1.0f), "%s", mensajeCargaJSON.c_str());
            }
            ImGui::TextDisabled("JSON esperado en C:\\Users\\EQUIPO\\Downloads\\ o en la carpeta que indiques.");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Registro manual de pelicula:");

            ImGui::InputInt("ID Numérico (para ordenar BST)", &inputIdNum);
            ImGui::InputText("Código Película", inputId, IM_ARRAYSIZE(inputId));
            ImGui::InputText("Título", inputTitulo, IM_ARRAYSIZE(inputTitulo));
            ImGui::InputText("Género", inputGenero, IM_ARRAYSIZE(inputGenero));
            ImGui::InputInt("Duración (min)", &inputDuracion);
            ImGui::InputText("Clasificación", inputClasificacion, IM_ARRAYSIZE(inputClasificacion));
            ImGui::InputText("Idioma", inputIdioma, IM_ARRAYSIZE(inputIdioma));
            ImGui::InputText("Fecha Estreno", inputEstreno, IM_ARRAYSIZE(inputEstreno));
            ImGui::InputText("Fecha Fin", inputFin, IM_ARRAYSIZE(inputFin));

            if (ImGui::Button("Guardar Pelicula en Cartelera", ImVec2(220, 30)))
            {
                Pelicula *nuevaPelicula = new Pelicula(inputIdNum, inputTitulo, inputGenero, inputDuracion, inputClasificacion, inputFin, inputId, inputIdioma, inputEstreno);
                cartelera.insertar(nuevaPelicula);
                std::cout << "Película " << inputTitulo << " agregada al BST con éxito.\n";
            }

            ImGui::SameLine();
            if (ImGui::Button("Generar Reporte Graphviz (BST)", ImVec2(220, 30)))
            {
                cartelera.generarReporteGraphviz();
            }

            ImGui::SameLine();
            if (ImGui::Button("Verificar BST", ImVec2(140, 30)))
            {
                bool correcto = cartelera.validarBST();
                std::cout << "[BST] El arbol " << (correcto ? "cumple" : "NO cumple")
                          << " la regla: izquierda < raiz < derecha.\n";
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Carga Masiva de Peliculas desde CSV:");
            ImGui::InputText("Carpeta CSV", rutaCSV, IM_ARRAYSIZE(rutaCSV));
            ImGui::InputText("Archivo CSV", archivoCSV, IM_ARRAYSIZE(archivoCSV));
            if (ImGui::Button("Cargar CSV a Cartelera", ImVec2(220, 30)))
            {
                std::string rutaCompleta = rutaCSV;
                if (!rutaCompleta.empty() && rutaCompleta.back() != '\\' && rutaCompleta.back() != '/')
                {
                    rutaCompleta += "\\";
                }
                rutaCompleta += archivoCSV;

                int cargadas = cartelera.cargarPeliculasCSV(rutaCompleta);

                Pelicula *peliculasActuales[100];
                int totalActual = 0;
                cartelera.obtenerPeliculas(peliculasActuales, totalActual);

                mensajeCargaCSV = "CSV cargado: " + std::to_string(cargadas) +
                                  " nuevas | Total en BST: " + std::to_string(totalActual) +
                                  " | Ruta: " + rutaCompleta;

                if (totalActual > 0)
                {
                    cartelera.generarReporteGraphviz();
                }
            }
            if (!mensajeCargaCSV.empty())
            {
                ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.3f, 1.0f), "%s", mensajeCargaCSV.c_str());
            }
            ImGui::TextDisabled("Formato: codigo,titulo,genero,duracion,clasificacion,idioma,fecha_estreno,fecha_fin");

            ImGui::Spacing();
            ImGui::Separator();
            Pelicula *arregloPeliculas[100];
            int cantidadPeliculas = 0;
            cartelera.obtenerPeliculas(arregloPeliculas, cantidadPeliculas);

            ImGui::Text("Visualización de cartelera");
            ImGui::TextDisabled("Total actual en BST: %d peliculas", cantidadPeliculas);
            ImGui::Separator();

            ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit;

            if (ImGui::BeginTable("TablaCartelera", 8, flags))
            {
                ImGui::TableSetupColumn("CO-PELICULA");
                ImGui::TableSetupColumn("TITULO");
                ImGui::TableSetupColumn("GENERO");
                ImGui::TableSetupColumn("DURACION");
                ImGui::TableSetupColumn("CLASIFICACION");
                ImGui::TableSetupColumn("IDIOMA");
                ImGui::TableSetupColumn("FECHA ESTRENO");
                ImGui::TableSetupColumn("FECHA FIN");
                ImGui::TableHeadersRow();

                for (int i = 0; i < cantidadPeliculas; i++)
                {
                    Pelicula *p = arregloPeliculas[i];
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", p->getCodigo().c_str());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", p->getTitulo().c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%s", p->getGenero().c_str());

                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%d min", p->getDuracion());

                    ImGui::TableSetColumnIndex(4);
                    ImGui::Text("%s", p->getClasificacion().c_str());

                    ImGui::TableSetColumnIndex(5);
                    ImGui::Text("%s", p->getIdioma().empty() ? "Sin idioma" : p->getIdioma().c_str());
                    ImGui::TableSetColumnIndex(6);
                    ImGui::Text("%s", p->getFechaEstreno().empty() ? "Sin fecha" : p->getFechaEstreno().c_str());
                    ImGui::TableSetColumnIndex(7);
                    ImGui::Text("%s", p->getFechaFin().empty() ? "Sin fecha" : p->getFechaFin().c_str());
                }

                if (cantidadPeliculas == 0)
                {
                    for (int i = 0; i < 4; i++)
                    {
                        ImGui::TableNextRow();
                        for (int c = 0; c < 8; c++)
                        {
                            ImGui::TableSetColumnIndex(c);
                            ImGui::Text(" ");
                        }
                    }
                }

                ImGui::EndTable();
            }

            ImGui::EndTabItem();
        }

        // =========================================================
        // PESTAÑA 2: GESTIONAR FUNCIONES Y MAPA DE ASIENTOS (MATRIZ)
        // =========================================================
        if (ImGui::BeginTabItem("Gestionar Funciones y Sala"))
        {
            if (ImGui::CollapsingHeader("Configuración de Función y Sala", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("1. Seleccionar Película de la Cartelera:");

                Pelicula *listaPeliculasArbol[100];
                int totalPeliculas = 0;
                // CORRECCIÓN: Usar la instancia 'cartelera'
                cartelera.obtenerPeliculas(listaPeliculasArbol, totalPeliculas);

                if (totalPeliculas == 0)
                {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Aviso: No hay películas registradas en el árbol.");
                }
                else
                {
                    if (ImGui::BeginCombo("Película", peliculaSeleccionadaParaFuncion ? peliculaSeleccionadaParaFuncion->getTitulo().c_str() : "Seleccione una película"))
                    {
                        for (int i = 0; i < totalPeliculas; i++)
                        {
                            bool isSelected = (peliculaSeleccionadaParaFuncion == listaPeliculasArbol[i]);
                            std::string etiqueta = "ID: " + std::to_string(listaPeliculasArbol[i]->getId()) + " - " + listaPeliculasArbol[i]->getTitulo();

                            if (ImGui::Selectable(etiqueta.c_str(), isSelected))
                            {
                                peliculaSeleccionadaParaFuncion = listaPeliculasArbol[i];
                            }
                            if (isSelected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                }

                ImGui::Separator();
                ImGui::Text("2. Configurar Horario y Sala:");
                ImGui::InputText("Horario (ej. 14:00)", inputHorario, IM_ARRAYSIZE(inputHorario));
                ImGui::InputText("Sala (ej. Sala 1)", inputSala, IM_ARRAYSIZE(inputSala));

                ImGui::Separator();
                ImGui::Text("3. Definir Dimensiones de la Sala:");
                ImGui::SliderInt("Filas", &inputFilas, 1, 30);
                ImGui::SliderInt("Columnas / Asientos", &inputColumnas, 1, 30);

                if (ImGui::Button("Crear / Sobrescribir Función y Asientos", ImVec2(300, 30)))
                {
                    if (peliculaSeleccionadaParaFuncion != nullptr)
                    {
                        int idFuncion = siguienteIdFuncion++;
                        std::string codigoFuncion = generarCodigoFuncionAdmin(idFuncion);
                        std::string archivoAsientos = codigoFuncion + "_funcion.json";

                        funciones.emplace_back(idFuncion, codigoFuncion, peliculaSeleccionadaParaFuncion, inputHorario, inputSala, inputFilas, inputColumnas, archivoAsientos);
                        arbolFunciones.insertar(funciones.back());
                        crearArchivoAsientosAdmin(archivoAsientos, codigoFuncion);
                        funcionSeleccionadaIndex = (int)funciones.size() - 1;

                        std::cout << "[INFO] Función creada exitosamente para: "
                                  << peliculaSeleccionadaParaFuncion->getTitulo()
                                  << " a las " << inputHorario << " en " << inputSala
                                  << " (" << inputFilas << "x" << inputColumnas << ")\n";
                    }
                    else
                    {
                        std::cout << "[ERROR] Debe seleccionar una película primero.\n";
                    }
                }

                ImGui::Separator();
                ImGui::Text("4. Funciones creadas:");
                if (funciones.empty())
                {
                    ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "Todavia no hay funciones creadas.");
                }
                else
                {
                    std::string funcionActualLabel = obtenerFuncionActual() ? obtenerFuncionActual()->obtenerEtiqueta() : "Seleccione una funcion";
                    if (ImGui::BeginCombo("Funcion activa", funcionActualLabel.c_str()))
                    {
                        for (int i = 0; i < (int)funciones.size(); i++)
                        {
                            bool isSelected = funcionSeleccionadaIndex == i;
                            std::string label = "#" + std::to_string(funciones[i].id) + " - " + funciones[i].obtenerEtiqueta() +
                                                " - " + std::to_string(funciones[i].asientos.obtenerFilas()) + "x" +
                                                std::to_string(funciones[i].asientos.obtenerColumnas());
                            if (ImGui::Selectable(label.c_str(), isSelected))
                            {
                                funcionSeleccionadaIndex = i;
                            }
                            if (isSelected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                }
            }

            ImGui::Spacing();
            if (ImGui::CollapsingHeader("Simulación y Reporte de Asientos"))
            {
                static int fReserva = 0;
                static int cReserva = 0;
                static char clienteReserva[100] = "";

                ImGui::InputInt("Fila Asiento", &fReserva);
                ImGui::InputInt("Columna Asiento", &cReserva);
                ImGui::InputText("Cliente", clienteReserva, IM_ARRAYSIZE(clienteReserva));

                if (ImGui::Button("Reservar Asiento", ImVec2(150, 30)))
                {
                    FuncionCine* funcionActual = obtenerFuncionActual();
                    if (funcionActual != nullptr)
                    {
                        funcionActual->asientos.reservarAsiento(fReserva, cReserva, clienteReserva);
                    }
                    else
                    {
                        std::cout << "[ERROR] Debe seleccionar una funcion primero.\n";
                    }
                }

                ImGui::SameLine();
                if (ImGui::Button("Cancelar Reserva", ImVec2(150, 30)))
                {
                    FuncionCine* funcionActual = obtenerFuncionActual();
                    if (funcionActual != nullptr)
                    {
                        funcionActual->asientos.cancelarReserva(fReserva, cReserva);
                    }
                    else
                    {
                        std::cout << "[ERROR] Debe seleccionar una funcion primero.\n";
                    }
                }

                ImGui::Spacing();
                if (ImGui::Button("Generar Reporte Graphviz (Matriz de Asientos)", ImVec2(300, 30)))
                {
                    FuncionCine* funcionActual = obtenerFuncionActual();
                    if (funcionActual != nullptr)
                    {
                        std::string tituloReporte = "Funcion: " + funcionActual->pelicula->getTitulo() +
                                                    " - " + funcionActual->horario +
                                                    " - " + funcionActual->sala;
                        funcionActual->asientos.generarReporteGraphviz(tituloReporte);
                    }
                    else
                    {
                        std::cout << "[ERROR] Debe seleccionar una funcion primero.\n";
                    }
                }
            }

            // ==========================================
            // VISUALIZAR MAPA DE OCUPACIÓN ACTUAL
            // ==========================================
            ImGui::Spacing();
            if (ImGui::CollapsingHeader("Mapa de Ocupación de la Función Actual"))
            {
                FuncionCine* funcionActual = obtenerFuncionActual();
                if (funcionActual != nullptr)
                {
                    ImGui::Text("Pelicula Activa: %s", funcionActual->pelicula->getTitulo().c_str());
                    ImGui::Text("Horario: %s | Sala: %s", funcionActual->horario.c_str(), funcionActual->sala.c_str());
                    int reservados = funcionActual->asientos.contarReservados();
                    int capacidad = funcionActual->asientos.obtenerFilas() * funcionActual->asientos.obtenerColumnas();
                    ImGui::Text("Dimensiones: %d filas x %d columnas", funcionActual->asientos.obtenerFilas(), funcionActual->asientos.obtenerColumnas());
                    ImGui::Text("Reservados: %d | Libres: %d", reservados, capacidad - reservados);

                    if (funcionActual->asientos.estaConfigurada())
                    {
                        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.3f, 1.0f), "[ VERDE = Libre ]");
                        ImGui::SameLine();
                        ImGui::TextColored(ImVec4(0.9f, 0.2f, 0.2f, 1.0f), "[ ROJO = Ocupado ]");
                        ImGui::Spacing();

                        for (int f = 1; f <= funcionActual->asientos.obtenerFilas(); f++)
                        {
                            ImGui::Text("Fila %d:", f);
                            ImGui::SameLine();

                            for (int c = 1; c <= funcionActual->asientos.obtenerColumnas(); c++)
                            {
                                NodoMatriz* asiento = funcionActual->asientos.obtenerAsiento(f, c);
                                bool ocupado = asiento != nullptr;

                                if (ocupado)
                                {
                                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.85f, 0.15f, 0.15f, 1.0f));
                                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.25f, 0.25f, 1.0f));
                                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
                                }
                                else
                                {
                                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.75f, 0.3f, 1.0f));
                                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.85f, 0.4f, 1.0f));
                                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.55f, 0.2f, 1.0f));
                                }

                                std::string idAsiento = "A##admin_" + std::to_string(f) + "_" + std::to_string(c);
                                if (ImGui::Button(idAsiento.c_str(), ImVec2(32, 28)) && ocupado)
                                {
                                    funcionActual->asientos.cancelarReserva(f, c);
                                }

                                if (ocupado && ImGui::IsItemHovered())
                                {
                                    ImGui::SetTooltip("Reservado por: %s", asiento->titular.c_str());
                                }

                                ImGui::PopStyleColor(3);
                                if (c < funcionActual->asientos.obtenerColumnas()) ImGui::SameLine();
                            }
                        }
                    }
                }
                else
                {
                    ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "No hay ninguna función activa actualmente.");
                }
            }

            ImGui::EndTabItem();
        }

        // =========================================================
        // PESTAÑA 3: PROMOCIONES (LISTA CIRCULAR DE LISTAS)
        // =========================================================
        if (ImGui::BeginTabItem("Promociones"))
        {
            ImGui::Spacing();
            ImGui::Text("Registrar Promocion:");
            ImGui::Separator();

            static int promoId = 3;
            static char promoNombre[100] = "Martes de Locura";
            static char promoDescripcion[150] = "Descuento especial en entradas y combos";

            ImGui::InputInt("ID de promocion", &promoId);
            ImGui::InputText("Nombre de la promoción", promoNombre, IM_ARRAYSIZE(promoNombre));
            ImGui::InputText("Descripcion", promoDescripcion, IM_ARRAYSIZE(promoDescripcion));

            ImGui::Spacing();

            if (ImGui::Button("Agregar Una Promocion", ImVec2(180, 30)))
            {
                promociones.insertarPromocion(new Promocion(promoId, promoNombre, promoDescripcion));
                promoId++;
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Añadir Beneficio a una Promoción:");

            static int promoTargetId = 1;
            static int beneficioId = 200;
            static char beneficioDesc[100] = "papas medianas";
            static int beneficioDescuento = 10;

            std::string promoDestinoLabel = "Seleccione una promocion";
            NodoPromocion* promoDestinoActual = promociones.buscarPromocion(promoTargetId);
            if (promoDestinoActual != nullptr)
            {
                promoDestinoLabel = "Promo " + std::to_string(promoDestinoActual->promocion->idPromocion) + " - " + promoDestinoActual->promocion->nombre;
            }

            if (ImGui::BeginCombo("Promocion destino", promoDestinoLabel.c_str()))
            {
                if (promociones.cabeza != nullptr)
                {
                    NodoPromocion* actual = promociones.cabeza;
                    do
                    {
                        bool isSelected = promoTargetId == actual->promocion->idPromocion;
                        std::string label = "Promo " + std::to_string(actual->promocion->idPromocion) + " - " + actual->promocion->nombre;
                        if (ImGui::Selectable(label.c_str(), isSelected))
                        {
                            promoTargetId = actual->promocion->idPromocion;
                        }
                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                        actual = actual->siguiente;
                    } while (actual != promociones.cabeza);
                }
                else
                {
                    ImGui::TextDisabled("No hay promociones registradas.");
                }
                ImGui::EndCombo();
            }

            ImGui::InputInt("ID Promo Destino", &promoTargetId);
            ImGui::InputInt("ID Beneficio", &beneficioId);
            ImGui::InputText("Descripción del Beneficio", beneficioDesc, IM_ARRAYSIZE(beneficioDesc));
            ImGui::SliderInt("Porcentaje de descuento", &beneficioDescuento, 0, 100);

            if (ImGui::Button("Añadir Beneficio", ImVec2(150, 30)))
            {
                if (promociones.agregarBeneficioAPromocion(promoTargetId, new Beneficio(beneficioId, beneficioDesc, beneficioDescuento)))
                {
                    beneficioId++;
                }
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Estructura en Memoria (Lista Circular -> Sublistas):");
            ImGui::Spacing();

            ImGui::Text("Total promociones: %d", promociones.contarPromociones());
            if (promociones.cabeza == nullptr)
            {
                ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "No hay promociones registradas.");
            }
            else
            {
                NodoPromocion* actual = promociones.cabeza;
                do
                {
                    ImGui::Separator();
                    ImGui::Text("Promo %d - %s", actual->promocion->idPromocion, actual->promocion->nombre.c_str());
                    ImGui::TextWrapped("%s", actual->promocion->descripcion.c_str());
                    ImGui::Text("Beneficios: %d", promociones.contarBeneficios(actual));

                    NodoBeneficio* beneficio = actual->listaBeneficios->cabeza;
                    while (beneficio != nullptr)
                    {
                        ImGui::BulletText("ID %d | %s | %d%%",
                                          beneficio->beneficio->idBeneficio,
                                          beneficio->beneficio->descripcion.c_str(),
                                          beneficio->beneficio->porcentajeDescuento);
                        beneficio = beneficio->siguiente;
                    }

                    actual = actual->siguiente;
                } while (actual != promociones.cabeza);
            }

            ImGui::Spacing();
            if (ImGui::Button("Generar Reporte Graphviz (Promociones)", ImVec2(300, 30)))
            {
                promociones.generarReporteGraphviz();
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Solicitudes Especiales"))
        {
            ImGui::Spacing();
            ImGui::Text("Gestionar Solicitudes Especiales de Clientes");
            ImGui::Separator();

            static int solicitudSeleccionadaId = 0;
            ImGui::Text("Total: %d | Pendientes: %d | Aprobadas: %d | Rechazadas: %d",
                        solicitudes.contarSolicitudes(),
                        solicitudes.contarPorEstado("Pendiente"),
                        solicitudes.contarPorEstado("Aprobada"),
                        solicitudes.contarPorEstado("Rechazada"));

            std::string solicitudLabel = "Seleccione una solicitud";
            NodoSolicitud* solicitudActual = solicitudes.buscarSolicitud(solicitudSeleccionadaId);
            if (solicitudActual != nullptr)
            {
                solicitudLabel = "#" + std::to_string(solicitudActual->solicitud->idSolicitud) + " - " +
                                 solicitudActual->solicitud->nombreCliente + " - " +
                                 solicitudActual->solicitud->estado;
            }

            if (ImGui::BeginCombo("Solicitud activa", solicitudLabel.c_str()))
            {
                if (solicitudes.cabeza != nullptr)
                {
                    NodoSolicitud* actual = solicitudes.cabeza;
                    do
                    {
                        bool isSelected = solicitudSeleccionadaId == actual->solicitud->idSolicitud;
                        std::string label = "#" + std::to_string(actual->solicitud->idSolicitud) + " - " +
                                            actual->solicitud->nombreCliente + " - " + actual->solicitud->estado;
                        if (ImGui::Selectable(label.c_str(), isSelected))
                        {
                            solicitudSeleccionadaId = actual->solicitud->idSolicitud;
                        }
                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                        actual = actual->siguiente;
                    } while (actual != solicitudes.cabeza);
                }
                else
                {
                    ImGui::TextDisabled("No hay solicitudes registradas.");
                }
                ImGui::EndCombo();
            }

            if (solicitudActual != nullptr)
            {
                ImGui::Spacing();
                ImGui::Text("Cliente: %s", solicitudActual->solicitud->nombreCliente.c_str());
                ImGui::TextWrapped("Evento: %s", solicitudActual->solicitud->descripcionEvento.c_str());
                ImGui::Text("Estado actual: %s", solicitudActual->solicitud->estado.c_str());

                if (ImGui::Button("Aprobar", ImVec2(110, 30)))
                {
                    solicitudes.cambiarEstadoSolicitud(solicitudSeleccionadaId, "Aprobada");
                }
                ImGui::SameLine();
                if (ImGui::Button("Rechazar", ImVec2(110, 30)))
                {
                    solicitudes.cambiarEstadoSolicitud(solicitudSeleccionadaId, "Rechazada");
                }
                ImGui::SameLine();
                if (ImGui::Button("Pendiente", ImVec2(110, 30)))
                {
                    solicitudes.cambiarEstadoSolicitud(solicitudSeleccionadaId, "Pendiente");
                }
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Listado general");

            if (solicitudes.cabeza == nullptr)
            {
                ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "No hay solicitudes registradas.");
            }
            else
            {
                NodoSolicitud* actual = solicitudes.cabeza;
                do
                {
                    ImGui::Separator();
                    ImGui::Text("Solicitud #%d | %s | %s",
                                actual->solicitud->idSolicitud,
                                actual->solicitud->nombreCliente.c_str(),
                                actual->solicitud->estado.c_str());
                    ImGui::TextWrapped("%s", actual->solicitud->descripcionEvento.c_str());
                    actual = actual->siguiente;
                } while (actual != solicitudes.cabeza);
            }

            ImGui::Spacing();
            if (ImGui::Button("Generar Reporte Graphviz (Solicitudes)", ImVec2(320, 30)))
            {
                solicitudes.generarReporteGraphviz();
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Clientes y Reservas"))
        {
            ImGui::Spacing();
            ImGui::Text("Arbol B de orden 4: Clientes");
            ImGui::Separator();

            static char nuevoId[32] = "U006";
            static char nuevoNombre[100] = "Cliente Nuevo";
            static char nuevoCorreo[128] = "cliente6@cine.com";
            static char nuevoTelefono[32] = "55550006";
            static char nuevoPassword[128] = "123456";
            static char buscarClienteId[32] = "U001";
            static std::string mensajeClientes = "";

            ImGui::InputText("ID", nuevoId, IM_ARRAYSIZE(nuevoId));
            ImGui::InputText("Nombre Cliente", nuevoNombre, IM_ARRAYSIZE(nuevoNombre));
            ImGui::InputText("Correo Cliente", nuevoCorreo, IM_ARRAYSIZE(nuevoCorreo));
            ImGui::InputText("Telefono Cliente", nuevoTelefono, IM_ARRAYSIZE(nuevoTelefono));
            ImGui::InputText("Password Cliente", nuevoPassword, IM_ARRAYSIZE(nuevoPassword));

            if (ImGui::Button("Agregar Cliente al Arbol B", ImVec2(240, 30)))
            {
                Cliente nuevo(nuevoId, nuevoNombre, nuevoCorreo, nuevoTelefono, nuevoPassword);
                mensajeClientes = clientes.insertar(nuevo)
                    ? "Cliente agregado al Arbol B: " + nuevo.id
                    : "No se agrego el cliente. Revise ID/correo duplicado o campos vacios.";
            }
            ImGui::SameLine();
            if (ImGui::Button("Reporte Arbol B Clientes", ImVec2(220, 30)))
            {
                clientes.generarReporteGraphviz();
                mensajeClientes = "Reporte generado: reporte_arbol_b_clientes.png";
            }
            ImGui::SameLine();
            if (ImGui::Button("Reporte Hash Reservas", ImVec2(210, 30)))
            {
                reservas.generarReporteGraphviz();
                mensajeClientes = "Reporte generado: reporte_tabla_hash_reservas.png";
            }

            if (!mensajeClientes.empty())
            {
                ImGui::TextWrapped("%s", mensajeClientes.c_str());
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Clientes registrados: %d | Reservas en Hash: %d", clientes.contar(), reservas.contar());

            std::vector<Cliente> listaClientes = clientes.listarOrdenado();
            if (ImGui::BeginTable("TablaClientesB", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            {
                ImGui::TableSetupColumn("ID");
                ImGui::TableSetupColumn("Nombre");
                ImGui::TableSetupColumn("Correo");
                ImGui::TableSetupColumn("Telefono");
                ImGui::TableSetupColumn("Tipo");
                ImGui::TableSetupColumn("Reservas");
                ImGui::TableHeadersRow();

                for (const Cliente& cliente : listaClientes)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("%s", cliente.id.c_str());
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%s", cliente.nombre.c_str());
                    ImGui::TableSetColumnIndex(2); ImGui::Text("%s", cliente.correo.c_str());
                    ImGui::TableSetColumnIndex(3); ImGui::Text("%s", cliente.telefono.c_str());
                    ImGui::TableSetColumnIndex(4); ImGui::Text("%s", cliente.tipo.c_str());
                    ImGui::TableSetColumnIndex(5); ImGui::Text("%d", (int)cliente.codigosReserva.size());
                }
                ImGui::EndTable();
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Ver reservas por cliente");
            ImGui::InputText("ID Cliente a consultar", buscarClienteId, IM_ARRAYSIZE(buscarClienteId));

            Cliente* clienteEncontrado = clientes.buscarPorId(buscarClienteId);
            if (clienteEncontrado == nullptr)
            {
                ImGui::TextDisabled("No se encontro ese cliente en el Arbol B.");
            }
            else
            {
                ImGui::Text("Cliente: %s | %s", clienteEncontrado->nombre.c_str(), clienteEncontrado->correo.c_str());
                std::vector<Reserva> reservasCliente = reservas.listarPorCliente(clienteEncontrado->id);
                if (ImGui::BeginTable("TablaReservasClienteAdmin", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                {
                    ImGui::TableSetupColumn("Reserva");
                    ImGui::TableSetupColumn("Funcion");
                    ImGui::TableSetupColumn("Fecha");
                    ImGui::TableSetupColumn("Fila");
                    ImGui::TableSetupColumn("Columna");
                    ImGui::TableSetupColumn("Estado");
                    ImGui::TableHeadersRow();

                    for (const Reserva& reserva : reservasCliente)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0); ImGui::Text("%s", reserva.codigoReserva.c_str());
                        ImGui::TableSetColumnIndex(1); ImGui::Text("%s", reserva.codigoFuncion.c_str());
                        ImGui::TableSetColumnIndex(2); ImGui::Text("%s", reserva.fechaReserva.c_str());
                        ImGui::TableSetColumnIndex(3); ImGui::Text("%d", reserva.fila);
                        ImGui::TableSetColumnIndex(4); ImGui::Text("%d", reserva.columna);
                        ImGui::TableSetColumnIndex(5); ImGui::Text("%s", reserva.estado.c_str());
                    }
                    ImGui::EndTable();
                }
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    // =========================================================
    // BOTÓN DE CERRAR SESIÓN (AL FINAL DEL PANEL ADMIN)
    // =========================================================
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Cambiar color a un tono rojizo/anaranjado para destacar la acción de salir
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));

    if (ImGui::Button("Cerrar Sesión (Admin)", ImVec2(180, 35)))
    {
        g_estaAutenticado = false; // Cambia el estado para volver automáticamente al login
        g_rolUsuarioActual.clear();
        g_correoUsuarioActual.clear();
        g_nombreUsuarioActual.clear();
        std::cout << "[LOGIN] Sesión de Administrador cerrada.\n";
    }

    ImGui::PopStyleColor(2);

    ImGui::End();
}
