#include "api_trasbordo.h"

VueloTrasbordo ApiTrasbordo::proximo_vuelo_trasbordo() {
	throw std::runtime_error("Unimplemented");
}

void ApiTrasbordo::poner_en_cinta_principal(const Equipaje& equipaje) {
	cinta.poner_valija(equipaje, 0);
}

ApiTrasbordo::ApiTrasbordo(int id_robot, const char* path_to_trasbordo, const char* path_to_cinta_central, int id_cinta_central) :
	id_robot(id_robot), path_to_trasbordo(path_to_trasbordo),
	cinta(path_to_cinta_central, id_cinta_central) { }
ApiTrasbordo::~ApiTrasbordo() {}
