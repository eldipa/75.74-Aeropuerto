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

int main(int argc, char** argv) {
	int i;
	if (argc < 2) {
		Log::crit("Insuficientes parametros para tractor, se esperaba (id_tractor)\n");
		exit(1);
	}

	Log::info("Iniciando la torre de control");
	ApiTractor tractor(atoi(argv[1]));
	std::vector<Contenedor> contenedores;
	BloqueContenedores bloque;
	while (true) {
		bloque = tractor.obtener_contenedores_a_cargar();
		sleep(rand() % 10); // Lo que tardo en llegar al avion
		for (i = 0; i < bloque.contenedores_actual; i++) {
			tractor.cargar_contenedor_en_avion(bloque.contenedores[i], bloque.vuelo_destino,
					bloque.cantidad_total_contenedores);
		}
	}

	Log::info("Nada mas que hacer");
	return 0;
}
