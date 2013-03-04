/*
 * dir.h
 *
 *  Created on: 18/02/2013
 *      Author: gonzalo
 */

#ifndef DIR_H_
#define DIR_H_

#include <cstdio>

void locate_dir(char launch_dir [FILENAME_MAX], char current_working_dir [FILENAME_MAX], char dir [FILENAME_MAX]);

void relativize_dir(char directorio_relativo [FILENAME_MAX], const char directorio_de_trabajo [FILENAME_MAX],
	const char launch_dir [FILENAME_MAX],const char current_dir [FILENAME_MAX]);

void relativize_file(char archivo_relativo [FILENAME_MAX], const char archivo [FILENAME_MAX],
	const char launch_dir [FILENAME_MAX]);

int cp(const char *to, const char *from);
#endif /* DIR_H_ */
