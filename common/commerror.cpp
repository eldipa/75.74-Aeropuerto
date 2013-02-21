/*
 * GenericError.cpp
 *
 *  Created on: 06/02/2013
 *      Author: gonzalo
 */

#include "commerror.h"

#include <cstdio>
#include <cstdarg>

#include "traceback.h"
#include <cstring>

CommError::CommError(const char *format, ...) throw() {
   va_list args;
   va_start (args, format);

   // NOTE: vsprintf doesn't check the bounds.
   vsprintf(msg_error, format, args);

   int count = strlen(msg_error);
   if(count < CommError_LEN_BUFF_ERROR) {
      msg_error[CommError_LEN_BUFF_ERROR-1] = 0; //guard
      count = strlen(msg_error);
      msg_error[count] = '\n';
      traceback(msg_error+count+1, CommError_LEN_BUFF_ERROR-count-1);
   }

   msg_error[CommError_LEN_BUFF_ERROR-1] = 0; //guard
   va_end(args);
}

const char *CommError::what() const throw() {
   return msg_error;
}
