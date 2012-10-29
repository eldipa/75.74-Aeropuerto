#include "api_scanner.h"
#include <stdlib.h>

ApiScanner::ApiScanner(int numero) {
   this->numero = numero;
}

void ApiScanner::equipaje_sospechoso( Equipaje& ) {}

void ApiScanner::equipaje_limpio( Equipaje& ) {}

bool ApiScanner::es_sospechoso( Equipaje& ) { return ((rand() % 100) == 0); }

ApiScanner::~ApiScanner() {}
