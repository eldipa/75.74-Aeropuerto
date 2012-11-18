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

int main(int argc, char** argv) {
	int id_robot_carga;
	char path[300];

	if (argc < 2) {
		Log::crit(
				"Insuficientes parametros para control_carga_contenedores, se esperaba (id_robot_carga)\n",
				argv[1]);
		exit(1);
	}
	id_robot_carga = atoi(argv[1]);

	MENSAJE_CHECKIN_CERRADO mensaje;
	strcpy(path, PATH_KEYS);
	strcat(path, PATH_COLA_CONTROL_CARGA_CHECKIN);
	MessageQueue cola_mensajes_con_despachante_de_vuelo(path, id_robot_carga);

	strcpy(path, PATH_KEYS);
	strcat(path, PATH_CINTA_CONTENEDOR);
	CintaContenedor cinta_contenedor_carga(path, id_robot_carga);

	Log::info("Control Carga Checkin(%s) espero mensaje checkin cerrado\n", argv[1]);

	cola_mensajes_con_despachante_de_vuelo.pull(&mensaje,
			sizeof(MENSAJE_CHECKIN_CERRADO) - sizeof(long), 0);

	Log::info("Aviso al robot de carga el cierre del checkin\n");

	cinta_contenedor_carga.avisar_cierre_de_checkin(mensaje.cantidad_equipaje_total);

	Log::info("Nada mas que hacer");
	return 0;
}
