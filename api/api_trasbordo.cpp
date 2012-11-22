#include "api_trasbordo.h"
#include "api_constants.h"
#include "messagequeue.h"
#include <exception>
#include <stdexcept>

ApiTrasbordo::ApiTrasbordo(int id_robot, const char* path_to_trasbordo,
		const char* path_to_cinta_central) :
		id_robot(id_robot), path_to_trasbordo(path_to_trasbordo), cinta(path_to_cinta_central) {
}

ApiTrasbordo::~ApiTrasbordo() {
}

void ApiTrasbordo::poner_en_cinta_principal(const Equipaje& equipaje) {
	cinta.colocar_elemento(&equipaje, id_robot);
	//cinta.poner_valija(equipaje, 0);
}

int ApiTrasbordo::esperar_zona_asignada(int numero_vuelo) {
	numero_vuelo--;
	//cinta.colocar_elemento(&equipaje, id_robot);
	//cinta.poner_valija(equipaje, 0);
	return -1;
}

