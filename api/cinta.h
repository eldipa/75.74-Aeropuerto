#ifndef CINTA_H_
#define CINTA_H_

#include <vector>
#include "sharedmemory.h"
#include "semaphoreset.h"
#include <cstring>
#include "memoria_distribuida.h"
#include "semaphore_set_distribuido.h"
#include "api_constants.h"
#include "api_common.h"

template <typename T>
class Cinta {
private:
	/*SharedMemory * memoria_compartida;
	 SemaphoreSet * semaforo_productores;
	 SemaphoreSet * semaforo_consumidores;
	 SemaphoreSet * mutex;*/
	MemoriaDistribuida * memoria_compartida;
	SemaphoreSetDistribuido * semaforo_productores;
	SemaphoreSetDistribuido * semaforo_consumidores;

	const int * tamanio_vector;
	int * posicion_libre;
	int * posicion_ocupada;
	int * cantidad_elementos;
	int * cantidad_productores;
	int * cantidad_consumidores;
	int * cantidad_productores_esperando;
	int * cantidad_consumidores_esperando;
	int * ids_productores_esperando;
	int * ids_consumidores_esperando;
	T * vector_elementos;

	const static int cant_ipc = 4;

	void despertar_consumidores();
	void despertar_productores();

public:
	Cinta(const char * app_name, const char * directorio_de_trabajo, char numero_cinta, bool create = true);
	virtual ~Cinta();

	void poner_equipaje(const T & elemento, int id_productor);
	T sacar_equipaje(int id_consumidor);
};

template <typename T> Cinta<T>::Cinta(const char * app_name, const char * directorio_de_trabajo, char numero_cinta,
	bool create)
{

	std::vector<unsigned short> valores;
	/*this->memoria_compartida = new SharedMemory(absolute_path, cant_ipc * (numero_cinta - 1), 0, false, false);

	 this->mutex = new SemaphoreSet(absolute_path, cant_ipc * (numero_cinta - 1) + 1, 0, 0);
	 this->semaforo_productores = new SemaphoreSet(absolute_path, cant_ipc * (numero_cinta - 1) + 2, 0, 0);
	 this->semaforo_consumidores = new SemaphoreSet(absolute_path, cant_ipc * (numero_cinta - 1) + 3, 0, 0);
	 */
	this->memoria_compartida = new MemoriaDistribuida(directorio_de_trabajo, app_name,
		std::string("cinta_checkin")/*.append(intToString(numero_cinta))*/.c_str(), numero_cinta,
		TAMANIO_MEMORIA_CINTA_CHECKIN, create);
	valores.clear();
	if (numero_cinta == -1) {
		for (unsigned short i = 0 ; i < CANTIDAD_MAX_PRODUCTORES_CINTA_CHECKIN ; i++) {
			valores.push_back(i);
		}
	} else {
		valores.push_back(numero_cinta - 1);
	}
	this->semaforo_productores = new SemaphoreSetDistribuido(valores, directorio_de_trabajo, app_name,
		std::string("chk_sem_prod_"), 0, CANTIDAD_MAX_PRODUCTORES_CINTA_CHECKIN, create);
	valores.clear();
	for (unsigned short i = 0 ; i < CANTIDAD_MAX_CONSUMIDORES_CINTA_CHECKIN ; i++) {
		valores.push_back(i);
	}
	this->semaforo_consumidores = new SemaphoreSetDistribuido(valores, directorio_de_trabajo, app_name,
		std::string("chk_sem_cons_"), 0, CANTIDAD_MAX_CONSUMIDORES_CINTA_CHECKIN, create);

	tamanio_vector = static_cast<const int *>(memoria_compartida->memory_pointer());
	posicion_libre = const_cast<int*>(tamanio_vector) + 1;
	posicion_ocupada = posicion_libre + 1;
	cantidad_elementos = posicion_ocupada + 1;
	cantidad_productores = cantidad_elementos + 1;
	cantidad_consumidores = cantidad_productores + 1;
	cantidad_productores_esperando = cantidad_consumidores + 1;
	cantidad_consumidores_esperando = cantidad_productores_esperando + 1;
	ids_productores_esperando = cantidad_consumidores_esperando + 1;
	ids_consumidores_esperando = ids_productores_esperando + *cantidad_productores;
	vector_elementos = reinterpret_cast<T *>(ids_consumidores_esperando + *cantidad_consumidores);
}

template <typename T> Cinta<T>::~Cinta() {
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
void Cinta<T>::despertar_consumidores() { // se llama con el mutex tomado
	int i;
	if (*cantidad_consumidores_esperando > 0) {
		for (i = 0; i < *this->cantidad_consumidores ; i++) {
			if (this->ids_consumidores_esperando [i] == 1) {
				semaforo_consumidores->signalize(i);
				this->ids_consumidores_esperando [i] = 0;
			}
		}
		*this->cantidad_consumidores_esperando = 0;
	}
}

template <typename T>
void Cinta<T>::despertar_productores() { // se llama con el mutex tomado
	int i;
	if (*cantidad_productores_esperando > 0) {
		for (i = 0; i < *this->cantidad_productores ; i++) {
			if (this->ids_productores_esperando [i] == 1) {
				semaforo_productores->signalize(i);
				this->ids_productores_esperando [i] = 0;
			}
		}
		*this->cantidad_productores_esperando = 0;
	}
}

template <typename T>
void Cinta<T>::poner_equipaje(const T & elemento, int id_productor) {
	bool coloque = false;

	while (!coloque) {

		semaforo_productores->wait_on(id_productor - 1); // espera por si esta lleno

		//mutex->wait_on(0);
		memoria_compartida->lock();

		if (*this->cantidad_elementos < *this->tamanio_vector) { // puedo colocar
			memcpy(&(vector_elementos [*this->posicion_libre]), &elemento, sizeof(T));
			*posicion_libre = (*posicion_libre + 1) % *this->tamanio_vector;
			(*this->cantidad_elementos)++;
			coloque = true;

			if (*this->cantidad_elementos == 1) { // estaba vacio
				despertar_consumidores();
			}
		}

		if (*this->cantidad_elementos == *this->tamanio_vector) { // lo llené
			(*this->cantidad_productores_esperando)++;
			this->ids_productores_esperando [id_productor - 1] = 1;
		} else {
			semaforo_productores->signalize(id_productor - 1);
		}

		//mutex->signalize(0);
		memoria_compartida->unlock();
	}
}

template <typename T>
T Cinta<T>::sacar_equipaje(int id_consumidor) {
	T elemento;
	bool extrajo = false;

	while (!extrajo) {

		this->semaforo_consumidores->wait_on(id_consumidor - 1);

		//mutex->wait_on(0);
		memoria_compartida->lock();

		if (*this->cantidad_elementos > 0) {
			extrajo = true;
			memcpy((void *)&elemento, &(vector_elementos [*this->posicion_ocupada]), sizeof(T));
			*this->posicion_ocupada = (*this->posicion_ocupada + 1) % *this->tamanio_vector;
			(*this->cantidad_elementos)--;

			if (*cantidad_elementos == *tamanio_vector - 1) { // estaba lleno
				despertar_productores();
			}
		}

		if (*this->cantidad_elementos == 0) {
			(*this->cantidad_consumidores_esperando)++;
			this->ids_consumidores_esperando [id_consumidor - 1] = 1;
		} else {
			this->semaforo_consumidores->signalize(id_consumidor - 1);
		}

		//mutex->signalize(0);
		memoria_compartida->unlock();
	}
	return elemento;
}

#endif /* CINTA_H_ */
