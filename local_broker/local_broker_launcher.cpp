/*
 * local_broker_launcher.cpp
 *
 *  Created on: 05/02/2013
 *      Author: gonzalo
 */

#include <iostream>
#include "log.h"
#include "local_broker.h"
#include "daemon.h"
#include "oserror.h"
#include "yasper.h"

int main(int argc, char * argv [])
try
{
	char groups [MAX_PATH_SIZE];
	if (argc < 3) {
		std::cout << "Faltan Argumentos (directorio de trabajo, puerto)" << std::endl;
		return 1;
	}

	strcpy(groups, argv [1]);
	strcat(groups, "/group_list.txt");

	yasper::ptr<LocalBroker> server;
	try {
		server = new LocalBroker(argv [1], groups, argv [2]);
	} catch (OSError & e) {
		std::cerr << "Error puerto en uso" << std::endl;
		std::cerr << e.what() << std::endl;
	}

	server->run();

}
catch (const std::exception & e) {
	std::cerr << "Error Broker Local" << std::endl;
	std::cerr << e.what() << std::endl;
	Log::crit("%s", e.what());
}
catch (...) {
	std::cerr << "Error Broker Local" << std::endl;
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}
