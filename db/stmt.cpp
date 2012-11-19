
#include <sqlite3.h>
#include "stmt.h"
#include "dberror.h"
#include "valueerror.h"
#include "tupleiter.h"
#include "log.h"
#include "yasper.h"


Statement::Statement(sqlite3 &db, const char *sql_stmt) : db(db), stmt(0) {
   if(sqlite3_prepare(&db, sql_stmt, -1, &stmt, 0) != SQLITE_OK)
      throw DBError(db, "The SQL statement '%s' cannot be created to be used by the database engine.", sql_stmt);
}

int Statement::index(const char* arg_name) {
   int i = sqlite3_bind_parameter_index(stmt, arg_name);
   if(i <= 0) 
      throw ValueError("The key '%s' is invalid (doesn't match with any argument.", arg_name);
   return i;
}

void Statement::set(const char* arg_name, Element::blob val, int size) {
   if(size <= 0) 
      throw ValueError("The size must be greater that zero.");
   if(not Element::_private::E<Element::blob>(*stmt, index(arg_name)).set(val, size))
      throw DBError(db, "The argument '%s' cannot be set.", arg_name);
}

void Statement::unset(const char* arg_name) {
   if(sqlite3_bind_null(stmt, index(arg_name)) != SQLITE_OK)
      throw DBError(db, "The argument '%s' cannot be unset (set to null).", arg_name);
}

void Statement::zeros(const char* arg_name, int size) {
   if(size <= 0) 
      throw ValueError("The size must be greater that zero.");
   if(sqlite3_bind_zeroblob(stmt, index(arg_name), size) != SQLITE_OK)
      throw DBError(db, "The argument '%s' cannot be filled with zeros.", arg_name);
}

yasper::ptr<TupleIterator> Statement::begin() {
   if(not stmt)
      throw ValueError("The statement is null.");

   if(sqlite3_reset(stmt) != SQLITE_OK)
      throw DBError(db, "The statement cannot be reset (or initializated).");

   return yasper::ptr<TupleIterator>(new TupleIterator(db, *stmt));
}

yasper::ptr<TupleIterator> Statement::end() {
   if(not stmt)
      throw ValueError("The statement is null.");

   return yasper::ptr<TupleIterator>(new TupleIterator(db, *stmt, true));
}

void Statement::finalize() {
   if(sqlite3_finalize(stmt) != SQLITE_OK) {
      throw DBError(db, "The SQL statement cannot be finalized.");
   }
   stmt = 0;
}

Statement::~Statement() throw() {
   if(sqlite3_finalize(stmt) != SQLITE_OK) {
      Log::crit("An exception happend during the course of a destructor:\n%s", DBError(db, "The SQL statement cannot be finalized.").what());
   }
} 
