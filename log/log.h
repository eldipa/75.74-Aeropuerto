#ifndef LOG_H_
#define LOG_H_

namespace Log {
   //system is unusable
   void emerg(const char *format, ...);

   //action must be taken immediately
   void alert(const char *format, ...);

   //critical conditions
   void crit(const char *format, ...);

   //error conditions
   void err(const char *format, ...);

   //warning conditions
   void warning(const char *format, ...);

   //normal, but significant, condition
   void notice(const char *format, ...);

   //informational message
   void info(const char *format, ...);

   //debug-level message
   void debug(const char *format, ...);
}

#endif
