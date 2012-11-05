#include "log.h"
#include "vuelo_trasbordo.h"
#include "api_constants.h"
#include "messagequeue.h"

#include <stdlib.h>

/*
 * Este proceso es un worker que se lanza cuando llega un vuelo de intercargo
 * el mismo espera que se habilite un checkin de destino y recién ahí envía la notificación
 * para que se pasen esos equipajes a la cinta principal
 */
int main( int argc, char** argv ) {

    if (argc < 4) {
       Log::crit("Insuficientes parametros para torre de control, se esperaba (path, vuelo_origen, vuelo_destino)\n");
       exit(1);
    }

    MessageQueue checkin(argv[1], Q_CHECKINS_HABILITADOS);
    MessageQueue trasbordo(argv[1], Q_TRASBORDO_LISTO);

    VueloTrasbordo mensaje = { atoi(argv[2]), atoi(argv[3]) };

    Log::info("Esperando que se habilite el vuelo %s", mensaje.vuelo_destino);
    // No me interesa el mensaje en si, sólo que se haya enviado
    checkin.pull(NULL, sizeof(int), mensaje.vuelo_destino);

    Log::info("Se habilitó el checkin %s. Notificando al robot de trasbordo", mensaje.vuelo_destino);

    trasbordo.push(&mensaje, sizeof(mensaje));

    return 0;
}