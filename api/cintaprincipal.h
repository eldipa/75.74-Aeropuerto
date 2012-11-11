#ifndef CINTAPRINCIPAL_H_
#define CINTAPRINCIPAL_H_

#include "semaphoreset.h"
#include "sharedmemory.h"
#include "oserror.h"
#include "log.h"

#include <cstring>
#include <cstdio>
#include <vector>
#include <string>
#include <iostream>
#include "../common/valueerror.h"

using namespace std;

template<typename T>
class CintaPrincipal {
private:
	SemaphoreSet * mutex_control;
	SemaphoreSet * semaforos_productores;
	SemaphoreSet * semaforos_consumidores;

	SharedMemory * memoria_compartida;

	bool soy_productor;

	int * cantidad_maxima_productores;
	int * tamanio_vector;
	int * cantidad_elementos;
	int * posicion_libre;
	int * posicion_ocupada;
	int * cantidad_de_productores_esperando;
	int * cantidad_de_productores_actual;
	int * cantidad_de_consumidores;
	int * consumidor_esperando;
	int * ids_productores_esperando;

	T * vector_elementos;

	void liberar_recursos();
	void inicializar_cinta(const std::string & file_key, int cantidad_maxima_de_productores,
			int cantidad_consumidores, int tamanio_asignado_a_vector);
public:
	CintaPrincipal(const std::string & file, int cantidad_maxima_de_productores,
			int cantidad_consumidores, int tamanio_asignado_a_vector);
	CintaPrincipal(const std::string & file, bool es_productor);
	virtual ~CintaPrincipal();

	void colocar_elemento(const T * elemento, int id);
	void extraer_elemento();
	void avanzar_cinta(int id);
	void leer_elemento(T * elemento, int id);

	static void destruir_cinta(const std::string & file_key);
};

template<typename T> CintaPrincipal<T>::CintaPrincipal(const std::string & file_key,
		int cantidad_maxima_de_productores, int cantidad_consumidores,
		int tamanio_asignado_a_vector) {

	inicializar_cinta(file_key, cantidad_maxima_de_productores, cantidad_consumidores,
			tamanio_asignado_a_vector);

}

template<typename T> CintaPrincipal<T>::CintaPrincipal(const std::string & file_key,
		bool es_productor) {

	this->memoria_compartida = new SharedMemory(file_key.c_str(), 0, 0, false, false);

	this->cantidad_maxima_productores =
			static_cast<int *>(this->memoria_compartida->memory_pointer());
	this->tamanio_vector = this->cantidad_maxima_productores + 1;
	this->cantidad_elementos = this->tamanio_vector + 1;
	this->posicion_libre = this->cantidad_elementos + 1;
	this->posicion_ocupada = this->posicion_libre + 1;
	this->cantidad_de_productores_esperando = this->posicion_ocupada + 1;
	this->cantidad_de_productores_actual = this->cantidad_de_productores_esperando + 1;
	this->cantidad_de_consumidores = this->cantidad_de_productores_actual + 1;
	this->consumidor_esperando = this->cantidad_de_consumidores + 1;
	this->ids_productores_esperando = this->consumidor_esperando + 1;
	this->vector_elementos = reinterpret_cast<T *>((this->ids_productores_esperando
			+ *this->cantidad_maxima_productores));

	this->mutex_control = new SemaphoreSet(file_key.c_str(), 1, 0, 0);
	this->semaforos_productores = new SemaphoreSet(file_key.c_str(), 2, 0, 0);
	this->semaforos_consumidores = new SemaphoreSet(file_key.c_str(), 3, 0, 0);

	if (es_productor) {
		this->mutex_control->wait_on(0);
		if (*this->cantidad_de_productores_actual >= *this->cantidad_maxima_productores) {
			this->mutex_control->signalize(0);
			liberar_recursos();
			throw ValueError("Error se supero la cantidad maxima de productores: %d",
					*this->cantidad_maxima_productores);
			*this->cantidad_de_productores_actual = *this->cantidad_de_productores_actual + 1;
			this->mutex_control->signalize(0);
		}
		this->soy_productor = es_productor;

	}
}

template<typename T> CintaPrincipal<T>::~CintaPrincipal() {
	if (this->soy_productor) {
		this->mutex_control->wait_on(0);
		(*this->cantidad_de_productores_actual)--;
		this->mutex_control->signalize(0);
	}

	liberar_recursos();
}

template<typename T>
void CintaPrincipal<T>::liberar_recursos() {
	if (this->memoria_compartida) {
		delete this->memoria_compartida;
		this->memoria_compartida = NULL;
	}
	if (this->mutex_control) {
		delete this->mutex_control;
		this->mutex_control = NULL;
	}
	if (this->semaforos_consumidores) {
		delete this->semaforos_consumidores;
		this->semaforos_consumidores = NULL;
	}
	if (this->semaforos_productores) {
		delete this->semaforos_productores;
		this->semaforos_productores = NULL;
	}
}

template<typename T> void CintaPrincipal<T>::colocar_elemento(const T * elemento, int id) {
	int mi_cantidad_elementos;
	bool coloque = false;
	//int mi_posicion;

	while (!coloque) {
		Log::debug("Tomo Semaforo %d", id - 1);
		semaforos_productores->wait_on(id - 1); // espera por si esta lleno

		Log::debug("Tomo Mutex Cinta Central");
		mutex_control->wait_on(0);
		mi_cantidad_elementos = *this->cantidad_elementos;
		if (*this->cantidad_elementos < *this->tamanio_vector) { // puedo colocar
			memcpy(&(vector_elementos[*this->posicion_libre]), elemento, sizeof(T));
			*posicion_libre = (*posicion_libre + 1) % *this->tamanio_vector;
			(*this->cantidad_elementos)++;
			Log::debug("Coloque en cinta");
			coloque = true;
		}

		if (coloque && *this->cantidad_elementos == *this->tamanio_vector) { // lo llené
			(*this->cantidad_de_productores_esperando)++;
			this->ids_productores_esperando[id - 1] = 1;
			Log::debug("Espero Consumidor");
		} else {
			Log::debug("Señalizo Semaforo Productor %d", id - 1);
			semaforos_productores->signalize(id - 1);
		}

		if (mi_cantidad_elementos == 0) { // estaba vacio
			if (*this->consumidor_esperando) {
				Log::debug("Señalizo Semaforo Consumidor %d", 0);
				this->semaforos_consumidores->signalize(0);
			}
		}
		Log::debug("Señalizo Mutex Cinta Central: %d", coloque);
		mutex_control->signalize(0);
	}
}

template<typename T> void CintaPrincipal<T>::extraer_elemento() {
	int i;

	mutex_control->wait_on(0);

	*this->posicion_ocupada = (*this->posicion_ocupada + 1) % *this->tamanio_vector;
	(*this->cantidad_elementos)--;

	if (*cantidad_de_productores_esperando > 0) {
		for (i = 0; i < *this->cantidad_maxima_productores; i++) {
			if (this->ids_productores_esperando[i] == 1) {
				semaforos_productores->signalize(i);
				this->ids_productores_esperando[i] = 0;
			}
		}
		*this->cantidad_de_productores_esperando = 0;
	}

	mutex_control->signalize(0);

	semaforos_consumidores->signalize(0);
}

template<typename T> void CintaPrincipal<T>::avanzar_cinta(int id) {

	if (id < *this->cantidad_de_consumidores) {
		this->semaforos_consumidores->signalize(id);
	} else {
		this->semaforos_consumidores->signalize(0);
	}

}

template<typename T> void CintaPrincipal<T>::leer_elemento(T * elemento, int id) {
	bool leyo = false;

	while (!leyo) {
		Log::debug("Tomo Semaforo %d", id - 1);
		this->semaforos_consumidores->wait_on(id - 1);
		Log::debug("Tomo Mutex Cinta Central");
		mutex_control->wait_on(0);

		if (*this->cantidad_elementos > 0) {
			leyo = true;
			memcpy(elemento, &(vector_elementos[*this->posicion_ocupada]), sizeof(T));
			Log::debug("Señalizo Semaforo %d", id - 1);
			this->semaforos_consumidores->signalize(id - 1);
		} else {
			*this->consumidor_esperando = 1;
		}
		Log::debug("Señalizo Mutex Cinta Central");
		mutex_control->signalize(0);

	}
}

template<typename T>
void CintaPrincipal<T>::inicializar_cinta(const std::string & file_key,
		int cantidad_maxima_de_productores, int cantidad_consumidores,
		int tamanio_asignado_a_vector) {

	int tamanio_shared_memory;
	int tamanio_control;
	int tamanio_id_productores;
	int tamanio_dato;
	int permisos = 0664;

	int i;

	std::vector<unsigned short> valores;

	tamanio_control = 8 * sizeof(int);
	tamanio_id_productores = cantidad_maxima_de_productores * sizeof(int);
	tamanio_dato = sizeof(T);
	tamanio_shared_memory = tamanio_control + tamanio_id_productores
			+ tamanio_asignado_a_vector * tamanio_dato;

	valores.push_back(1);
	mutex_control = new SemaphoreSet(valores, file_key.c_str(), 1, permisos);

	valores.clear();
	for (i = 0; i < cantidad_maxima_de_productores; i++) {
		valores.push_back(1);
	}
	semaforos_productores = new SemaphoreSet(valores, file_key.c_str(), 2, permisos);

	valores.clear();
	valores.push_back(1);
	for (i = 1; i < cantidad_consumidores; i++) {
		valores.push_back(0);
	}
	semaforos_consumidores = new SemaphoreSet(valores, file_key.c_str(), 3, permisos);

	memoria_compartida = new SharedMemory(file_key.c_str(), 0, tamanio_shared_memory, 0664, true,
			false);

	cantidad_maxima_productores = static_cast<int *>(memoria_compartida->memory_pointer());
	tamanio_vector = cantidad_maxima_productores + 1;
	cantidad_elementos = tamanio_vector + 1;
	posicion_libre = cantidad_elementos + 1;
	posicion_ocupada = posicion_libre + 1;
	cantidad_de_productores_esperando = posicion_ocupada + 1;
	cantidad_de_productores_actual = cantidad_de_productores_esperando + 1;
	cantidad_de_consumidores = cantidad_de_productores_actual + 1;
	ids_productores_esperando = cantidad_de_consumidores + 1;
	vector_elementos = reinterpret_cast<T *>(ids_productores_esperando
			+ cantidad_maxima_de_productores);

	// Inicializo el control de la memoria
	*cantidad_maxima_productores = cantidad_maxima_de_productores;
	*tamanio_vector = tamanio_asignado_a_vector;
	*cantidad_elementos = 0;
	*posicion_libre = 0;
	*posicion_ocupada = 0;
	*cantidad_de_productores_esperando = 0;
	*cantidad_de_productores_actual = 0;
	*cantidad_de_consumidores = cantidad_consumidores;
	for (i = 0; i < cantidad_maxima_de_productores; i++) {
		ids_productores_esperando[i] = 0;
	}

}

#endif /* CINTAPRINCIPAL_H_ */
