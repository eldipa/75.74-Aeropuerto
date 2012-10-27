
#include "log.h"
#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <syslog.h>

#define LogMessage_LOG_THIS(logtag, logpriority) \
      va_list args;                                \
      va_start (args, format);                     \
      char msg_error[1024];                        \
                                                   \
      const char *tag = logtag;                    \
      const int offset = strlen(tag);              \
      memcpy(msg_error, tag, offset);              \
      strncpy(msg_error + offset, format, 1024);   \
      msg_error[1023] = 0;                         \
                                                   \
      vsyslog(logpriority, msg_error, args);       \
      va_end(args)

namespace Log {
   void emerg(const char *format, ...) {
      LogMessage_LOG_THIS("[Emerg] ", LOG_EMERG);
   }

   void alert(const char *format, ...) {
      LogMessage_LOG_THIS("[Alert] ", LOG_ALERT);
   }

   void crit(const char *format, ...) {
      LogMessage_LOG_THIS("[Crit] ", LOG_CRIT);
   }

   void err(const char *format, ...) {
      LogMessage_LOG_THIS("[Err] ", LOG_ERR);
   }

   void warning(const char *format, ...) {
      LogMessage_LOG_THIS("[Warning] ", LOG_WARNING);
   }

   void notice(const char *format, ...) {
      LogMessage_LOG_THIS("[Notice] ", LOG_NOTICE);
   }

   void info(const char *format, ...) {
      LogMessage_LOG_THIS("[Info] ", LOG_INFO);
   }

   void debug(const char *format, ...) {
      LogMessage_LOG_THIS("[Debug] ", LOG_DEBUG);
   }
}
