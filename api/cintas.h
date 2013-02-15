#ifndef CINTAS_H_
#define CINTAS_H_

#include "cinta.h"
#include "cintaprincipal.h"
#include "cintacarga.h"
#include "equipaje.h"
#include "cintascanner.h"

#include "api_constants.h"



//typedef Cinta< Equipaje, CAPACIDAD_CINTA_CHECKIN > CintaCheckin;
//typedef Cinta< Equipaje, CAPACIDAD_CINTA_SCANNER > CintaScanner;
//typedef v0::CintaPrincipal< Equipaje, CANTIDAD_DE_CONSUMIDORES_PPAL > CintaCentral;
//typedef Cinta< Equipaje, CAPACIDAD_CINTA_CONTENEDOR > CintaContenedor;

typedef CintaPrincipal<Equipaje> CintaCentral;
typedef Cinta<Equipaje> CintaCheckin;
//typedef Cinta<Equipaje> CintaScanner;
typedef CintaCarga<Equipaje> CintaContenedor;

#endif
