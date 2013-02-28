/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "torre_de_control.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "yasper.h"
#include "api_torre_de_control_svc.h"
#include "api_configuracion.h"
#include "genericerror.h"


#include "api_constants.h"

#include <sys/stat.h>
#include <cerrno>
#include <cstdio>
#include <fcntl.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

void torredecontrolprog_1(struct svc_req *rqstp, register SVCXPRT *transp)
{

   
	union {
		Operandos asignar_vuelo_1_arg;
		int desasignar_vuelo_1_arg;
		int get_zona_1_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case ASIGNAR_VUELO:
		_xdr_argument = (xdrproc_t) xdr_Operandos;
		_xdr_result = (xdrproc_t) xdr_void;
		local = (char *(*)(char *, struct svc_req *)) asignar_vuelo_1_svc;
		break;

	case DESASIGNAR_VUELO:
		_xdr_argument = (xdrproc_t) xdr_int;
		_xdr_result = (xdrproc_t) xdr_void;
		local = (char *(*)(char *, struct svc_req *)) desasignar_vuelo_1_svc;
		break;

	case GET_ZONA:
		_xdr_argument = (xdrproc_t) xdr_int;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (char *(*)(char *, struct svc_req *)) get_zona_1_svc;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	return;
}

void crear_archivos_lck_torre(const char *path_to_locks);
void crear_archivo_lck_torre(const char *path_to_locks, const char * nombre_archivo);

class TorreDeControlSvc {
public:
   TorreDeControlSvc(const char* directorio_de_trabajo):
      sem_set(std::vector<short unsigned int>(1, 1),
              std::string(directorio_de_trabajo).append(PATH_TORRE_DE_CONTROL_SVC).c_str(),0), 
      asignaciones(ZonasAsignadas(), std::string(directorio_de_trabajo).append(PATH_TORRE_DE_CONTROL_SVC).c_str(), 1)  {
   }

private:
   SemaphoreSet sem_set;
   SharedObject<ZonasAsignadas> asignaciones;

};

yasper::ptr<TorreDeControlSvc> torre_de_control = NULL;
yasper::ptr<ApiTorreDeControlSvc> api_torre = NULL;


int
main (int argc, char **argv)
{  
try {

   if(argc<2) {
      throw GenericError("Faltan parametros para torre_de_control_svc: ./torre_de_control_svc config_file_abs_path");
   }
    
   std::string wkdir = ApiConfiguracion::get_torre_wkdir(argv[1]);
   crear_archivos_lck_torre(wkdir.c_str());

   torre_de_control = new TorreDeControlSvc(wkdir.c_str());
   api_torre = new ApiTorreDeControlSvc(wkdir.c_str(), argv[1]);

	register SVCXPRT *transp;

	pmap_unset (TORREDECONTROLPROG, TORREDECONTROLVERS);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, TORREDECONTROLPROG, TORREDECONTROLVERS, torredecontrolprog_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (TORREDECONTROLPROG, TORREDECONTROLVERS, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, TORREDECONTROLPROG, TORREDECONTROLVERS, torredecontrolprog_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (TORREDECONTROLPROG, TORREDECONTROLVERS, tcp).");
		exit(1);
	}

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
   return(0);
	// exit (1);
	/* NOTREACHED */

 } catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

}

void crear_archivos_lck_torre(const char *path_to_locks) {

   struct stat buf;
   int result;

   result = stat(path_to_locks, &buf);

   if (result != 0) {
      /* Directory does not exist. EEXIST for race condition */
      if (mkdir(path_to_locks, 0770) != 0 && errno != EEXIST) {
         //THROW OSERROR
      }
   } else if (!S_ISDIR(buf.st_mode)) {
      errno = ENOTDIR;
      //THROW OSERROR
   }

   crear_archivo_lck_torre(path_to_locks, PATH_TORRE_DE_CONTROL_SVC);
}

void crear_archivo_lck_torre(const char *path_to_locks, const char * nombre_archivo) {
   char path[300];
   int file;
   struct stat buf;
   
   strcpy(path, path_to_locks);
   strcat(path, nombre_archivo);
   
   if (stat(path, &buf) != 0) {
      file = open(path, O_CREAT | 0664);
      if (file != -1) {
         close(file);
      } else {
         //THROW OSERROR
      }
   }
}