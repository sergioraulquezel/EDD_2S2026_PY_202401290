#include "ui/PanelCliente.h"
#include "imgui.h"
#include <iostream>
#include <string>

extern bool g_estaAutenticado;

PanelCliente::PanelCliente() {}

static void dibujarPromocionCliente(NodoPromocion* nodo)
{
    if (nodo == nullptr || nodo->promocion == nullptr) return;

    ImGui::Separator();
    ImGui::Text("Promo %d - %s", nodo->promocion->idPromocion, nodo->promocion->nombre.c_str());
    ImGui::TextWrapped("%s", nodo->promocion->descripcion.c_str());

    NodoBeneficio* beneficio = nodo->listaBeneficios->cabeza;
    if (beneficio == nullptr)
    {
        ImGui::TextDisabled("Sin beneficios registrados.");
    }
    while (beneficio != nullptr)
    {
        ImGui::BulletText("%s (%d%%)", beneficio->beneficio->descripcion.c_str(), beneficio->beneficio->porcentajeDescuento);
        beneficio = beneficio->siguiente;
    }
}

void PanelCliente::dibujar(ArbolBinario &cartelera, std::vector<FuncionCine> &funciones, ListaCircularPromociones &promociones, ListaCircularDoble &solicitudes)
{
    ImGui::Begin("PANEL DE USUARIO - RESERVAS", nullptr, ImGuiWindowFlags_NoCollapse);
    ImGui::SetWindowSize(ImVec2(860, 740), ImGuiCond_FirstUseEver);

    if (ImGui::BeginTabBar("MenuClienteTabs"))
    {
        if (ImGui::BeginTabItem("Consultar Cartelera"))
        {
            ImGui::Spacing();
            ImGui::Text("Cartelera de Peliculas Disponibles:");
            ImGui::Separator();

            Pelicula *listaPeliculas[100];
            int totalPeliculas = 0;
            cartelera.obtenerPeliculas(listaPeliculas, totalPeliculas);

            if (totalPeliculas == 0)
            {
                ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "No hay peliculas registradas.");
            }
            else
            {
                ImGui::Columns(2, "grid_cartelera", false);
                for (int i = 0; i < totalPeliculas; i++)
                {
                    Pelicula *p = listaPeliculas[i];
                    ImGui::BeginGroup();
                    ImGui::Button(p->getTitulo().c_str(), ImVec2(220, 130));
                    ImGui::Text("ID: CO-%d", p->getId());
                    ImGui::Text("Genero: %s", p->getGenero().c_str());
                    ImGui::Text("Duracion: %d min", p->getDuracion());
                    ImGui::Text("Clasificacion: %s", p->getClasificacion().c_str());
                    ImGui::EndGroup();
                    ImGui::NextColumn();
                }
                ImGui::Columns(1);
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Promociones"))
        {
            ImGui::Spacing();
            ImGui::Text("Promociones disponibles");
            ImGui::Separator();

            if (promociones.cabeza == nullptr)
            {
                ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "No hay promociones disponibles.");
            }
            else
            {
                NodoPromocion* actual = promociones.cabeza;
                do
                {
                    dibujarPromocionCliente(actual);
                    actual = actual->siguiente;
                } while (actual != promociones.cabeza);
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Solicitudes Especiales"))
        {
            ImGui::Spacing();
            ImGui::Text("Crear solicitud especial");
            ImGui::Separator();

            static int siguienteIdSolicitudCliente = 1;
            static char nombreCliente[100] = "ClienteActual";
            static char descripcionSolicitud[220] = "Celebracion de cumpleanos con decoracion especial";

            ImGui::InputText("Nombre del cliente", nombreCliente, IM_ARRAYSIZE(nombreCliente));
            ImGui::InputTextMultiline("Descripcion del evento", descripcionSolicitud, IM_ARRAYSIZE(descripcionSolicitud), ImVec2(520, 90));

            if (ImGui::Button("Enviar Solicitud", ImVec2(170, 30)))
            {
                solicitudes.insertarSolicitud(new Solicitud(siguienteIdSolicitudCliente++, nombreCliente, descripcionSolicitud));
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Mis solicitudes registradas");

            if (solicitudes.cabeza == nullptr)
            {
                ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "No hay solicitudes registradas.");
            }
            else
            {
                NodoSolicitud* actual = solicitudes.cabeza;
                do
                {
                    if (actual->solicitud->nombreCliente == nombreCliente)
                    {
                        ImGui::Separator();
                        ImGui::Text("Solicitud #%d", actual->solicitud->idSolicitud);
                        ImGui::TextWrapped("%s", actual->solicitud->descripcionEvento.c_str());
                        ImVec4 colorEstado = ImVec4(1.0f, 0.75f, 0.2f, 1.0f);
                        if (actual->solicitud->estado == "Aprobada") colorEstado = ImVec4(0.2f, 0.8f, 0.3f, 1.0f);
                        if (actual->solicitud->estado == "Rechazada") colorEstado = ImVec4(0.9f, 0.2f, 0.2f, 1.0f);
                        ImGui::TextColored(colorEstado, "Estado: %s", actual->solicitud->estado.c_str());
                    }
                    actual = actual->siguiente;
                } while (actual != solicitudes.cabeza);
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Realizar Reserva de Asientos"))
        {
            ImGui::Spacing();
            ImGui::Text("Seleccionar pelicula, funcion, promocion y asiento");
            ImGui::Separator();

            static Pelicula *peliculaSeleccionada = nullptr;
            static int funcionSeleccionadaIndex = -1;
            static int promocionSeleccionadaId = -1;

            Pelicula *listaPeliculas[100];
            int totalPeliculas = 0;
            cartelera.obtenerPeliculas(listaPeliculas, totalPeliculas);

            if (totalPeliculas > 0)
            {
                if (ImGui::BeginCombo("Pelicula", peliculaSeleccionada ? peliculaSeleccionada->getTitulo().c_str() : "Seleccione una pelicula"))
                {
                    for (int i = 0; i < totalPeliculas; i++)
                    {
                        bool isSelected = peliculaSeleccionada == listaPeliculas[i];
                        std::string label = "CO-" + std::to_string(listaPeliculas[i]->getId()) + " - " + listaPeliculas[i]->getTitulo();
                        if (ImGui::Selectable(label.c_str(), isSelected))
                        {
                            peliculaSeleccionada = listaPeliculas[i];
                            funcionSeleccionadaIndex = -1;
                        }
                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            }
            else
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Aviso: No hay peliculas disponibles para reservar.");
            }

            if (peliculaSeleccionada != nullptr)
            {
                std::string textoFuncion = "Seleccione una funcion";
                if (funcionSeleccionadaIndex >= 0 && funcionSeleccionadaIndex < (int)funciones.size())
                {
                    textoFuncion = funciones[funcionSeleccionadaIndex].obtenerEtiqueta();
                }

                if (ImGui::BeginCombo("Funcion / Horario / Sala", textoFuncion.c_str()))
                {
                    bool hayFunciones = false;
                    for (int i = 0; i < (int)funciones.size(); i++)
                    {
                        if (funciones[i].pelicula != peliculaSeleccionada) continue;

                        hayFunciones = true;
                        bool isSelected = funcionSeleccionadaIndex == i;
                        std::string label = funciones[i].horario + " | " + funciones[i].sala +
                                            " | " + std::to_string(funciones[i].asientos.obtenerFilas()) + "x" +
                                            std::to_string(funciones[i].asientos.obtenerColumnas());

                        if (ImGui::Selectable(label.c_str(), isSelected))
                        {
                            funcionSeleccionadaIndex = i;
                        }
                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }

                    if (!hayFunciones)
                    {
                        ImGui::TextDisabled("No hay funciones creadas para esta pelicula.");
                    }

                    ImGui::EndCombo();
                }
            }

            std::string textoPromo = "Sin promocion";
            NodoPromocion* promoSeleccionada = promociones.buscarPromocion(promocionSeleccionadaId);
            if (promoSeleccionada != nullptr)
            {
                textoPromo = promoSeleccionada->promocion->nombre;
            }

            if (ImGui::BeginCombo("Promocion opcional", textoPromo.c_str()))
            {
                bool sinPromo = promocionSeleccionadaId == -1;
                if (ImGui::Selectable("Sin promocion", sinPromo))
                {
                    promocionSeleccionadaId = -1;
                }
                if (sinPromo) ImGui::SetItemDefaultFocus();

                if (promociones.cabeza != nullptr)
                {
                    NodoPromocion* actual = promociones.cabeza;
                    do
                    {
                        bool isSelected = promocionSeleccionadaId == actual->promocion->idPromocion;
                        std::string label = "Promo " + std::to_string(actual->promocion->idPromocion) + " - " + actual->promocion->nombre;
                        if (ImGui::Selectable(label.c_str(), isSelected))
                        {
                            promocionSeleccionadaId = actual->promocion->idPromocion;
                        }
                        if (isSelected) ImGui::SetItemDefaultFocus();
                        actual = actual->siguiente;
                    } while (actual != promociones.cabeza);
                }
                ImGui::EndCombo();
            }

            ImGui::Spacing();
            ImGui::Separator();

            if (funcionSeleccionadaIndex < 0 || funcionSeleccionadaIndex >= (int)funciones.size())
            {
                ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "Seleccione una funcion creada por el administrador.");
                ImGui::EndTabItem();
                ImGui::EndTabBar();
                ImGui::Spacing();
                ImGui::Separator();
                if (ImGui::Button("Cerrar Sesion", ImVec2(120, 30)))
                {
                    g_estaAutenticado = false;
                }
                ImGui::End();
                return;
            }

            FuncionCine &funcion = funciones[funcionSeleccionadaIndex];
            MatrizDispersa &sala = funcion.asientos;

            ImGui::Text("Mapa de Asientos - %s", funcion.obtenerEtiqueta().c_str());
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.3f, 1.0f), "[ VERDE = Libre ]");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.9f, 0.2f, 0.2f, 1.0f), "[ ROJO = Ocupado ]");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), "[ Click azul = cancelar ]");
            ImGui::Spacing();

            for (int f = 1; f <= sala.obtenerFilas(); f++)
            {
                ImGui::Text("Fila %d: ", f);
                ImGui::SameLine();

                for (int c = 1; c <= sala.obtenerColumnas(); c++)
                {
                    NodoMatriz *nodoAsiento = sala.obtenerAsiento(f, c);
                    bool ocupado = nodoAsiento != nullptr;
                    bool propio = ocupado && nodoAsiento->titular == "ClienteActual";

                    if (ocupado)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, propio ? ImVec4(0.15f, 0.45f, 0.9f, 1.0f) : ImVec4(0.85f, 0.15f, 0.15f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, propio ? ImVec4(0.25f, 0.55f, 1.0f, 1.0f) : ImVec4(0.95f, 0.25f, 0.25f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, propio ? ImVec4(0.1f, 0.35f, 0.75f, 1.0f) : ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
                    }
                    else
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.75f, 0.3f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.85f, 0.4f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.55f, 0.2f, 1.0f));
                    }

                    std::string idAsiento = "[" + std::to_string(f) + "," + std::to_string(c) + "]";
                    if (ImGui::Button(idAsiento.c_str(), ImVec2(45, 30)))
                    {
                        if (!ocupado)
                        {
                            sala.reservarAsiento(f, c, "ClienteActual");
                            if (promoSeleccionada != nullptr)
                            {
                                std::cout << "[INFO] Reserva con promocion '" << promoSeleccionada->promocion->nombre << "'.\n";
                            }
                        }
                        else if (propio)
                        {
                            sala.cancelarReserva(f, c);
                        }
                        else
                        {
                            std::cout << "[AVISO] Asiento ocupado por otro cliente.\n";
                        }
                    }

                    if (ocupado && ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip(propio ? "Tu reserva. Click para cancelar." : "Asiento ocupado.");
                    }

                    ImGui::PopStyleColor(3);
                    if (c < sala.obtenerColumnas()) ImGui::SameLine();
                }
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::Spacing();
    ImGui::Separator();
    if (ImGui::Button("Cerrar Sesion", ImVec2(120, 30)))
    {
        g_estaAutenticado = false;
    }

    ImGui::End();
}
