#ifndef API_ZONA_H_
#define API_ZONA_H_

#include <vector>
#include <string>

#include "api_constants.h"
#include "contenedor.h"
#include "messagequeue.h"

/*
 * Zona: Cada zona tiene un area de carga y puestos.
 * Cada puesto tiene asignada una escala.puede o no tener un Contenedor en cada momento.
 * En cada contenedor se guardan los Equipajes de acuerdo a la escala de destino.
 * En el Area de Carga de la zona se llevan los contenedores completos que estan listos
 * para cargar en el avion.
 */
class ApiZona {
private:
	int num_zona;
	MessageQueue cola_tractores;


public:
	ApiZona(int num_zona);
	virtual ~ApiZona();

	/* API que usa el robot de vuelo */

	/*
	 * Crea un puesto de equipaje por cada escala.En cada puesto de equipaje
	 * puede o no puede haber un contenedor.
	 */
	void crear_puestos_equipaje(std::vector<std::string> escalas);

	/*
	 * Libera los puestos de equipaje.
	 */
	void liberar_puestos_equipaje();

	/*
	 * Retorna True si el puesto de equipaje de esa escala tiene contenedor
	 **/
	bool tiene_contenedor(std::string& escala);

	/*
	 * Pide un contenedor y se
	 * bloquea hasta que llegue un Contenedor al puesto de equipaje
	 **/
	Contenedor esperar_contenedor(std::string& escala) const;

	/*
	 * Pone Contenedor en el puesto de equipaje y desbloquea si hay alguien esperando.
	 **/
	void llego_contenedor(std::string& escala, int num_contenedor);

	/*
	 * Se lleva el contenedor al area de carga de la zona.
	 * Se llama cuando el contenedor se llenó.
	 **/
	void retirar_contenedor(std::string& escala);

	/* API para el controlador de carga */

	void esperar_avion();
	void llego_avion(int numero_vuelo, int num_avion);

};

#endif /* API_ZONA_ */
