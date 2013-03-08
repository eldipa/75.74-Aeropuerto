#ifndef CINTAPRINCIPAL_H_
#define CINTAPRINCIPAL_H_

#include <vector>
#include "sharedmemory.h"
#include "semaphoreset.h"
//#include "mutex.h"
#include "memoria_distribuida.h"
#include "semaphore_set_distribuido.h"
#include <cstring>
#include "api_constants.h"
#include <iostream>



template <typename T>
class CintaPrincipal {
private:
	//SharedMemory * memoria_compartida;
	MemoriaDistribuida * memoria_compartida;
	//SemaphoreSet * semaforo_productores;
	SemaphoreSetDistribuido * semaforo_productores;
	//SemaphoreSet * semaforo_consumidores;
	SemaphoreSetDistribuido * semaforo_consumidores;
	//SemaphoreSet * mutex;

	const int * tamanio_vector; 			//[0]
	int * posicion_libre;					//[1]
	int * posicion_ocupada;					//[2]
	int * cantidad_elementos;				//[3]
	int * cantidad_productores;				//[4]
	int * cantidad_consumidores;			//[5]
	int * cantidad_productores_esperando;	//[6]
	int * consumidor_esperando;				//[7]
	int * ids_productores_esperando;		//[8]
	T * vector_elementos;

	const static int cant_ipc = 4;

public:

	CintaPrincipal(const char * app_name, const char * absolute_path, int id_consumidor, int id_productor, bool create =
		true);
	virtual ~CintaPrincipal();

	void colocar_elemento(const T * elemento, int id_productor);
	void extraer_elemento();
	void avanzar_cinta(int id_consumidor);
	void leer_elemento(T * elemento, int id_consumidor);
};

template <typename T> CintaPrincipal<T>::CintaPrincipal(const char * app_name, const char * absolute_path,
	int id_consumidor, int id_productor, bool create)
{
	std::vector<unsigned short> valores;
	//int i;
	//this->memoria_compartida = new SharedMemory(absolute_path, 0, 0, false, false);
	memoria_compartida = new MemoriaDistribuida(absolute_path, app_name, (char *)"cinta_principal", 0,
		TAMANIO_MEMORIA_CINTA_PRINCIPAL, create);

	valores.clear();
	if (id_consumidor == CANTIDAD_MAX_CONSUMIDORES_CINTA_CENTRAL - 1) { //soy el ultimo consumidor
		valores.push_back(0);
		valores.push_back(id_consumidor);
	} else if (id_consumidor > 0 && id_consumidor < CANTIDAD_MAX_CONSUMIDORES_CINTA_CENTRAL - 1) {
		valores.push_back(0); //soy consumidor
		valores.push_back(id_consumidor);
		valores.push_back(id_consumidor + 1);
	} else if (id_consumidor == 0) {
		valores.push_back(0); //soy el primer consumidor
		valores.push_back(1);
	} else { //soy productor
		valores.push_back(0);
	}
	semaforo_consumidores = new SemaphoreSetDistribuido(valores, absolute_path, app_name, "cpp_sem_cons_", char(0),
		CANTIDAD_MAX_CONSUMIDORES_CINTA_CENTRAL, create);
	valores.clear();
	if (id_productor >= 0) { // soy productor
		valores.push_back(id_productor);
	} else if (id_productor == -2) { // caso especial intercargo
		for (unsigned short i = 5 ; i < CANTIDAD_MAX_PRODUCTORES_CINTA_CENTRAL ; i++) {
			valores.push_back(i);
		}
	} else { //soy consumidor
		for (unsigned short i = 0 ; i < CANTIDAD_MAX_PRODUCTORES_CINTA_CENTRAL ; i++) {
			valores.push_back(i);
		}
	}
	semaforo_productores = new SemaphoreSetDistribuido(valores, absolute_path, app_name, "cpp_sem_prod_", char(0),
		CANTIDAD_MAX_PRODUCTORES_CINTA_CENTRAL, create);
	//this->mutex = new SemaphoreSet(absolute_path, 1, 0, 0);

	//this->semaforo_productores = new SemaphoreSet(absolute_path, 2, 0, 0);

	//this->semaforo_consumidores = new SemaphoreSet(absolute_path, 3, 0, 0);

	tamanio_vector = static_cast<const int *>(memoria_compartida->memory_pointer());
	posicion_libre = const_cast<int*>(tamanio_vector) + 1;
	posicion_ocupada = posicion_libre + 1;
	cantidad_elementos = posicion_ocupada + 1;
	cantidad_productores = cantidad_elementos + 1;
	cantidad_consumidores = cantidad_productores + 1;
	cantidad_productores_esperando = cantidad_consumidores + 1;
	consumidor_esperando = cantidad_productores_esperando + 1;
	ids_productores_esperando = consumidor_esperando + 1;
	vector_elementos = reinterpret_cast<T *>(ids_productores_esperando + *cantidad_productores);
}

template <typename T> CintaPrincipal<T>::~CintaPrincipal() {
	if (this->memoria_compartida) {
		delete this->memoria_compartida;
		this->memoria_compartida = NULL;
	}
	/*if (this->mutex) {
	 delete this->mutex;
	 this->mutex = NULL;
	 }*/
	if (this->semaforo_consumidores) {
		delete this->semaforo_consumidores;
		this->semaforo_consumidores = NULL;
	}
	if (this->semaforo_productores) {
		delete this->semaforo_productores;
		this->semaforo_productores = NULL;
	}

}

template <typename T>
void CintaPrincipal<T>::colocar_elemento(const T * elemento, int id_productor) {
	bool coloque = false;

	while (!coloque) {

#if  DEBUG_CINTA_PRINCIPAL == 1
		std::cout << "productor wait cpp_sem_prod_" << id_productor - 1 << std::endl;
#endif
		semaforo_productores->wait_on(id_productor - 1); // espera por si esta lleno

		//mutex->wait_on(0);
#if  DEBUG_CINTA_PRINCIPAL == 1
		std::cout << "productor wait mutex cinta_principal" << std::endl;
#endif
		memoria_compartida->lock();
#if  DEBUG_CINTA_PRINCIPAL == 1
		std::cout << "productor cant_elementos = " << *cantidad_elementos << std::endl;
#endif

		if (*this->cantidad_elementos < *this->tamanio_vector) { // puedo colocar
			//memcpy(&(vector_elementos [*this->posicion_libre]), elemento, sizeof(T));
#if  DEBUG_CINTA_PRINCIPAL == 1
			std::cout << "productor elemento colocado" << std::endl;
#endif
			vector_elementos [*this->posicion_libre] = *elemento;
			*posicion_libre = (*posicion_libre + 1) % *this->tamanio_vector;
			(*this->cantidad_elementos)++;
			coloque = true;

			if (*this->cantidad_elementos == 1) { // estaba vacio
				if (*consumidor_esperando > 0) {
					*consumidor_esperando = 0;
#if  DEBUG_CINTA_PRINCIPAL == 1
					std::cout << "productor signal cpp_sem_cons_0" << std::endl;
#endif
					semaforo_consumidores->signalize(0);
				}
			}
		}

		if (*this->cantidad_elementos == *this->tamanio_vector) { // lo llené
			(*this->cantidad_productores_esperando)++;
			this->ids_productores_esperando [id_productor - 1] = 1;
		} else {
#if  DEBUG_CINTA_PRINCIPAL == 1
			std::cout << "productor signal cpp_sem_prod_" << id_productor - 1 << std::endl;
#endif
			semaforo_productores->signalize(id_productor - 1);
		}

		//mutex->signalize(0);
#if  DEBUG_CINTA_PRINCIPAL == 1
		std::cout << "productor cant_elementos = " << *cantidad_elementos << std::endl;
		std::cout << "productor signal mutex cinta_principal" << std::endl;
#endif
		memoria_compartida->unlock();
	}
}

template <typename T>
void CintaPrincipal<T>::avanzar_cinta(int id_consumidor) {
	if (id_consumidor < *this->cantidad_consumidores) {
#if  DEBUG_CINTA_PRINCIPAL == 1
		std::cout << "consumidor signal cpp_sem_cons_" << id_consumidor << std::endl;
#endif
		this->semaforo_consumidores->signalize(id_consumidor);
	} else {
#if  DEBUG_CINTA_PRINCIPAL == 1
		std::cout << "consumidor signal cpp_sem_cons_0" << std::endl;
#endif
		this->semaforo_consumidores->signalize(0);
	}
}

template <typename T>
void CintaPrincipal<T>::extraer_elemento() {
	int i;

	//mutex->wait_on(0);
#if  DEBUG_CINTA_PRINCIPAL == 1
	std::cout << "consumidor wait mutex cinta_principal" << std::endl;
#endif
	memoria_compartida->lock();
#if  DEBUG_CINTA_PRINCIPAL == 1
	std::cout << "consumidor cant_elementos = " << *cantidad_elementos << std::endl;
#endif

	*this->posicion_ocupada = (*this->posicion_ocupada + 1) % *this->tamanio_vector;
	(*this->cantidad_elementos)--;
#if  DEBUG_CINTA_PRINCIPAL == 1
	std::cout << "consumidor elemento extraido" << std::endl;
#endif

	if (*cantidad_elementos == *tamanio_vector - 1) { // estaba lleno
		if (*cantidad_productores_esperando > 0) {
			for (i = 0; i < *this->cantidad_productores ; i++) {
				if (this->ids_productores_esperando [i] == 1) {
#if  DEBUG_CINTA_PRINCIPAL == 1
					std::cout << "signal cpp_sem_prod_" << i << std::endl;
#endif
					semaforo_productores->signalize(i);
					this->ids_productores_esperando [i] = 0;
				}
			}
			*this->cantidad_productores_esperando = 0;
		}
	}

	if (*this->cantidad_elementos == 0) {
		*this->consumidor_esperando = 1;
	} else {
#if  DEBUG_CINTA_PRINCIPAL == 1
		std::cout << "consumidor signal cpp_sem_cons_0" << std::endl;
#endif
		semaforo_consumidores->signalize(0);
	}

	//mutex->signalize(0);
#if  DEBUG_CINTA_PRINCIPAL == 1
	std::cout << "consumidor cant_elementos = " << *cantidad_elementos << std::endl;
	std::cout << "consumidor signal mutex cinta_principal" << std::endl;
#endif
	memoria_compartida->unlock();

}

template <typename T>
void CintaPrincipal<T>::leer_elemento(T * elemento, int id_consumidor) {
	bool leyo = false;

	while (!leyo) {
#if  DEBUG_CINTA_PRINCIPAL == 1
		std::cout << "consumidor wait cpp_sem_cons_" << id_consumidor - 1 << std::endl;
#endif
		this->semaforo_consumidores->wait_on(id_consumidor - 1);

		//mutex->wait_on(0);
#if  DEBUG_CINTA_PRINCIPAL == 1
		std::cout << "consumidor wait mutex cinta_principal" << std::endl;
#endif
		memoria_compartida->lock();
#if  DEBUG_CINTA_PRINCIPAL == 1
		std::cout << "consumidor cant_elementos = " << *cantidad_elementos << std::endl;
#endif

		if (*this->cantidad_elementos > 0) {
			leyo = true;
			*elemento = vector_elementos [*this->posicion_ocupada];
#if  DEBUG_CINTA_PRINCIPAL == 1
			std::cout << "consumidor elemento leido" << std::endl;
#endif
			//memcpy((void *)elemento, &(vector_elementos [*this->posicion_ocupada]), sizeof(T));
		}

		if (*this->cantidad_elementos == 0) {
			*this->consumidor_esperando = 1;
		}

		//mutex->signalize(0);
#if  DEBUG_CINTA_PRINCIPAL == 1
		std::cout << "consumidor cant_elementos = " << *cantidad_elementos << std::endl;
		std::cout << "consumidor signal mutex cinta_principal" << std::endl;
#endif
		memoria_compartida->unlock();
	}
}

#endif /* CINTAPRINCIPAL_H_ */
