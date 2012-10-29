#ifndef TUPLE_H_
#define TUPLE_H_

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
      
      friend std::auto_ptr<TupleIterator> Statement::begin();
      friend std::auto_ptr<TupleIterator> Statement::end();
   public:

      bool operator!=(const TupleIterator& t);
      TupleIterator& operator++();
      const Tuple operator*();

      ~TupleIterator() throw();
};

class Tuple {
   private:
      mutable sqlite3_stmt *stmt;

      Tuple(sqlite3_stmt &stmt);
      Tuple(const Tuple& t);
      Tuple& operator=(const Tuple&);

      void check(int i) const;
      
      friend const Tuple TupleIterator::operator*();
   public:
      int size() const;

      template<class T> T operator[](int i) const {
         throw NotImplementedError("This template funcion should be called with a template argument T specific. See the class Tuple.");
      }

};

template<> int Tuple::operator[]<int>(int i) const {
   check(i);
   return sqlite3_column_int(stmt, i);
}

template<> double Tuple::operator[]<double>(int i) const {
   check(i);
   return sqlite3_column_double(stmt, i);
}

template<> const unsigned char* Tuple::operator[]<const unsigned char*>(int i) const {
   check(i);
   return sqlite3_column_text(stmt, i);
}

template<> const void* Tuple::operator[]<const void*>(int i) const {
   check(i);
   return sqlite3_column_blob(stmt, i);
}

#endif
