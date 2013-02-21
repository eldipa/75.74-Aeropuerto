/*
 * InitParser.h
 *
 *  Created on: 14/02/2013
 *      Author: gonzalo
 */

#ifndef INITPARSER_H_
#define INITPARSER_H_
#include <cstring>
#include <cstdio>
#include <cstdlib>

typedef enum {
	T_INT
} T_TYPE;

class InitParser {
private:

	char * linea;
	int size;

	int valor_int;
	int cant_vector;
	T_TYPE tipo;

	static bool cmp(const char *, const char *);

	void parse_type(const char *);

public:
	InitParser();
	virtual ~InitParser();

	void parse_line(const char * line);
	static int parse_int_val(const char *);
	static size_t parse_size_t_val(const char *);
	bool is_vector();
	int vector();
	int int_val();

	T_TYPE type();
};

#endif /* INITPARSER_H_ */
