#include "api_despachante.h"

ApiDespachante::ApiDespachante(unsigned int numero_despachante, const char* path_to_cinta_central) :
		cinta(path_to_cinta_central, numero_despachante, false) {
	this->numero_despachante = numero_despachante;
	terminar = false;
}

ApiDespachante::~ApiDespachante() {

}

Equipaje ApiDespachante::obtener_proximo_equipaje(const std::set<int> & vuelos_a_despachar) {
	Equipaje e;
	bool elemento_extraido;
	elemento_extraido = false;

	if (terminar) {
		return e;
	}
	do {
		cinta.leer_elemento(&e);
		if (e.getRfid().numero_de_vuelo_destino == 0) {
			cinta.avanzar_cinta();
			this->terminar = true;
			//} else if (rfid.getNumeroDeVuelo() == e.get_rfid().getNumeroDeVuelo()) {
		} else if (vuelos_a_despachar.count(e.getRfid().numero_de_vuelo_destino) > 0) {
			cinta.extraer_elemento();
			elemento_extraido = true;
		} else {
			cinta.avanzar_cinta();
		}
	} while (!elemento_extraido && !terminar);
	return e;
}
