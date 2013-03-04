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
#include "api_carga.h"
#include <cstring>
#include "mensajes.h"
#include "cintas.h"
#include <string>

int main(int argc, char** argv)
try {

	if (argc < 3) {
		Log::crit(
				"Insuficientes parametros para control_carga_contenedores, se esperaba (directorio_de_trabajo, config_file, id_robot_carga)\n");
		exit(1);
	}


	int id_robot_carga = atoi(argv[3]);
	int  equipajes_por_cargar = 0;

	Log::info("Iniciando controlador de carga para robot_carga %d\n", id_robot_carga);

	ApiCarga api_carga(std::string("control_carga_contenedores").append(intToString(id_robot_carga)).c_str(),argv[1], argv[2], id_robot_carga,  id_robot_carga,false);
	//CintaContenedor cinta_contenedor_carga(std::string("controlador_puesto_checkin").append(argv[3]).c_str(),argv[1], id_robot_carga);

	Log::info("Espero mensaje checkin cerrado\n");

   for(;;) {
      equipajes_por_cargar = api_carga.get_equipajes_por_cargar();
      api_carga.avisar_cierre_de_checkin(equipajes_por_cargar);
   }

	return 0;
}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}
