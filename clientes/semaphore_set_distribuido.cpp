/*
 * SemaphoreSetDistribuido.cpp
 *
 *  Created on: 18/02/2013
 *      Author: gonzalo
 */

#include "semaphore_set_distribuido.h"

SemaphoreSetDistribuido::SemaphoreSetDistribuido(std::vector<unsigned short> & valores,
	const std::string & directorio_de_trabajo, const std::string & nombre_grupo, bool create)
{
	std::string a = directorio_de_trabajo + nombre_grupo;
	if (create) {
		valores.clear();
	}
}

SemaphoreSetDistribuido::~SemaphoreSetDistribuido() {

}

void SemaphoreSetDistribuido::wait_on(int num_sem) {
	int a = num_sem + 1;
	a++;
}

void SemaphoreSetDistribuido::signalize(int num_sem) {
	int a = num_sem+ 1;
	a++;
}
