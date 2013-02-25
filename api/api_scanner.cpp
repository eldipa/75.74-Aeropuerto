#include "api_scanner.h"
#include "api_constants.h"
#include <stdlib.h>
#include <string>
#include <sstream>
#include "api_common.h"


ApiScanner::ApiScanner(const char* directorio_de_trabajo, const char* config_file, int numero_escaner)
	: numero_escaner(numero_escaner),
		cinta(std::string("escaner").append(intToString(numero_escaner)).c_str(), directorio_de_trabajo, -1,
			numero_escaner - 1)
{
	config_file = config_file + 1 - 1;
}

ApiScanner::~ApiScanner() {
}

void ApiScanner::colocar_equipaje_en_cinta_principal(const Equipaje & equipaje) {
	cinta.colocar_elemento(&equipaje, this->numero_escaner);
}
