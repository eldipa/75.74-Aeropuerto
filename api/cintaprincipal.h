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

	const int * tamanio_vector;
	int * posicion_libre;
	int * posicion_ocupada;
	int * cantidad_elementos;
	int * cantidad_productores;
	int * cantidad_consumidores;
	int * cantidad_productores_esperando;
	int * consumidor_esperando;
	int * ids_productores_esperando;
	T * vector_elementos;

	const static int cant_ipc = 4;

public:
	CintaPrincipal(const char * app_name, const char * absolute_path, int size, bool create);
	CintaPrincipal(const char * app_name, const char * absolute_path, int id_consumidor, int id_productor);
	virtual ~CintaPrincipal();

	void colocar_elemento(const T * elemento, int id_productor);
	void extraer_elemento();
	void avanzar_cinta(int id_consumidor);
	void leer_elemento(T * elemento, int id_consumidor);
};

template <typename T> CintaPrincipal<T>::CintaPrincipal(const char * app_name, const char * absolute_path, int size,
	bool create)
{

	int * tamanio_de_vector;
	int tamanio_shared_memory;
	int tamanio_control;
	int tamanio_id_productores;
	int tamanio_id_consumidores;
	int tamanio_dato;

	int i;

	std::vector<unsigned short> valores;
	if (create) {

		tamanio_control = 8 * sizeof(int);
		tamanio_id_productores = CANTIDAD_MAX_PRODUCTORES_CINTA_CENTRAL * sizeof(int);
		tamanio_id_consumidores = CANTIDAD_MAX_CONSUMIDORES_CINTA_CENTRAL * sizeof(int);
		tamanio_dato = sizeof(T);
		tamanio_shared_memory = tamanio_control + tamanio_id_productores + tamanio_id_consumidores
			+ size * tamanio_dato;
		//memoria_compartida = new SharedMemory(absolute_path, 0, tamanio_shared_memory, 0664, true, false);
		memoria_compartida = new MemoriaDistribuida(absolute_path, app_name, (char *)"cinta_principal", 0,
			TAMANIO_MEMORIA_CINTA_PRINCIPAL);
		//valores.push_back(1);
		//mutex = new SemaphoreSet(valores, absolute_path, 1, 0664);

		valores.clear();
		for (i = 0; i < CANTIDAD_MAX_PRODUCTORES_CINTA_CENTRAL ; i++) {
			valores.push_back(1);
		}
		//semaforo_productores = new SemaphoreSet(valores, absolute_path, 2, 0664);
		semaforo_productores = new SemaphoreSetDistribuido(valores, absolute_path, app_name, "cpp_sem_prod_",
			true);

		valores.clear();
		valores.push_back(1);
		for (i = 1; i < CANTIDAD_MAX_CONSUMIDORES_CINTA_CENTRAL ; i++) {
			valores.push_back(0);
		}
		//semaforo_consumidores = new SemaphoreSetDistribuido(valores, absolute_path, 3, 0664);
		semaforo_consumidores = new SemaphoreSetDistribuido(valores, absolute_path, app_name, "cpp_sem_cons_",
			true);
	}
	tamanio_de_vector = static_cast<int *>(memoria_compartida->memory_pointer());
	tamanio_vector = const_cast<const int *>(tamanio_de_vector);
	posicion_libre = tamanio_de_vector + 1;
	posicion_ocupada = posicion_libre + 1;
	cantidad_elementos = posicion_ocupada + 1;
	cantidad_productores = cantidad_elementos + 1;
	cantidad_consumidores = cantidad_productores + 1;
	cantidad_productores_esperando = cantidad_consumidores + 1;
	consumidor_esperando = cantidad_productores_esperando + 1;
	ids_productores_esperando = consumidor_esperando + 1;
	vector_elementos = reinterpret_cast<T *>(ids_productores_esperando + CANTIDAD_MAX_PRODUCTORES_CINTA_CENTRAL);

	// Inicializo el control de la memoria
	*tamanio_de_vector = size;
	*posicion_libre = 0;
	*posicion_ocupada = 0;
	*cantidad_elementos = 0;
	*cantidad_productores = CANTIDAD_MAX_PRODUCTORES_CINTA_CENTRAL;
	*cantidad_consumidores = CANTIDAD_MAX_CONSUMIDORES_CINTA_CENTRAL;
	*cantidad_productores_esperando = 0;
	*consumidor_esperando = 0;
	for (i = 0; i < CANTIDAD_MAX_PRODUCTORES_CINTA_CENTRAL ; i++) {
		ids_productores_esperando [i] = 0;
	}
}

template <typename T> CintaPrincipal<T>::CintaPrincipal(const char * app_name, const char * absolute_path,
	int id_consumidor, int id_productor)
{
	std::vector<unsigned short> valores;
	//int i;
	//this->memoria_compartida = new SharedMemory(absolute_path, 0, 0, false, false);
	memoria_compartida = new MemoriaDistribuida(absolute_path, app_name, (char *)"cinta_principal", 0,
		TAMANIO_MEMORIA_CINTA_PRINCIPAL);

	valores.clear();
	if (id_consumidor == CANTIDAD_MAX_CONSUMIDORES_CINTA_CENTRAL - 1) { //soy el ultimo consumidor
		valores.push_back(0);
	} else if (id_consumidor >= 0 && id_consumidor < CANTIDAD_MAX_CONSUMIDORES_CINTA_CENTRAL - 1) {
		valores.push_back(0); //soy consumidor
		valores.push_back(id_consumidor + 1);
	} else { //soy productor
		valores.push_back(0);
	}
	semaforo_consumidores = new SemaphoreSetDistribuido(valores, absolute_path, app_name, "semaforo_consumidores_",
		true);
	valores.clear();
	if (id_productor >= 0) { // soy productor
		valores.push_back(id_productor);
	} else { //soy consumidor
		for (unsigned short i ; i < CANTIDAD_MAX_PRODUCTORES_CINTA_CENTRAL ; i++) {
			valores.push_back(i);
		}
	}
	semaforo_productores = new SemaphoreSetDistribuido(valores, absolute_path, app_name, "semaforo_productores_", true);
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

		semaforo_productores->wait_on(id_productor - 1); // espera por si esta lleno

		//mutex->wait_on(0);
		memoria_compartida->lock();

		if (*this->cantidad_elementos < *this->tamanio_vector) { // puedo colocar
			memcpy(&(vector_elementos [*this->posicion_libre]), elemento, sizeof(T));
			*posicion_libre = (*posicion_libre + 1) % *this->tamanio_vector;
			(*this->cantidad_elementos)++;
			coloque = true;

			if (*this->cantidad_elementos == 1) { // estaba vacio
				if (*consumidor_esperando > 0) {
					*consumidor_esperando = 0;
					semaforo_consumidores->signalize(0);
				}
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
void CintaPrincipal<T>::avanzar_cinta(int id_consumidor) {
	if (id_consumidor < *this->cantidad_consumidores) {
		this->semaforo_consumidores->signalize(id_consumidor);
	} else {
		this->semaforo_consumidores->signalize(0);
	}
}

template <typename T>
void CintaPrincipal<T>::extraer_elemento() {
	int i;

	//mutex->wait_on(0);
	memoria_compartida->lock();

	*this->posicion_ocupada = (*this->posicion_ocupada + 1) % *this->tamanio_vector;
	(*this->cantidad_elementos)--;

	if (*cantidad_elementos == *tamanio_vector - 1) { // estaba lleno
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

	if (*this->cantidad_elementos == 0) {
		*this->consumidor_esperando = 1;
	} else {
		semaforo_consumidores->signalize(0);
	}

	//mutex->signalize(0);
	memoria_compartida->unlock();

}

template <typename T>
void CintaPrincipal<T>::leer_elemento(T * elemento, int id_consumidor) {
	bool leyo = false;

	while (!leyo) {

		this->semaforo_consumidores->wait_on(id_consumidor - 1);

		//mutex->wait_on(0);
		memoria_compartida->lock();

		if (*this->cantidad_elementos > 0) {
			leyo = true;
			memcpy((void *)elemento, &(vector_elementos [*this->posicion_ocupada]), sizeof(T));
		}

		if (*this->cantidad_elementos == 0) {
			*this->consumidor_esperando = 1;
		}

		//mutex->signalize(0);
		memoria_compartida->unlock();
	}
}

#endif /* CINTAPRINCIPAL_H_ */
