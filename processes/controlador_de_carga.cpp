#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "api_carga.h"
#include "contenedor.h"
#include "log.h"

//int main(int argc, char** argv) {
/*
	if (argc < 4) {
		Log::crit(
				"Insuficientes parametros contr_de_carga, se esperaba (path_controlador_carga, id_robot_carga num_vuelo, path_cola_tractores)\n");
		exit(1);
	}

	ApiCarga api_carga(argv[1], argv[3], atoi(argv[2]));
	// esperar_cierre_checkin
	// esperar_vuelos_intercargo
	// leer de la BD la cantidad de equipajes por cargar para el num_vuelo
	// Hay que ver si esto es asi o el robot_carga va tomando los equipajes de la BD.
	// hasta que lleguen los vuelos_intercargo y cierre el checkin.
	int cant_equipajes = 10;
	std::vector<Contenedor> equipajes_de_vuelo = api_carga.cargar_equipajes(cant_equipajes);

	if (equipajes_de_vuelo.size() > 0) {
		Log::info("Llego el equipaje completo del vuelo %s", argv[4]);
	} else {
		Log::info("La zona ya esta tomada por otro u");
	}*/
//}
