#include "torre_de_control.h"
#include "api_torre_de_control_svc.h"

#include "yasper.h"
#include "log.h"

extern yasper::ptr<ApiTorreDeControlSvc> api_torre;

void * asignar_vuelo_1_svc(Operandos * op, struct svc_req * req) {
   Log::info("TorreDeControlRPCServer asignar_vuelo_1_svc");
   req = req;
   api_torre->asignar_vuelo(op->num_zona, op->num_vuelo);
   return (void*)op;
}

void * desasignar_vuelo_1_svc(int * num_vuelo, struct svc_req * req) {
   Log::info("TorreDeControlRPCServer desasignar_vuelo_1_svc");
   req = req;
   api_torre->desasignar_vuelo(*num_vuelo);
   return (void*)num_vuelo;
}
int * get_zona_1_svc(int * num_vuelo, struct svc_req * req) {
   static int num_zona;
   req = req;
   num_zona = api_torre->get_zona(*num_vuelo);
   Log::info("TorreDeControlRPCServer get_zona_1_svc response %d", num_zona);
   return &num_zona;
}
