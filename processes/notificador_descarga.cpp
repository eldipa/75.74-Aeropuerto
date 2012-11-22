#include "log.h"
#include "api_constants.h"
#include "messagequeue.h"

#include <stdlib.h>
#include <cstring>

/*
 * Este proceso es un worker que se lanza cuando llega un vuelo de intercargo
 * el mismo espera que se habilite un checkin de destino y recién ahí envía la notificación
 * para que se pasen esos equipajes a la cinta principal
 */
/*
int main(int argc, char** argv)
try {
	char path[300];

	if (argc < 4) {
		Log::crit(
				"Insuficientes parametros para torre de control, se esperaba (vuelo_destino,zona_asignada)\n");
		exit(1);
	}

	strcpy(path, PATH_KEYS);
	strcat(path, PATH_CINTA_CHECKIN);

	MessageQueue checkin(path, Q_CHECKINS_HABILITADOS);
	MessageQueue trasbordo(path, Q_TRASBORDO_LISTO);

	MENSAJE_ZONA_ASIGNADA mensaje = { 1, atoi(argv[1]), atoi(argv[2]) };

	Log::info("Esperando que se habilite el vuelo %s", mensaje.vuelo_destino);
	// No me interesa el mensaje en si, sólo que se haya enviado
	checkin.pull(NULL, 2 * sizeof(int), mensaje.vuelo_destino);

	Log::info("Se habilitó el checkin %s. Notificando al robot de trasbordo",
			mensaje.vuelo_destino);

	trasbordo.push(&mensaje, sizeof(mensaje));

	return 0;
}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}
*/
