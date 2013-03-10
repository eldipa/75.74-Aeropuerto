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
#include "dir.h"
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

	ApiDespachante despachante_cinta_central(argv [1], argv [2], "robot_despachante", id_robot, true);

	std::vector<yasper::ptr<CintaContenedor> > cintas_contenedor;

	for (int i = zona_desde ; i <= zona_hasta ; i++) {
		Log::debug("Conectando con cinta %d\n", i);

		cintas_contenedor.push_back(
			new CintaContenedor(std::string("robot_despacho").append(argv [3]).c_str(), argv [1], i, true));

	}

	for (; ;) {

		sleep(rand() % SLEEP_ROBOT_DESPACHO);

		Log::info("Leyendo equipaje de Cinta Principal\n");
		Rfid rfid_equipaje = despachante_cinta_central.leer_proximo_equipaje();
		int num_zona = despachante_cinta_central.get_zona(rfid_equipaje.numero_de_vuelo_destino);

		Log::info("Equipaje (%d:vuelo %d:zona %d:%s) leido de Cinta Principal\n", rfid_equipaje.rfid,
			rfid_equipaje.numero_de_vuelo_destino, num_zona, (rfid_equipaje.sospechoso) ? "Sospechoso" : "Limpio");

		if (rfid_equipaje.sospechoso) {

			Log::info("Equipaje %d Sospechoso, avanzo la Cinta Principal\n", rfid_equipaje.rfid);
			despachante_cinta_central.avanzar_cinta();

		} else if ((num_zona == -1) || (num_zona < zona_desde) || (num_zona > zona_hasta)) {

			Log::info("Equipaje (%d:zona %d) no pertenece a mis zonas, avanzo la Cinta Principal\n", rfid_equipaje.rfid,
				num_zona);
			despachante_cinta_central.avanzar_cinta();

		} else {

			Log::info("Tomando Equipaje (%d) de Cinta Principal\n", rfid_equipaje.rfid);
			Equipaje equipaje = despachante_cinta_central.extraer_equipaje();
			Log::info("Equipaje (%s) extraido de Cinta Principal\n", equipaje.toString().c_str());

			Log::info("Coloco Equipaje (%s) en Cinta Contenedor (zona %d)\n", equipaje.toString().c_str(), num_zona);
			cintas_contenedor [num_zona - zona_desde]->poner_equipaje(equipaje, 1);
			Log::info("Equipaje (%s) colocado en Cinta Contenedor (zona %d)\n", equipaje.toString().c_str(), num_zona);
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
			"insuf. param para robot de despacho,se esperaba(directorio_de_trabajo, config_file, id, zona_desde, zona_hasta)\n");
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

	for (int i = 0; i < 6; i++) {

		printf("Intentando tomar un nuevo equipaje de cinta central\n");

		Rfid rfid_equipaje = despachante_cinta_central.leer_proximo_equipaje();

		printf("Leido equipaje %d con destino a vuelo (%d)\n", rfid_equipaje.rfid, rfid_equipaje.numero_de_vuelo_destino);

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

int main(int argc, char** argv)
try
{

	printf("Debuggeando robot_despacho\n");

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

	std::vector<yasper::ptr<CintaContenedor> > cintas_contenedor;

	Log::info("creando Cintas Contenedor\n");
	for (int i = zona_desde; i <= zona_hasta; i++) {
		Log::debug("Conectando con cinta %d\n", i);
		cintas_contenedor.push_back(
			new CintaContenedor(std::string("robot_despacho").append(argv [3]).c_str(), argv [1], i, true));
	}

	for (int i = 0; i < 6; i++) {

		Equipaje equipaje(Rfid(i, i % 2 + 1));
		int num_zona = i % 2 + 3;

		printf("Colocando equipaje (%s) en cinta contenedor (%d)\n", equipaje.toString().c_str(), i % 2 + 1);
		cintas_contenedor [i % 2]->poner_equipaje(equipaje, num_zona);

	}

}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

#endif
