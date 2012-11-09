#include "api_zona.h"

ApiZona::ApiZona(int) { }
ApiZona::~ApiZona() {}

Contenedor ApiZona::esperar_contenedor(std::string&) const {
   Contenedor contenedor;
   return contenedor;
}
void ApiZona::retirar_contenedor(std::string&) {}
