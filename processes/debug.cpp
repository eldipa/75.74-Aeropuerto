#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include "api_tractor.h"
#include "log.h"
#include "local_broker_comm.h"

int main(int argc, char * argv [])
try
{
	// data seria la memoria distribuida
	char data [512];
	if (argc < 2) {
		std::cerr << "Faltan argumentos" << std::endl;
		return -1;
	}
	int a;

	LocalBrokerComm broker(argv [1], "localbroker1.sitio1.aeropuerto1");

	broker.join("cinta_principal");

	for (int i = 0 ; i < 3 ; i++) {
		broker.wait_mutex(data);


		sscanf(data, "%*[^:]:%d", &a);
		a++;
		snprintf(data, 512, "%s:%d", argv [1], a);
		std::cout << data << std::endl;

		broker.free_mutex(data);
	}
}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}
