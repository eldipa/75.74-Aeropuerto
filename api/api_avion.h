#ifndef API_AVION_H_
#define API_AVION_H_

#include <vector>
#include <string>

#include "equipaje.h"
#include "pasajero.h"
#include "contenedor.h"

class ApiAvion {
private:

public:
   ApiAvion();
   virtual ~ApiAvion();
   
   /*
    * Registra que el pasajero ya subio al avion
    **/
   void subir_pasajero( int num_avion, Pasajero& );
   
   /*
    * Sube el contenedor al avion
    **/
   void subir_contenedor( int num_avion, Contenedor& );
   
   /*
    * Bloquea hasta que el controlador de carga libera la zona y habilita el despegue
    * Se llama una vez que llegaron todos vuelo de intercargo, subieron todos los equipajes
    * y los pasajeros ( los de chckin y los de vuelos intermedios )
    **/
   void esperar_habilitacion_despegue( int num_avion );
   
   /*
    * Desbloquea al avion, o permite el despegue
    **/
   void habilitar_depegue( int num_avion );
   
   /*
    * Devuelve True si el equipaje esta arriba del avion
    **/
   bool equipaje_cargado( Equipaje& );
   
   /*
    * Devuelve True si el pasajero esta arriba del avion
    **/
   bool subio_pasajero( Pasajero& );
};

#endif /* API_AVION_ */
