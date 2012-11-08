#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include "cintas.h"
#include "api_constants.h"

#define MAX_PUESTOS_CHECKIN 10
#define MAX_PUESTOS_SCANNER 4
#define MAX_CONECTADOS_A_PPAL 2

// Sleeps
#define SLEEP_PUESTO_CHECKIN 1
#define SLEEP_ROBOT_CHECKIN 2
#define SLEEP_ROBOT_SCANNER 2
#define SLEEP_TRASBORDO 2
#define SLEEP_ROBOT_DESPACHO 2
#define SLEEP_ROBOT_SOSPECHOSOS 2

#define TIEMPO_SIMULACION 60

// Other
#define TOPE_PESO_VALIJA 20

// Other macros
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)

#endif
