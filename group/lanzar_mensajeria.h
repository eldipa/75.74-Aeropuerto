#ifndef LANZAR_MENSAJERIA_H_
#define LANZAR_MENSAJERIA_H_

/*
 * Lanza el sistema de mensajeria.
 *
 * Precondicion:
 *  - el sistema NO debio ser lanzado previamente (y seguir ejecutandose)
 *  - el proceso que invoque esta funcion debe setear su working directory (aka "cd")
 *    al directorio donde los ejecutables de la capa de mensajeria residen.
 *    Una vez invocada esta funcion, el proceso puede reestablece sur working
 *    directory al destino que le sea conveniente.
 *
 * Vease la documentacion de inbound.py.
 * */
void lanzar_mensajeria(const char* absolute_path, char proj_id, char group_id, const char* localhost_name,
	const char* network_name);

void lanzar_beacon_svc(const char* localhost_name);

#endif
