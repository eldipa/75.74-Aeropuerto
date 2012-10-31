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
      /*
       * This will execute the statement and will return an iterator to the first element.
       * If the statement doesn't return any result, the iterator returned will be the same like
       * the iterator returned by end().
       *
       * Because this is a sensible operation the iterator is returned as an automatic pointer. 
       * This is important, when you don't need more the iterator, the automatic pointer will release any resource
       * when the object is out of scope.
       *
       * AVOID to keep any iterator alive more than the necessary.
       *
       * Also beware that when you call to begin(), any other iterator will be invalid.
       * If you need to do more advanced operations, you should probably copy the results in a container like std::list or std::vector.
       * */
      std::auto_ptr<TupleIterator> begin();
      std::auto_ptr<TupleIterator> end();
   
      /*
       * The engine allows SQL statements to contain parameters which are "bound" to values prior to being evaluated. 
       * (See the documentation of Statement)
       * The SQL statement can contain parameters of the form:
       *    :AAA
       *    $AAA
       *    @AAA 
       * 
       * Where AAA is an alphanumeric identifier. 
       * To set a value to each parameter you need to call this method, where 'arg_name' is the name of you identifier
       * (including the special character :, $ or @)
       * The value 'val' is then copied and assigned.
       * 
       * In this case T must be Element::integer, Element::real or Element::text.
       * If you need to set a blob, use the overloaded version of set.
       * In the case of Element::text, the size of the text is deducted from the position of the first nul character.
       *
       * If you don't set any value (or if you call the method unset()), the value used will be the special value NULL.
       * */
      template<class T> void set(const char* arg_name, T val) {
         if(not Element::_private::E<T>(*stmt, index(arg_name)).set(val))
            throw DBError(db, "The argument '%s' cannot be set.", arg_name);
      }

      void set(const char* arg_name, Element::blob val, int size);
      void unset(const char* arg_name);

      /*
       * This special method will set a blob of size 'size' filled with zeros.
       * */
      void zeros(const char* arg_name, int size);
    
      void finalize();

      ~Statement() throw();
};

#endif
