#ifndef CINTAS_H_
#define CINTAS_H_

#include "cinta.h"
#include "cintaprincipal.h"
#include "cintaprincipalv0.h"
#include "cintacarga.h"
#include "equipaje.h"

// Configuración cintas
#define CAPACIDAD_CINTA_CHECKIN 3
#define CAPACIDAD_CINTA_SCANNER 3
#define CAPACIDAD_CINTA_CENTRAL 4
#define CANTIDAD_MAX_PRODUCTORES_CINTA_CHECKIN 4
#define CANTIDAD_MAX_CONSUMIDORES_CINTA_CHECKIN 1
#define CANTIDAD_MAX_PRODUCTORES_CINTA_SCANNER 1
#define CANTIDAD_MAX_CONSUMIDORES_CINTA_SCANNER 1
#define CANTIDAD_MAX_PRODUCTORES_CINTA_CONTENEDOR 1
#define CANTIDAD_MAX_CONSUMIDORES_CINTA_CONTENEDOR 1
#define CANTIDAD_MAX_PRODUCTORES_CINTA_CENTRAL 10
#define CANTIDAD_MAX_CONSUMIDORES_CINTA_CENTRAL 5
// This define may need to disappear
#define CANTIDAD_DE_CONSUMIDORES_PPAL 2
#define CAPACIDAD_CINTA_CONTENEDOR 3

//typedef Cinta< Equipaje, CAPACIDAD_CINTA_CHECKIN > CintaCheckin;
//typedef Cinta< Equipaje, CAPACIDAD_CINTA_SCANNER > CintaScanner;
//typedef v0::CintaPrincipal< Equipaje, CANTIDAD_DE_CONSUMIDORES_PPAL > CintaCentral;
//typedef Cinta< Equipaje, CAPACIDAD_CINTA_CONTENEDOR > CintaContenedor;

typedef CintaPrincipal<Equipaje> CintaCentral;
typedef Cinta<Equipaje> CintaCheckin;
typedef Cinta<Equipaje> CintaScanner;
typedef CintaCarga<Equipaje> CintaContenedor;

#endif
