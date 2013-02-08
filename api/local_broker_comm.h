/*
 * LocalBrokerComm.h
 *
 *  Created on: 05/02/2013
 *      Author: gonzalo
 */

#ifndef LOCALBROKERCOMM_H_
#define LOCALBROKERCOMM_H_

#include <string>
#include "socket.h"

class LocalBrokerComm {
private:

	Socket socket_broker;

public:
	LocalBrokerComm(const std::string & app_name, const std::string & broker_hostname);
	virtual ~LocalBrokerComm();

	void join(const std::string & nombre_recurso);
	void leave();

	void wait_mutex(const std::string & nombre_recurso);
	void free_mutex(const std::string & nombre_recurso);
};

#endif /* LOCALBROKERCOMM_H_ */
