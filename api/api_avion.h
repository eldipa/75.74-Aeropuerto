#ifndef API_AVION_H_
#define API_AVION_H_

#include <vector>
#include <string>

#include "equipaje.h"
#include "pasajero.h"
#include "contenedor.h"
#include "api_constants.h"
#include "messagequeue.h"

#include "log.h"

class ApiAvion {
private:

	int numero_de_vuelo;
	MessageQueue cola_tractor;

public:
	ApiAvion(char * directorio_de_trabajo, int numero_de_vuelo) :
			cola_tractor(
					std::string(directorio_de_trabajo).append(PATH_COLA_TRACTORES_AVIONES).c_str(),
					0) {
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

	void notificar_avion_en_zona() {

	}

	/*
	 * Bloquea hasta que el controlador de carga libera la zona y habilita el despegue
	 * Se llama una vez que llegaron todos vuelo de intercargo, subieron todos los equipajes
	 * y los pasajeros ( los de chckin y los de vuelos intermedios )
	 **/
	void esperar_habilitacion_despegue(int num_avion);

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
		bool mostrar_cant_contenedores = true;

		cantidad_contenedores_actual = 0;
		cantidad_contenedores_totales = 0;

		do {
			Log::info("esperando equipaje", this->numero_de_vuelo);
			cola_tractor.pull(&contenedor_avion, sizeof(ContenedorParaAvion) - sizeof(long),
					this->numero_de_vuelo);
			if (mostrar_cant_contenedores) {
				Log::info("contenedores total %d", this->numero_de_vuelo,
						contenedor_avion.cantidad_total_contenedores);
				mostrar_cant_contenedores = false;
			}
			cantidad_contenedores_actual++;
			Log::info("llegaron %d contenedores", this->numero_de_vuelo,
					cantidad_contenedores_actual);
			cantidad_contenedores_totales = contenedor_avion.cantidad_total_contenedores;
			contenedores.push_back(contenedor_avion.contenedor);

		} while (cantidad_contenedores_actual < cantidad_contenedores_totales);

		return contenedores;
	}

};

#endif /* API_AVION_ */
