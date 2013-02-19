/*
 * SemaphoreSetDistribuido.h
 *
 *  Created on: 18/02/2013
 *      Author: gonzalo
 */

#ifndef SEMAPHORESETDISTRIBUIDO_H_
#define SEMAPHORESETDISTRIBUIDO_H_

#include "mutex_distribuido.h"
#include <string>

class SemaphoreSetDistribuido {
private:
	std::vector<MutexDistribuido *> set;
public:
	SemaphoreSetDistribuido(std::vector<unsigned short> & valores, const std::string & directorio_de_trabajo,
		const std::string & nombre_grupo, bool create);
	virtual ~SemaphoreSetDistribuido();

	void wait_on(int num_sem);
	void signalize(int num_sem);
};

#endif /* SEMAPHORESETDISTRIBUIDO_H_ */
