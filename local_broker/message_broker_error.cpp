#include "message_broker_error.h"

#include <cstdio>
#include <cstdarg>

#include "traceback.h"
#include <cstring>

MessageBrokerError::MessageBrokerError(const char *format, ...) throw() {
   va_list args;
   va_start (args, format);

   // NOTE: vsprintf doesn't check the bounds.
   vsprintf(msg_error, format, args);

   int count = strlen(msg_error);
   if(count < MessageBrokerError_LEN_BUFF_ERROR) {
      msg_error[MessageBrokerError_LEN_BUFF_ERROR-1] = 0; //guard
      count = strlen(msg_error);
      msg_error[count] = '\n';
      traceback(msg_error+count+1, MessageBrokerError_LEN_BUFF_ERROR-count-1);
   }

   msg_error[MessageBrokerError_LEN_BUFF_ERROR-1] = 0; //guard
   va_end(args);
}

const char *MessageBrokerError::what() const throw() {
   return msg_error;
}
