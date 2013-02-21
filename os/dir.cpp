/*
 * dir.cpp
 *
 *  Created on: 18/02/2013
 *      Author: gonzalo
 */

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "dir.h"
#include <cstring>
#include "genericerror.h"

void locate_dir(char launch_dir [FILENAME_MAX], char current_working_dir [FILENAME_MAX], char dir [FILENAME_MAX]) {
	struct stat buf;
	char aux_dir [FILENAME_MAX];
	char * dir_base1;
	//const char * dir_base2;

	strcpy(aux_dir, current_working_dir);
	strcpy(launch_dir, aux_dir);
	strcat(launch_dir, "/");
	strcat(launch_dir, dir);
	if (stat(launch_dir, &buf) != 0) { // no existe
		dir_base1 = strrchr(aux_dir, '/') + 1;
		(*dir_base1) = '\0';
		strcpy(launch_dir, aux_dir);
		strcat(launch_dir, dir);
		if (stat(launch_dir, &buf) != 0) {
			throw GenericError("Cannot locate %s", dir);
		}
	}
}

void relativize_dir(char directorio_relativo [FILENAME_MAX], const char directorio_de_trabajo [FILENAME_MAX],
	const char launch_dir [FILENAME_MAX], const char current_dir [FILENAME_MAX])
{
	const char * dir;
	const char *dir_cmp;
	const char *dir_move;
	size_t i;
	const char * dir_base1;
	const char * dir_base2;
	char * dir_base_aux;

	dir = strrchr(launch_dir, '/');
	if (dir != NULL) {
		dir++;

		if (directorio_de_trabajo [0] == '.' && directorio_de_trabajo [1] == '/') {
			dir_cmp = directorio_de_trabajo + 2;
			if (strncmp(dir_cmp, dir, strlen(dir)) == 0) {
				strcpy(directorio_relativo, "./");

				dir_move = strchr(dir_cmp, '/') + 1;
				strcat(directorio_relativo, dir_move);
			} else {
				dir_base1 = current_dir;
				dir_base2 = launch_dir;
				while (*dir_base1 == *dir_base2) {
					dir_base1++;
					dir_base2++;
				}
				strcpy(directorio_relativo, launch_dir);
				if (*dir_base2 != 0) {
					dir_base_aux = strrchr(directorio_relativo, '/') + 1;
					*dir_base_aux = '\0';
					//strcat(directorio_relativo, "../");
				} else {
					strcat(directorio_relativo, "/");
				}
				for (i = 0; i < strlen(dir_base2) ; i++) {
					if (dir_base2 [i] == '/') {
						dir_base_aux = strrchr(directorio_relativo, '/') + 1;
						*dir_base_aux = '\0';
						//strcat(directorio_relativo, "../");
					}
				}
				if (*dir_base1 != '\0') {
					strcat(directorio_relativo, dir_base1);
					strcat(directorio_relativo, "/");
				}
				strcat(directorio_relativo, directorio_de_trabajo);
			}
		}
	}
}

void relativize_file(char archivo_relativo [FILENAME_MAX], const char archivo [FILENAME_MAX],
	const char launch_dir [FILENAME_MAX])
{
	const char * dir;
	const char *dir_cmp;
	const char *dir_move;
	struct stat buf;
	char aux [FILENAME_MAX];

	dir = strrchr(launch_dir, '/');
	if (dir != NULL) {
		dir++;

		if (archivo [0] == '.' && archivo [1] == '/') {
			dir_cmp = archivo + 2;
			if (strncmp(dir_cmp, dir, strlen(dir)) == 0) {
				strcpy(archivo_relativo, "./");

				dir_move = strchr(dir_cmp, '/') + 1;
				strcat(archivo_relativo, dir_move);
			} else {
				strcpy(archivo_relativo, launch_dir);
				strcat(archivo_relativo, "/");
				int up = 0;
				//TODO fix
				strcpy(aux, archivo_relativo);
				strcat(aux, archivo);
				while (up < 2 && stat(archivo_relativo, &buf) != 0) {
					up++;
					strcpy(aux, archivo_relativo);
					strcat(archivo_relativo, "../");
					strcat(aux, archivo);
				}
				strcat(archivo_relativo, archivo);
			}
		}
	}
}
