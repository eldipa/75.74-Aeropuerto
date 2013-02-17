/*
 * GroupReceiver.h
 *
 *  Created on: 17/02/2013
 *      Author: gonzalo
 */

#ifndef GROUPRECEIVER_H_
#define GROUPRECEIVER_H_

#include "local_broker_constants.h"
#include "messagequeue.h"
#include "grupo.h"
#include "group_interface.h"
#include <string>

class GroupReceiver {
private:
	MessageQueue cola_token_manager;
	GroupInterface grupo_remoto;
	Grupo grupo;
	std::string broker_remoto;

	void loop_token();

	void send_token();
	size_t recv_token();

public:
	GroupReceiver(const std::string & directorio_de_trabajo, const std::string & nombre_grupo, char id,
		const std::string & nombre_broker_remoto);
	virtual ~GroupReceiver();

	void run();
};

#endif /* GROUPRECEIVER_H_ */
