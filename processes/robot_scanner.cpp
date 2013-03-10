#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "api_scanner.h"
#include "log.h"
#include "cintas.h"
#include "constants.h"


#if DEBUG_ROBOT_SCANNER_COLOCA_CINTA_PPAL == 0 && DEBUG_ROBOT_SCANNER_EXTRAE_CINTA_SCANNER == 0

int main(int argc, char *argv [])
try
{
	int id_robot;
	int id_cinta_scanner;
	if (argc < 3) {
		Log::crit(
			"Insuficientes parametros para scanner, se esperaba (directorio_de_trabajo, config_file, id)\n");
		return (1);
	}

	id_robot = atoi(argv [3]); // de 1 a N

	if (id_robot < 1) {
		Log::crit("ID de robot_scanner incorrecto %d\n", id_robot);
		exit(1);
	}

	id_cinta_scanner = atoi(argv [4]);

	CintaScanner<Equipaje> cinta_scanner(std::string("escaner").append(intToString(id_robot)).c_str(), argv [1],
		id_cinta_scanner, id_robot);
	ApiScanner api_escaner_cinta_central(argv [1], argv [2], id_robot);

	Log::info("Iniciando scanner(%s), %s\n", argv [3], argv [4]);

	for (;;) {
		Log::info("Intentando tomar un nuevo equipaje de cinta de scanner\n");
		Equipaje equipaje = cinta_scanner.sacar_equipaje();

		Log::info("Escaneando equipaje %s\n", equipaje.toString().c_str());
		equipaje.set_sospechoso((rand() % CANT_SOSPECHOSOS) == 0);

		sleep(rand() % SLEEP_ROBOT_SCANNER);

		if (equipaje.es_sospechoso()) {
			Log::info("se encontro sospechoso el equipaje %s\n", equipaje.toString().c_str());
		} else {
			Log::info("equipaje limpio: %s\n", equipaje.toString().c_str());
		}

		Log::info("pasando equipaje a cinta central (%s)\n", argv [3]);
		api_escaner_cinta_central.colocar_equipaje_en_cinta_principal(equipaje);
	}

}
catch (const std::exception &e) {
	std::cerr << e.what() << std::endl;
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

#elif  DEBUG_ROBOT_SCANNER_COLOCA_CINTA_PPAL == 1

int main(int argc, char *argv [])
try
{
	int id_robot;

	printf("Debuggeando robot_scanner\n");

	if (argc < 3) {
		Log::crit(
			"Insuficientes parametros para scanner, se esperaba (directorio_de_trabajo, config_file, id, id_cinta_scanner)\n");
		return (1);
	}

	id_robot = atoi(argv [3]); // de 1 a N

	if (id_robot < 1) {
		Log::crit("ID de robot_scanner incorrecto %d\n", id_robot);
		exit(1);
	}

	//CintaScanner<Equipaje> cinta_scanner(std::string("escaner").append(intToString(id_robot)).c_str(), argv [1],
	//	id_cinta_scanner, id_robot);
	ApiScanner api_escaner_cinta_central(argv [1], argv [2], id_robot);

	Log::info("Iniciando scanner(%s), %s\n", argv [3], argv [4]);

	for (int i = 0 ; i < 3 ; i++) {

		Equipaje equipaje(Rfid(i, 1000));
		equipaje.set_sospechoso(false);
		equipaje.set_descripcion("Descripcion");

		printf("pasando equipaje (%s) a cinta central (%s)\n", equipaje.toString().c_str(), argv [3]);
		api_escaner_cinta_central.colocar_equipaje_en_cinta_principal(equipaje);
	}

}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

#elif  DEBUG_ROBOT_SCANNER_EXTRAE_CINTA_SCANNER == 1

int main(int argc, char *argv [])
try
{
	int id_robot;

	printf("Debuggeando robot_scanner\n");

	if (argc < 3) {
		Log::crit(
			"Insuficientes parametros para scanner, se esperaba (directorio_de_trabajo, config_file, id, id_cinta_scanner)\n");
		return (1);
	}

	id_robot = atoi(argv [3]); // de 1 a N

	if (id_robot < 1) {
		Log::crit("ID de robot_scanner incorrecto %d\n", id_robot);
		exit(1);
	}

	CintaScanner<Equipaje> cinta_scanner(std::string("escaner").append(intToString(id_robot)).c_str(), argv [1],
		id_robot, id_robot);
	//ApiScanner api_escaner_cinta_central(argv [1], argv [2], id_robot);

	Log::info("Iniciando scanner(%s), %s\n", argv [3], argv [4]);

	for (int i = 0 ; i < 2 ; i++) {
		printf("Extraer equipaje cinta scanner\n");
		Equipaje equipaje = cinta_scanner.sacar_equipaje();
		printf("Robot Scanner %d Equipaje extraido %s\n", id_robot, equipaje.toString().c_str());
	}
	printf("Robot Scanner %d Terminando\n", id_robot);
}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

#endif
