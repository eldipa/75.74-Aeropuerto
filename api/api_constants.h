// Central

#ifndef APICONSTANTS_H_
#define APICONSTANTS_H_

#include "globalconstants.h"
#include "equipaje.h"

// IPCS sobre torre_de_control.lck
#define MTX_CENTRAL 0
#define Q_CHECKINS_HABILITADOS 1
#define Q_CHECKINS_CERRADO 2
#define Q_TRASBORDO_LISTO 3
#define Q_CONTENEDORES 4
#define Q_PUESTOS_CHECKIN 5
#define Q_ZONAS 6

// MTX central
#define MUTEX_INFO_VUELOS 0
#define CANT_MUTEX_CENTRAL 1

// #define PATH_KEYS "/tmp/carpeta_keys_ftok"
#define DEFAULT_CONFIG_FILE "./config/simulacion.ini"

#define PATH_CINTA_CHECKIN "/cinta_checkin.lck"
#define PATH_CINTA_SCANNER "/cinta_scanner.lck"
#define PATH_CINTA_CENTRAL "/cinta_central.lck"
#define PATH_CINTA_CONTENEDOR "/cinta_contenedor.lck"
#define PATH_TORRE_DE_CONTROL "/torre_de_control.lck"
#define PATH_CONTROLADOR_DE_CARGA "/controlador_de_carga.lck"
#define PATH_ADMIN_CONTENEDORES "/admin_contenedores.lck"
#define PATH_COLA_TRACTORES_AVIONES "/cola_tractores_aviones.lck"
#define PATH_COLA_AVIONES_ROBOTS_ZONA "/cola_aviones_robots_zona.lck"
#define PATH_COLA_ROBOTS_ZONA_TRACTORES "/cola_robots_zona_tractores.lck"
#define PATH_PUESTO_CHECKIN "/puesto_checkin.lck"
#define PATH_COLA_CONTROL_CARGA_CHECKIN "/cola_control_carga.lck"
#define PATH_COLA_CONTROL_CHECKIN "/cola_control_checkin.lck"
#define PATH_ROBOT_DESPACHO "/robot_despacho.lck"
#define PATH_COLA_ROBOTS_INTERCARGO "/cola_escucha_robots_intercargo.lck"
#define PATH_COLA_ESCUCHA_ZONA_ASIGNADA "/cola_escucha_zona_asignada.lck"
// #define PATH_IPC_ROBOTS_INTERCARGO "/ipc_robots_intercargo.lck"
#define PATH_CONTROL_SOSPECHOSOS "/control_sospechosos.lck"

#define PATH_TORRE_DE_CONTROL_SVC "/torre_de_control_svc.lck"
#define PATH_MESSAGE_BROKER_QUEUES "/message_broker_queues.lck"
#define PATH_MESSAGE_BROKER "/message_broker.lck"
#define PATH_COLA_INTERCARGO "/intercargo.lck"
//CINTA CENTRAL

#define ID_ESCUCHA_TRACTOR 1

#define ZONAS_POR_ROBOT_DESPACHO 2
#define MAX_ZONAS 10
#define MAX_ROBOTS_INTERCARGO_ESPERANDO_POR_ZONAS 10
#define MAX_INTERCARGOS_ESPERANDO 10 // cant max de robots de intercargo que esperan que se asigne una zona
#define MAX_PATH_SIZE 300

// Configuración cintas
#define CAPACIDAD_CINTA_CHECKIN 3
#define CAPACIDAD_CINTA_SCANNER 3
#define CAPACIDAD_CINTA_CENTRAL 20
#define CAPACIDAD_CINTA_CONTENEDOR 3
#define CANTIDAD_MAX_PRODUCTORES_CINTA_CHECKIN 4
#define CANTIDAD_MAX_CONSUMIDORES_CINTA_CHECKIN 1
#define CANTIDAD_MAX_PRODUCTORES_CINTA_SCANNER 1
#define CANTIDAD_MAX_CONSUMIDORES_CINTA_SCANNER 1
#define CANTIDAD_MAX_PRODUCTORES_CINTA_CONTENEDOR 1
#define CANTIDAD_MAX_CONSUMIDORES_CINTA_CONTENEDOR 1
#define CANTIDAD_MAX_PRODUCTORES_CINTA_CENTRAL 20
#define CANTIDAD_MAX_CONSUMIDORES_CINTA_CENTRAL 3

#define CANTIDAD_MAX_CINTAS_CONTENEDOR 8

#define MAX_SCANNERS 4
#define MAX_DATA_SIZE 60

#define TAMANIO_MEMORIA_CINTA_PRINCIPAL ((8 +CANTIDAD_MAX_PRODUCTORES_CINTA_CENTRAL  +CANTIDAD_MAX_CONSUMIDORES_CINTA_CENTRAL) * sizeof(int) +sizeof(Equipaje) * CAPACIDAD_CINTA_CENTRAL)
#define TAMANIO_MEMORIA_CINTA_SCANNER (sizeof(int) * 5 + CAPACIDAD_CINTA_SCANNER * sizeof(Equipaje))
#define TAMANIO_MEMORIA_CONTROL_CINTA_SCANNER (sizeof(int) * 2 + sizeof(int) * MAX_SCANNERS * 2)
#define TAMANIO_MEMORIA_CINTA_CARGA (10 * sizeof(int) + sizeof(int)* (CANTIDAD_MAX_PRODUCTORES_CINTA_CONTENEDOR + CANTIDAD_MAX_CONSUMIDORES_CINTA_CONTENEDOR)+ sizeof(Equipaje) *CAPACIDAD_CINTA_CONTENEDOR)
#define TAMANIO_MEMORIA_CINTA_CHECKIN (8 * sizeof(int) + sizeof(int) *(CANTIDAD_MAX_PRODUCTORES_CINTA_CHECKIN +CANTIDAD_MAX_CONSUMIDORES_CINTA_CHECKIN) + CAPACIDAD_CINTA_CHECKIN * sizeof(Equipaje))
#define TAMANIO_MEMORIA_ZONAS_ASIGNADAS (sizeof(int) * (MAX_ROBOTS_INTERCARGO_ESPERANDO_POR_ZONAS + MAX_ZONAS))
#endif
