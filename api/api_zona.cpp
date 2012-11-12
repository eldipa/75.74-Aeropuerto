#include "api_zona.h"

ApiZona::ApiZona(int zona) :
		num_zona(zona), cola_tractores(PATH_COLA_ROBOTS_ZONA_TRACTORES, 0) {
}

ApiZona::~ApiZona() {
}

Contenedor ApiZona::esperar_contenedor(std::string&) const {
	Contenedor contenedor;
	return contenedor;
}

void ApiZona::retirar_contenedor(std::string&) {
}


