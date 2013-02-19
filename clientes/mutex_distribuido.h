/*
 * MutexDistribuido.h
 *
 *  Created on: 12/02/2013
 *      Author: gonzalo
 */

#ifndef MUTEXDISTRIBUIDO_H_
#define MUTEXDISTRIBUIDO_H_

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

class MutexDistribuido {
private:

	SemaphoreSet * mutex;
	std::string nombre_recurso;
	ControlTokens * control;

public:
	MutexDistribuido(const std::string & directorio_de_trabajo, const std::string & nombre_grupo, char id, bool create);
	MutexDistribuido(const std::string & directorio_de_trabajo, const std::string & nombre_grupo, char id);
	virtual ~MutexDistribuido();

	void lock();
	void unlock();

	void entregar_token();
	void esperar_token();
	void lanzar_comunicacion(const std::string & directorio_de_trabajo, const std::string & nombre_app,
		const std::string & local_brokers_file, const std::string & nombre_grupo, char id, size_t tamanio_memoria);

};

#endif /* MUTEXDISTRIBUIDO_H_ */
