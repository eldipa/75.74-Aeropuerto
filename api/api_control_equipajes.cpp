
#include "api_control_equipajes.h"
#include "api_constants.h"

#include "api_configuracion.h"
#include <string>

ApiControlEquipajes::ApiControlEquipajes(const char * directorio_de_trabajo,int pos_consumidor_cinta_central,int pos_productor_cinta_central, bool create) :
   pos_consumidor_cinta_central(pos_consumidor_cinta_central), 
   pos_productor_cinta_central(pos_productor_cinta_central), 
   cinta_central(std::string(directorio_de_trabajo).append(PATH_CINTA_CENTRAL).c_str()),

   queue_manager( ApiConfiguracion::get_queue_manager(directorio_de_trabajo) ),
   queue_to_control_sospechosos( queue_manager->get_queue(PATH_CONTROL_SOSPECHOSOS, 1,  create ) ) {

   create = create;
}

ApiControlEquipajes::ApiControlEquipajes(const char * directorio_de_trabajo,int pos_consumidor_cinta_central,int pos_productor_cinta_central) :
      pos_consumidor_cinta_central(pos_consumidor_cinta_central), 
      pos_productor_cinta_central(pos_productor_cinta_central), 
      cinta_central(std::string(directorio_de_trabajo).append(PATH_CINTA_CENTRAL).c_str()),

      queue_manager( ApiConfiguracion::get_queue_manager(directorio_de_trabajo) ),
      queue_to_control_sospechosos( queue_manager->get_queue(PATH_CONTROL_SOSPECHOSOS, 1) ) {
   
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
   queue_to_control_sospechosos->push(&msg, sizeof(tMsgSospechoso)-sizeof(long));
}

Equipaje ApiControlEquipajes::get_equipaje_a_controlar() {
   tMsgSospechoso msg;
   queue_to_control_sospechosos->pull(&msg, sizeof(tMsgSospechoso)-sizeof(long));
   return msg.equipaje;
}
