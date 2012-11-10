#ifndef API_AVION_H_
#define API_AVION_H_

#include <vector>
#include <string>

#include "equipaje.h"
#include "pasajero.h"
#include "contenedor.h"
#include "api_constants.h"
#include "messagequeue.h"

class ApiAvion {
private:

	int numero_de_vuelo;
	MessageQueue cola_tractor;
	MessageQueue cola_robot_zona; // O controlador

public:
	ApiAvion(int numero_de_vuelo) :
			cola_tractor(PATH_COLA_TRACTORES_AVIONES, 0), cola_robot_zona(
					PATH_COLA_AVIONES_ROBOTS_ZONA, 0) {
		this->numero_de_vuelo = numero_de_vuelo;
	}

	virtual ~ApiAvion() {

	}

	/*
	 * Registra que el pasajero ya subio al avion
	 **/
	void subir_pasajero(int num_avion, Pasajero&);

	/*
	 * Sube el contenedor al avion
	 **/
	void subir_contenedor(int num_avion, Contenedor&);

	void notificar_avion_en_zona(){

	}

	/*
	 * Bloquea hasta que el controlador de carga libera la zona y habilita el despegue
	 * Se llama una vez que llegaron todos vuelo de intercargo, subieron todos los equipajes
	 * y los pasajeros ( los de chckin y los de vuelos intermedios )
	 **/
	void esperar_habilitacion_despegue(int num_avion);

	/*
	 * Desbloquea al avion, o permite el despegue
	 **/
	void notificar_depegue() {

	}

	/*
	 * Devuelve True si el equipaje esta arriba del avion
	 **/
	bool equipaje_cargado(Equipaje&);

	/*
	 * Devuelve True si el pasajero esta arriba del avion
	 **/
	bool subio_pasajero(Pasajero&);

	std::vector<Contenedor> esperar_carga_equipajes() {
		std::vector<Contenedor> contenedores;
		Contenedor contenedor;
		ContenedorParaAvion contenedor_avion;
		int cantidad_contenedores_totales;
		int cantidad_contenedores_actual;

		cantidad_contenedores_actual = 0;
		cantidad_contenedores_totales = 0;

		do {

			cola_tractor.pull(&contenedor_avion, sizeof(ContenedorParaAvion),
					this->numero_de_vuelo);

			cantidad_contenedores_actual++;
			cantidad_contenedores_totales = contenedor_avion.cantidad_total_contenedores;
			contenedores.push_back(contenedor_avion.contenedor);

		} while (cantidad_contenedores_actual < cantidad_contenedores_totales);

		return contenedores;
	}

};

#endif /* API_AVION_ */
