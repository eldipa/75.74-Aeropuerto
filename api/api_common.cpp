/*
 * ApiCommon.cpp
 *
 *  Created on: 20/02/2013
 *      Author: gonzalo
 */

#include "api_common.h"
#include <sstream>

std::string intToString(int numero) {
	std::stringstream ss;
	ss << numero;
	return ss.str();
}
