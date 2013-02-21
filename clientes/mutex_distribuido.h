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
#include "process.h"

class MutexDistribuido {
private:

	SemaphoreSet * mutex;
	std::string nombre_recurso;
	ControlTokens * control;
	Process * handler;

	void lanzar_comunicacion(const std::string & directorio_de_trabajo, const std::string & nombre_app,
		const std::string & local_brokers_file, const std::string & nombre_grupo, char id, size_t tamanio_memoria);


public:
	MutexDistribuido(const std::string & directorio_de_trabajo, const std::string & nombre_app,
		const std::string & nombre_grupo, char id, bool create);
	MutexDistribuido(const std::string & directorio_de_trabajo, const std::string & nombre_grupo, char id);

	MutexDistribuido(const std::string & directorio_de_trabajo, const std::string & nombre_app,
		const std::string & file_key, char id, const std::string & nombre_grupo, bool create);

	MutexDistribuido(const std::string & directorio_de_trabajo, const std::string & nombre_app,
		const std::string & file_key, char id, const std::string & nombre_grupo);
	virtual ~MutexDistribuido();

	void lock();
	void unlock();

	void entregar_token();
	void esperar_token();
	void forwardear_token();
};

#endif /* MUTEXDISTRIBUIDO_H_ */
