#include "api_scanner.h"
#include <stdlib.h>

ApiScanner::ApiScanner(int numero_escaner, const char* path_to_cinta_central, int id_cinta_central) :
   numero_escaner(numero_escaner), cinta(path_to_cinta_central, id_cinta_central) {
}

ApiScanner::~ApiScanner() {

}

void ApiScanner::colocar_equipaje_en_cinta_principal(const Equipaje & equipaje) {
   this->numero_escaner = numero_escaner;
   cinta.poner_valija(equipaje, 0);
	//cinta.colocar_elemento(&equipaje, this->numero_escaner);
}
