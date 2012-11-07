#include "api_despachante.h"

ApiDespachante::ApiDespachante(int numero_despachante, const char* path_to_cinta_central) :
		cinta(path_to_cinta_central, false) {
	char msgerror[1024];
	this->numero_despachante = numero_despachante;
	if (numero_despachante < 1) {
		sprintf(msgerror, "Error ID = %d invalido\n", numero_despachante);
		// TODO tirar ValueError
		//throw ErrorEnCintaPrincipal(msgerror);
		throw "ERROR";
	}
	this->saco_elemento = true;
}

ApiDespachante::~ApiDespachante() {

}

Rfid ApiDespachante::leer_proximo_equipaje() {

	if (!saco_elemento) {
		this->saco_elemento = true;
		cinta.avanzar_cinta(this->numero_despachante);
	}
	cinta.leer_elemento(&ultimo_equipaje_leido, this->numero_despachante);

	this->saco_elemento = false; // Tiene que avanzar o extraer el elemento leido

	return ultimo_equipaje_leido.getRfid();
}

Equipaje ApiDespachante::extraer_equipaje() {
	cinta.extraer_elemento();
	this->saco_elemento = true;
	return this->ultimo_equipaje_leido;
}

