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

class GroupCommManager {
private:

public:
	GroupCommManager(const std::string & directorio_de_trabajo, const std::string & path_to_inbound);
	virtual ~GroupCommManager();

	void levantar_grupo(const std::string & nombre_grupo, int numero_grupo);
};

#endif /* GROUPCOMMMANAGER_H_ */
