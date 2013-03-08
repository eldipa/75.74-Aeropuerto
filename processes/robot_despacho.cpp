#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <cstdio>

#include "api_despachante.h"
#include "equipaje.h"
#include "constants.h"
#include "cintas.h"

#include "database.h"
#include "stmt.h"
#include "tupleiter.h"

#include "log.h"

#include <string>
#include "yasper.h"

#if DEBUG_ROBOT_DESPACHO_EXTRAE_CINTA_PPAL==0 && DEBUG_ROBOT_DESPACHO_COLOCA_CINTA_CONTENEDOR==0

int main(int argc, char** argv)
try
{

	if (argc < 5) {
		Log::info(
			"insuf. param para robot de despacho,se esperaba(directorio_de_trabajo, config_file, id, zona_desde, zona_hasta...)\n");
		exit(1);
	}

	int id_robot = atoi(argv [3]);

	if (id_robot < 1) {
		Log::crit("ID de robot_despacho incorrecto %d\n", id_robot);
		exit(1);
	}

	int zona_desde = atoi(argv [4]);
	int zona_hasta = atoi(argv [5]);

	ApiDespachante despachante_cinta_central(argv [1], argv [2],"robot_despachante", id_robot,true);

	for (;;) {

		sleep(rand() % SLEEP_ROBOT_DESPACHO);

		Log::info("Intentando tomar un nuevo equipaje de cinta central\n");

		Rfid rfid_equipaje = despachante_cinta_central.leer_proximo_equipaje();

		// Me parece que no van
		//zona_desde++;
		//zona_hasta++;

		int num_zona = despachante_cinta_central.get_zona(rfid_equipaje.numero_de_vuelo_destino);

		Log::info("toma el equipaje %d con destino a zona (%d)",
			rfid_equipaje.rfid, num_zona);

		if (rfid_equipaje.sospechoso) {

			Log::info("Equipaje %d sospechoso, vuelvo a ponerlo en la cinta central\n",
				rfid_equipaje.rfid);
			despachante_cinta_central.avanzar_cinta();

		} else if ((num_zona == -1) || (num_zona < zona_desde) || (num_zona > zona_hasta)) {

			Log::info("Equipaje %d no pertenece a mis zonas, vuelve a cinta central\n",
				rfid_equipaje.rfid);
			despachante_cinta_central.avanzar_cinta();

		} else {

			Log::info("OK, es para mi.Equipaje %d limpio envio a robot_carga zona %d\n",
				rfid_equipaje.rfid, num_zona);
			Equipaje equipaje = despachante_cinta_central.extraer_equipaje();
			CintaContenedor cinta_contenedor(std::string("robot_despacho").append(argv [3]).c_str(),std::string(argv[1]).append(PATH_CINTA_CONTENEDOR).c_str(), num_zona, true);
			cinta_contenedor.poner_equipaje(equipaje, 1);
		}
	}

}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

#elif DEBUG_ROBOT_DESPACHO_EXTRAE_CINTA_PPAL==1

int main(int argc, char** argv)
try
{

	printf("Debuggeando robot_despacho\n");

	if (argc < 5) {
		printf(
			"insuf. param para robot de despacho,se esperaba(directorio_de_trabajo, config_file, id, zona_desde, zona_hasta...)\n");
		exit(1);
	}

	int id_robot = atoi(argv [3]);

	if (id_robot < 1) {
		Log::crit("ID de robot_despacho incorrecto %d\n", id_robot);
		exit(1);
	}

	int zona_desde = atoi(argv [4]);
	int zona_hasta = atoi(argv [5]);

	ApiDespachante despachante_cinta_central(argv [1], argv [2], "robot_despachante", id_robot, true);

	for (int i = 0 ; i < 3 ; i++) {

		printf("Intentando tomar un nuevo equipaje de cinta central\n");

		Rfid rfid_equipaje = despachante_cinta_central.leer_proximo_equipaje();

		printf("Leido equipaje %d con destino a vuelo (%d)", rfid_equipaje.rfid, rfid_equipaje.numero_de_vuelo_destino);

		int num_zona = zona_desde + 1;

		if (rfid_equipaje.sospechoso) {

			printf("Equipaje %d sospechoso, vuelvo a ponerlo en la cinta central\n", rfid_equipaje.rfid);
			despachante_cinta_central.avanzar_cinta();

		} else if ((num_zona == -1) || (num_zona < zona_desde) || (num_zona > zona_hasta)) {

			printf("Equipaje %d no pertenece a mis zonas, vuelve a cinta central\n", rfid_equipaje.rfid);
			despachante_cinta_central.avanzar_cinta();

		} else {

			printf("OK, es para mi. Extrayendo equipaje %d de cinta\n", rfid_equipaje.rfid);
			Equipaje equipaje = despachante_cinta_central.extraer_equipaje();
			printf("Equipaje extraido %s\n", equipaje.toString().c_str());
		}
	}

}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

#elif  DEBUG_ROBOT_DESPACHO_COLOCA_CINTA_CONTENEDOR==1

#endif
