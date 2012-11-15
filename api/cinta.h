#ifndef CINTA_H_
#define CINTA_H_

#include <vector>
#include "sharedmemory.h"
#include "semaphoreset.h"
#include <cstring>

template<typename T>
class Cinta {
private:
	SharedMemory * memoria_compartida;
	SemaphoreSet * semaforo_productores;
	SemaphoreSet * semaforo_consumidores;
	SemaphoreSet * mutex;

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
	Cinta(const char * absolute_path, int numero_cinta, int size, int cant_productores,
			int cant_consumidores);
	Cinta(const char * absolute_path, int numero_cinta);
	virtual ~Cinta();

	void poner_equipaje(const T & elemento, int id_productor);
	T sacar_equipaje(int id_consumidor);
};

template<typename T> Cinta<T>::Cinta(const char * absolute_path, int numero_cinta, int size,
		int cant_productores, int cant_consumidores) {

	int * tamanio_de_vector;
	int tamanio_shared_memory;
	int tamanio_control;
	int tamanio_id_productores;
	int tamanio_id_consumidores;
	int tamanio_dato;

	int i;

	std::vector<unsigned short> valores;

	tamanio_control = 8 * sizeof(int);
	tamanio_id_productores = cant_productores * sizeof(int);
	tamanio_id_consumidores = cant_consumidores * sizeof(int);
	tamanio_dato = sizeof(T);
	tamanio_shared_memory = tamanio_control + tamanio_id_productores + tamanio_id_consumidores
			+ size * tamanio_dato;
	memoria_compartida = new SharedMemory(absolute_path, cant_ipc * (numero_cinta - 1),
			tamanio_shared_memory, 0664, true, false);

	valores.push_back(1);
	mutex = new SemaphoreSet(valores, absolute_path, cant_ipc * (numero_cinta - 1) + 1, 0664);

	valores.clear();
	for (i = 0; i < cant_productores; i++) {
		valores.push_back(1);
	}
	semaforo_productores = new SemaphoreSet(valores, absolute_path,
			cant_ipc * (numero_cinta - 1) + 2, 0664);

	valores.clear();
	for (i = 0; i < cant_consumidores; i++) {
		valores.push_back(1);
	}
	semaforo_consumidores = new SemaphoreSet(valores, absolute_path,
			cant_ipc * (numero_cinta - 1) + 3, 0664);

	tamanio_de_vector = static_cast<int *>(memoria_compartida->memory_pointer());
	tamanio_vector = const_cast<const int *>(tamanio_de_vector);
	posicion_libre = tamanio_de_vector + 1;
	posicion_ocupada = posicion_libre + 1;
	cantidad_elementos = posicion_ocupada + 1;
	cantidad_productores = cantidad_elementos + 1;
	cantidad_consumidores = cantidad_productores + 1;
	cantidad_productores_esperando = cantidad_consumidores + 1;
	cantidad_consumidores_esperando = cantidad_productores_esperando + 1;
	ids_productores_esperando = cantidad_consumidores_esperando + 1;
	ids_consumidores_esperando = ids_productores_esperando + cant_productores;
	vector_elementos = reinterpret_cast<T *>(ids_consumidores_esperando + cant_consumidores);

	// Inicializo el control de la memoria
	*tamanio_de_vector = size;
	*posicion_libre = 0;
	*posicion_ocupada = 0;
	*cantidad_elementos = 0;
	*cantidad_productores = cant_productores;
	*cantidad_consumidores = cant_consumidores;
	*cantidad_productores_esperando = 0;
	*cantidad_consumidores_esperando = 0;
	for (i = 0; i < cant_productores; i++) {
		ids_productores_esperando[i] = 0;
	}
	for (i = 0; i < cant_consumidores; i++) {
		ids_consumidores_esperando[i] = 0;
	}
}

template<typename T> Cinta<T>::Cinta(const char * absolute_path, int numero_cinta) {

	this->memoria_compartida = new SharedMemory(absolute_path, cant_ipc * (numero_cinta - 1), 0,
			false, false);

	this->mutex = new SemaphoreSet(absolute_path, cant_ipc * (numero_cinta - 1) + 1, 0, 0);
	this->semaforo_productores = new SemaphoreSet(absolute_path, cant_ipc * (numero_cinta - 1) + 2,
			0, 0);
	this->semaforo_consumidores = new SemaphoreSet(absolute_path, cant_ipc * (numero_cinta - 1) + 3,
			0, 0);

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

template<typename T> Cinta<T>::~Cinta() {
	if (this->memoria_compartida) {
		delete this->memoria_compartida;
		this->memoria_compartida = NULL;
	}
	if (this->mutex) {
		delete this->mutex;
		this->mutex = NULL;
	}
	if (this->semaforo_consumidores) {
		delete this->semaforo_consumidores;
		this->semaforo_consumidores = NULL;
	}
	if (this->semaforo_productores) {
		delete this->semaforo_productores;
		this->semaforo_productores = NULL;
	}
}

template<typename T>
void Cinta<T>::despertar_consumidores() { // se llama con el mutex tomado
	int i;
	if (*cantidad_consumidores_esperando > 0) {
		for (i = 0; i < *this->cantidad_consumidores; i++) {
			if (this->ids_consumidores_esperando[i] == 1) {
				semaforo_consumidores->signalize(i);
				this->ids_consumidores_esperando[i] = 0;
			}
		}
		*this->cantidad_consumidores_esperando = 0;
	}
}

template<typename T>
void Cinta<T>::despertar_productores() { // se llama con el mutex tomado
	int i;
	if (*cantidad_productores_esperando > 0) {
		for (i = 0; i < *this->cantidad_productores; i++) {
			if (this->ids_productores_esperando[i] == 1) {
				semaforo_productores->signalize(i);
				this->ids_productores_esperando[i] = 0;
			}
		}
		*this->cantidad_productores_esperando = 0;
	}
}

template<typename T>
void Cinta<T>::poner_equipaje(const T & elemento, int id_productor) {
	bool coloque = false;

	while (!coloque) {

		semaforo_productores->wait_on(id_productor - 1); // espera por si esta lleno

		mutex->wait_on(0);

		if (*this->cantidad_elementos < *this->tamanio_vector) { // puedo colocar
			memcpy(&(vector_elementos[*this->posicion_libre]), &elemento, sizeof(T));
			*posicion_libre = (*posicion_libre + 1) % *this->tamanio_vector;
			(*this->cantidad_elementos)++;
			coloque = true;

			if (*this->cantidad_elementos == 1) { // estaba vacio
				despertar_consumidores();
			}
		}

		if (*this->cantidad_elementos == *this->tamanio_vector) { // lo llené
			(*this->cantidad_productores_esperando)++;
			this->ids_productores_esperando[id_productor - 1] = 1;
		} else {
			semaforo_productores->signalize(id_productor - 1);
		}

		mutex->signalize(0);
	}
}

template<typename T>
T Cinta<T>::sacar_equipaje(int id_consumidor) {
	T elemento;
	bool extrajo = false;

	while (!extrajo) {

		this->semaforo_consumidores->wait_on(id_consumidor - 1);

		mutex->wait_on(0);

		if (*this->cantidad_elementos > 0) {
			extrajo = true;
			memcpy((void *) &elemento, &(vector_elementos[*this->posicion_ocupada]), sizeof(T));
			*this->posicion_ocupada = (*this->posicion_ocupada + 1) % *this->tamanio_vector;
			(*this->cantidad_elementos)--;

			if (*cantidad_elementos == *tamanio_vector - 1) { // estaba lleno
				despertar_productores();
			}
		}

		if (*this->cantidad_elementos == 0) {
			(*this->cantidad_consumidores_esperando)++;
			this->ids_consumidores_esperando[id_consumidor - 1] = 1;
		} else {
			this->semaforo_consumidores->signalize(id_consumidor - 1);
		}

		mutex->signalize(0);
	}
	return elemento;
}

#endif /* CINTA_H_ */
