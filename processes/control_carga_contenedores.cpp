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
#include "messagequeue.h"
#include "semaphoreset.h"
#include "sharedmemory.h"
#include "api_constants.h"
#include <cstring>
#include "mensajes.h"
#include "cintas.h"
#include <string>

int main(int argc, char** argv)
try {
	int id_robot_carga;

	if (argc < 2) {
		Log::crit(
				"Insuficientes parametros para control_carga_contenedores, se esperaba (directorio_de_trabajo, id_robot_carga)\n");
		exit(1);
	}

	id_robot_carga = atoi(argv[2]);
	Log::info("Iniciando controlador de carga para robot_carga %d\n", id_robot_carga);

	MENSAJE_CHECKIN_CERRADO mensaje;
	MessageQueue cola_mensajes_con_despachante_de_vuelo(
			std::string(argv[1]).append(PATH_COLA_CONTROL_CARGA_CHECKIN).c_str(), id_robot_carga);

	CintaContenedor cinta_contenedor_carga(
			std::string(argv[1]).append(PATH_CINTA_CONTENEDOR).c_str(), id_robot_carga);

	Log::info("Espero mensaje checkin cerrado\n");

	cola_mensajes_con_despachante_de_vuelo.pull(&mensaje,
			sizeof(MENSAJE_CHECKIN_CERRADO) - sizeof(long), 0);

	Log::info("Aviso al robot de carga el cierre del checkin\n");

	cinta_contenedor_carga.avisar_cierre_de_checkin(mensaje.cantidad_equipaje_total);

	Log::info("Nada mas que hacer");
	return 0;
}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}
