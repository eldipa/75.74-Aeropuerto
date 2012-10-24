#ifndef API_ADMIN_H_
#define API_ADMIN_H_

#include <vector>
#include <string>
#include <time.h>

#include "vuelo.h"
#include "pasajero.h"
#include "equipaje.h"

/*
 * carga la BD con vuelos, pasajeros y equipajes en tránsito.
 * Formato de los archivos (un registro por línea):
 *
 * archivo de vuelos (origen y destino van vacios si nuestro aeropuerto es el origen o destino):
 * "num_vuelo:compania:origen:destino:fecha_hora_checkin:fecha_hora_checkin:fecha_hora_llegada:num_zona_asignada"
 *
 * archivo de equipajes en transito:
 * "num_vuelo:rfid"
 *
 * archivo de pasajeros:
 * "dni:nombre:num_boleto:num_vuelo"
 *
 **/
class ApiAdmin {
private:

public:
	ApiAdmin();
	virtual ~ApiAdmin();

   /*
    * registra un nuevo vuelo en la BD (archivo de vuelos)
    * Pueden ser vuelos que llegan o vuelos que salen.
    **/
	bool alta_vuelo( Vuelo& vuelo, std::vector<int> vuelos_transito );

   /*
    * Lee un vuelo de la base de datos (archivo de vuelos)
    **/
   Vuelo& get_vuelo( int num_vuelo );

   /*
    * Registra un equipaje en transito.
    **/
   bool alta_equipaje_transito( Equipaje&, int num_vuelo );

   /*
    * Devuelve los equipajes en transito de un determinado vuelo.
    * Son aquellos equipajes que cambian de avion.
    **/
   std::vector<Equipaje> get_equipajes_en_transito( int num_vuelo );
   
   /*
    * Da de alta un pasajero en la BD (archivo de pasajeros)
    * Pueden ser pasajeros correspondientes a vuelos que salen
    * o pasajeros que hacen escala.
    **/
	bool alta_pasajero( int num_vuelo, Pasajero& );
	


	
};

#endif /* API_Admin_ */
