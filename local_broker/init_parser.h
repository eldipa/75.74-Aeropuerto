/*
 * InitParser.h
 *
 *  Created on: 14/02/2013
 *      Author: gonzalo
 */

#ifndef INITPARSER_H_
#define INITPARSER_H_

typedef enum {
	T_INT
}T_TYPE;

class InitParser {
private:

	char * linea;
	int size;

	int valor_int;
	int cant_vector;
	T_TYPE tipo;

	bool cmp(const char *, const char *);

	int parse_int_val(const char *);
	void parse_type(const char *);

public:
	InitParser();
	virtual ~InitParser();

	void parse_line(const char * line);

	bool is_vector();
	int vector();
	int int_val();

	T_TYPE type();
};

#endif /* INITPARSER_H_ */
