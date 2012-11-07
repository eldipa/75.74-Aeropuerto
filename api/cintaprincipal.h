#ifndef CINTAPRINCIPAL_H_
#define CINTAPRINCIPAL_H_

#include "semaphoreset.h"
#include "sharedmemory.h"
#include "oserror.h"
#include <cstring>
#include <cstdio>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

template<typename T>
class CintaPrincipal {
private:
	SemaphoreSet * mutex_control;
	SemaphoreSet * semaforo_vector_consumir;
	SemaphoreSet * semaforos_productores;
	SemaphoreSet * semaforos_consumidores;

	SharedMemory * memoria_compartida;

	char mostrar[500];

	unsigned int id;
	bool soy_productor;

	unsigned int * cantidad_maxima_productores;
	unsigned int * tamanio_vector;
	unsigned int * cantidad_elementos;
	unsigned int * posicion_libre;
	unsigned int * posicion_ocupada;
	unsigned int * cantidad_de_productores_esperando;
	unsigned int * cantidad_de_productores_actual;
	unsigned int * cantidad_de_consumidores;
	unsigned int * ids_productores_esperando;
	T * vector_elementos;

	void liberar_recursos();
	void inicializar_cinta(const std::string & file_key, int cantidad_maxima_de_productores,
			int cantidad_consumidores, int tamanio_asignado_a_vector);
public:
	CintaPrincipal(const std::string & file, int cantidad_maxima_de_productores,
			int cantidad_consumidores, int tamanio_asignado_a_vector);
	CintaPrincipal(const std::string & file, unsigned int id, bool es_productor);
	virtual ~CintaPrincipal();

	void colocar_elemento(const T * elemento);
	void extraer_elemento();
	void avanzar_cinta();
	void leer_elemento(T * elemento);

	static void destruir_cinta(const std::string & file_key);
};

template<typename T> CintaPrincipal<T>::CintaPrincipal(const std::string & file_key,
		int cantidad_maxima_de_productores, int cantidad_consumidores,
		int tamanio_asignado_a_vector) {

	inicializar_cinta(file_key, cantidad_maxima_de_productores, cantidad_consumidores,
			tamanio_asignado_a_vector);

}

template<typename T> CintaPrincipal<T>::CintaPrincipal(const std::string & file_key,
		unsigned int id, bool es_productor) {
	char msgerror[1024];

	if (id < 1) {
		sprintf(msgerror, "Error ID = %d invalido\n", id);
		// TODO tirar ValueError
		//throw ErrorEnCintaPrincipal(msgerror);
		throw "ERROR";
	}

	this->memoria_compartida = new SharedMemory(file_key.c_str(), 0, 0, false, false);

	this->cantidad_maxima_productores =
			static_cast<unsigned int *>(this->memoria_compartida->memory_pointer());
	this->tamanio_vector = this->cantidad_maxima_productores + 1;
	this->cantidad_elementos = this->tamanio_vector + 1;
	this->posicion_libre = this->cantidad_elementos + 1;
	this->posicion_ocupada = this->posicion_libre + 1;
	this->cantidad_de_productores_esperando = this->posicion_ocupada + 1;
	this->cantidad_de_productores_actual = this->cantidad_de_productores_esperando + 1;
	this->cantidad_de_consumidores = this->cantidad_de_productores_actual + 1;
	this->ids_productores_esperando = this->cantidad_de_consumidores + 1;
	this->vector_elementos = reinterpret_cast<T *>((this->ids_productores_esperando
			+ *this->cantidad_maxima_productores));

	this->mutex_control = new SemaphoreSet(file_key.c_str(), 0, 0, 0);
	this->semaforo_vector_consumir = new SemaphoreSet(file_key.c_str(), 1, 0, 0);
	this->semaforos_productores = new SemaphoreSet(file_key.c_str(), 2, 0, 0);
	this->semaforos_consumidores = new SemaphoreSet(file_key.c_str(), 3, 0, 0);

	if (es_productor) {
		this->mutex_control->wait_on(0);
		if (this->cantidad_de_productores_actual >= this->cantidad_maxima_productores) {
			this->mutex_control->signalize(0);
			liberar_recursos();
			// TODO tirar ValueError
			//throw ErrorEnCintaPrincipal("CANTIDAD MAXIMA DE PRODUCTORES ALCANZADA");
			throw "ERROR";
		} else if (id > *this->cantidad_maxima_productores) {
			this->mutex_control->signalize(0);
			// TODO tirar ValueError
			liberar_recursos();
			//throw ErrorEnCintaPrincipal("Error ID invalido");
			throw "ERROR";
		}
		*this->cantidad_de_productores_actual = *this->cantidad_de_productores_actual + 1;
		this->mutex_control->signalize(0);
	}
	this->id = id;
	this->soy_productor = es_productor;

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
	if (this->semaforo_vector_consumir) {
		delete this->semaforo_vector_consumir;
		this->semaforo_vector_consumir = NULL;
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

// NO ESTA GARANTIZADO EL ORDEN DE LLEGADA TODAVIA
template<typename T> void CintaPrincipal<T>::colocar_elemento(const T * elemento) {

	unsigned int mi_posicion;
	if (!this->soy_productor) {
		// TODO tirar ValueError
		throw "No fui creado para producir";
	}

	mutex_control->wait_on(0);

	while (*this->posicion_libre == *this->posicion_ocupada && *this->cantidad_elementos > 0) { // Espero una posicion libre
		(*this->cantidad_de_productores_esperando)++;
		this->ids_productores_esperando[this->id - 1] = 1;
		mutex_control->signalize(0);
		semaforos_productores->wait_on(this->id - 1);
		mutex_control->wait_on(0);
	}

	mi_posicion = *posicion_libre;

	*posicion_libre = (*posicion_libre + 1) % *this->tamanio_vector;
	(*this->cantidad_elementos)++;

	mutex_control->signalize(0);

	// Puedo colocar mi elemento en la posicion libre
	memcpy(&(vector_elementos[mi_posicion]), elemento, sizeof(T));

	this->semaforo_vector_consumir->signalize(mi_posicion);

}

template<typename T> void CintaPrincipal<T>::extraer_elemento() {
	unsigned int i;

	if (this->soy_productor) {
		// TODO tirar ValueError
		throw "No fui creado para consumir";
	}
	sprintf(mostrar, "Extraigo Elemento \n");
	write(fileno(stdout), mostrar, strlen(mostrar));

	mutex_control->wait_on(0);

	*this->posicion_ocupada = (*this->posicion_ocupada + 1) % *this->tamanio_vector;

	if (*cantidad_de_productores_esperando > 0) {
		for (i = 0; i < *this->cantidad_maxima_productores; i++) {
			if (this->ids_productores_esperando[i] == 1) {
				semaforos_productores->signalize(i);
				this->ids_productores_esperando[i] = 0;
			}
		}
		*this->cantidad_de_productores_esperando = 0;
	}

	(*this->cantidad_elementos)--;

	mutex_control->signalize(0);
	sprintf(mostrar, "Elemento Extraido habilito a 0\n");
	write(fileno(stdout), mostrar, strlen(mostrar));

	semaforos_consumidores->signalize(0);
}

template<typename T> void CintaPrincipal<T>::avanzar_cinta() {
	if (this->soy_productor) {
		// TODO tirar ValueError
		throw "No fui creado para consumir";
	}
	//this->semaforo_vector_consumir->signalize(*this->posicion_ocupada);

	if (this->id < *this->cantidad_de_consumidores) {
		sprintf(mostrar, "Señalizo semaforo consumidor %d\n", this->id);
		write(fileno(stdout), mostrar, strlen(mostrar));
		this->semaforos_consumidores->signalize(this->id);
	} else {
		this->semaforos_consumidores->signalize(0);
	}

}

template<typename T> void CintaPrincipal<T>::leer_elemento(T * elemento) {
	sprintf(mostrar, "Esperando semaforo consumidor %d\n", this->id - 1);
	write(fileno(stdout), mostrar, strlen(mostrar));
	this->semaforos_consumidores->wait_on(this->id - 1);

	if (this->id == 1) {
		sprintf(mostrar, "Esperando productor \n");
		write(fileno(stdout), mostrar, strlen(mostrar));
		this->semaforo_vector_consumir->wait_on(*this->posicion_ocupada);
	}
	memcpy(elemento, &(vector_elementos[*this->posicion_ocupada]), sizeof(T));

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

	std::vector<unsigned short int> valores;

	tamanio_control = 8 * sizeof(unsigned int);
	tamanio_id_productores = cantidad_maxima_de_productores * sizeof(unsigned int);
	tamanio_dato = sizeof(T);
	tamanio_shared_memory = tamanio_control + tamanio_id_productores
			+ tamanio_asignado_a_vector * tamanio_dato;

	valores.push_back(1);
	mutex_control = new SemaphoreSet(valores, file_key.c_str(), id, permisos);

	valores.clear();
	for (i = 0; i < tamanio_asignado_a_vector; i++) {
		valores.push_back(0);
	}
	semaforo_vector_consumir = new SemaphoreSet(valores, file_key.c_str(), id, permisos);

	valores.clear();
	for (i = 0; i < cantidad_maxima_de_productores; i++) {
		valores.push_back(0);
	}
	semaforos_productores = new SemaphoreSet(valores, file_key.c_str(), id, permisos);

	valores.clear();
	valores.push_back(1);
	for (i = 1; i < cantidad_consumidores; i++) {
		valores.push_back(0);
	}
	semaforos_consumidores = new SemaphoreSet(valores, file_key.c_str(), id, permisos);

	try {
		// Trato de crear la memoria compartida
		memoria_compartida = new SharedMemory(file_key.c_str(), tamanio_shared_memory, 0664, true,
				false);
	} catch (const OSError & error) {
		// ERROR si la memoria ya existia (la destruyo y la creo de vuelta)
		memoria_compartida = new SharedMemory(file_key.c_str(), 0, 0, false, false);
		//memoria_compartida->destroy();
		delete memoria_compartida;
		memoria_compartida = new SharedMemory(file_key.c_str(), tamanio_shared_memory, 0664, true,
				false);
	}

	cantidad_maxima_productores = static_cast<unsigned int *>(memoria_compartida->memory_pointer());
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
/*
 template<typename T>
 void CintaPrincipal<T>::destruir_cinta(const std::string & file_key) {
 SemaphoreSet * set;

 set = new SemaphoreSet(file_key.c_str(), 0, 0, 0);
 set->destroy();
 delete set;
 set = new SemaphoreSet(file_key.c_str(), 1, 0, 0);
 set->destroy();
 delete set;
 set = new SemaphoreSet(file_key.c_str(), 2, 0, 0);
 set->destroy();
 delete set;
 set = new SemaphoreSet(file_key.c_str(), 3, 0, 0);
 set->destroy();
 delete set;

 SharedMemory * memoria_compartida;
 memoria_compartida = new SharedMemory(file_key.c_str(), 0, 0, false, false);
 //memoria_compartida->destroy();
 delete memoria_compartida;
 }
 */
#endif /* CINTAPRINCIPAL_H_ */
