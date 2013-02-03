#include "api_scanner.h"
#include "api_constants.h"
#include <stdlib.h>
#include <string>
#include "mensajes.h"
#include <iostream>

ApiScanner::ApiScanner(const char * directorio_de_trabajo, const char* nombre_aeropuerto, int numero_de_sitio,
	int numero_escaner)
	: numero_escaner(numero_escaner),
		cola_grupo_envio(std::string(directorio_de_trabajo).append(PATH_GRUPO_CINTA_CENTRAL).c_str(), char(128)),
		cola_grupo_recepcion(std::string(directorio_de_trabajo).append(PATH_GRUPO_CINTA_CENTRAL).c_str(), char(0))
{

	nombre_aeropuerto = NULL;
	numero_de_sitio = 0;

	// crear Manejador de grupos y colas
	// escaner pertenece a los siguientes grupos
	// MemoriaCompartida Cinta con RobotCheckin (+ Mutex Vacio)
	// MemoriaCompartida Central con RobotCheckin
	// MemoriaCompartida Cinta Principal (+1 Mutex lleno)

}

ApiScanner::~ApiScanner() {
	// destriur Manejador de grupos y colas
}

void ApiScanner::colocar_equipaje_en_cinta_principal(const Equipaje & equipaje) {
	int peso;
	//MENSAJE_ENVIO_GRUPO mensaje;

	/*mensaje.mtype = 1;

	 mensaje.id_emisor = 1;
	 mensaje.dato = 2;*/
	peso = equipaje.peso(); // pongo esto para que compile

	/*cola_grupo_envio.push(&mensaje, sizeof(MENSAJE_ENVIO_GRUPO) - sizeof(long));*/

	broker.test_imprimir_mensaje("Mensaje");
}

Equipaje ApiScanner::extraer_equipaje_de_cinta_escaner() {
	MENSAJE_ENVIO_GRUPO mensaje;

	cola_grupo_recepcion.pull(&mensaje, sizeof(MENSAJE_ENVIO_GRUPO) - sizeof(long), 0);

	std::cout << "Id: " << mensaje.id_emisor << std::endl;
	std::cout << "Dato: " << mensaje.dato << std::endl;

	return Equipaje();
}
