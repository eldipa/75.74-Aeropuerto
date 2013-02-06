/*
 * LocalBroker.h
 *
 *  Created on: 05/02/2013
 *      Author: gonzalo
 */

#ifndef LOCALBROKER_H_
#define LOCALBROKER_H_

#include "socket.h"

class LocalBroker {
private:

	Socket server_socket;

public:

	LocalBroker();
	virtual ~LocalBroker();

	void run();
};

#endif /* LOCALBROKER_H_ */
