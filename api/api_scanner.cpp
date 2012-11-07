#include "api_scanner.h"
#include <stdlib.h>

ApiScanner::ApiScanner(unsigned int numero_escaner,const char* path_to_cinta_central) :
		cinta(path_to_cinta_central, numero_escaner, true) {
	this->numero_escaner = numero_escaner;
}

ApiScanner::~ApiScanner() {

}

void ApiScanner::colocar_equipaje_en_cinta_principal(const Equipaje & equipaje) {
	cinta.colocar_elemento(&equipaje);
}
