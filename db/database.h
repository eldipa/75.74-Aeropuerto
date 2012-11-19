#ifndef DATABASE_H_
#define DATABASE_H_

#include <string>
#include "yasper.h"

class Statement;
class sqlite3;

/*
 * This is the primary object that establish a connection with the database engine
 * and its implementation.
 *
 */

class Database {
   private:
      std::string filename;
      sqlite3 *database;

   public:
      /*
       * Create a connection with one database (specified with the path of the filename of
       * the database.
       * Multiple connections con be instantiated with the same or distinct database.
       *
       * For now, one Database object can connect with only one database file.
       *
       * If the parameter 'only_for_read' is true, the only permited operations must not
       * change the data in the database.
       * */
      Database(const char* filename, bool only_for_read=false);

      /*
       * This method will create an instruction to be executed one or more times.
       * The particular instruction can be any simple instruction permited by SQL like
       * INSERT, DROP, UPDATE, SELECT, etc.
       * Only simple and single instruction are allowed and must not use the COMMIT
       * statement.
       *
       * Usually, though, it is not useful to evaluate exactly the same SQL statement more than once. 
       * More often, one wants to evaluate similar statements. 
       * For example, you might want to evaluate an INSERT statement multiple times though with different values to insert. 
       * To accommodate this kind of flexibility, the engine allows SQL statements to contain parameters 
       * which are "bound" to values prior to being evaluated. 
       * These values can later be changed and the same prepared statement can be evaluated a 
       * second time using the new values. 
       *
       * The SQL statement can contain parameters of the form:
       *     :AAA
       *     $AAA
       *     @AAA 
       * 
       * Where AAA is an alphanumeric identifier. (Documentation extracted from SQLite 3)
       * To set or unset any of that parameter, see the documentation of Statement.
       *
       * This method will return an automatic pointer to Statement. This is required because the resources allocated 
       * by Statement must be released.
       * */
      yasper::ptr<Statement> statement(const char* sql_stmt);

      ~Database() throw();
};

#endif
