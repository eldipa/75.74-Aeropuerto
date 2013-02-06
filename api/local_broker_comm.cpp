/*
 * LocalBrokerComm.cpp
 *
 *  Created on: 05/02/2013
 *      Author: gonzalo
 */

#include "local_broker_comm.h"
#include <cstdio>
#include <cstring>
#include <iostream>

char data [200];


LocalBrokerComm::LocalBrokerComm(const std::string & broker_hostname)
	: socket_broker(true)
{
	std::string name;
	name = broker_hostname;
	if (name == name) {

	}
	
	socket_broker.destination("localhost","1234");

	sprintf(data,"HOLA");

	socket_broker.sendsome(data,strlen(data));

	socket_broker.receivesome(data,200);

	std::cout << data << std::endl;

}

LocalBrokerComm::~LocalBrokerComm() {

}

