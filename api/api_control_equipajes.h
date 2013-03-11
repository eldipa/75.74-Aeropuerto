#ifndef APICONTROLEQUIPAJES_H_
#define APICONTROLEQUIPAJES_H_

#include "equipaje.h"
#include "messagequeue.h"
#include "cintas.h"
#include "rfid.h"
#include "torre_de_control.h"

#include "yasper.h"
#include "imessagequeue.h"
#include "iqueue_manager.h"

typedef struct t_msg_sospechoso {
   long mtype;
   Equipaje equipaje;
} tMsgSospechoso;

class ApiControlEquipajes {
private:
   CLIENT *clnt_torre_de_control;

	int pos_consumidor_cinta_central;
	int pos_productor_cinta_central;
	CintaCentral cinta_central;

   yasper::ptr<IQueueManager> queue_manager;
   yasper::ptr<IMessageQueue> queue_to_control_sospechosos;

public:
   ApiControlEquipajes(const char * directorio_de_trabajo,const char* config_file, 
                       int pos_consumidor_cinta_centralint,int pos_productor_cinta_central, bool create);

	virtual ~ApiControlEquipajes();

	Equipaje obtener_proximo_equipaje_sospechoso();

	void volver_a_colocar_equipaje_en_cinta_principal(const Equipaje & e);

   void enviar_equipaje_a_control(Equipaje&);
   Equipaje get_equipaje_a_controlar();

   int get_zona(int num_vuelo);

};

#endif /* APICONTROLEQUIPAJES_H_ */
