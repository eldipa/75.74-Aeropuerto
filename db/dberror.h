#ifndef DB_ERROR_H_
#define DB_ERROR_H_

#include <typeinfo>
#include <sqlite3.h>

#define DBError_LEN_BUFF_ERROR  1024

/*
 * This exception is raised when a function returns a database-related error 
 * The error_number method returns a numeric error code (see sqlite3_errcode())
 * The 'what' message is the concatenation of the message of the user (passed in the
 * constructor) and the corresponding string, as would be printed by sqlite3_errmsg()
 *
 * See sqlite3_errmsg and sqlite3_errcode.
 *
 * */

class DBError : public std::exception {
   private:
      char msg_error[DBError_LEN_BUFF_ERROR];
      int _errno;

   public:
      explicit DBError(sqlite3 &db, const char *format, ...) throw();
      int error_number() const throw();
      virtual const char *what() const throw(); 
      virtual ~DBError() throw() {}
};

#endif
