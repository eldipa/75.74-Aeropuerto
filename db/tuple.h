#ifndef TUPLE_H_
#define TUPLE_H_

#include <memory>
#include "stmt.h"
#include "element.h"

class sqlite3;
class sqlite3_stmt;

class TupleIterator {
   private:
      sqlite3 &db;
      sqlite3_stmt &stmt;
      bool done;
      bool reset;

      TupleIterator(sqlite3 &db, sqlite3_stmt &stmt, bool done=false);
      TupleIterator(const TupleIterator&);
      TupleIterator& operator=(const TupleIterator&);
      
      void check(int i) const;

      friend std::auto_ptr<TupleIterator> Statement::begin();
      friend std::auto_ptr<TupleIterator> Statement::end();
   public:

      bool operator!=(const TupleIterator& t);
      TupleIterator& operator++();
      
      int size() const;

      template<typename T> T at(int i) {
         check(i);
         return E<T>(stmt, i);
      }

      ~TupleIterator() throw();
};


#endif
