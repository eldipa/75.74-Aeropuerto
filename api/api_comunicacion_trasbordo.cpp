#include "api_comunicacion_trasbordo.h"

ApiComunicacionTrasbordo::ApiComunicacionTrasbordo(char * path_to_robots_intercargo,
		int id_instancia) {
	// TODO Auto-generated constructor stub
	id_instancia--;
	path_to_robots_intercargo = NULL;
}

ApiComunicacionTrasbordo::~ApiComunicacionTrasbordo() {
	// TODO Auto-generated destructor stub
}

void ApiComunicacionTrasbordo::notificar_asignacion_de_zona(int numero_de_zona, int numero_de_vuelo){
	numero_de_zona--;
	numero_de_vuelo--;
}

void ApiComunicacionTrasbordo::esperar_mensaje_zona_asignada(){

}

int ApiComunicacionTrasbordo::get_zona_asignada(){
	return -1;
}

int ApiComunicacionTrasbordo::get_vuelo_destino(){
	return -1;
}
