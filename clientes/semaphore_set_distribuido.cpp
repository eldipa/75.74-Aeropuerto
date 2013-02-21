/*
 * SemaphoreSetDistribuido.cpp
 *
 *  Created on: 18/02/2013
 *      Author: gonzalo
 */

#include "semaphore_set_distribuido.h"
#include <cstdio>

SemaphoreSetDistribuido::SemaphoreSetDistribuido(std::vector<unsigned short> & numeros_de_semaforo,
	const std::string & directorio_de_trabajo, const std::string & nombre_app, const std::string & nombre_grupo,
	bool create)
{
	std::string a = directorio_de_trabajo + nombre_grupo;
	std::string prefijo_archivo;
	prefijo_archivo = std::string(directorio_de_trabajo).append(PREFIJO_RECURSO).append(nombre_grupo);
	char numero [5];
	unsigned short valor;
	unsigned short valor_anterior = 0;
	if (create) {
		set.clear();
	}
	for (size_t i = 0 ; i < numeros_de_semaforo.size() ; i++) {
		//unsigned short valor = valores [i];

		valor = numeros_de_semaforo [i];

		sprintf(numero, "%d", valor);

		while (valor > valor_anterior) {
			set.push_back(NULL);
			valor_anterior++;
		}
		MutexDistribuido * m = new MutexDistribuido(directorio_de_trabajo, nombre_app,
			std::string(prefijo_archivo).append(numero).append(POSTFIJO_LCK).c_str(), char(valor),
			std::string(nombre_grupo).append(numero), create);
		set.push_back(m);
		valor_anterior = valor + 1;
	}
}

SemaphoreSetDistribuido::~SemaphoreSetDistribuido() {
	std::vector<MutexDistribuido*>::iterator i;

	for (i = set.begin(); i != set.end() ; i++) {
		if ((*i) != NULL)
			delete (*i);
	}
}

void SemaphoreSetDistribuido::wait_on(int num_sem) {
	set.at(num_sem)->lock();
}

void SemaphoreSetDistribuido::signalize(int num_sem) {
	set.at(num_sem)->unlock();
}
