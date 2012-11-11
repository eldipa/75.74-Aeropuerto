#include "api_despachante.h"
#include "log.h"
ApiDespachante::ApiDespachante(int numero_despachante, const char* path_to_cinta_central,
		int id_cinta_central) :
		cinta(path_to_cinta_central, id_cinta_central) {
	/*
	 char msgerror[1024];
	 this->numero_despachante = numero_despachante;
	 if (numero_despachante < 1) {
	 sprintf(msgerror, "Error ID = %d invalido\n", numero_despachante);
	 // TODO tirar ValueError
	 //throw ErrorEnCintaPrincipal(msgerror);
	 throw "ERROR";
	 }
	 */
	this->numero_despachante = numero_despachante;
	this->saco_elemento = true;

}

ApiDespachante::~ApiDespachante() {

}

Rfid ApiDespachante::leer_proximo_equipaje() {

	if (!saco_elemento) {
		this->saco_elemento = true;
		//Log::debug("Avanzando cinta");
		cinta.avanzar_cinta(this->numero_despachante);
	}
	//Log::debug("Leyendo de cinta");
	cinta.leer_elemento(&ultimo_equipaje_leido, this->numero_despachante);

	this->saco_elemento = false; // Tiene que avanzar o extraer el elemento leido

	return ultimo_equipaje_leido.getRfid();

}

Equipaje ApiDespachante::extraer_equipaje() {

	cinta.extraer_elemento();
	this->saco_elemento = true;
	return this->ultimo_equipaje_leido;

}

void ApiDespachante::avanzar_cinta() {

	if (!saco_elemento) {
		cinta.avanzar_cinta(this->numero_despachante);
		this->saco_elemento = true;
	}

}

/*
 const Equipaje& ApiDespachante::obtener_valija_para(int robot_id) {
 return cinta.obtener_valija_para(robot_id);
 }

 void ApiDespachante::mover_valija(int desde_robot_id, int hasta_robot_id) {
 cinta.mover_valija(desde_robot_id, hasta_robot_id);
 }

 void ApiDespachante::listo_para_recibir_valija_para(int robot_id) {
 cinta.listo_para_recibir_valija_para(robot_id);
 }

 void ApiDespachante::poner_valija(const Equipaje& e, int robot_id) {
 cinta.poner_valija(e, robot_id);
 }
 */
