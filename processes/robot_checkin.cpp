#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "log.h"
#include "equipaje.h"

#include <vector>
#include <string>
#include <cstring>
#include "constants.h"

int main(int argc, char** argv) {
	int id_robot;
	char path[300];
	int num_cinta_checkin;
	if (argc < 3) {
		Log::crit(
				"Insuf parametros para robot de checkin,se esperaba (id, num_cinta_checkin_in num_cinta_scanner)\n");
		exit(1);
	}

	id_robot = atoi(argv[1]); // de 1 a N
	num_cinta_checkin=atoi(argv[2]);

	strcpy(path, PATH_KEYS);
	strcat(path, PATH_CINTA_CHECKIN);
	CintaCheckin cinta_checkin(path, num_cinta_checkin);

	strcpy(path, PATH_KEYS);
	strcat(path, PATH_CINTA_SCANNER);
	CintaScanner<Equipaje> cinta_scanner(path, atoi(argv[3]));


	// cada robot de checkin distribuye entre n scanners
	for (;;) {
		Log::info("Robot de checkin(%s), va a sacar equipaje.", argv[1]);

		sleep(rand() % SLEEP_ROBOT_CHECKIN);

		Equipaje equipaje = cinta_checkin.sacar_equipaje(id_robot);
		Log::info("Robot de checkin(%s), sacó %s", argv[1], equipaje.toString().c_str());

		sleep(rand() % SLEEP_ROBOT_CHECKIN);

		Log::info("Robot de checkin(%s), pone %s en siguiente cinta", argv[1],
				equipaje.toString().c_str());
		cinta_scanner.poner_equipaje(equipaje);
	}
}
