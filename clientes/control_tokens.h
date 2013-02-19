/*
 * ControlRecursos.h
 *
 *  Created on: 12/02/2013
 *      Author: gonzalo
 */

#ifndef CONTROLRECURSOS_H_
#define CONTROLRECURSOS_H_

#include "sharedmemory.h"
#include "semaphoreset.h"
#include "mensajes_de_red.h"

class ControlTokens {
private:
	SharedMemory memoria_control;
	SemaphoreSet mutex;

	char * token_esperando;

	static ControlTokens * instance;

	ControlTokens(const std::string & directorio_de_trabajo);
	ControlTokens(const std::string & directorio_de_trabajo, bool create);

	virtual ~ControlTokens();

public:

	void cargar_esperando_token(const char nombre [MAX_NOMBRE_RECURSO]);
	void limpiar_esperando_token();
	bool comparar_token(const char nombre [MAX_NOMBRE_RECURSO]);

	static ControlTokens * get_instance(const std::string & directorio_de_trabajo);
	static ControlTokens * get_instance(const std::string & directorio_de_trabajo, bool create);

	static void destroy_instance();
};

#endif /* CONTROLRECURSOS_H_ */
