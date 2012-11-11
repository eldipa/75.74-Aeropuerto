#ifndef APITRACTOR_H_
#define APITRACTOR_H_

#include "messagequeue.h"
#include "api_constants.h"
#include "contenedor.h"
#include <vector>



class ApiTractor {
private:

	long id;
	MessageQueue cola_avion;
	MessageQueue cola_robot_zona;

public:
	ApiTractor(int id_tractor) :
			id(id_tractor), cola_avion(PATH_COLA_TRACTORES_AVIONES, 0), cola_robot_zona(
					PATH_COLA_ROBOTS_ZONA_TRACTORES, 0) {

	}

	virtual ~ApiTractor() {

	}

	BloqueContenedores obtener_contenedores_a_cargar() {
		BloqueContenedores contenedores;
		cola_robot_zona.pull(&contenedores, sizeof(BloqueContenedores), ID_ESCUCHA_TRACTOR);
		return contenedores;
	}

	void cargar_contenedor_en_avion(const Contenedor & contenedor, int vuelo,
			int cantidad_total_contenedores) {
		ContenedorParaAvion contenedor_para_avion;
		contenedor_para_avion.mtype = vuelo;
		contenedor_para_avion.cantidad_total_contenedores = cantidad_total_contenedores;
		contenedor_para_avion.contenedor = contenedor;
		cola_avion.push(&contenedor_para_avion, sizeof(ContenedorParaAvion));
	}
};

#endif /* APITRACTOR_H_ */
