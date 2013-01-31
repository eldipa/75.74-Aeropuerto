/*
 * rpc_local_broker.cpp
 *
 *  Created on: 29/01/2013
 *      Author: gonzalo
 */

#include <iostream>
#include <stdio.h>
#include <rpc/rpc.h>	/* necesario siempre */
#include "local_broker_rpc.h"	/* generado por rpcgen */
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
/*
 * Version remota de "printmessage"
 */
int *printmessage_1_svc(char **msg, struct svc_req *svc_req)
         // char **msg;                        /* doble indireccion  obligada por rpcgen */
	// struct svc_req *svc_req;    /* handle de la comunicacion */
{
	static int result; /*static obligatorio para los resultados */


	std::cout << "Mensaje: " << msg << std::endl;

	result = 1;     /* termina bien */
	printf("servidor: %s\n", *msg);
	return (&result);
}


