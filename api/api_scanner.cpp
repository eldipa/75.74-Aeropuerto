#include "api_scanner.h"
#include <stdlib.h>

ApiScanner::ApiScanner(unsigned int numero_escaner, const char* path_to_cinta_central) :
		cinta(path_to_cinta_central, true) {
	char msgerror [1024];
	this->numero_escaner = numero_escaner;
	if (numero_escaner < 1) {
		sprintf(msgerror, "Error ID = %d invalido\n", numero_escaner);
		// TODO tirar ValueError
		//throw ErrorEnCintaPrincipal(msgerror);
		throw "ERROR";
	}
}

ApiScanner::~ApiScanner() {

}

void ApiScanner::colocar_equipaje_en_cinta_principal(const Equipaje & equipaje) {
	cinta.colocar_elemento(&equipaje, this->numero_escaner);
}
