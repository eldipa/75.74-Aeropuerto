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

public:
	MutexDistribuido(const std::string & directorio_de_trabajo, const std::string & nombre_grupo, char id, bool create);
	MutexDistribuido(const std::string & directorio_de_trabajo, const std::string & nombre_grupo, char id);
	virtual ~MutexDistribuido();

	void lock(ControlTokens &);
	void unlock(ControlTokens &);

	void entregar_token();
	void esperar_token();

};

#endif /* MUTEXDISTRIBUIDO_H_ */
