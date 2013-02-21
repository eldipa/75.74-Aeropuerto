/*
 * GroupCommManager.h
 *
 *  Created on: 16/02/2013
 *      Author: gonzalo
 */

#ifndef GROUPCOMMMANAGER_H_
#define GROUPCOMMMANAGER_H_

#include "messagequeue.h"
#include "group_interface.h"
#include <cstdio>


class GroupCommManager {
private:
	std::string directorio_de_trabajo;

public:
	GroupCommManager(const std::string & directorio_de_trabajo);
	virtual ~GroupCommManager();

	void levantar_grupo(const std::string & nombre_grupo, char numero_grupo, int token);
};

#endif /* GROUPCOMMMANAGER_H_ */
