#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "log.h"
#include "equipaje.h"

#include <vector>
#include <string>
#include <cstring>
#include "constants.h"

int main(int argc, char** argv)
try
{
	int id_robot;
	int num_cinta_checkin;
	if (argc < 5) {
		Log::crit(
			"Insuf parametros para robot de checkin,se esperaba (directorio_de_trabajo, config_file, id, num_cinta_checkin_in num_cinta_scanner)\n");
		exit(1);
	}

	id_robot = atoi(argv [3]); // de 1 a N
	num_cinta_checkin = atoi(argv [4]);

	CintaCheckin cinta_checkin(std::string("robot_checkin").append(argv [4]).c_str(), argv [1], num_cinta_checkin);

	CintaScanner<Equipaje> cinta_scanner(std::string("robot_checkin").append(argv [3]).c_str(), argv [1],
		1);

	// cada robot de checkin distribuye entre n scanners
	for (; ;) {
		Log::info("va a sacar equipaje.");

		sleep(rand() % SLEEP_ROBOT_CHECKIN);

		Equipaje equipaje = cinta_checkin.sacar_equipaje(id_robot);
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
