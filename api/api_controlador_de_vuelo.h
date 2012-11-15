#ifndef API_CONTROLADOR_DE_VUELO_H_
#define API_CONTROLADOR_DE_VUELO_H_

#include <string>

class ApiControladorDeVuelo {
public:
   ApiControladorDeVuelo(const char* path_to_locks, int num_vuelo);
   void iniciar_checkin(int num_puesto_checkin);
   void cerrar_checkin(int num_puesto_checkin);
   virtual ~ApiControladorDeVuelo();

private:
   void enviar_mensaje_to_checkin(int num_puesto_checkin, bool iniciar_checkin);

   std::string path_to_locks;
   int num_vuelo;
};

#endif



