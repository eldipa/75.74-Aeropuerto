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

// Todas las apis tienen que tener uno

class ControlTokens {
private:
	SharedMemory memoria_control;
	SemaphoreSet mutex;

	char * token_esperando;

public:
	ControlTokens(const std::string & directorio_de_trabajo);
	ControlTokens(const std::string & directorio_de_trabajo, bool create);
	virtual ~ControlTokens();

	void cargar_esperando_token(const char nombre [MAX_NAME_SIZE]);
	void limpiar_esperando_token();
	bool comparar_token(const char nombre [MAX_NAME_SIZE]);
};

#endif /* CONTROLRECURSOS_H_ */
