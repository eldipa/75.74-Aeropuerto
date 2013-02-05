
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include "api_tractor.h"
#include "log.h"

int main(int argc, char** argv)
try {
	int i;
	int id_tractor;
	if (argc < 2) {
		Log::crit("Insuficientes parametros para tractor, se esperaba (directorio_de_trabajo, id_tractor)\n");
		exit(1);
	}
	id_tractor = atoi(argv[2]);

	ApiTractor tractor(argv[1], id_tractor);
	std::vector<Contenedor> contenedores;
	BloqueContenedores bloque;
	while (true) {
		Log::info("Esperando contenedores");
		tractor.obtener_contenedores_a_cargar(bloque);
		Log::info("Contenedor Recibido");
		sleep(rand() % 10); // Lo que tardo en llegar al avion
		for (i = 0; i < bloque.contenedores_actual; i++) {
			Log::info("Cargando contenedores en avion (%d)",
					bloque.vuelo_destino);
			tractor.cargar_contenedor_en_avion(bloque.contenedores[i], bloque.vuelo_destino,
					bloque.cantidad_total_contenedores);
		}
	}

	Log::info("Nada mas que hacer");
	return 0;
}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}
