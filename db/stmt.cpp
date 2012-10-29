
#include <sqlite3.h>
#include <memory>
#include "stmt.h"
#include "dberror.h"
#include "valueerror.h"
#include "tuple.h"
#include "log.h"


Statement::Statement(sqlite3 &db, const char *sql_stmt) : db(db), stmt(0) {
   if(sqlite3_prepare(&db, sql_stmt, -1, &stmt, 0) != SQLITE_OK)
      throw DBError(db, "The SQL statement '%s' cannot be created to be used by the database engine.", sql_stmt);
}

std::auto_ptr<TupleIterator> Statement::begin() {
   if(not stmt)
      throw ValueError("The statement is null.");

   if(sqlite3_reset(stmt) != SQLITE_OK)
      throw DBError(db, "The statement cannot be reset (or initializated).");

   return std::auto_ptr<TupleIterator>(new TupleIterator(db, *stmt));
}

std::auto_ptr<TupleIterator> Statement::end() {
   if(not stmt)
      throw ValueError("The statement is null.");

   return std::auto_ptr<TupleIterator>(new TupleIterator(db, *stmt, true));
}

void Statement::finalize() {
   if(sqlite3_finalize(stmt) != SQLITE_OK) {
      throw DBError(db, "The SQL statement cannot be finalized.");
   }
   stmt = 0;
}

Statement::~Statement() throw() try {
   finalize();
} catch(const std::exception &e) {
   Log::crit("An exception happend during the course of a destructor:\n%s", e.what());
} catch(...) {
   Log::crit("An unknow exception happend during the course of a destructor.");
}
