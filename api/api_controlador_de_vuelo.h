#ifndef API_CONTROLADOR_DE_VUELO_H_
#define API_CONTROLADOR_DE_VUELO_H_

#include <string>

class ApiControladorDeVuelo {
public:
   ApiControladorDeVuelo(const char* directorio_de_trabajo, int num_vuelo);
   void iniciar_checkin(int num_puesto_checkin, int num_zona);
   void cerrar_checkin(int num_puesto_checkin, int num_zona);
   virtual ~ApiControladorDeVuelo();

private:
   void enviar_mensaje_to_checkin(int num_puesto_checkin, int num_zona, bool iniciar_checkin);

   std::string path_to_locks;
   int num_vuelo;
};

#endif



