#include "api_admin.h"

#include <stdlib.h>

ApiAdmin::ApiAdmin() { }
std::string ApiAdmin::get_escala(Equipaje&) {
	return "escala";
}

int ApiAdmin::get_numero_vuelo( Equipaje& equipaje ) { 
   equipaje.toString();
   return (rand() % 100); 
}

int ApiAdmin::get_numero_zona( int num_vuelo ) { 
   num_vuelo += 1;
   return (rand() % 10); 
}

ApiAdmin::~ApiAdmin() { }

