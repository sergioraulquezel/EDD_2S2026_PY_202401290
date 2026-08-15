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

# Archivos fuente del proyecto (iremos agregando más aquí)
set(PROJECT_SOURCES
    src/main.cpp
    src/models/Pelicula.cpp
    src/models/Promocion.cpp           
    src/models/Beneficio.cpp
    src/models/Solicitud.cpp
    src/structures/ArbolBinario.cpp
    src/structures/MatrizDispersa.cpp
    src/structures/ListasPromociones.cpp
    src/structures/ListaSolicitudes.cpp
)

# Crear el ejecutable
add_executable(CinemaProject ${PROJECT_SOURCES} ${IMGUI_SOURCES})

# Enlazar librerías
target_link_libraries(CinemaProject glfw OpenGL::GL)