#ifndef STATEMENT_H_
#define STATEMENT_H_

#include <memory>
#include "database.h"

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
   public:
      std::auto_ptr<TupleIterator> begin();
      std::auto_ptr<TupleIterator> end();

      void finalize();

      ~Statement() throw();
};

#endif
