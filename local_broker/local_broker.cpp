/*
 * LocalBroker.cpp
 *
 *  Created on: 05/02/2013
 *      Author: gonzalo
 */

#include "local_broker.h"

#include <memory>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "process.h"

static char num_socket [11];
static char * args_client_handler [] = {(char*)"client_handler", num_socket, NULL};

LocalBroker::LocalBroker()
	: server_socket(true)
{
	std::string puerto("1234");

	server_socket.source(puerto);

}

LocalBroker::~LocalBroker() {

}

void LocalBroker::run() {

	std::auto_ptr<Socket> new_socket;

	new_socket = server_socket.listen(10);

	snprintf(num_socket, 10, "%d", new_socket->get_fd());

	Process p("client_handler", args_client_handler);

	//p.wait();

}
