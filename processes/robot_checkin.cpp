#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "log.h"
#include "equipaje.h"

#include <vector>
#include <string>
#include <cstring>
#include "constants.h"

#if DEBUG_ROBOT_CHECKIN_EXTRAE_CINTA_CHECKIN==0 && DEBUG_ROBOT_CHECKIN_COLOCA_CINTA_SCANNER==0

int main(int argc, char** argv)
try
{
	//int id_robot;
	//int num_cinta_checkin;
	if (argc < 5) {
		Log::crit(
			"Insuf parametros para robot de checkin,se esperaba (directorio_de_trabajo, config_file, id, num_cinta_checkin_in num_cinta_scanner)\n");
		exit(1);
	}

	//id_robot = atoi(argv [3]); // de 1 a N
	//num_cinta_checkin = atoi(argv [4]);
	// num_cinta_checkin = num_cinta_checkin;

	CintaCheckin cinta_checkin(std::string("robot_checkin").c_str(), argv [1], -1);

	CintaScanner<Equipaje> cinta_scanner(std::string("robot_checkin").append(argv [3]).c_str(), argv [1],
		1);

	// cada robot de checkin distribuye entre n scanners
	for (;;) {
		Log::info("va a sacar equipaje.");

		sleep(rand() % SLEEP_ROBOT_CHECKIN);

		Equipaje equipaje = cinta_checkin.sacar_equipaje();
		Log::info("sacó %s", equipaje.toString().c_str());

		sleep(rand() % SLEEP_ROBOT_CHECKIN);

		Log::info("pone %s en siguiente cinta", equipaje.toString().c_str());
		cinta_scanner.poner_equipaje(equipaje);
	}
}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

#elif DEBUG_ROBOT_CHECKIN_COLOCA_CINTA_SCANNER == 1

#include <cstdio>

int main(int argc, char** argv)
try
{
	//int id_robot;
	//int num_cinta_checkin;

	if (argc < 5) {
		Log::crit(
			"Insuf parametros para robot de checkin,se esperaba (directorio_de_trabajo, config_file, id, num_cinta_checkin_in num_cinta_scanner)\n");
		exit(1);
	}

	//CintaCheckin cinta_checkin(std::string("robot_checkin").c_str(), argv [1], -1);

	CintaScanner<Equipaje> cinta_scanner(std::string("robot_checkin").c_str(), argv [1], 1);

	// cada robot de checkin distribuye entre n scanners
	for (int i = 0 ; i < 5 ; i++) {

		Equipaje equipaje(Rfid(i, 1000));
		equipaje.set_descripcion("Descripcion");

		printf("pone %s en siguiente cinta", equipaje.toString().c_str());
		cinta_scanner.poner_equipaje(equipaje);
	}
}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

#elif DEBUG_ROBOT_CHECKIN_EXTRAE_CINTA_CHECKIN == 1

#include <cstdio>

int main(int argc, char** argv)
try
{
	//int id_robot;
	//int num_cinta_checkin;

	if (argc < 5) {
		Log::crit(
			"Insuf parametros para robot de checkin,se esperaba (directorio_de_trabajo, config_file, id, num_cinta_checkin_in num_cinta_scanner)\n");
		exit(1);
	}

	CintaCheckin cinta_checkin(std::string("robot_checkin").c_str(), argv [1], -1);

	//CintaScanner<Equipaje> cinta_scanner(std::string("robot_checkin").c_str(), argv [1], 1);

	// cada robot de checkin distribuye entre n scanners
	Equipaje equipaje(Rfid(0, 1000));
	for (int i = 0 ; i < 3 ; i++) {

		printf("Extraer equipaje cinta checkin\n");
		equipaje = cinta_checkin.sacar_equipaje();

		printf("Equipaje extraido %s\n", equipaje.toString().c_str());
	}
}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

#endif

