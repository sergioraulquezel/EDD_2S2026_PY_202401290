#include "utils/CargaJSONClientesReservas.h"
#include "utils/Validaciones.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>

static std::string ultimoMensajeClientesReservas = "";

std::string obtenerUltimoMensajeCargaClientesReservas()
{
    return ultimoMensajeClientesReservas;
}

static std::string limpiarRutaClientes(const std::string& texto)
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

static std::string leerArchivoClientes(const std::string& ruta)
{
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) return "";
    std::stringstream buffer;
    buffer << archivo.rdbuf();
    return buffer.str();
}

static size_t saltarEspaciosClientes(const std::string& texto, size_t pos)
{
    while (pos < texto.size() && std::isspace(static_cast<unsigned char>(texto[pos]))) ++pos;
    return pos;
}

static std::string obtenerStringClientes(const std::string& objeto, const std::string& clave)
{
    std::string patron = "\"" + clave + "\"";
    size_t pos = objeto.find(patron);
    if (pos == std::string::npos) return "";
    pos = objeto.find(':', pos);
    if (pos == std::string::npos) return "";
    pos = saltarEspaciosClientes(objeto, pos + 1);
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

static int obtenerIntClientes(const std::string& objeto, const std::string& clave)
{
    std::string patron = "\"" + clave + "\"";
    size_t pos = objeto.find(patron);
    if (pos == std::string::npos) return 0;
    pos = objeto.find(':', pos);
    if (pos == std::string::npos) return 0;
    pos = saltarEspaciosClientes(objeto, pos + 1);

    std::string numero;
    while (pos < objeto.size() && (std::isdigit(static_cast<unsigned char>(objeto[pos])) || objeto[pos] == '-')) {
        numero += objeto[pos++];
    }
    return numero.empty() ? 0 : std::stoi(numero);
}

static std::string extraerArregloClientes(const std::string& objeto, const std::string& clave)
{
    std::string patron = "\"" + clave + "\"";
    size_t pos = objeto.find(patron);
    if (pos == std::string::npos) return "";
    pos = objeto.find('[', pos);
    if (pos == std::string::npos) return "";

    int nivel = 0;
    size_t inicio = pos;
    bool enString = false;
    for (; pos < objeto.size(); ++pos) {
        char ch = objeto[pos];
        if (ch == '"' && (pos == 0 || objeto[pos - 1] != '\\')) {
            enString = !enString;
        }
        if (enString) continue;

        if (ch == '[') ++nivel;
        else if (ch == ']') {
            --nivel;
            if (nivel == 0) {
                return objeto.substr(inicio + 1, pos - inicio - 1);
            }
        }
    }
    return "";
}

static std::vector<std::string> extraerObjetosClientes(const std::string& texto)
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

static FuncionCine* buscarFuncionPorCodigo(std::vector<FuncionCine>& funciones, const std::string& codigoFuncion)
{
    for (FuncionCine& funcion : funciones) {
        if (funcion.codigoFuncion == codigoFuncion) {
            return &funcion;
        }
    }
    return nullptr;
}

static void guardarArchivoAsientosFuncion(const FuncionCine& funcion)
{
    std::ofstream archivo(funcion.archivoAsientos);
    if (!archivo.is_open()) {
        std::cout << "[JSON CLIENTES] No se pudo escribir " << funcion.archivoAsientos << ".\n";
        return;
    }

    archivo << "{\n";
    archivo << "  \"codigo_funcion\": \"" << funcion.codigoFuncion << "\",\n";
    archivo << "  \"asientos_ocupados\": [\n";

    bool primero = true;
    for (int f = 1; f <= funcion.asientos.obtenerFilas(); ++f) {
        for (int c = 1; c <= funcion.asientos.obtenerColumnas(); ++c) {
            NodoMatriz* asiento = const_cast<MatrizDispersa&>(funcion.asientos).obtenerAsiento(f, c);
            if (asiento == nullptr) continue;

            if (!primero) archivo << ",\n";
            primero = false;
            archivo << "    {\n";
            archivo << "      \"fila\": " << f << ",\n";
            archivo << "      \"columna\": " << c << ",\n";
            archivo << "      \"codigo_reserva\": \"" << asiento->titular << "\"\n";
            archivo << "    }";
        }
    }

    archivo << "\n  ]\n";
    archivo << "}\n";
}

int cargarClientesYReservasJSON(const std::string& rutaArchivo,
                                ArbolBClientes& clientes,
                                TablaHashReservas& reservas,
                                std::vector<FuncionCine>& funciones)
{
    std::string ruta = limpiarRutaClientes(rutaArchivo);
    std::string contenido = leerArchivoClientes(ruta);
    if (contenido.empty()) {
        ultimoMensajeClientesReservas = "No se pudo abrir o leer el archivo: " + ruta;
        std::cout << "[JSON CLIENTES] " << ultimoMensajeClientesReservas << "\n";
        return 0;
    }

    int clientesInsertados = 0;
    int clientesRechazados = 0;
    int reservasInsertadas = 0;
    int reservasRechazadas = 0;

    std::string arregloClientes = extraerArregloClientes(contenido, "clientes");
    std::vector<std::string> clientesJSON = extraerObjetosClientes(arregloClientes);

    for (const std::string& clienteObj : clientesJSON) {
        std::string id = obtenerStringClientes(clienteObj, "id");
        std::string nombre = obtenerStringClientes(clienteObj, "nombre");
        std::string correo = obtenerStringClientes(clienteObj, "correo");
        std::string telefono = obtenerStringClientes(clienteObj, "telefono");
        std::string password = obtenerStringClientes(clienteObj, "password");
        std::string tipo = obtenerStringClientes(clienteObj, "tipo");
        if (tipo.empty()) tipo = "cliente";

        if (campoVacio(id) || campoVacio(nombre) || campoVacio(correo) ||
            campoVacio(telefono) || campoVacio(password) || !correoValido(correo)) {
            ++clientesRechazados;
            std::cout << "[JSON CLIENTES] Cliente ignorado por campos obligatorios invalidos.\n";
            continue;
        }

        Cliente* cliente = clientes.buscarPorId(id);
        if (cliente == nullptr && clientes.buscarPorCorreo(correo) != nullptr) {
            ++clientesRechazados;
            std::cout << "[JSON CLIENTES] Cliente ignorado por correo duplicado: " << correo << "\n";
            continue;
        }
        if (cliente == nullptr) {
            Cliente nuevo(id, nombre, correo, telefono, password, tipo);
            if (clientes.insertar(nuevo)) {
                ++clientesInsertados;
            }
            cliente = clientes.buscarPorId(id);
        }

        if (cliente == nullptr) {
            std::cout << "[JSON CLIENTES] No se pudo obtener cliente " << id << " para sus reservas.\n";
            continue;
        }

        std::string arregloReservas = extraerArregloClientes(clienteObj, "reservas");
        std::vector<std::string> reservasJSON = extraerObjetosClientes(arregloReservas);
        for (const std::string& reservaObj : reservasJSON) {
            std::string codigoReserva = obtenerStringClientes(reservaObj, "codigo_reserva");
            std::string codigoFuncion = obtenerStringClientes(reservaObj, "codigo_funcion");
            int fila = obtenerIntClientes(reservaObj, "fila");
            int columna = obtenerIntClientes(reservaObj, "columna");
            std::string fechaReserva = obtenerStringClientes(reservaObj, "fecha_reserva");

            FuncionCine* funcion = buscarFuncionPorCodigo(funciones, codigoFuncion);
            if (campoVacio(codigoReserva) || campoVacio(codigoFuncion) || fila <= 0 || columna <= 0 ||
                !fechaValida(fechaReserva) || funcion == nullptr) {
                ++reservasRechazadas;
                std::cout << "[JSON CLIENTES] Reserva ignorada por datos invalidos o funcion inexistente: "
                          << codigoReserva << "\n";
                continue;
            }

            if (!funcion->asientos.asientoValido(fila, columna) || funcion->asientos.obtenerAsiento(fila, columna) != nullptr) {
                ++reservasRechazadas;
                std::cout << "[JSON CLIENTES] Reserva ignorada porque el asiento no existe o esta ocupado: "
                          << codigoReserva << "\n";
                continue;
            }

            Reserva nuevaReserva(codigoReserva, cliente->id, codigoFuncion, fila, columna, fechaReserva);
            if (!reservas.insertar(nuevaReserva)) {
                ++reservasRechazadas;
                continue;
            }

            if (!funcion->asientos.reservarAsiento(fila, columna, codigoReserva)) {
                reservas.eliminar(codigoReserva);
                ++reservasRechazadas;
                continue;
            }

            cliente->agregarReserva(codigoReserva);
            guardarArchivoAsientosFuncion(*funcion);
            ++reservasInsertadas;
        }
    }

    ultimoMensajeClientesReservas = "Clientes insertados: " + std::to_string(clientesInsertados) +
                                    " | Clientes rechazados: " + std::to_string(clientesRechazados) +
                                    " | Reservas insertadas: " + std::to_string(reservasInsertadas) +
                                    " | Reservas rechazadas: " + std::to_string(reservasRechazadas);
    std::cout << "[JSON CLIENTES] " << ultimoMensajeClientesReservas << "\n";
    return clientesInsertados + reservasInsertadas;
}
