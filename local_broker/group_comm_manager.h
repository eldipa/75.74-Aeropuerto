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
#include "process.h"
#include <vector>


class GroupCommManager {
private:
	std::string directorio_de_trabajo;
	std::vector<Process *> procesos;
public:
	GroupCommManager(const std::string & directorio_de_trabajo);
	virtual ~GroupCommManager();

	void levantar_multiplexor(char id_aeropuerto);
	void levantar_grupo(const std::string & nombre_grupo, int token);
	void levantar_servicio();
	void bajar_servicio();
};

#endif /* GROUPCOMMMANAGER_H_ */
