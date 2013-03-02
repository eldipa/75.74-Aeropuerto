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
#include "group_comm_manager.h"
#include <vector>
#include <map>

class TokenManager {
private:
	MessageQueue clientes;
	std::map<std::string, Grupo *> grupos;
	std::string directorio_de_trabajo;
	std::string groups_file;
	GroupCommManager manager;
	Grupo * grupo_maestro;

	int * cantidad_clientes_esperando;
	char * grupos_creados [MAX_GRUPOS];

	size_t memoria_total;
	size_t cantidad_de_grupos;

	void agregar_grupo_a_tabla_creados(const char nombre_grupo [MAX_NOMBRE_RECURSO]);
	void crear_grupo(const std::string & directorio_de_trabajo,
			const std::string & groups_file, const std::string & nombre_grupo);

	void chequear_memoria_disponible();
public:
	TokenManager(const std::string & directorio_de_trabajo,
			const std::string & groups_file);
	virtual ~TokenManager();

	void run();
};

#endif /* TOKENMANAGER_H_ */
