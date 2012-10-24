#ifndef APIESCANER_H_
#define APIESCANER_H_

#include "equipaje.h"

class ApiScanner {
private:

public:
   ApiScanner();
   virtual ~ApiScanner();

   /*
    * Marca/Registra un equipaje como sospechoso.
    * El robot de despacho deja pasar el equipaje y lo toma
    * el RobotSospechosos.
    **/
   void equipaje_sospechoso( Equipaje& );

   /*
    * Marca/Registra un equipaje como limpio.
    * El robot de despacho puede tomar el equipaje.
    **/
   void equipaje_limpio( Equipaje& );
};

#endif /* APIESCANER_H_ */


