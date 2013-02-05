#include "api_scanner.h"
#include "api_constants.h"
#include <stdlib.h>
#include <string>

ApiScanner::ApiScanner(const char* directorio_de_trabajo, int numero_escaner) :
		numero_escaner(numero_escaner), cinta(
				std::string(directorio_de_trabajo).append(PATH_CINTA_CENTRAL).c_str()) {
}

ApiScanner::~ApiScanner() {
}

void ApiScanner::colocar_equipaje_en_cinta_principal(const Equipaje & equipaje) {
	cinta.colocar_elemento(&equipaje, this->numero_escaner);
}
