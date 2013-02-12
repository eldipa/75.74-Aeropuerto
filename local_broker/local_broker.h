/*
 * LocalBroker.h
 *
 *  Created on: 05/02/2013
 *      Author: gonzalo
 */

#ifndef LOCALBROKER_H_
#define LOCALBROKER_H_

#include "socket.h"
#include "sharedmemory.h"
#include "messagequeue.h"
#include "semaphoreset.h"
#include "mutex.h"
#include "grupo.h"

class LocalBroker {
private:

	Socket server_socket;

public:

	LocalBroker(const std::string & directorio_de_trabajo, const std::string & groups_file);
	virtual ~LocalBroker();

	void run();
};

#endif /* LOCALBROKER_H_ */
