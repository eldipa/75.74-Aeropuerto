
#include <sqlite3.h>
#include <string>
#include "dberror.h"
#include "log.h"
#include "stmt.h"
#include "yasper.h"

Database::Database(const char* filename, bool only_for_read) : filename(filename), database(0) {
   if(sqlite3_open_v2(filename, 
            &database, 
            only_for_read ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE,
            0) != SQLITE_OK) {
      
      sqlite3_close(database); // This function has leaks
      throw DBError(*database, "The database '%s' cannot be open.", filename);
   }
}

yasper::ptr<Statement> Database::statement(const char* sql_stmt) {
   return yasper::ptr<Statement>(new Statement(*database, sql_stmt));
}

Database::~Database() throw()  {
   if(sqlite3_close(database) != SQLITE_OK)
      Log::crit("An exception happend during the course of a destructor:\n%s", DBError(*database, "The database '%s' cannot be closed.", filename.c_str()).what());
}


