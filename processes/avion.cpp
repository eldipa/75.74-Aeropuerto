/*
 * avion.cpp
 *
 *  Created on: 10/11/2012
 *      Author: gonzalo
 */
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "log.h"

#include "api_avion.h"

int main(int argc, char** argv) {
	//bool vuelo_cargado;
	std::vector<Contenedor> contenedores;

	if (argc < 2) {
		Log::crit(
				"Insuficientes parametros para avion, se esperaba (numero_de_vuelo, path_cola_aviones)\n");
		exit(1);
	}

	ApiAvion vuelo(atoi(argv[1]), argv[2]);
	//vuelo_cargado = false;
	// Acá tendría que ir la lógica de la torre de control
	// Por ahora harcodeo una notificación de llegada de vuelo
	//while (!vuelo_cargado) {
	// carga de pasajeros
	sleep(rand() % 10);
	vuelo.notificar_avion_en_zona();
	contenedores = vuelo.esperar_carga_equipajes();
	vuelo.notificar_depegue();
	//}

	Log::info("Nada mas que hacer");
	return 0;
}
