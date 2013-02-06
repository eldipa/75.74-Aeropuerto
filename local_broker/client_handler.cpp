/*
 * client_handler.cpp
 *
 *  Created on: 05/02/2013
 *      Author: gonzalo
 */


/*
 * local_broker_launcher.cpp
 *
 *  Created on: 05/02/2013
 *      Author: gonzalo
 */

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "log.h"
#include "local_broker.h"

char debug [200];

int main(int argc, char * argv [])
try
{
	if(argc != 2){
		std::cerr << "Falta el socket" << std::endl;
		return -1;
	}
	int fd;
	fd = atoi(argv[2]);

	Socket socket(fd);

	std::cout << "client_handler" << std::endl;

	socket.receivesome(debug,200);

	std::cout << "Recibido: " << debug << std::endl;

	snprintf(debug,200,"%s","RECIBIDO!");

	socket.sendsome(debug,strlen(debug));

}
catch (const std::exception & e) {
	std::cerr << "Error Broker Local" << std::endl;
	std::cerr << e.what() << std::endl;
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}
