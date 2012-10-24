#ifndef APICHECKIN_H_
#define APICHECKIN_H_

#include <vector>

class ApiCheckIn {
private:
	//TODO CONTROL
	typedef struct {
		int algo;
	} CONTROL;

public:

	APICheckIn();
	virtual ~APICheckIn();
	
	// se asocia a la cola del robot que distribuye el equipaje
	int iniciarAplicacion(int ID_VUELO, CONTROL & con);

	/*
	 * Asigna un vuelo al puesto de checkin.
	 * No se reciben equipajes hasta que no se abra el checkin.
	 **/
	void asignar_vuelo( int numero_vuelo, int num_puesto_checkin );

	/*
	 * El puesto de checkin comienza a recibir pasajeros y equipajes
	 * para el vuelo asignado.
	 */
	void abrir_checkin( int num_puesto_checkin );

	/*
	 * Registra el checkin cerrado.Deja de recibir equipajes y pasajeros.
	 */
	void cerrar_checkin( int num_puesto_checkin );

	/*
	 * Registra que el pasajero hizo el checkin.
	 **/
	void hacer_checkin( int num_puesto_checkin, Boleto& );

	/*
	 *  Registra un equipaje que hace checkin.
	 *  A partir de aca el avion no sale hasta que llega el equipaje
	 **/
	void registrar_equipaje( int num_puesto_checkin, Equipaje&, Boleto& );

	/*
	 * Pone el equipaje en la cinta de checkin asociada al puesto.
	 **/
	void enviar_equipaje_a_cinta_de_checkin( int num_puesto_checkin, Equipaje& );

	int cerrarAplicacion(CONTROL & con);
};

#endif /* APICHECKIN_H_ */
