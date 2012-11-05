#ifndef APICHECKIN_H_
#define APICHECKIN_H_

#include "equipaje.h"

class ApiCheckIn {
public:

  ApiCheckIn(int id_checkin, const char* path_to_locks, int id_cinta_checkin);
  virtual ~ApiCheckIn();
   
  /*
  * Asigna un vuelo al puesto de checkin.
  **/
  void iniciar_checkin( int numero_vuelo );
  
  /*
   *  Registra un equipaje que hace checkin.
   *  A partir de aca el avion no sale hasta que llega el equipaje
   **/
  void registrar_equipaje( Equipaje& );
  
  /*
   * Cierra el checkin iniciado con #iniciar_checkin 
   * El checkin debe haber sido abi
   **/
  void cerrar_checkin();

private:

  int id_checkin;
  char path_to_torre_de_control_lock[128];
  char path_to_cinta_checkin_lock[128];
  int id_cinta_checkin;
  int vuelo_actual;
};

#endif /* APICHECKIN_H_ */
