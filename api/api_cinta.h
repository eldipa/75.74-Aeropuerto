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

   Equipaje sacar_equipaje() const;
   void poner_equipaje(Equipaje &);
   int get_numero();

private:
	int numero;

};

#endif
