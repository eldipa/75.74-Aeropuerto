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
#include "process.h"
#include <vector>
#include "signalhandler.h"
#include "eventhandler.h"

class LocalBroker : public EventHandler{
private:

	Socket server_socket;
	std::vector<Process *> hijos;
	sig_atomic_t salir;

	virtual void handleSignal(int signum);

public:

	LocalBroker(const std::string & directorio_de_trabajo, const std::string & groups_file,const std::string & servicio);
	virtual ~LocalBroker();

	void run();
};

#endif /* LOCALBROKER_H_ */
