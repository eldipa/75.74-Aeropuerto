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

	void relativize_dir(const char directorio_de_trabajo [FILENAME_MAX], char launch_dir [FILENAME_MAX]);

	void locate_dir(char launch_dir [FILENAME_MAX], char current_working_dir [FILENAME_MAX], char dir [FILENAME_MAX]);
public:
	GroupCommManager(const std::string & directorio_de_trabajo);
	virtual ~GroupCommManager();

	void levantar_grupo(const std::string & nombre_grupo, char numero_grupo);
};

#endif /* GROUPCOMMMANAGER_H_ */
