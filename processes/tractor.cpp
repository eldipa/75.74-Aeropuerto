/*
 * tractor.cpp
 *
 *  Created on: 10/11/2012
 *      Author: gonzalo
 */
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include "api_tractor.h"
#include "log.h"

int main(int argc, char** argv) {
	int i;
	if (argc < 2) {
		Log::crit(
				"Insuficientes parametros para tractor, se esperaba (id_tractor,path_cola_tractores, path_cola_aviones)\n");
		exit(1);
	}

	ApiTractor tractor(atoi(argv[1]), argv[2], argv[3]);
	std::vector<Contenedor> contenedores;
	BloqueContenedores bloque;
	while (true) {
		Log::info("Tractor(%s) esperando contenedores", argv[1]);
		tractor.obtener_contenedores_a_cargar(bloque);
		Log::info("Tractor(%s) Contenedor Recibido", argv[1]);
		sleep(rand() % 10); // Lo que tardo en llegar al avion
		for (i = 0; i < bloque.contenedores_actual; i++) {
			Log::info("Tractor(%s) cargando contenedores en avion (%d)", argv[1],
					bloque.vuelo_destino);
			tractor.cargar_contenedor_en_avion(bloque.contenedores[i], bloque.vuelo_destino,
					bloque.cantidad_total_contenedores);
		}
	}

	Log::info("Nada mas que hacer");
	return 0;
}
