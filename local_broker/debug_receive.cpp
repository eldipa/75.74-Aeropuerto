#include "messagequeue.h"
#include <cstring>
#include <cstdio>
#include <iostream>
#include "group_interface.h"

int main() {
	char data[100];
	GroupInterface grupo("/tmp/test", char(0));

	for (int i = 0; i < 10; i++) {

		grupo.pull(data, sizeof(data));

		std::cout << "recibido: " << data << std::endl;

	}

	/*MessageQueue outbound("/tmp/test", char(128));
	 MessageQueue inbound("/tmp/test", char(0));
	 mensaje_t mensaje;

	 mensaje.mtype = 1;

	 for (int i = 0; i < 10; i++) {
	 outbound.pull(&mensaje, sizeof(mensaje_t) - sizeof(long), 0);

	 std::cout << "recibido: "<< mensaje.msg << std::endl;

	 sprintf(mensaje.msg, "hola%d", i);

	 }*/
	return -1;
}
