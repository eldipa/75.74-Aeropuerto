#include "api_despachante.h"
#include "log.h"
#include "mensajes.h"

#include <iostream>

ApiDespachante::ApiDespachante(const char * directorio_de_trabajo, const char * nombre_aeropuerto,
		int numero_de_sitio, int numero_despachante) :
		cola_grupo_envio(
				std::string(directorio_de_trabajo).append(PATH_GRUPO_CINTA_CENTRAL).c_str(),
				49), cola_grupo_recepcion(
				std::string(directorio_de_trabajo).append(PATH_GRUPO_CINTA_CENTRAL).c_str(),
				48) {
	
	nombre_aeropuerto = NULL;
	numero_de_sitio = 0;
	this->numero_despachante = numero_despachante;
	this->saco_elemento = true;


}

ApiDespachante::~ApiDespachante() {
}

Rfid ApiDespachante::leer_proximo_equipaje() {

	/*if (!saco_elemento) {
	 this->saco_elemento = true;

	 cinta.avanzar_cinta(this->numero_despachante);
	 }

	 cinta.leer_elemento(&ultimo_equipaje_leido, this->numero_despachante);

	 this->saco_elemento = false; // Tiene que avanzar o extraer el elemento leido

	 return ultimo_equipaje_leido.getRfid();*/

	MENSAJE_ENVIO_GRUPO mensaje;

	cola_grupo_recepcion.pull(&mensaje, sizeof(MENSAJE_ENVIO_GRUPO) - sizeof(long), 0);

	std::cout << "Id: " << mensaje.id_emisor << std::endl;
	std::cout << "Dato: " << mensaje.dato << std::endl;

	return Rfid(0,0);
}
/*
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
	Log::info("asignando zona %d a vuelo %d", numero_despachante, zona, vuelo);
	mutex_asignaciones.lock();
	asignaciones->asignar_vuelo(zona, vuelo);
	mutex_asignaciones.unlock();
}

void ApiDespachante::desasignar_vuelo(int num_vuelo) {
	Log::info(" desasignando vuelo %d", numero_despachante, num_vuelo);
	mutex_asignaciones.lock();
	asignaciones->desasignar_vuelo(num_vuelo);
	mutex_asignaciones.unlock();
}

int ApiDespachante::get_zona(int num_vuelo) {
	mutex_asignaciones.lock();
	int zona = asignaciones->get_zona(num_vuelo);
	mutex_asignaciones.unlock();
	return zona;
}*/
