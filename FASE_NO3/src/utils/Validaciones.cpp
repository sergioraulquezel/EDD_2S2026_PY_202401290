#include "utils/Validaciones.h"
#include <cstdio>

std::string limpiarCampo(const std::string& texto)
{
    size_t inicio = texto.find_first_not_of(" \t\r\n");
    if (inicio == std::string::npos) return "";
    size_t fin = texto.find_last_not_of(" \t\r\n");
    return texto.substr(inicio, fin - inicio + 1);
}

bool campoVacio(const std::string& texto)
{
    return limpiarCampo(texto).empty();
}

bool correoValido(const std::string& correo)
{
    std::string limpio = limpiarCampo(correo);
    size_t arroba = limpio.find('@');
    size_t punto = limpio.find('.', arroba == std::string::npos ? 0 : arroba);
    return arroba != std::string::npos && arroba > 0 && punto != std::string::npos && punto + 1 < limpio.size();
}

static bool bisiesto(int anio)
{
    return (anio % 400 == 0) || (anio % 4 == 0 && anio % 100 != 0);
}

static bool fechaComponentesValidos(int anio, int mes, int dia)
{
    if (anio < 1900 || mes < 1 || mes > 12 || dia < 1) return false;
    int diasMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (mes == 2 && bisiesto(anio)) return dia <= 29;
    return dia <= diasMes[mes - 1];
}

bool fechaValida(const std::string& fecha)
{
    std::string limpio = limpiarCampo(fecha);
    int anio = 0;
    int mes = 0;
    int dia = 0;

    if (std::sscanf(limpio.c_str(), "%d-%d-%d", &anio, &mes, &dia) == 3) {
        return fechaComponentesValidos(anio, mes, dia);
    }
    if (std::sscanf(limpio.c_str(), "%d/%d/%d", &dia, &mes, &anio) == 3) {
        return fechaComponentesValidos(anio, mes, dia);
    }
    return false;
}

bool positivo(int valor)
{
    return valor > 0;
}
