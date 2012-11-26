#ifndef API_CARGA_H_
#define API_CARGA_H_

#include "equipaje.h"
#include "contenedor.h"
#include "messagequeue.h"
#include "api_constants.h"
#include "log.h"
#include "semaphoreset.h"
#include "sharedmemory.h"
#include "mutex.h"
#include "cintas.h"
#include <vector>
#include <string>

class ApiCarga {
private:
	//std::string path;
	int id_robot_carga;
	std::vector<Contenedor> contenedores;
	MessageQueue cola_tractores;
	CintaContenedor cinta_contenedor;

	int * cerro_el_checkin;
	int * cant_equipaje_total;
	//int * cant_equipaje_restante;

public:

	/*
	 * Api que maneja la sincronizacion y el pasaje de contenedores entre el robot_carga y el controlador_de_carga.
	 * El primer constructor crea los ipcs.Se llama solo una vez por cada controlador_de_carga.
	 * recibe un path_carga y el id_robot_carga.Va a existir una ApiCarga por cada robot_carga.
	 **/
	ApiCarga(const char * directorio_de_trabajo,int id_robot_carga, int num_cinta) :
			id_robot_carga(id_robot_carga), cola_tractores(std::string(directorio_de_trabajo).append(PATH_COLA_ROBOTS_ZONA_TRACTORES).c_str(), 0), cinta_contenedor(
					std::string(directorio_de_trabajo).append(PATH_CINTA_CONTENEDOR).c_str(), num_cinta) {

	}

	~ApiCarga() {
	}

	/*
	 * Bloquea hasta que el robot carga cargue n equipaejes en contenedores.
	 * No bloquea y retorna un vector vacío si el robot_carga esta ocupado en otra carga.
	 * En caso contrario devulve los contenedores con equipajes.
	 * Cada contenedor guarda los rfids
	 **/
	std::vector<Contenedor> cargar_equipajes(int n) {
		n = n - 1;
		return std::vector<Contenedor>();
	}

	/* API para el controlador de carga */

	void esperar_avion_en_zona() {

	}

	bool checkin_cerrado() {
		return cinta_contenedor.checkin_ya_cerro();
	}

	Equipaje sacar_equipaje() {
		return cinta_contenedor.sacar_equipaje(id_robot_carga);
	}

	int obtener_cantidad_equipaje_total() {
		return cinta_contenedor.cantidad_valijas_totales();
	}

	/*
	 * Guarda el contenedor con equipajes recibidos por el robot_carga.
	 * Guarda los contenedores en una sharedmemory
	 **/
	void agregar_contenedor_cargado(Contenedor& c) {
		contenedores.push_back(c);
	}

	/* Api que llama el robot de zona para entregarle los contenedores
	 * a los tractores
	 */
	void enviar_contenedores_a_avion(int numero_de_vuelo) {
		BloqueContenedores bloque;
		int cantidad_total_contenedores;
		int cant_bloques;
		int i, j;

		cantidad_total_contenedores = contenedores.size();
		bloque.mtype = ID_ESCUCHA_TRACTOR;
		bloque.cantidad_total_contenedores = cantidad_total_contenedores;
		bloque.vuelo_destino = numero_de_vuelo;

		cant_bloques = cantidad_total_contenedores / MAX_CONTENEDORES_POR_TRACTOR;
		if (cantidad_total_contenedores > cant_bloques * MAX_CONTENEDORES_POR_TRACTOR) {
			cant_bloques++;
		}

		for (i = 0; i < cant_bloques; i++) {
			bloque.contenedores_actual = 0;
			for (j = 0; j < MAX_CONTENEDORES_POR_TRACTOR; j++) {
				bloque.contenedores[j] = contenedores[i * MAX_CONTENEDORES_POR_TRACTOR + j];
				bloque.contenedores_actual++;
			}
			Log::info("Enviando Bloque %d/%d a tractores tamaño %d #vuelo= %d", i + 1, cant_bloques,
					sizeof(BloqueContenedores), numero_de_vuelo);
			cola_tractores.push((const void *) &bloque, sizeof(BloqueContenedores) - sizeof(long));
		}
	}


private:

};

#endif
