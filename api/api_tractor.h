#ifndef APITRACTOR_H_
#define APITRACTOR_H_

#include "messagequeue.h"
#include "api_constants.h"
#include "contenedor.h"
#include <vector>
#include "log.h"

class ApiTractor {
private:

	long id;
	MessageQueue cola_avion;
	MessageQueue cola_robot_zona;

public:
	ApiTractor(int id_tractor, char * path_lock_cola_tractores, char * path_lock_cola_aviones) :
			id(id_tractor), cola_avion(path_lock_cola_aviones, 0), cola_robot_zona(
					path_lock_cola_tractores, 0) {

	}

	virtual ~ApiTractor() {

	}

	void obtener_contenedores_a_cargar(BloqueContenedores &contenedores) {
		cola_robot_zona.pull((void *) &contenedores, sizeof(BloqueContenedores) - sizeof(long),
				ID_ESCUCHA_TRACTOR);
		Log::debug("Bloque Recibido: numero_vuelo=%d cantidad_total_contenedores=%d",
				contenedores.vuelo_destino, contenedores.cantidad_total_contenedores);
	}

	void cargar_contenedor_en_avion(const Contenedor & contenedor, int vuelo,
			int cantidad_total_contenedores) {
		ContenedorParaAvion contenedor_para_avion;
		contenedor_para_avion.mtype = vuelo;
		contenedor_para_avion.cantidad_total_contenedores = cantidad_total_contenedores;
		contenedor_para_avion.contenedor = contenedor;
		cola_avion.push(&contenedor_para_avion, sizeof(ContenedorParaAvion) - sizeof(long));
	}
};

#endif /* APITRACTOR_H_ */
