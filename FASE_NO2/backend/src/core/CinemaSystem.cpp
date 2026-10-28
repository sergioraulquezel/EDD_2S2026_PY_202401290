#include "core/CinemaSystem.h"
#include "utils/PersistenciaAsientos.h"
#include "utils/Validaciones.h"
#include <cstdio>
#include <ctime>

CinemaSystem::CinemaSystem()
{
    cargarDatosIniciales();
}

void CinemaSystem::cargarDatosIniciales()
{
    clientes.insertar(Cliente("U001", "Cliente 1", "cliente1@cine.com", "55550001", "123456"));
    clientes.insertar(Cliente("U002", "Cliente 2", "cliente2@cine.com", "55550002", "123456"));
    clientes.insertar(Cliente("U003", "Cliente 3", "cliente3@cine.com", "55550003", "123456"));
    clientes.insertar(Cliente("U004", "Cliente 4", "cliente4@cine.com", "55550004", "123456"));
    clientes.insertar(Cliente("U005", "Cliente 5", "cliente5@cine.com", "55550005", "123456"));

    promociones.insertarPromocion(new Promocion(1, "Lunes Estudiantil", "Descuentos con carnet universitario"));
    promociones.insertarPromocion(new Promocion(2, "Miercoles 2x1", "Lleva a un acompaniante gratis"));

    promociones.agregarBeneficioAPromocion(1, new Beneficio(101, "Descuento en boleto", 50));
    promociones.agregarBeneficioAPromocion(1, new Beneficio(102, "Descuento en pipocas grandes", 20));
    promociones.agregarBeneficioAPromocion(2, new Beneficio(103, "Boleto extra gratis", 100));

    // Datos iniciales para que la API tenga una cartelera visible al arrancar.
    auto* peliculaDemo = new Pelicula(901, "Dune: Parte Dos", "Ciencia ficcion", 166,
                                      "B15", "2026-12-31", "WEB901", "Espanol",
                                      "2026-10-01");
    auto* peliculaDemo2 = new Pelicula(902, "Intensamente 2", "Animacion", 100,
                                       "AA", "2026-11-30", "WEB902", "Espanol",
                                       "2026-09-15");
    cartelera.insertar(peliculaDemo);
    cartelera.insertar(peliculaDemo2);

    grafoSedes.agregarSede({"S001","CinemaUSAC Zona 10","Zona 10, Ciudad de Guatemala",3,"1122-3344"});
    grafoSedes.agregarSede({"S002","CinemaUSAC Zona 4","Zona 4, Ciudad de Guatemala",3,"1122-3344"});
    funciones.emplace_back(1, "F001", peliculaDemo, "19:00", "1", 6, 10, "F001_funcion.json", "S001");
    arbolFunciones.insertar(funciones.back());
    crearArchivoAsientosFuncion(funciones.back());
    funciones.emplace_back(2, "F002", peliculaDemo2, "21:30", "2", 5, 8, "F002_funcion.json", "S002");
    arbolFunciones.insertar(funciones.back());
    crearArchivoAsientosFuncion(funciones.back());
}

ArbolBinario& CinemaSystem::obtenerCartelera()
{
    return cartelera;
}

std::vector<FuncionCine>& CinemaSystem::obtenerFunciones()
{
    return funciones;
}

ArbolAVLFunciones& CinemaSystem::obtenerArbolFunciones()
{
    return arbolFunciones;
}

FuncionCine* CinemaSystem::buscarFuncion(const std::string& codigoFuncion)
{
    return buscarFuncionEnVector(codigoFuncion);
}

ArbolBClientes& CinemaSystem::obtenerClientes()
{
    return clientes;
}

GrafoSedes& CinemaSystem::obtenerGrafoSedes(){return grafoSedes;}
void CinemaSystem::actualizarGrafoSedes(){grafoSedes.reconstruir(funciones);}

bool CinemaSystem::crearSede(const Sede& sede, std::string& mensaje)
{
    if (campoVacio(sede.codigo) || campoVacio(sede.nombre) || campoVacio(sede.direccion) ||
        campoVacio(sede.telefono) || sede.cantidadSalas <= 0) {
        mensaje = "Codigo, nombre, direccion, telefono y cantidad de salas valida son obligatorios."; return false;
    }
    if (!grafoSedes.agregarSede(sede)) { mensaje = "Ya existe una sede con ese codigo."; return false; }
    actualizarGrafoSedes(); mensaje = "Sede registrada sin conexiones iniciales."; return true;
}

TablaHashReservas& CinemaSystem::obtenerReservas()
{
    return reservas;
}

ListaCircularPromociones& CinemaSystem::obtenerPromociones()
{
    return promociones;
}

ListaCircularDoble& CinemaSystem::obtenerSolicitudes()
{
    return solicitudes;
}

FuncionCine* CinemaSystem::buscarFuncionEnVector(const std::string& codigoFuncion)
{
    for (FuncionCine& funcion : funciones) {
        if (funcion.codigoFuncion == codigoFuncion) return &funcion;
    }
    return nullptr;
}

bool CinemaSystem::crearPelicula(int id, const std::string& titulo, const std::string& genero, int duracion,
                                 const std::string& clasificacion, const std::string& fechaFin,
                                 const std::string& codigo, const std::string& idioma,
                                 const std::string& fechaEstreno, std::string& mensaje)
{
    if (id <= 0 || campoVacio(codigo) || campoVacio(titulo) || campoVacio(genero) ||
        campoVacio(clasificacion) || campoVacio(idioma) || campoVacio(fechaEstreno) ||
        campoVacio(fechaFin)) {
        mensaje = "Complete todos los campos obligatorios de la pelicula.";
        return false;
    }
    if (duracion <= 0) {
        mensaje = "La duracion debe ser mayor que cero.";
        return false;
    }
    if (!fechaValida(fechaEstreno) || !fechaValida(fechaFin)) {
        mensaje = "Las fechas de la pelicula no son validas.";
        return false;
    }
    if (cartelera.buscarPorCodigo(codigo) != nullptr || cartelera.buscarPorId(id) != nullptr) {
        mensaje = "No se puede crear la pelicula: codigo o ID duplicado.";
        return false;
    }

    cartelera.insertar(new Pelicula(id, titulo, genero, duracion, clasificacion,
                                    fechaFin, codigo, idioma, fechaEstreno));
    mensaje = "Pelicula creada en el BST: " + codigo;
    return true;
}

bool CinemaSystem::crearFuncion(Pelicula* pelicula, const std::string& horario, const std::string& sala,
                                const std::string& codigoSede, int filas, int columnas, std::string& codigoCreado,
                                std::string& mensaje)
{
    if (pelicula == nullptr) {
        mensaje = "Debe seleccionar una pelicula.";
        return false;
    }
    if (campoVacio(horario) || campoVacio(sala) || filas <= 0 || columnas <= 0 || campoVacio(codigoSede)) {
        mensaje = "Sede, horario, sala, filas y columnas son obligatorios y deben ser validos.";
        return false;
    }
    if (!grafoSedes.existeSede(codigoSede)) { mensaje = "La sede seleccionada no existe."; return false; }

    int idFuncion = 1;
    do {
        char buffer[16];
        std::snprintf(buffer, sizeof(buffer), "F%03d", idFuncion++);
        codigoCreado = buffer;
    } while (arbolFunciones.buscar(codigoCreado) != nullptr);

    std::string archivoAsientos = codigoCreado + "_funcion.json";
    funciones.emplace_back(idFuncion - 1, codigoCreado, pelicula, horario, sala,
                           filas, columnas, archivoAsientos, codigoSede);
    arbolFunciones.insertar(funciones.back());
    crearArchivoAsientosFuncion(funciones.back());
    actualizarGrafoSedes();
    mensaje = "Funcion creada: " + codigoCreado;
    return true;
}

bool CinemaSystem::editarFuncion(const std::string& codigoFuncion, const std::string& horario,
                                 const std::string& sala, int filas, int columnas,
                                 std::string& mensaje)
{
    FuncionCine* funcionActual = buscarFuncionEnVector(codigoFuncion);
    if (funcionActual == nullptr || arbolFunciones.buscar(codigoFuncion) == nullptr) {
        mensaje = "No se encontro la funcion en el sistema.";
        return false;
    }
    if (campoVacio(horario) || campoVacio(sala) || filas <= 0 || columnas <= 0) {
        mensaje = "Horario, sala, filas y columnas deben ser validos.";
        return false;
    }

    cargarAsientosFuncion(*funcionActual);
    struct ReservaAsiento {
        int fila;
        int columna;
        std::string titular;
    };
    std::vector<ReservaAsiento> asientosReservados;

    for (int f = 1; f <= funcionActual->asientos.obtenerFilas(); ++f) {
        for (int c = 1; c <= funcionActual->asientos.obtenerColumnas(); ++c) {
            NodoMatriz* asiento = funcionActual->asientos.obtenerAsiento(f, c);
            if (asiento == nullptr) continue;
            if (f > filas || c > columnas) {
                mensaje = "No se puede reducir la sala: hay reservas fuera del nuevo rango.";
                return false;
            }
            asientosReservados.push_back({f, c, asiento->titular});
        }
    }

    FuncionCine actualizada(funcionActual->id, funcionActual->codigoFuncion, funcionActual->pelicula,
                            horario, sala, filas, columnas, funcionActual->archivoAsientos);
    for (const ReservaAsiento& asiento : asientosReservados) {
        actualizada.asientos.reservarAsiento(asiento.fila, asiento.columna, asiento.titular, false);
    }

    *funcionActual = actualizada;
    arbolFunciones.insertar(*funcionActual);
    guardarAsientosFuncion(*funcionActual);
    mensaje = "Funcion editada conservando " + std::to_string(asientosReservados.size()) + " reservas.";
    return true;
}

bool CinemaSystem::reservarAsiento(const std::string& idCliente, const std::string& codigoFuncion,
                                   int fila, int columna, std::string& codigoReserva,
                                   std::string& mensaje)
{
    Cliente* cliente = clientes.buscarPorId(idCliente);
    FuncionCine* funcion = buscarFuncionEnVector(codigoFuncion);
    if (cliente == nullptr) {
        mensaje = "No se encontro el cliente.";
        return false;
    }
    if (funcion == nullptr) {
        mensaje = "No se encontro la funcion.";
        return false;
    }

    cargarAsientosFuncion(*funcion);
    if (!funcion->asientos.asientoValido(fila, columna)) {
        mensaje = "El asiento esta fuera de los limites de la sala.";
        return false;
    }
    if (funcion->asientos.obtenerAsiento(fila, columna) != nullptr) {
        mensaje = "El asiento ya esta reservado.";
        return false;
    }

    int numero = 1;
    do {
        char buffer[16];
        std::snprintf(buffer, sizeof(buffer), "R%03d", numero++);
        codigoReserva = buffer;
    } while (reservas.buscar(codigoReserva) != nullptr);

    if (!funcion->asientos.reservarAsiento(fila, columna, codigoReserva)) {
        mensaje = "No se pudo reservar el asiento.";
        return false;
    }

    std::time_t ahora = std::time(nullptr);
    std::tm tiempoLocal{};
#ifdef _WIN32
    localtime_s(&tiempoLocal, &ahora);
#else
    localtime_r(&ahora, &tiempoLocal);
#endif
    char fecha[16];
    std::snprintf(fecha, sizeof(fecha), "%04d-%02d-%02d", tiempoLocal.tm_year + 1900,
                  tiempoLocal.tm_mon + 1, tiempoLocal.tm_mday);

    Reserva nueva(codigoReserva, idCliente, codigoFuncion, fila, columna, fecha);
    if (!reservas.insertar(nueva)) {
        funcion->asientos.cancelarReserva(fila, columna);
        mensaje = "No se pudo guardar la reserva.";
        return false;
    }

    cliente->agregarReserva(codigoReserva);
    guardarAsientosFuncion(*funcion);
    mensaje = "Reserva creada: " + codigoReserva;
    return true;
}

bool CinemaSystem::cancelarReserva(const std::string& idCliente, const std::string& codigoReserva,
                                   std::string& mensaje)
{
    Reserva* reserva = reservas.buscar(codigoReserva);
    if (reserva == nullptr) {
        mensaje = "No existe esa reserva.";
        return false;
    }
    if (reserva->idCliente != idCliente) {
        mensaje = "No puedes cancelar una reserva de otro cliente.";
        return false;
    }

    FuncionCine* funcion = buscarFuncionEnVector(reserva->codigoFuncion);
    if (funcion != nullptr) {
        cargarAsientosFuncion(*funcion);
        funcion->asientos.cancelarReserva(reserva->fila, reserva->columna);
        guardarAsientosFuncion(*funcion);
    }

    Cliente* cliente = clientes.buscarPorId(idCliente);
    if (cliente != nullptr) cliente->eliminarReserva(codigoReserva);
    reservas.eliminar(codigoReserva);
    mensaje = "Reserva cancelada y asiento liberado: " + codigoReserva;
    return true;
}

bool CinemaSystem::eliminarCliente(const std::string& idCliente, std::string& mensaje)
{
    Cliente* cliente = clientes.buscarPorId(idCliente);
    if (cliente == nullptr) {
        mensaje = "No se encontro el cliente.";
        return false;
    }

    for (const Reserva& reserva : reservas.listarPorCliente(idCliente)) {
        FuncionCine* funcion = buscarFuncionEnVector(reserva.codigoFuncion);
        if (funcion != nullptr) {
            cargarAsientosFuncion(*funcion);
            funcion->asientos.cancelarReserva(reserva.fila, reserva.columna);
            guardarAsientosFuncion(*funcion);
        }
        reservas.eliminar(reserva.codigoReserva);
    }

    clientes.eliminar(idCliente);
    mensaje = "Cliente eliminado y sus reservas liberadas.";
    return true;
}
