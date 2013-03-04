/*
 * SemaphoreSetDistribuido.h
 *
 *  Created on: 18/02/2013
 *      Author: gonzalo
 */

#ifndef SEMAPHORESETDISTRIBUIDO_H_
#define SEMAPHORESETDISTRIBUIDO_H_

#include "mutex_distribuido.h"
#include "control_tokens.h"
#include <string>
#include <vector>
#include "process.h"

class SemaphoreSetDistribuido {
private:
	ControlTokens * control;
	SemaphoreSet * semaforos;
	std::string nombre_recurso;
	int cantidad_de_semaforos;
	std::vector< Process *> handlers;

	void lanzar_comunicacion(const std::string & directorio_de_trabajo, const std::string & nombre_app,
		const std::string & local_brokers_file, const std::string & nombre_grupo, char id, size_t tamanio_memoria,
		int numero_semaforo);

public:
	SemaphoreSetDistribuido(std::vector<unsigned short> & numeros_de_semaforo,
		const std::string & directorio_de_trabajo, const std::string & nombre_app, const std::string & nombre_grupo,
		char id, int cantidad_de_semaforos, bool create = true);
	SemaphoreSetDistribuido(const std::string & directorio_de_trabajo, const std::string & nombre_grupo, char id, int cantidad_de_semaforos);
	virtual ~SemaphoreSetDistribuido();

	void wait_on(int num_sem);
	void signalize(int num_sem);

	void entregar_token(int num_sem);
	void esperar_token(int num_sem);
	void forwardear_token(int num_sem);
};

#endif /* SEMAPHORESETDISTRIBUIDO_H_ */
