#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sstream>

#include "log.h"

#include "process.h"
#include "constants.h"
#include "daemon.h"
#include "conexiones_aeropuerto.h"

#include "database.h"
#include "stmt.h"
#include "tupleiter.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <sys/types.h>
#include "key.h"
#include <list>
#include <map>

// static char buffer_num_vuelo[20];
static char directorio_de_trabajo [MAX_PATH_SIZE];
static char config_file [MAX_PATH_SIZE];

// Defino los espacios de trabajo de los procesos

static char working_dir_puesto_checkin1 [MAX_PATH_SIZE] = "/tmp/wd_puesto_checkin1";
static char working_dir_puesto_checkin2 [MAX_PATH_SIZE] = "/tmp/wd_puesto_checkin2";
static char working_dir_puesto_checkin3 [MAX_PATH_SIZE] = "/tmp/wd_puesto_checkin3";
static char working_dir_robot_checkin [MAX_PATH_SIZE] = "/tmp/wd_robot_checkin";
static char working_dir_robot_carga1 [MAX_PATH_SIZE] = "/tmp/wd_robot_carga1";
static char working_dir_robot_carga2 [MAX_PATH_SIZE] = "/tmp/wd_robot_carga2";
static char working_dir_robot_carga3 [MAX_PATH_SIZE] = "/tmp/wd_robot_carga3";
static char working_dir_robot_carga4 [MAX_PATH_SIZE] = "/tmp/wd_robot_carga4";
static char working_dir_robot_escaner1 [MAX_PATH_SIZE] = "/tmp/wd_robot_escaner1";
static char working_dir_robot_escaner2 [MAX_PATH_SIZE] = "/tmp/wd_robot_escaner2";
static char working_dir_robot_escaner3 [MAX_PATH_SIZE] = "/tmp/wd_robot_escaner3";
static char working_dir_robot_escaner4 [MAX_PATH_SIZE] = "/tmp/wd_robot_escaner4";
static char working_dir_robot_despacho1 [MAX_PATH_SIZE] = "/tmp/wd_robot_despacho1";
static char working_dir_robot_despacho2 [MAX_PATH_SIZE] = "/tmp/wd_robot_despacho2";
static char working_dir_robot_control_equipaje [MAX_PATH_SIZE] = "/tmp/wd_robot_control_equipaje";
static char working_dir_robot_intercargo [MAX_PATH_SIZE] = "/tmp/wd_robot_intercargo";
static char working_dir_control_equipaje_sospechoso [MAX_PATH_SIZE] = "/tmp/wd_control_equipaje_sospechoso";
static char working_dir_control_carga_contenedores [MAX_PATH_SIZE] = "/tmp/control_carga_contenedores";

static char working_dir_local_broker1 [MAX_PATH_SIZE] = "/tmp/wd_local_broker1";
static char working_dir_local_broker2 [MAX_PATH_SIZE] = "/tmp/wd_local_broker2";
static char working_dir_local_broker3 [MAX_PATH_SIZE] = "/tmp/wd_local_broker3";
static char working_dir_local_broker4 [MAX_PATH_SIZE] = "/tmp/wd_local_broker4";

static char working_dir_generador_vuelos_trasbordo [MAX_PATH_SIZE] = "/tmp/wd_generador_vuelos_trasbordo";
static char working_dir_avion1 [MAX_PATH_SIZE] = "/tmp/wd_avion1";
static char working_dir_avion2 [MAX_PATH_SIZE] = "/tmp/wd_avion2";
static char working_dir_tractor1 [MAX_PATH_SIZE] = "/tmp/wd_tractor1";
static char working_dir_tractor2 [MAX_PATH_SIZE] = "/tmp/wd_tractor2";
static char working_dir_tractor3 [MAX_PATH_SIZE] = "/tmp/wd_tractor3";

static char working_dir_1 [MAX_PATH_SIZE] = "/tmp/carpeta_keys_ftok";
static char working_dir_2 [MAX_PATH_SIZE] = "/tmp/carpeta_keys_ftok_broker";
static char working_dir_3 [MAX_PATH_SIZE] = "/tmp/carpeta_keys_ftok_intermediate_broker";
static char working_dir_4 [MAX_PATH_SIZE] = "/tmp/carpeta_keys_ftok_torre";

static char id_productor_cinta_central [10];

static char *args_puesto_checkin1 [] = {
	(char*)"puesto_checkin_1", working_dir_puesto_checkin1, config_file, (char*)"1", (char*)"1", NULL};
static char *args_puesto_checkin2 [] = {
	(char*)"puesto_checkin_2", working_dir_puesto_checkin2, config_file, (char*)"2", (char*)"1", NULL};
static char *args_puesto_checkin3 [] = {
	(char*)"puesto_checkin_3", working_dir_puesto_checkin3, config_file, (char*)"3", (char*)"1", NULL};

static char *args_robot_checkin [] = {
	(char*)"robot_checkin", working_dir_robot_checkin, config_file, (char*)"1", (char*)"1", (char*)"1", NULL};

static char *args_robot_carga1 [] = {(char*)"robot_carga_1", working_dir_robot_carga1, config_file, (char*)"1", (char*)"1", NULL};
static char *args_robot_carga2 [] = {(char*)"robot_carga_2", working_dir_robot_carga2, config_file, (char*)"2", (char*)"2", NULL};
static char *args_robot_carga3 [] = {(char*)"robot_carga_3", working_dir_robot_carga3, config_file, (char*)"3", (char*)"3", NULL};
static char *args_robot_carga4 [] = {(char*)"robot_carga_4", working_dir_robot_carga4, config_file, (char*)"4", (char*)"4", NULL};

static char *args_scanner1 [] = {
	(char*)"robot_scanner_1", working_dir_robot_escaner1, config_file, (char*)"1", (char*)"1", (char*)"1", NULL};

static char *args_scanner2 [] = {
	(char*)"robot_scanner_2", working_dir_robot_escaner2, config_file, (char*)"2", (char*)"1", (char*)"1", NULL};

/*static char *args_scanner3 [] = {
 (char*)"robot_scanner_3", working_dir_robot_escaner3, config_file, (char*)"3", (char*)"1", (char*)"1", NULL};*/

/*static char *args_scanner4 [] = {
 (char*)"robot_scanner_4", working_dir_robot_escaner4, config_file, (char*)"4", (char*)"1", (char*)"1", NULL};*/

static char *args_robot_despacho1 [] = {
	(char*)"robot_despacho_1", working_dir_robot_despacho1, config_file, (char*)"1", (char*)"1", (char*)"2", NULL};

static char *args_robot_despacho2 [] = {
	(char*)"robot_despacho_2", working_dir_robot_despacho2, config_file, (char*)"2", (char*)"3", (char*)"4", NULL};

static char *args_robot_sospechosos [] = {
	(char*)"robot_control_equipaje_sospechoso", working_dir_robot_control_equipaje, config_file, (char*)"3",
	id_productor_cinta_central, NULL};

static char *args_robot_intercargo [] = {
	(char*)"robot_intercargo", working_dir_robot_intercargo, config_file, NULL};

/*static char *args_control_sospechosos [] = {
 (char*)"robot_control_equipaje_sospechoso", working_dir_control_equipaje_sospechoso, config_file, (char*)"3",
 id_productor_cinta_central, NULL};*/

static char *args_local_broker1 [] = {(char*)"localbroker1", working_dir_local_broker1, (char*)"1234", NULL};

//static char *args_local_broker2 [] = {(char*)"localbroker2", working_dir_local_broker2, "1235", NULL};

//static char *args_local_broker3 [] = {(char*)"localbroker3", working_dir_local_broker3, "1236", NULL};

//static char *args_local_broker4 [] = {(char*)"localbroker4", working_dir_local_broker4, "1237", NULL};

static char * args_generador_vuelos_trasbordo [] = {
	(char*)"generador_vuelos_trasbordo", working_dir_generador_vuelos_trasbordo, config_file,
	(char*)"./entrada/vuelos_entrantes.csv", (char*)"./entrada/vuelos_interconexion.csv", NULL};

//static char *args_avion1 [] = {(char*)"avion1", working_dir_avion1, config_file, (char*)"1", NULL};

static char *args_tractor1 [] = {(char*)"tractor_1", working_dir_tractor1, config_file, (char*)"1", NULL};
//static char *args_tractor2 [] = {(char*)"tractor_2", working_dir_tractor2, config_file, (char*)"2", NULL};
//static char *args_tractor3 [] = {(char*)"tractor_3", working_dir_tractor3, config_file, (char*)"3", NULL};

static char *args_scheduler_aviones [] = {(char*)"scheduler_aviones", directorio_de_trabajo, config_file, NULL};
static char *args_scheduler_vuelos [] = {(char*)"scheduler_vuelos", directorio_de_trabajo, config_file, NULL};

static std::vector<char *> all_dirs;

void load_all_dirs() {
	all_dirs.push_back(working_dir_puesto_checkin1);
	all_dirs.push_back(working_dir_puesto_checkin2);
	all_dirs.push_back(working_dir_puesto_checkin3);
	//all_dirs.push_back(working_dir_puesto_checkin4);
	all_dirs.push_back(working_dir_robot_carga1);
	all_dirs.push_back(working_dir_robot_carga2);
	all_dirs.push_back(working_dir_robot_carga3);
	all_dirs.push_back(working_dir_robot_carga4);
	all_dirs.push_back(working_dir_robot_checkin);
	all_dirs.push_back(working_dir_robot_escaner1);
	all_dirs.push_back(working_dir_robot_escaner2);
	all_dirs.push_back(working_dir_robot_escaner3);
	all_dirs.push_back(working_dir_robot_escaner4);
	all_dirs.push_back(working_dir_robot_despacho1);
	all_dirs.push_back(working_dir_robot_despacho2);
	all_dirs.push_back(working_dir_robot_control_equipaje);
	all_dirs.push_back(working_dir_control_equipaje_sospechoso);
	all_dirs.push_back(working_dir_robot_intercargo);
	all_dirs.push_back(working_dir_control_carga_contenedores);
	all_dirs.push_back(working_dir_local_broker1);
	all_dirs.push_back(working_dir_local_broker2);
	all_dirs.push_back(working_dir_local_broker3);
	all_dirs.push_back(working_dir_local_broker4);
	all_dirs.push_back(working_dir_generador_vuelos_trasbordo);
	all_dirs.push_back(working_dir_avion1);
	all_dirs.push_back(working_dir_avion2);
	all_dirs.push_back(working_dir_tractor1);
	all_dirs.push_back(working_dir_tractor2);
	all_dirs.push_back(working_dir_tractor3);
	all_dirs.push_back(working_dir_1);
	all_dirs.push_back(working_dir_2);
	all_dirs.push_back(working_dir_3);
	all_dirs.push_back(working_dir_4);
}

void mkdirs() {
	std::vector<char *>::iterator i;
	for (i = all_dirs.begin(); i != all_dirs.end() ; i++) {
		mkdir(*i, 0770);
	}
}



void copy_files() {
	std::vector<char *>::iterator i;
	char file_dest [MAX_PATH_SIZE];
	char file_src [MAX_PATH_SIZE];
	strcpy(file_dest, "/group_list.txt");
	strcpy(file_src, "../local_broker/locks/group_list_all.txt");
	cp(std::string(working_dir_local_broker1).append(file_dest).c_str(), file_src);
	cp(std::string(working_dir_local_broker2).append(file_dest).c_str(), file_src);
	cp(std::string(working_dir_local_broker3).append(file_dest).c_str(), file_src);
	cp(std::string(working_dir_local_broker4).append(file_dest).c_str(), file_src);
	strcpy(file_dest, "/cinta_checkin.init");
	strcpy(file_src, "../local_broker/locks/cinta_checkin.init");
	cp(std::string(working_dir_local_broker1).append(file_dest).c_str(), file_src);
	cp(std::string(working_dir_local_broker2).append(file_dest).c_str(), file_src);
	cp(std::string(working_dir_local_broker3).append(file_dest).c_str(), file_src);
	cp(std::string(working_dir_local_broker4).append(file_dest).c_str(), file_src);
	strcpy(file_dest, "/cinta_contenedor.init");
	strcpy(file_src, "../local_broker/locks/cinta_contenedor.init");
	cp(std::string(working_dir_local_broker1).append(file_dest).c_str(), file_src);
	cp(std::string(working_dir_local_broker2).append(file_dest).c_str(), file_src);
	cp(std::string(working_dir_local_broker3).append(file_dest).c_str(), file_src);
	cp(std::string(working_dir_local_broker4).append(file_dest).c_str(), file_src);
	strcpy(file_dest, "/cinta_escaner_control.init");
	strcpy(file_src, file_src);
	cp(std::string(working_dir_local_broker1).append(file_dest).c_str(), file_src);
	cp(std::string(working_dir_local_broker2).append(file_dest).c_str(), file_src);
	cp(std::string(working_dir_local_broker3).append(file_dest).c_str(), file_src);
	cp(std::string(working_dir_local_broker4).append(file_dest).c_str(), file_src);
	strcpy(file_dest, "/cinta_escaner.init");
	strcpy(file_src, "../local_broker/locks/cinta_escaner.init");
	cp(std::string(working_dir_local_broker1).append(file_dest).c_str(), file_src);
	cp(std::string(working_dir_local_broker2).append(file_dest).c_str(), file_src);
	cp(std::string(working_dir_local_broker3).append(file_dest).c_str(), file_src);
	cp(std::string(working_dir_local_broker4).append(file_dest).c_str(), file_src);
	strcpy(file_dest, "/cinta_principal.init");
	strcpy(file_src, "../local_broker/locks/cinta_principal.init");
	cp(std::string(working_dir_local_broker1).append(file_dest).c_str(), file_src);
	cp(std::string(working_dir_local_broker2).append(file_dest).c_str(), file_src);
	cp(std::string(working_dir_local_broker3).append(file_dest).c_str(), file_src);
	cp(std::string(working_dir_local_broker4).append(file_dest).c_str(), file_src);

	strcpy(file_dest, "/simulacion.ini");
	strcpy(file_src, "./config/simulacion.ini");
	for (i = all_dirs.begin(); i != all_dirs.end() ; i++) {
		cp(std::string(*i).append(file_dest).c_str(), file_src);
	}

	strcpy(file_dest, "/local_broker.list");
	strcpy(file_src, "./locks/local_broker.list");
	for (i = all_dirs.begin(); i != all_dirs.end() ; i++) {
		cp(std::string(*i).append(file_dest).c_str(), file_src);
	}

	strcpy(file_dest, "/aviones.csv");
	strcpy(file_src, "./locks/aviones.csv");
	cp(std::string(working_dir_generador_vuelos_trasbordo).append(file_dest).c_str(), file_src);

	strcpy(file_dest, "/vuelos_interconexion.csv");
	strcpy(file_src, "./locks/vuelos_interconexion.csv");
	cp(std::string(working_dir_generador_vuelos_trasbordo).append(file_dest).c_str(), file_src);

	strcpy(file_dest, "/vuelos_entrantes.csv");
	strcpy(file_src, "./locks/vuelos_entrantes.csv");
	cp(std::string(working_dir_generador_vuelos_trasbordo).append(file_dest).c_str(), file_src);

	strcpy(file_dest, "/equipaje_destino_1.csv");
	strcpy(file_src, "./locks/equipaje_destino_1.csv");
	cp(std::string(working_dir_generador_vuelos_trasbordo).append(file_dest).c_str(), file_src);

	strcpy(file_dest, "/equipaje_destino_101.csv");
	strcpy(file_src, "./locks/equipaje_destino_101.csv");
	cp(std::string(working_dir_generador_vuelos_trasbordo).append(file_dest).c_str(), file_src);

	strcpy(file_dest, "/equipaje_destino_102.csv");
	strcpy(file_src, "./locks/equipaje_destino_102.csv");
	cp(std::string(working_dir_generador_vuelos_trasbordo).append(file_dest).c_str(), file_src);

	strcpy(file_dest, "/equipaje_destino_3.csv");
	strcpy(file_src, "./locks/equipaje_destino_3.csv");
	cp(std::string(working_dir_generador_vuelos_trasbordo).append(file_dest).c_str(), file_src);

	strcpy(file_dest, "/equipajes_intercargo.csv");
	strcpy(file_src, "./locks/equipajes_intercargo.csv");
	cp(std::string(working_dir_generador_vuelos_trasbordo).append(file_dest).c_str(), file_src);

	strcpy(file_dest, "/equipajes.csv");
	strcpy(file_src, "./locks/equipajes.csv");
	cp(std::string(working_dir_generador_vuelos_trasbordo).append(file_dest).c_str(), file_src);

	strcpy(file_dest, "/pasajeros.csv");
	strcpy(file_src, "./locks/pasajeros.csv");
	cp(std::string(working_dir_generador_vuelos_trasbordo).append(file_dest).c_str(), file_src);

	strcpy(file_dest, "/vuelos.csv");
	strcpy(file_src, "./locks/vuelos.csv");
	cp(std::string(working_dir_generador_vuelos_trasbordo).append(file_dest).c_str(), file_src);

}

static char *args_intermediate_broker [] = {(char*)"intermediate_broker_launcher", NULL};
static char *args_message_broker [] = {(char*)"message_broker_launcher", NULL};
static char *args_torre_de_control [] = {(char*)"torre_de_control_launcher", NULL};

static int selecc_arch(const struct dirent *entry) {
	if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))
		return 0;
	else
		return 1;
}

std::list<std::string> ls(const char * folder) {
	std::list<std::string> lista;
	std::string s;
	//char cadena[80];
	int contar,i;

	struct dirent **archivos;
	//chdir(folder.c_str()); // OK para moverte de directorios
	//cout << getcwd(cadena,80) << endl; // Para saber en que directorio estas

	// Devuelve una lista con los archivos en la carpeta
	// si le pasas NULL(funcion comparar, te devuelve . y ..) si no queres eso
	// usa la funcion select
	contar = scandir(folder, &archivos, selecc_arch, alphasort);

	/* Si no se encontraron archivos *//*Version Simple del comando ls*/
	/*if (contar <= 0) {
	 cout << "No hay archivos en este direntorio\n";
	 exit(0);
	 }*/
	//cout << "Hay " << contar << " archivos.\n";
	for (i = 0; i < contar ; ++i) {
		//cout << archivos[i]->d_name << "\n";
		lista.push_back(archivos [i]->d_name);
		free(archivos [i]);
	}
	free(archivos);
	//cout << "\n"; /* flush buffer */

	return lista;
}

void chequear_si_hay_duplicados() {
   /*
	std::vector<char *>::iterator i;
	std::list<std::string>::iterator j;
	std::map<Key, std::string> keys;
	std::list<std::string> lista;
	for (i = all_dirs.begin(); i != all_dirs.end() ; i++) {
		lista = ls(*i);
		for (j = lista.begin(); j != lista.end() ; j++) {
			if ((*j).find(".lck", (*j).size() - 4) != std::string::npos) {
				Key k = get_key(std::string(*i).append("/").append(*j).c_str(), 0);
				if(keys.count(k)>0){
					std::cout << "Colision " << std::string(*i).append("/").append(*j) << ":" << keys.at(k) << std::endl;
				}else {
					keys.insert(std::pair<Key,std::string>(k,std::string(*i).append("/").append(*j)));
				}
			}
		}
	}
   */
}

int main(int argc, char** argv)

try
{
	strcpy(config_file, (argc > 1) ? argv [1] : DEFAULT_CONFIG_FILE);

	chdir("processes");
	//be_a_daemon();

	ignore_signals();
	Log::notice("Iniciando...");
	Log::info("Creando aeropuerto...");
	std::list<Process> processes;

	load_all_dirs();
	chequear_si_hay_duplicados();
	mkdirs();
	copy_files();

	{
		strncpy(directorio_de_trabajo, ApiConfiguracion::get_wkdir(config_file).c_str(), MAX_PATH_SIZE);
		sprintf(id_productor_cinta_central, "%d", MAX_SCANNERS + 1);

		//ConexionesAeropuerto aeropuerto(directorio_de_trabajo, config_file);

		Log::info("iniciando simulación...");

		// Lanzo primero el broker
		chdir("../local_broker");
		processes.push_back(Process("local_broker_launcher", args_local_broker1));

		chdir("../processes");

		processes.push_back(Process("intermediate_broker_launcher", args_intermediate_broker));
		processes.push_back(Process("message_broker_launcher", args_message_broker));
		sleep(2);
		processes.push_back(Process("torre_de_control_launcher", args_torre_de_control));
      sleep(1);

		processes.push_back(Process("puesto_checkin", args_puesto_checkin1));
		processes.push_back(Process("puesto_checkin", args_puesto_checkin2));
		processes.push_back(Process("puesto_checkin", args_puesto_checkin3));

		processes.push_back(Process("robot_checkin", args_robot_checkin));
		processes.push_back(Process("robot_scanner", args_scanner1));
		processes.push_back(Process("robot_scanner", args_scanner2));
		processes.push_back(Process("robot_despacho", args_robot_despacho1));
		processes.push_back(Process("robot_despacho", args_robot_despacho2));
		processes.push_back(Process("robot_carga", args_robot_carga1));
		processes.push_back(Process("robot_carga", args_robot_carga2));
		processes.push_back(Process("robot_carga", args_robot_carga3));
		processes.push_back(Process("robot_carga", args_robot_carga4));
		processes.push_back(Process("robot_control_equipaje_sospechoso", args_robot_sospechosos));
		processes.push_back(Process("robot_intercargo", args_robot_intercargo));
		//processes.push_back(Process("control_equipaje_sospechoso", args_robot_sospechosos));
		processes.push_back(Process("generador_vuelos_trasbordo", args_generador_vuelos_trasbordo));
		// processes.push_back(Process("escucha_zonas_asignadas", args_escucha_zonas_asignadas));
//		Pocesses.push_back(Process("torre_de_control", args_torre_de_control));
		processes.push_back(Process("tractor", args_tractor1));
		//processes.push_back(Process("tractor", args_tractor2));
		processes.push_back(Process("scheduler_aviones", args_scheduler_aviones));
		processes.push_back(Process("scheduler_vuelos", args_scheduler_vuelos));

		// sin argumentos lanzo todos los vuelos posibles.
		// if (argc == 1) {
		// 	Log::info("Lanzo todos los vuelos registrados %s", argv[0]);
		// 	lanzar_vuelos();
		// } else {
		//    for(int i = 1; i<argc-1; i++) {
		//       lanzar_vuelo(i);
		//    }
		// }

		Log::notice("Done, waiting for a SIGINT or a SIGTERM signal.");
		wait_signal();
		Log::notice("Signal recieved. Shutdown...");
	}

	sleep(5);
	Log::notice("Finalizando procesos. Enviando SIGTERM...");
	for (std::list<Process>::iterator it = processes.end() ; it != processes.begin() ; --it)
		it->send_signal(SIGTERM);

	sleep(2);
	Log::notice("Finalizando procesos. Enviando SIGKILL...");
	for (std::list<Process>::iterator it = processes.end() ; it != processes.begin() ; --it)
		it->send_signal(SIGKILL);

	Log::info("finalizando simulación...");

	return 0;
}

catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

// void lanzar_vuelos() {
// 	Database db("aeropuerto", true);
// 	yasper::ptr<Statement> query = db.statement("select id from Vuelo");

// 	yasper::ptr<TupleIterator> p_it = query->begin();
// 	yasper::ptr<TupleIterator> p_end = query->end();

// 	for (; (*p_it) != (*p_end); ++(*p_it)) {
// 		lanzar_vuelo(p_it->at<int>(0));
// 	}
// }

// void lanzar_vuelo(int num_vuelo) {
// 	Log::info("Lanzando vuelo %d", num_vuelo);
// 	std::stringstream sstr;
// 	sstr << num_vuelo;
// 	strcpy(buffer_num_vuelo, sstr.str().c_str());

// 	Process controlador_vuelo1("controlador_de_vuelo", args_controlador_vuelo);
// }
