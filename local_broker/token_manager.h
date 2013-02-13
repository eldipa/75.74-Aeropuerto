/*
 * TokenManager.h
 *
 *  Created on: 10/02/2013
 *      Author: gonzalo
 */

#ifndef TOKENMANAGER_H_
#define TOKENMANAGER_H_

#include "grupo.h"
#include "messagequeue.h"
#include <vector>
#include <map>

class TokenManager {
private:
	MessageQueue clientes;
	std::map<std::string, Grupo *> grupos;
	MessageQueue outbound;
	MessageQueue inbound;

	void crear_grupos(const std::string & directorio_de_trabajo, const std::string & groups_file);
public:
	TokenManager(const std::string & directorio_de_trabajo, char id, const std::string & groups_file);
	virtual ~TokenManager();

	void run();
};

#endif /* TOKENMANAGER_H_ */
