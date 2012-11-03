#ifndef CONSTANTS_H_
#define CONSTANTS_H_

// Configuración cintas
#define CAPACIDAD_CINTA_CHECKIN 3
#define PUESTOS_CHECKIN_POR_CINTA 1
#define ROBOT_CHECKIN_POR_CINTA 1

// Sleeps
#define SLEEP_PUESTO_CHECKIN 3
#define SLEEP_ROBOT_CHECKIN 20

#define TIEMPO_SIMULACION 100

// Other macros
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)

#endif
