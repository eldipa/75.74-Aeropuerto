/*
 * GenericError.cpp
 *
 *  Created on: 06/02/2013
 *      Author: gonzalo
 */

#include "genericerror.h"

#include <cstdio>
#include <cstdarg>

#include "traceback.h"
#include <cstring>

GenericError::GenericError(const char *format, ...) throw() {
   va_list args;
   va_start (args, format);

   // NOTE: vsprintf doesn't check the bounds.
   vsprintf(msg_error, format, args);

   int count = strlen(msg_error);
   if(count < GenericError_LEN_BUFF_ERROR) {
      msg_error[GenericError_LEN_BUFF_ERROR-1] = 0; //guard
      count = strlen(msg_error);
      msg_error[count] = '\n';
      traceback(msg_error+count+1, GenericError_LEN_BUFF_ERROR-count-1);
   }

   msg_error[GenericError_LEN_BUFF_ERROR-1] = 0; //guard
   va_end(args);
}

const char *GenericError::what() const throw() {
   return msg_error;
}
