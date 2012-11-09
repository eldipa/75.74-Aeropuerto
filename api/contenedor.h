#ifndef CONTENEDOR_H_
#define CONTENEDOR_H_

#include "equipaje.h"
#include <vector>

#define MAX_PESO_CONTENEDOR 30
#define MAX_VALIJAS_POR_CONTENEDOR 30

class Contenedor {
public:
   Contenedor();
   void agregar_equipaje(const Equipaje& e);
   bool hay_lugar(const Equipaje& e);
   std::vector<int> get_rfids();
   virtual ~Contenedor();

private:
   int peso;
   int cant_equipajes;
   int equipajes[MAX_VALIJAS_POR_CONTENEDOR];

};

#endif /* EQUIPAJE_H_ */
