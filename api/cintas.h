#ifndef CINTAS_H_
#define CINTAS_H_

#include "cinta.h"
#include "cintaprincipal.h"
#include "cintaprincipalv0.h"
#include "equipaje.h"

// Configuración cintas
#define CAPACIDAD_CINTA_CHECKIN 3
#define CAPACIDAD_CINTA_SCANNER 3
// This define may need to disappear
#define CANTIDAD_DE_CONSUMIDORES_PPAL 2
#define CAPACIDAD_CINTA_CONTENEDOR 3

typedef Cinta< Equipaje, CAPACIDAD_CINTA_CHECKIN > CintaCheckin;
typedef Cinta< Equipaje, CAPACIDAD_CINTA_SCANNER > CintaScanner;

//typedef CintaPrincipal< Equipaje > CintaCentral;
typedef v0::CintaPrincipal< Equipaje, CANTIDAD_DE_CONSUMIDORES_PPAL > CintaCentral;

typedef Cinta< Equipaje, CAPACIDAD_CINTA_CONTENEDOR > CintaContenedor;

#endif
