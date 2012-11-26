#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include "cintas.h"
#include "api_constants.h"

#define CANT_CONTENEDORES_INICIAL 10

#define MAX_PUESTOS_CHECKIN 10
#define MAX_PUESTOS_SCANNER 4
#define MAX_CONECTADOS_A_PPAL 3

// Sleeps
#define SLEEP_PUESTO_CHECKIN 1
#define SLEEP_ROBOT_CHECKIN 2
#define SLEEP_ROBOT_SCANNER 2
#define SLEEP_TRASBORDO 2
#define SLEEP_ROBOT_DESPACHO 4
#define SLEEP_ROBOT_SOSPECHOSOS 6
#define SLEEP_LLEGADA_PASAJEROS_A_CHECKIN 3
#define SLEEP_DESPACHO_VUELO 3
#define SLEEP_DURACION_CHECKIN 15
#define SLEEP_ROBOT_CARGA 2

#define CANT_SOSPECHOSOS 10 // 1 de cada CANT_SOSPECHOSOS es sospechoso.
// Other
#define TOPE_PESO_VALIJA 20

// Other macros
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)



#endif
