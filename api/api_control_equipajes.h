#ifndef APICONTROLEQUIPAJES_H_
#define APICONTROLEQUIPAJES_H_

#include "equipaje.h"
#include "messagequeue.h"
#include "cintas.h"
#include "rfid.h"

typedef struct t_msg_sospechoso {
   long mtype;
   Equipaje equipaje;
} tMsgSospechoso;

class ApiControlEquipajes {
private:
	int pos_consumidor_cinta_central;
	int pos_productor_cinta_central;
	CintaCentral cinta_central;
   MessageQueue queue_to_control_sospechosos;

public:
   ApiControlEquipajes(const char * directorio_de_trabajo,int pos_consumidor_cinta_central,int pos_productor_cinta_central, bool create);
	ApiControlEquipajes(const char * directorio_de_trabajo, int pos_consumidor_cinta_central, int pos_productor_cinta_central);
	virtual ~ApiControlEquipajes();

	Equipaje obtener_proximo_equipaje_sospechoso();

	void volver_a_colocar_equipaje_en_cinta_principal(const Equipaje & e);

   void enviar_equipaje_a_control(Equipaje&);
   Equipaje get_equipaje_a_controlar();
};

#endif /* APICONTROLEQUIPAJES_H_ */
