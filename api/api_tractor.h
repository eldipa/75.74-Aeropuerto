#ifndef APITRACTOR_H_
#define APITRACTOR_H_

#include "messagequeue.h"
#include "api_constants.h"
#include "contenedor.h"
#include <vector>
#include "log.h"
#include <string>

class ApiTractor {
private:

	long id;
	MessageQueue cola_avion;
	MessageQueue cola_robot_zona;

public:
	ApiTractor(const char * directorio_de_trabajo, int id_tractor) :
			id(id_tractor), cola_avion(
					std::string(directorio_de_trabajo).append(PATH_COLA_TRACTORES_AVIONES).c_str(),
					0), cola_robot_zona(
					std::string(directorio_de_trabajo).append(PATH_COLA_ROBOTS_ZONA_TRACTORES).c_str(),
					0) {

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
		Log::debug("cargar_contenedor_en_avion: numero_vuelo=%d", vuelo);
		contenedor_para_avion.mtype = vuelo;
		contenedor_para_avion.cantidad_total_contenedores = cantidad_total_contenedores;
		contenedor_para_avion.contenedor = contenedor;
		cola_avion.push(&contenedor_para_avion, sizeof(ContenedorParaAvion) - sizeof(long));
	}
};

#endif /* APITRACTOR_H_ */
