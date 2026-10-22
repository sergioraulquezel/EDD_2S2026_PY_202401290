#include "utils/CargaJSONFase2.h"
#include "utils/Validaciones.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>

static std::string ultimoMensajeCargaFase2 = "";

std::string obtenerUltimoMensajeCargaFase2()
{
    return ultimoMensajeCargaFase2;
}

static std::string limpiarRuta(const std::string& texto)
{
    size_t inicio = texto.find_first_not_of(" \t\r\n");
    if (inicio == std::string::npos) return "";
    size_t fin = texto.find_last_not_of(" \t\r\n");
    std::string limpio = texto.substr(inicio, fin - inicio + 1);
    if (limpio.size() >= 2 && limpio.front() == '"' && limpio.back() == '"') {
        limpio = limpio.substr(1, limpio.size() - 2);
    }
    return limpio;
}

static std::string leerArchivoCompleto(const std::string& ruta)
{
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) return "";
    std::stringstream buffer;
    buffer << archivo.rdbuf();
    return buffer.str();
}

static int extraerNumero(const std::string& codigo)
{
    std::string digitos;
    for (char ch : codigo) {
        if (std::isdigit(static_cast<unsigned char>(ch))) {
            digitos += ch;
        }
    }
    return digitos.empty() ? 0 : std::stoi(digitos);
}

static size_t saltarEspacios(const std::string& texto, size_t pos)
{
    while (pos < texto.size() && std::isspace(static_cast<unsigned char>(texto[pos]))) ++pos;
    return pos;
}

static std::string obtenerString(const std::string& objeto, const std::string& clave)
{
    std::string patron = "\"" + clave + "\"";
    size_t pos = objeto.find(patron);
    if (pos == std::string::npos) return "";
    pos = objeto.find(':', pos);
    if (pos == std::string::npos) return "";
    pos = saltarEspacios(objeto, pos + 1);
    if (pos >= objeto.size() || objeto[pos] != '"') return "";

    std::string valor;
    for (++pos; pos < objeto.size(); ++pos) {
        char ch = objeto[pos];
        if (ch == '\\' && pos + 1 < objeto.size()) {
            valor += objeto[++pos];
        } else if (ch == '"') {
            break;
        } else {
            valor += ch;
        }
    }
    return valor;
}

static int obtenerInt(const std::string& objeto, const std::string& clave)
{
    std::string patron = "\"" + clave + "\"";
    size_t pos = objeto.find(patron);
    if (pos == std::string::npos) return 0;
    pos = objeto.find(':', pos);
    if (pos == std::string::npos) return 0;
    pos = saltarEspacios(objeto, pos + 1);

    std::string numero;
    while (pos < objeto.size() && (std::isdigit(static_cast<unsigned char>(objeto[pos])) || objeto[pos] == '-')) {
        numero += objeto[pos++];
    }
    return numero.empty() ? 0 : std::stoi(numero);
}

static std::string extraerArreglo(const std::string& objeto, const std::string& clave)
{
    std::string patron = "\"" + clave + "\"";
    size_t pos = objeto.find(patron);
    if (pos == std::string::npos) return "";
    pos = objeto.find('[', pos);
    if (pos == std::string::npos) return "";

    int nivel = 0;
    size_t inicio = pos;
    for (; pos < objeto.size(); ++pos) {
        if (objeto[pos] == '[') ++nivel;
        else if (objeto[pos] == ']') {
            --nivel;
            if (nivel == 0) {
                return objeto.substr(inicio + 1, pos - inicio - 1);
            }
        }
    }
    return "";
}

static std::vector<std::string> extraerObjetos(const std::string& texto)
{
    std::vector<std::string> objetos;
    int nivel = 0;
    size_t inicio = std::string::npos;
    bool enString = false;

    for (size_t i = 0; i < texto.size(); ++i) {
        char ch = texto[i];
        if (ch == '"' && (i == 0 || texto[i - 1] != '\\')) {
            enString = !enString;
        }
        if (enString) continue;

        if (ch == '{') {
            if (nivel == 0) inicio = i;
            ++nivel;
        } else if (ch == '}') {
            --nivel;
            if (nivel == 0 && inicio != std::string::npos) {
                objetos.push_back(texto.substr(inicio, i - inicio + 1));
                inicio = std::string::npos;
            }
        }
    }
    return objetos;
}

static void crearArchivoAsientosVacio(const std::string& nombreArchivo, const std::string& codigoFuncion)
{
    std::ifstream existente(nombreArchivo);
    if (existente.good()) {
        std::cout << "[JSON] Archivo de asientos existente conservado: " << nombreArchivo << "\n";
        return;
    }

    std::ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        std::cout << "[JSON] No se pudo crear archivo de asientos: " << nombreArchivo << "\n";
        return;
    }

    archivo << "{\n";
    archivo << "  \"codigo_funcion\": \"" << codigoFuncion << "\",\n";
    archivo << "  \"asientos_ocupados\": []\n";
    archivo << "}\n";
}

int cargarPeliculasYFuncionesJSON(const std::string& rutaArchivo,
                                  ArbolBinario& cartelera,
                                  std::vector<FuncionCine>& funciones,
                                  ArbolAVLFunciones& arbolFunciones)
{
    std::string ruta = limpiarRuta(rutaArchivo);
    std::string contenido = leerArchivoCompleto(ruta);
    if (contenido.empty()) {
        ultimoMensajeCargaFase2 = "No se pudo abrir o leer el archivo: " + ruta;
        std::cout << "[JSON] " << ultimoMensajeCargaFase2 << "\n";
        return 0;
    }

    std::string arregloPeliculas = extraerArreglo(contenido, "peliculas");
    std::vector<std::string> peliculasJSON = extraerObjetos(arregloPeliculas);
    int peliculasInsertadas = 0;
    int funcionesInsertadas = 0;
    int peliculasRechazadas = 0;
    int funcionesRechazadas = 0;

    for (const std::string& peliculaObj : peliculasJSON) {
        std::string codigo = obtenerString(peliculaObj, "codigo");
        std::string titulo = obtenerString(peliculaObj, "titulo");
        std::string genero = obtenerString(peliculaObj, "genero");
        int duracion = obtenerInt(peliculaObj, "duracion");
        std::string clasificacion = obtenerString(peliculaObj, "clasificacion");
        std::string idioma = obtenerString(peliculaObj, "idioma");
        std::string fechaEstreno = obtenerString(peliculaObj, "fecha_estreno");
        std::string fechaFin = obtenerString(peliculaObj, "fecha_fin");

        if (campoVacio(codigo) || campoVacio(titulo) || campoVacio(genero) ||
            campoVacio(clasificacion) || campoVacio(idioma) || duracion <= 0 ||
            !fechaValida(fechaEstreno) || !fechaValida(fechaFin)) {
            ++peliculasRechazadas;
            std::cout << "[JSON] Pelicula ignorada por campos obligatorios invalidos.\n";
            continue;
        }

        int id = extraerNumero(codigo);
        if (id <= 0) {
            ++peliculasRechazadas;
            std::cout << "[JSON] Pelicula ignorada por codigo invalido: " << codigo << "\n";
            continue;
        }

        Pelicula* pelicula = cartelera.buscarPorCodigo(codigo);
        if (pelicula != nullptr || cartelera.buscarPorId(id) != nullptr) {
            ++peliculasRechazadas;
            std::cout << "[JSON] Pelicula ignorada por codigo duplicado: " << codigo << "\n";
            continue;
        }

        pelicula = new Pelicula(id, titulo, genero, duracion, clasificacion, fechaFin, codigo, idioma, fechaEstreno);
        cartelera.insertar(pelicula);
        ++peliculasInsertadas;

        std::string arregloFunciones = extraerArreglo(peliculaObj, "funciones");
        std::vector<std::string> funcionesJSON = extraerObjetos(arregloFunciones);
        for (const std::string& funcionObj : funcionesJSON) {
            std::string codigoFuncion = obtenerString(funcionObj, "codigo_funcion");
            std::string horario = obtenerString(funcionObj, "horario");
            std::string sala = obtenerString(funcionObj, "sala");
            int filas = obtenerInt(funcionObj, "filas");
            int columnas = obtenerInt(funcionObj, "columnas");

            if (campoVacio(codigoFuncion) || campoVacio(horario) || campoVacio(sala) || filas <= 0 || columnas <= 0) {
                ++funcionesRechazadas;
                std::cout << "[JSON] Funcion ignorada por campos invalidos.\n";
                continue;
            }

            if (arbolFunciones.buscar(codigoFuncion) != nullptr) {
                ++funcionesRechazadas;
                std::cout << "[JSON] Funcion ignorada por codigo duplicado: " << codigoFuncion << "\n";
                continue;
            }

            int idFuncion = extraerNumero(codigoFuncion);
            std::string archivoAsientos = codigoFuncion + "_funcion.json";
            funciones.emplace_back(idFuncion, codigoFuncion, pelicula, horario, sala, filas, columnas, archivoAsientos);
            arbolFunciones.insertar(funciones.back());
            crearArchivoAsientosVacio(archivoAsientos, codigoFuncion);
            ++funcionesInsertadas;
        }
    }

    ultimoMensajeCargaFase2 = "Peliculas insertadas: " + std::to_string(peliculasInsertadas) +
                              " | Funciones insertadas: " + std::to_string(funcionesInsertadas) +
                              " | Peliculas rechazadas: " + std::to_string(peliculasRechazadas) +
                              " | Funciones rechazadas: " + std::to_string(funcionesRechazadas);
    std::cout << "[JSON] " << ultimoMensajeCargaFase2 << "\n";
    return peliculasInsertadas + funcionesInsertadas;
}
