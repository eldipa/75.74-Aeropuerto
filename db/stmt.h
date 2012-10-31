#ifndef STATEMENT_H_
#define STATEMENT_H_

#include <memory>
#include "database.h"
#include "element.h"
#include "dberror.h"

class TupleIterator;

class sqlite3;
class sqlite3_stmt;

class Statement {
   private:
      sqlite3 &db;
      sqlite3_stmt *stmt;

      Statement(sqlite3 &db, const char *sql_stmt);
      Statement(const Statement&);
      Statement operator=(const Statement&);

      friend std::auto_ptr<Statement> Database::statement(const char*);
      int index(const char* arg_name);
   public:
      std::auto_ptr<TupleIterator> begin();
      std::auto_ptr<TupleIterator> end();
   
      template<class T> void set(const char* arg_name, T val) {
         if(not Element::_private::E<T>(*stmt, index(arg_name)).set(val))
            throw DBError(db, "The argument '%s' cannot be set.", arg_name);
      }

      void set(const char* arg_name, Element::blob val, int size);
      void unset(const char* arg_name);
      void zeros(const char* arg_name, int size);

      void finalize();

      ~Statement() throw();
};

#endif
