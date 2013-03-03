/*
 * multiplexer.cpp
 *
 *  Created on: 03/03/2013
 *      Author: gonzalo
 */
#include "log.h"
#include "oserror.h"
#include "group_multiplexer.h"
#include "local_broker_constants.h"
#include <iostream>
#include <string>

int main(int argc, char * argv [])
try
{
	bool seguir = true;
	if (argc < 2) {
		std::cerr << "Falta directorio de trabajo" << std::endl;
	}

	GroupMultiplexer multiplexer(std::string(argv [1]).append(PATH_COLAS_BROKERS).c_str());

	while (seguir) {
		try {
			multiplexer.forward_message();
		} catch (OSError & error) {
			seguir = false;
		}
	}

	return 0;
}
catch (OSError & error) {
	std::cerr << error.what()<< std::endl;
	Log::crit(error.what());
}
