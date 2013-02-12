/*
 * local_broker_launcher.cpp
 *
 *  Created on: 05/02/2013
 *      Author: gonzalo
 */

#include <iostream>
#include "log.h"
#include "local_broker.h"

int main(int argc, char * argv [])
try
{
	argc = argc + 1 - 1;
	argv = argv + 1 - 1;

	LocalBroker server("./locks","./group_list.txt");

	server.run();

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
