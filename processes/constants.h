#ifndef CONSTANTS_H_
#define CONSTANTS_H_

// Configuración cintas
#define CAPACIDAD_CINTA_CHECKIN 3

#define MAX_PUESTOS_CHECKIN 10
#define MAX_PUESTOS_SCANNER 4
#define MAX_CONECTADOS_A_PPAL 2
// This define may need to disappear
#define CANTIDAD_DE_CONSUMIDORES_PPAL 2

// Sleeps
#define SLEEP_PUESTO_CHECKIN 3
#define SLEEP_ROBOT_CHECKIN 20

#define TIEMPO_SIMULACION 20

typedef Cinta< Equipaje, CAPACIDAD_CINTA_CHECKIN, MAX_PUESTOS_CHECKIN, 1 > CintaCheckin;
typedef Cinta< Equipaje, CAPACIDAD_CINTA_CHECKIN, 1, MAX_PUESTOS_SCANNER > CintaScanner;
typedef Cinta< Equipaje, CAPACIDAD_CINTA_CHECKIN, MAX_CONECTADOS_A_PPAL, CANTIDAD_DE_CONSUMIDORES_PPAL > CintaPrincipal; // Cambiar por la implementación de Gonzalo
typedef Cinta< Equipaje, CAPACIDAD_CINTA_CHECKIN, 1, 1 > CintaContenedor;

// Other macros
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)

#endif
