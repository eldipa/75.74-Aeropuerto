#include "api_despachante.h"
#include "log.h"
#include "api_configuracion.h"

#include "genericerror.h"


#include "api_common.h"

ApiDespachante::ApiDespachante(const char* directorio_de_trabajo, const char* config_file,const char * nombre_app, int numero_despachante) :
   cinta(std::string(nombre_app).append(intToString(numero_despachante)).c_str(), directorio_de_trabajo,numero_despachante - 1, -1),
   clnt_torre_de_control( clnt_create(ApiConfiguracion::get_torre_ip(config_file).c_str(), TORREDECONTROLPROG, TORREDECONTROLVERS, "tcp") ) {

	config_file = config_file + 1 - 1;
	this->numero_despachante = numero_despachante;
	this->saco_elemento = true;

   if (clnt_torre_de_control == NULL) {
      throw GenericError("Error al conectar con rpc_torre_de_control server");
   }
}

ApiDespachante::~ApiDespachante() {

}

Rfid ApiDespachante::leer_proximo_equipaje() {

	if (!saco_elemento) {
		this->saco_elemento = true;
		//Log::debug("Avanzando cinta");
		cinta.avanzar_cinta(this->numero_despachante);
	}
	//Log::debug("Leyendo de cinta");
	cinta.leer_elemento(&ultimo_equipaje_leido, this->numero_despachante);

	this->saco_elemento = false; // Tiene que avanzar o extraer el elemento leido

	return ultimo_equipaje_leido.getRfid();

}

Equipaje ApiDespachante::extraer_equipaje() {

	cinta.extraer_elemento();
	this->saco_elemento = true;
	return this->ultimo_equipaje_leido;

}

void ApiDespachante::avanzar_cinta() {

	if (!saco_elemento) {
		cinta.avanzar_cinta(this->numero_despachante);
		this->saco_elemento = true;
	}

}

void ApiDespachante::asignar_vuelo(int zona, int vuelo) {
   Log::info("asignando zona %d a vuelo %d", numero_despachante, zona, vuelo);
   
   Operandos op;
   op.num_zona = zona;
   op.num_vuelo = vuelo;
   void* result = asignar_vuelo_1(&op, clnt_torre_de_control);

   if (result != NULL) {
      Log::info("ApiDespachante obtuve resultado del rpc server: asignando zona %d a vuelo %d", zona, vuelo);
   } else {
      throw GenericError("ApiDespachante: Error al llamar al métdoo remoto asignar_vuelo_1 sobre el rpc server");
   }
   
}

void ApiDespachante::desasignar_vuelo(int num_vuelo) {
   Log::info(" desasignando vuelo %d", numero_despachante, num_vuelo);
   
   void* result = desasignar_vuelo_1(&num_vuelo, clnt_torre_de_control);

   if (result != NULL) {
      Log::info("ApiDespachante obtuve resultado del rpc server: desasignando zona del vuelo %d", num_vuelo);
   } else {
      throw GenericError("ApiDespachante: Error al llamar al métdoo remoto desasignar _vuelo_1 sobre el rpc server");
   }
      
}

int ApiDespachante::get_zona(int num_vuelo) {
   
   int* num_zona = get_zona_1(&num_vuelo, clnt_torre_de_control);

   if (num_zona != NULL) {
      Log::info("ApiDespachante obtuve resultado del rpc server: zona del vuelo %d es %d", num_vuelo, (*num_zona));
   } else {
      throw GenericError("ApiDespachante: Error al llamar al métdoo remoto desasignar _vuelo_1 sobre el rpc server");
   }

   return (*num_zona);;
   
}
