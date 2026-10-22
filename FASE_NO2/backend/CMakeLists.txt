cmake_minimum_required(VERSION 3.10)
project(CinemaProject)

# Usaremos C++17 para tener acceso a características modernas
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Incluir los directorios de cabeceras
include_directories(include)
include_directories(libs/imgui)

# Encontrar paquetes del sistema requeridos
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/backends)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/crow/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/asio/include)

# Archivos fuente de Dear ImGui
set(IMGUI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/backends/imgui_impl_opengl3.cpp
)

# Código compartido por la interfaz ImGui y el servidor web.
set(CORE_SOURCES
    src/core/CinemaSystem.cpp
    src/models/Pelicula.cpp
    src/models/Cliente.cpp
    src/models/Reserva.cpp
    src/models/Promocion.cpp           
    src/models/Beneficio.cpp
    src/models/Solicitud.cpp
    src/structures/ArbolBinario.cpp
    src/structures/ArbolBClientes.cpp
    src/structures/TablaHashReservas.cpp
    src/structures/MatrizDispersa.cpp
    src/structures/ListasPromociones.cpp
    src/structures/ListaSolicitudes.cpp
    src/structures/ArbolAVLFunciones.cpp
    src/utils/CargaJSONFase2.cpp
    src/utils/CargaJSONClientesReservas.cpp
    src/utils/PersistenciaAsientos.cpp
    src/utils/Validaciones.cpp
)

# Ejecutable de escritorio actual.
set(DESKTOP_SOURCES
    src/main.cpp
    src/ui/PanelAdmin.cpp
    src/ui/PanelLogin.cpp
    src/ui/PanelCliente.cpp
)

# Crear el ejecutable de escritorio.
add_executable(CinemaProject ${DESKTOP_SOURCES} ${CORE_SOURCES} ${IMGUI_SOURCES})

# Crear el servidor web separado.
add_executable(CinemaServer
    src/server/main.cpp
    src/api/CinemaApi.cpp
    ${CORE_SOURCES}
)
target_compile_definitions(CinemaServer PRIVATE ASIO_STANDALONE ASIO_NO_DEPRECATED _WIN32_WINNT=0x0601)
target_include_directories(CinemaServer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/crow/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/asio/include
)

# Enlazar librerías.
target_link_libraries(CinemaProject glfw OpenGL::GL)
target_link_libraries(CinemaServer ws2_32)
