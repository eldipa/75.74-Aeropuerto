
#include <sqlite3.h>
#include <memory>
#include <string>
#include "dberror.h"
#include "log.h"
#include "stmt.h"

Database::Database(const char* filename, bool only_for_read) : filename(filename), database(0) {
   if(sqlite3_open_v2(filename, 
            &database, 
            only_for_read ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE,
            0) != SQLITE_OK) {
      sqlite3_close(database);
      throw DBError(*database, "The database '%s' cannot be open.", filename);
   }
}

std::auto_ptr<Statement> Database::statement(const char* sql_stmt) {
   return std::auto_ptr<Statement>(new Statement(*database, sql_stmt));
}

Database::~Database() throw() try {
   sqlite3_close(database);
} catch(const std::exception &e) {
   Log::crit("An exception happend during the course of a destructor:\n%s", e.what());
} catch(...) {
   Log::crit("An unknow exception happend during the course of a destructor.");
}


