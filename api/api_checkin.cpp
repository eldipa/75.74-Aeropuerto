#include "api_checkin.h"

#include "cintas.h"
#include "api_constants.h"
#include "database.h"
#include "stmt.h"
#include "tupleiter.h"
#include "messagequeue.h"

ApiCheckIn::ApiCheckIn(int id_checkin, const char* path_to_locks, int id_cinta_checkin) : 
id_checkin(id_checkin), id_cinta_checkin(id_cinta_checkin), vuelo_actual(0)
{
    snprintf(path_to_torre_de_control_lock, 128, "%s%s", path_to_locks, PATH_TORRE_DE_CONTROL);
    snprintf(path_to_cinta_checkin_lock, 128, "%s%s", path_to_locks, PATH_CINTA_CHECKIN);
}

ApiCheckIn::~ApiCheckIn() { }
 
int cantidad_vuelos_trasbordo_a( int numero_vuelo ) {
    Database db("aeropuerto", true);

    std::auto_ptr<Statement> query = db.statement("select count(vuelo_destino) from VuelosIntercargo where vuelo_origen = :vuelo");
    query->set(":vuelo", numero_vuelo);

    return (query->begin()->at<int>(0));
}

void ApiCheckIn::iniciar_checkin( int numero_vuelo ) {
    MessageQueue checkin(path_to_torre_de_control_lock, Q_CHECKINS_HABILITADOS);

    Log::info("Notificando checkin abierto para vuelo %i", numero_vuelo);
    // Envio un mensaje por cada vuelo de trasbordo
    for (int i = 0; i < cantidad_vuelos_trasbordo_a(numero_vuelo); i ++ )
        checkin.push(&numero_vuelo, sizeof(numero_vuelo));

    // Actualizo la info sobre el vuelo actual
    vuelo_actual = numero_vuelo;
}

void ApiCheckIn::registrar_equipaje( Equipaje& equipaje ) {

    CintaCheckin cinta_checkin_out(path_to_cinta_checkin_lock, id_cinta_checkin);
    cinta_checkin_out.poner_equipaje( equipaje );

}

void ApiCheckIn::cerrar_checkin() {
    if (!vuelo_actual)
        throw std::runtime_error("No había ningún checkin abierto");

    MessageQueue checkin(path_to_torre_de_control_lock, Q_CHECKINS_CERRADO);

    Log::info("Notificando checkin cerrado para vuelo %i", vuelo_actual);
    checkin.push(&vuelo_actual, sizeof(vuelo_actual));

    vuelo_actual = 0;
}

