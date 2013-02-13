/*
 * MemoriaDistribuida.h
 *
 *  Created on: 12/02/2013
 *      Author: gonzalo
 */

#ifndef MEMORIADISTRIBUIDA_H_
#define MEMORIADISTRIBUIDA_H_

#include "sharedmemory.h"
#include "semaphoreset.h"
#include <fcntl.h>
#include "log.h"
#include "oserror.h"
#include "genericerror.h"
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#include "control_tokens.h"

class MemoriaDistribuida {
private:

	SharedMemory * memoria;
	SemaphoreSet * mutex;
	std::string nombre_recurso;
	size_t tamanio;
	ControlTokens * control;

public:
	MemoriaDistribuida(const std::string & directorio_de_trabajo, const std::string & nombre_grupo,char id, size_t tamanio);
	MemoriaDistribuida(const std::string & directorio_de_trabajo, const std::string & nombre_grupo,char id);
	virtual ~MemoriaDistribuida();

	void lock();
	void unlock();

	void entregar_token();
	void esperar_token();

	size_t get_mem_size();
	void * memory_pointer();
};

#endif /* MEMORIADISTRIBUIDA_H_ */
