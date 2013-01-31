#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "oserror.h"
#include "log.h"
#include "local_broker.h"

#include "messagequeue.h"

void test_existe_cola(const char * key, int id) {
	try {
		MessageQueue cola(key, id);
		std::cout << "OK: la cola " << key << " id: " << id << " Existe" << std::endl;
	} catch (const OSError & error) {
		std::cout << "Error: la cola " << key << " id: " << id << " No Existe" << std::endl;
	}
}

void test_rpc(std::string & mensaje) {
	LocalBroker broker;

	broker.test_imprimir_mensaje(mensaje);

}

int main()
try {
	std::string str("Hola");
	/*int i;

	for (i = 0; i < 255; i++) {
		char test_1[] = "/root/Escritorio/AeropuertoDistribuido/processes/locks/cinta_central.lck";
		//char id = i;

		test_existe_cola(test_1, i);
	}*/

	test_rpc(str);

}

catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

