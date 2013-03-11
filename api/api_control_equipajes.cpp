#include "api_control_equipajes.h"
#include "api_constants.h"

#include "api_configuracion.h"
#include <string>

ApiControlEquipajes::ApiControlEquipajes(const char * directorio_de_trabajo, const char* config_file,
	int pos_consumidor_cinta_central, int pos_productor_cinta_central, bool create)
	: 
		clnt_torre_de_control(clnt_create(ApiConfiguracion::get_torre_ip(config_file).c_str(), TORREDECONTROLPROG, TORREDECONTROLVERS, "tcp")),
      pos_consumidor_cinta_central(pos_consumidor_cinta_central),
		pos_productor_cinta_central(pos_productor_cinta_central),
		cinta_central(std::string("control_equipaje").append((pos_productor_cinta_central == -1) ? "cons":"prod").c_str(), std::string(directorio_de_trabajo).c_str(),
			pos_consumidor_cinta_central - 1, pos_productor_cinta_central - 1, create)
#if DEBUG_ROBOT_CONTROL_EXTRAE_CINTA_PRINCIPAL == 0 || DEBUG_ROBOT_CONTROL_COLOCA_CINTA_PRINCIPAL == 1
			, queue_manager(ApiConfiguracion::get_queue_manager(directorio_de_trabajo, config_file)),
		queue_to_control_sospechosos(queue_manager->get_queue(PATH_CONTROL_SOSPECHOSOS, 1, create))
#endif
{
#if DEBUG_ROBOT_CONTROL_EXTRAE_CINTA_PRINCIPAL == 1
	config_file = config_file + 1 - 1;
#endif
	create = !!create;
}

ApiControlEquipajes::~ApiControlEquipajes() {
}

Equipaje ApiControlEquipajes::obtener_proximo_equipaje_sospechoso() {
	Equipaje e;
	cinta_central.leer_elemento(&e, this->pos_consumidor_cinta_central);
	cinta_central.extraer_elemento();
	return e;

}

void ApiControlEquipajes::volver_a_colocar_equipaje_en_cinta_principal(const Equipaje & e) {
	//cinta_central.poner_valija(e, 0);
	cinta_central.colocar_elemento(&e, this->pos_productor_cinta_central);
}

void ApiControlEquipajes::enviar_equipaje_a_control(Equipaje& e) {
	tMsgSospechoso msg;
	msg.mtype = 1;
	msg.equipaje = e;
	queue_to_control_sospechosos->push(&msg, sizeof(tMsgSospechoso) - sizeof(long));
}

Equipaje ApiControlEquipajes::get_equipaje_a_controlar() {
	tMsgSospechoso msg;
	queue_to_control_sospechosos->pull(&msg, sizeof(tMsgSospechoso) - sizeof(long));
	return msg.equipaje;
}

int ApiControlEquipajes::get_zona(int num_vuelo) {

	int* num_zona = get_zona_1(&num_vuelo, clnt_torre_de_control);

	if (num_zona != NULL) {
		Log::info("ApiControlEquipajes obtuve resultado del rpc server: zona del vuelo %d es %d", num_vuelo, (*num_zona));
	} else {
		throw GenericError("ApiControlEquipajes: Error al llamar al métdoo remoto desasignar _vuelo_1 sobre el rpc server");
	}

	return (*num_zona);

}
