
#include <cstdio>
#include <errno.h>
#include <cstdarg> 

#include "dberror.h"
#include "traceback.h"
#include <cstring>


DBError::DBError(sqlite3 &db, const char *format, ...) throw() {
   _errno = sqlite3_errcode(&db);
   memset(msg_error, 0, DBError_LEN_BUFF_ERROR);

   va_list args;
   va_start (args, format);

   // NOTE: vsprintf doesn't check the bounds.
   vsprintf(msg_error, format, args);
   
   // msg_error + strlen(msg_error) allways overrides the '\0'
   // character from the previous inicialization of msg_error
   // In other words, this works as a concatenation of the message
   // of the user and the message of the system.
   sprintf(msg_error+strlen(msg_error), " [code %i]: ", _errno);

   const char *_m = sqlite3_errmsg(&db);
   int count = strlen(msg_error);
   if(count < DBError_LEN_BUFF_ERROR) {
      if(_m) {
         strncpy(msg_error+count, _m, DBError_LEN_BUFF_ERROR-count);
      } else {
         strncpy(msg_error+count, "Unknow.", DBError_LEN_BUFF_ERROR-count);
      }
   
      msg_error[DBError_LEN_BUFF_ERROR-1] = 0; //guard
      count = strlen(msg_error);
      msg_error[count] = '\n';
      traceback(msg_error+count+1, DBError_LEN_BUFF_ERROR-count-1);
   }
   msg_error[DBError_LEN_BUFF_ERROR-1] = 0; //guard
   va_end(args);
}

int DBError::error_number() const throw() {
   return _errno;
}

const char *DBError::what() const throw() {
   return msg_error;
}

