#include "api_despachante.h"
#include "log.h"

ApiDespachante::ApiDespachante(int numero_despachante, const char* path_to_locks) :
   sem_set(std::string(path_to_locks).append(PATH_ROBOT_DESPACHO).c_str(), numero_despachante*cant_ipcs, 1),
   mutex_asignaciones(sem_set, 0),
   asignaciones(std::string(path_to_locks).append(PATH_ROBOT_DESPACHO).c_str(), numero_despachante*cant_ipcs+1),
   cinta(std::string(path_to_locks).append(PATH_CINTA_CENTRAL).c_str()) {

	this->numero_despachante = numero_despachante;
	this->saco_elemento = true;

}

ApiDespachante::~ApiDespachante() {

}

Rfid ApiDespachante::leer_proximo_equipaje() {

	if (!saco_elemento) {
		this->saco_elemento = true;
		//Log::debug("Avanzando cinta");
		cinta.avanzar_cinta(this->numero_despachante);
	}
	//Log::debug("Leyendo de cinta");
	cinta.leer_elemento(&ultimo_equipaje_leido, this->numero_despachante);

	this->saco_elemento = false; // Tiene que avanzar o extraer el elemento leido

	return ultimo_equipaje_leido.getRfid();

}

Equipaje ApiDespachante::extraer_equipaje() {

	cinta.extraer_elemento();
	this->saco_elemento = true;
	return this->ultimo_equipaje_leido;

}

void ApiDespachante::avanzar_cinta() {

	if (!saco_elemento) {
		cinta.avanzar_cinta(this->numero_despachante);
		this->saco_elemento = true;
	}

}

void ApiDespachante::asignar_vuelo(int zona, int vuelo) {
   Log::info("ApiDespachante(%d) asignando zona %d a vuelo %d", numero_despachante, zona, vuelo);
   mutex_asignaciones.lock();
   asignaciones->asignar_vuelo(zona, vuelo);
   mutex_asignaciones.unlock();
}

void ApiDespachante::desasignar_vuelo(int num_vuelo) {
   Log::info("ApiDespachante(%d) desasignando vuelo %d", numero_despachante, num_vuelo);
   mutex_asignaciones.lock();
   asignaciones->desasignar_vuelo(num_vuelo);
   mutex_asignaciones.unlock();
}

int ApiDespachante::get_zona(int num_vuelo) {
   mutex_asignaciones.lock();
   int zona = asignaciones->get_zona(num_vuelo);
   mutex_asignaciones.unlock();
   return zona;
}
