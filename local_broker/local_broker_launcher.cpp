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

int main(int argc, char * argv [])
try
{
	argc = argc + 1 - 1;
	argv = argv + 1 - 1;
	ignore_signals();

	try {
		LocalBroker server("./locks", "./locks/group_list.txt");

		server.run();
	} catch (OSError & e) {
		std::cerr << "Error puerto en uso" << std::endl;
	}

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
