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

void ApiZona::enviar_contenedores_a_avion(const std::vector<Contenedor> contenedores,
		int numero_de_vuelo) {
	BloqueContenedores bloque;
	int cantidad_total_contenedores;
	int cant_bloques;
	int i, j;

	cantidad_total_contenedores = contenedores.size();
	bloque.mtype = ID_ESCUCHA_TRACTOR;
	bloque.cantidad_total_contenedores = cantidad_total_contenedores;
	bloque.vuelo_destino = numero_de_vuelo;

	cant_bloques = cantidad_total_contenedores / MAX_CONTENEDORES_POR_TRACTOR;
	if (cantidad_total_contenedores > cant_bloques * MAX_CONTENEDORES_POR_TRACTOR) {
		cant_bloques++;
	}

	for (i = 0; i < cant_bloques; i++) {
		bloque.contenedores_actual = 0;
		for (j = 0; j < MAX_CONTENEDORES_POR_TRACTOR; j++) {
			bloque.contenedores[j] = contenedores[i * MAX_CONTENEDORES_POR_TRACTOR + j];
			bloque.contenedores_actual++;
		}
		cola_tractores.push(&bloque, sizeof(BloqueContenedores));
	}
}
