/*
 * constants.h
 *
 *  Created on: 16/02/2013
 *      Author: gonzalo
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

// *** Constantes de  simulacion ***
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
#define TOPE_PESO_VALIJA 20
// *** Constantes de simulacion *** Sleeps

#define MAX_PUESTOS_CHECKIN 10
#define MAX_PUESTOS_SCANNER 4
#define MAX_CONECTADOS_A_PPAL 3
#define CANT_CONTENEDORES_INICIAL 10

// Other macros
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)

#define PREFIJO_RECURSO "/recurso_"
#define POSTFIJO_LCK ".lck"
#define PATH_CONTROL_TOKENS "/control_tokens.lck"
#define PATH_COLAS_GRUPO "/colas_grupo.lck"
#define MAX_NOMBRE_RECURSO 40


#endif /* CONSTANTS_H_ */
