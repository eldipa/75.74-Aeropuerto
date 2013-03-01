#ifndef API_COMUNICACION_AEROPUERTO_H_
#define API_COMUNICACION_AEROPUERTO_H_

#include "equipaje.h"
#include "cintas.h"
#include "mensajes.h"
#include "semaphoreset.h"
#include "sharedmemory.h"

#include "yasper.h"
#include "imessagequeue.h"
#include "iqueue_manager.h"
#include "equipaje.h"
class ApiComunicacionAeropuerto {

private:

   yasper::ptr<IQueueManager> queue_manager;
   yasper::ptr<IMessageQueue> queue_zona_asignada;
   yasper::ptr<IMessageQueue> queue_equipaje_to_aeropuerto;

public:
	ApiComunicacionAeropuerto(const char* directorio_de_trabajo, const char* config_file);
	ApiComunicacionAeropuerto(const char* directorio_de_trabajo, const char* config_file, bool create);

   /*
    * devuel el número de aeropuerto en donde se asigno
    **/
	int esperar_zona_asignada(int numero_vuelo);

   /*
    * Informa la asignacion de zona para un determinado vuelo y aeropuerto   
    */
   void zona_asignada(int num_vuelo, int num_zona, int id_aeropuerto);

   /*
    * Envía un equipaje al robot_intercargo del aeropuerto pasado en el constructor.
    **/
   void enviar_equipaje_intercargo(Equipaje&, int id_aeropuerto);

   /*
    * Obtiene el próximo equipaje de intercargo para el aeropuerto pasado en el constructor.
    **/
   Equipaje get_proximo_equipaje_intercargo(int id_aeropuerto);

	virtual ~ApiComunicacionAeropuerto();

};

#endif
