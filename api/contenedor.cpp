#include "contenedor.h"
#include <stdexcept>

Contenedor::Contenedor( ) : peso(0), cant_equipajes(0) {}
Contenedor::~Contenedor() { }

bool Contenedor::hay_lugar(const Equipaje& e) { 
   return (e.peso()+peso > MAX_PESO_CONTENEDOR);
}

void Contenedor::agregar_equipaje(const Equipaje& e) { 
   if(!hay_lugar(e))
      throw std::runtime_error("Contenedor");

   if(cant_equipajes > MAX_VALIJAS_POR_CONTENEDOR)
      throw std::runtime_error("Contenedor");

   equipajes[cant_equipajes] = e.getRfid().rfid;
   cant_equipajes++;
}

std::vector<int> Contenedor::get_rfids() {
   std::vector<int> rfids;
   for(int i=0;i<cant_equipajes;i++)
      rfids.push_back(equipajes[i]);
   return rfids;
}
