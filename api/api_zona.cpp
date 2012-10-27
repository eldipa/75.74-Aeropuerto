#include "api_zona.h"

ApiZona::ApiZona(int) { }
ApiZona::~ApiZona() {}

Contenedor ApiZona::esperar_contenedor(std::string&) const {
   Contenedor contenedor(1, 1);
   return contenedor;
}
void ApiZona::retirar_contenedor(std::string&) {}
