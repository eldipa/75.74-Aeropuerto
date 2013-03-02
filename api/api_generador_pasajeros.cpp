#include "api_generador_pasajeros.h"

#include "api_checkin.h"
#include "api_configuracion.h"
#include "mensajes.h"

ApiGeneradorPasajeros::ApiGeneradorPasajeros(const char* directorio_de_trabajo, const char* config_file) :
   queue_manager( ApiConfiguracion::get_queue_manager(directorio_de_trabajo, config_file ) ),
   queue_pasajeros( queue_manager->get_queue(PATH_PUESTO_CHECKIN, 0) ) {
}

ApiGeneradorPasajeros::~ApiGeneradorPasajeros() {
}

void ApiGeneradorPasajeros::llego_pasajero_para_checkin(int id_pasajero, int id_puesto_checkin, const std::vector<Equipaje>& equipajes) {
	tMensajePasajeroCheckin msg;
	msg.mtype = (long) id_puesto_checkin;
	msg.id_pasajero = id_pasajero;
	msg.cant_equipajes = 0;

	if (equipajes.size() > MAX_EQUIPAJES_POR_PERSONA)
		throw std::runtime_error(
				"llego_pasajero_para_checkin:intentando checkin con mas vliajs que MAX_EQUIPAJES_POR_PERSONA");

	std::vector<Equipaje>::const_iterator it = equipajes.begin();
	for (; it != equipajes.end(); it++) {
		msg.equipajes[msg.cant_equipajes] = (*it);
		msg.cant_equipajes++;
	}

	queue_pasajeros->push(&msg, sizeof(tMensajePasajeroCheckin)-sizeof(long));
}
