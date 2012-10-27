#include "api_cinta.h"

ApiCinta::ApiCinta(int numero) : numero(numero) {}

ApiCinta::~ApiCinta() {}

Equipaje & ApiCinta::sacar_equipaje() {
	Equipaje *equipaje;
	equipaje = new Equipaje(1);
	return *equipaje;
}

void ApiCinta::poner_equipaje(Equipaje & equipaje) {
	if (equipaje.getRfid().rfid == 0) {

	}
}