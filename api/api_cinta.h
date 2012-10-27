#ifndef API_CINTA_H_
#define API_CINTA_H_

#include "equipaje.h"

/*
 *
 **/
class ApiCinta {
public:
   ApiCinta(int numero);
   virtual ~ApiCinta();

   Equipaje & sacar_equipaje();
   void poner_equipaje(Equipaje &);

private:
	int numero;

};

#endif