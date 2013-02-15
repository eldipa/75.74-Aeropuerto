#ifndef CINTASCANNER_H_
#define CINTASCANNER_H_


#include <vector>
#include <cstring>
#include "sharedmemory.h"
#include "semaphoreset.h"
#include "valueerror.h"
#include "api_constants.h"

// 1 productor N consumidores 1 cinta(shared memory) por consumidor
template<typename T>
class CintaScanner {
private:
	SharedMemory * cintas[MAX_SCANNERS];
	SharedMemory * control;
	SemaphoreSet * semaforos;

	// SEMAFOROS
	// 0 Mutex información de control
	// 1 Mutex Cinta Scanner 1
	// 2 Mutex Cinta Scanner 2
	// 3 Mutex Cinta Scanner 3
	// 4 Mutex Cinta Scanner 4
	// 5 Semaforo Espera Productor
	// 6 Semaforo Espera Scanner 1
	// 7 Semaforo Espera Scanner 2
	// 8 Semaforo Espera Scanner 3
	// 9 Semaforo Espera Scanner 4

	const int * tamanio_vector[MAX_SCANNERS];
	int * posicion_libre[MAX_SCANNERS];
	int * posicion_ocupada[MAX_SCANNERS];
	int * cantidad_elementos[MAX_SCANNERS];
	int * consumidor_esperando[MAX_SCANNERS];
	T * vector_elementos[MAX_SCANNERS];

	const static int cant_ipc = 6;
	int proxima_cinta_a_colocar;
	int numero_cinta;

	int * cantidad_de_escaners_activos;
	int * ids_escaners_activos;
	int * cinta_llena;
	int * productor_esperando;


	void asignar_punteros(void * memory_pointer, int numero_cinta);
	void inicializar_estructuras(int tamanio_vectores);

public:
	// Se llama a este si se tiene que construir
	CintaScanner(const char* path_to_cinta_scanner, int id_cinta, int tamanio_cintas, bool create);

	// Se llama a este si es el productor de la cinta (robot_checkin)
	CintaScanner(const char* path_to_cinta_scanner, int id_cinta);

	// Se llama a este constructor si es el consumidor de la cinta (robot_scanner)
	CintaScanner(const char* path_to_cinta_scanner, int id_cinta, int numero_escaner);
	virtual ~CintaScanner();

	void poner_equipaje(const T & equipaje);

	T sacar_equipaje();
};

template<typename T>
CintaScanner<T>::CintaScanner(const char* path_to_cinta_scanner, int id_cinta, int tamanio_cintas,
		bool create) {
	int i;
	int inicio_ids = (id_cinta - 1) * cant_ipc;
	int tamanio_total;
	int tamanio_control;

	tamanio_total = sizeof(int) * 5;
	tamanio_total += tamanio_cintas * sizeof(T);

	tamanio_control = sizeof(int) * 2;
	tamanio_control += sizeof(int) * MAX_SCANNERS * 2;

	if (create) {
		control = new SharedMemory(path_to_cinta_scanner, inicio_ids, tamanio_control, 0664, true,
				false);

		for (i = 0; i < MAX_SCANNERS; i++) {
			cintas[i] = new SharedMemory(path_to_cinta_scanner, i + inicio_ids + 1, tamanio_total,
					0664, true, false);
			asignar_punteros(cintas[i]->memory_pointer(), i);
		}

		semaforos = new SemaphoreSet(std::vector<short unsigned int>(2 + 2 * MAX_SCANNERS, 1),
				path_to_cinta_scanner, inicio_ids + MAX_SCANNERS + 1, 0664);
		cantidad_de_escaners_activos = static_cast<int *>(control->memory_pointer());
		ids_escaners_activos = cantidad_de_escaners_activos + 1;
		cinta_llena = ids_escaners_activos + MAX_SCANNERS;
		productor_esperando = cinta_llena + MAX_SCANNERS;
		inicializar_estructuras(tamanio_cintas);
		proxima_cinta_a_colocar = 0;
		numero_cinta = -1;
	}
}

template<typename T>
CintaScanner<T>::CintaScanner(const char* path_to_cinta_scanner, int id_cinta) {
	int i;
	int inicio_ids = (id_cinta - 1) * cant_ipc;
	control = new SharedMemory(path_to_cinta_scanner, inicio_ids, 0, 0, false, false);

	for (i = 0; i < MAX_SCANNERS; i++) {
		cintas[i] = new SharedMemory(path_to_cinta_scanner, i + inicio_ids + 1, 0, 0, false, false);
		asignar_punteros(cintas[i]->memory_pointer(), i);
	}

	semaforos = new SemaphoreSet(path_to_cinta_scanner, inicio_ids + MAX_SCANNERS + 1, 0, 0);

	cantidad_de_escaners_activos = static_cast<int *>(control->memory_pointer());
	ids_escaners_activos = cantidad_de_escaners_activos + 1;
	cinta_llena = ids_escaners_activos + MAX_SCANNERS;
	productor_esperando = cinta_llena + MAX_SCANNERS;

	proxima_cinta_a_colocar = 0;
	numero_cinta = -1;
}

template<typename T>
CintaScanner<T>::CintaScanner(const char* path_to_cinta_scanner, int id_cinta, int numero_escaner) {
	int i;
	int inicio_ids = (id_cinta - 1) * cant_ipc;

	for (i = 0; i < MAX_SCANNERS; i++) {
		this->cintas[i] = NULL;
	}

	control = new SharedMemory(path_to_cinta_scanner, inicio_ids, 0, 0, false, false);

	cintas[numero_escaner - 1] = new SharedMemory(path_to_cinta_scanner,
			numero_escaner + inicio_ids, 0, 0, false, false);

	semaforos = new SemaphoreSet(path_to_cinta_scanner, inicio_ids + MAX_SCANNERS + 1, 0, 0);

	asignar_punteros(cintas[numero_escaner - 1]->memory_pointer(), numero_escaner - 1);

	cantidad_de_escaners_activos = static_cast<int *>(control->memory_pointer());
	ids_escaners_activos = cantidad_de_escaners_activos + 1;
	cinta_llena = ids_escaners_activos + MAX_SCANNERS;
	productor_esperando = cinta_llena + MAX_SCANNERS;

	this->numero_cinta = numero_escaner - 1;

	semaforos->wait_on(0);

	if (ids_escaners_activos[this->numero_cinta] == numero_escaner) {
		semaforos->signalize(0);
		delete this->semaforos;
		delete this->control;
		delete this->cintas[this->numero_cinta];
		throw ValueError("El escaner %d ya estába trabajando", numero_escaner);
	} else {
		ids_escaners_activos[this->numero_cinta] = numero_escaner;
		(*cantidad_de_escaners_activos)++;

		cinta_llena[numero_cinta] = 0;
		if (*productor_esperando == 1) {
			*productor_esperando = 0;
			semaforos->signalize(MAX_SCANNERS + 1);
		}
	}
	semaforos->signalize(0);
}

template<typename T>
CintaScanner<T>::~CintaScanner() {
	int i;
	if (numero_cinta != -1) {
		ids_escaners_activos[this->numero_cinta] = 0;
		(*cantidad_de_escaners_activos)--;
		cinta_llena[numero_cinta] = -1;
	}
	if (this->control) {
		delete this->control;
		this->control = NULL;
	}
	if (this->semaforos) {
		delete this->semaforos;
		this->semaforos = NULL;
	}
	for (i = 0; i < MAX_SCANNERS; i++) {
		if (this->cintas[i]) {
			delete this->cintas[i];
			this->cintas[i] = NULL;
		}
	}
}

template<typename T>
void CintaScanner<T>::asignar_punteros(void * memory_pointer, int numero_cinta) {
	tamanio_vector[numero_cinta] = static_cast<const int *>(memory_pointer);
	posicion_libre[numero_cinta] = const_cast<int *>(tamanio_vector[numero_cinta] + 1);
	posicion_ocupada[numero_cinta] = posicion_libre[numero_cinta] + 1;
	cantidad_elementos[numero_cinta] = posicion_ocupada[numero_cinta] + 1;
	consumidor_esperando[numero_cinta] = cantidad_elementos[numero_cinta] + 1;
	vector_elementos[numero_cinta] = reinterpret_cast<T *>(consumidor_esperando[numero_cinta] + 1);
}

template<typename T>
void CintaScanner<T>::inicializar_estructuras(int tamanio_vectores) {
	int i;
	int * tamanio;
	for (i = 0; i < MAX_SCANNERS; i++) {
		tamanio = const_cast<int *>(tamanio_vector[i]);
		*tamanio = tamanio_vectores;
		*posicion_libre[i] = 0;
		*posicion_ocupada[i] = 0;
		*cantidad_elementos[i] = 0;
		*consumidor_esperando[i] = 0;
		ids_escaners_activos[i] = 0;
		cinta_llena[i] = -1;
	}
	*cantidad_de_escaners_activos = 0;
	*productor_esperando = 0;
}

template<typename T>
void CintaScanner<T>::poner_equipaje(const T & equipaje) {
	bool coloque = false;
	int cantidad_cintas_llenas;
	int * posicion;
	T * destino;

	while (!coloque) {

		semaforos->wait_on(MAX_SCANNERS + 1);

		semaforos->wait_on(0); // Verifica si las cintas estan llenas

		cantidad_cintas_llenas = 0;

		while (cantidad_cintas_llenas < MAX_SCANNERS && cinta_llena[proxima_cinta_a_colocar] != 0) {
			proxima_cinta_a_colocar = (proxima_cinta_a_colocar + 1) % MAX_SCANNERS;
			cantidad_cintas_llenas++;
		}
		if (cantidad_cintas_llenas == MAX_SCANNERS) {
			*this->productor_esperando = 1;
		}

		semaforos->signalize(0);

		if (cantidad_cintas_llenas < MAX_SCANNERS) {
			coloque = true;
			semaforos->wait_on(proxima_cinta_a_colocar + 1);
			posicion = this->posicion_libre[proxima_cinta_a_colocar];
			destino = &((vector_elementos[proxima_cinta_a_colocar])[*posicion]);
			memcpy(destino, &equipaje, sizeof(T));
			*posicion_libre[proxima_cinta_a_colocar] =
					(*posicion_libre[proxima_cinta_a_colocar] + 1)
							% *this->tamanio_vector[proxima_cinta_a_colocar];
			(*this->cantidad_elementos[proxima_cinta_a_colocar])++;

			if (*this->cantidad_elementos[proxima_cinta_a_colocar] == 1) {
				if (*consumidor_esperando[proxima_cinta_a_colocar] == 1) {
					semaforos->signalize(proxima_cinta_a_colocar + MAX_SCANNERS + 2);
					*consumidor_esperando[proxima_cinta_a_colocar] = 0;
				}
			}

			if (*this->cantidad_elementos[proxima_cinta_a_colocar]
					== *this->tamanio_vector[proxima_cinta_a_colocar]) {
				this->semaforos->wait_on(0);
				cinta_llena[proxima_cinta_a_colocar] = 1;
				this->semaforos->signalize(0);
			}
			semaforos->signalize(proxima_cinta_a_colocar + 1);

			proxima_cinta_a_colocar = (proxima_cinta_a_colocar + 1) % MAX_SCANNERS;

			semaforos->signalize(MAX_SCANNERS + 1);
		}

	}
}

template<typename T>
T CintaScanner<T>::sacar_equipaje() {
	T elemento;
	bool extrajo = false;

	while (!extrajo) {

		this->semaforos->wait_on(numero_cinta + MAX_SCANNERS + 2);

		this->semaforos->wait_on(numero_cinta + 1);

		if (*cantidad_elementos[numero_cinta] > 0) {
			extrajo = true;
			memcpy((void *) &elemento,
					&(vector_elementos[numero_cinta][*this->posicion_ocupada[numero_cinta]]),
					sizeof(T));
			*this->posicion_ocupada[numero_cinta] = (*this->posicion_ocupada[numero_cinta] + 1)
					% *this->tamanio_vector[numero_cinta];
			(*this->cantidad_elementos[numero_cinta])--;

			if (*cantidad_elementos[numero_cinta] == *tamanio_vector[numero_cinta] - 1) { // estaba lleno
				this->semaforos->wait_on(0);
				if (cinta_llena[numero_cinta] == 1) {
					cinta_llena[numero_cinta] = 0;
					if (*productor_esperando == 1) {
						*productor_esperando = 0;
						semaforos->signalize(MAX_SCANNERS + 1);
					}
				}
				this->semaforos->signalize(0);
			}
		}

		if (*cantidad_elementos[numero_cinta] == 0) {
			*this->consumidor_esperando[numero_cinta] = 1;
		} else {
			this->semaforos->signalize(numero_cinta + MAX_SCANNERS + 2);
		}

		this->semaforos->signalize(numero_cinta + 1);
	}
	return elemento;
}

#endif /* CINTASCANNER_H_ */
