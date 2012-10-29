#ifndef TUPLE_H_
#define TUPLE_H_

#include <sqlite3.h>
#include <memory>
#include "stmt.h"
#include "notimplementederror.h"

class Tuple;
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

      template<class T> T at(int) const {
         throw NotImplementedError("This template funcion should be called with a template argument T specific. See the class Tuple.");
      }

      ~TupleIterator() throw();
};


template<> int TupleIterator::at<int>(int i) const {
   check(i);
   return sqlite3_column_int(&stmt, i);
}

template<> double TupleIterator::at<double>(int i) const {
   check(i);
   return sqlite3_column_double(&stmt, i);
}

template<> const unsigned char* TupleIterator::at<const unsigned char*>(int i) const {
   check(i);
   return sqlite3_column_text(&stmt, i);
}

template<> const void* TupleIterator::at<const void*>(int i) const {
   check(i);
   return sqlite3_column_blob(&stmt, i);
}

#endif
