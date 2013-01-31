/*
 * LocalBroker.cpp
 *
 *  Created on: 29/01/2013
 *      Author: gonzalo
 */

#include "local_broker.h"
#include "local_broker_rpc.h"

LocalBroker::LocalBroker(/*std::string nombre_aeropuerto, int numero_de_sitio*/)/* :
 nombre_aeropuerto(nombre_aeropuerto), numero_de_sitio(numero_de_sitio) */{
	nombre_broker_conectado = "localhost";
}

LocalBroker::~LocalBroker() {

}

void LocalBroker::registrar_aplicacion(std::string nombre_aplicacion, int numero) {
	nombre_aplicacion = "";
	numero = 0;
}

void LocalBroker::tomar_mutex(std::string nombre_mutex) {
	nombre_mutex = "";
}

void LocalBroker::test_imprimir_mensaje(const std::string & mensaje) {
	char * msg;
	CLIENT *clnt;
	int *result_1;
	msg = const_cast<char *>(mensaje.c_str());
	clnt = clnt_create(nombre_broker_conectado.c_str(), MESSAGEPROG, MESSAGEVERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror(nombre_broker_conectado.c_str());
		exit(1);
	}

	result_1 = printmessage_1(&msg, clnt);
	if (result_1 == (int *) NULL) {
		clnt_perror(clnt, "call failed");
	}

	clnt_destroy(clnt);
}
