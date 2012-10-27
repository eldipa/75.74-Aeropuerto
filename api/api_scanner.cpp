#include "api_scanner.h"

ApiScanner::ApiScanner(int numero) {
   this->numero = numero;
}

void ApiScanner::equipaje_sospechoso( Equipaje& ) {}

void ApiScanner::equipaje_limpio( Equipaje& ) {}

ApiScanner::~ApiScanner() {}
