#include "api_trasbordo.h"
#include "api_constants.h"
#include "messagequeue.h"
#include <exception>
#include <stdexcept>

VueloTrasbordo ApiTrasbordo::proximo_vuelo_trasbordo() {
	MessageQueue work_queue(path_to_trasbordo, Q_TRASBORDO_LISTO);
	VueloTrasbordo proximo_trasbordo;

	if (work_queue.pull(&proximo_trasbordo, sizeof(VueloTrasbordo), id_robot) == sizeof(VueloTrasbordo)) {
		return proximo_trasbordo;
	}
	throw std::runtime_error("Error al sacar de la cola de vuelos de trasbordo");

}

void ApiTrasbordo::poner_en_cinta_principal(const Equipaje& equipaje) {
	cinta.colocar_elemento(&equipaje, id_robot);
	//cinta.poner_valija(equipaje, 0);
}

ApiTrasbordo::ApiTrasbordo(int id_robot, const char* path_to_trasbordo, const char* path_to_cinta_central) :
		id_robot(id_robot), path_to_trasbordo(path_to_trasbordo), cinta(path_to_cinta_central) {
}
ApiTrasbordo::~ApiTrasbordo() {
}
