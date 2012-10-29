#ifndef ELEMENT_H_
#define ELEMENT_H_

#include <sqlite3.h>
#include "notimplementederror.h"

class TupleIterator;

class SuperE {
   protected:
      sqlite3_stmt &stmt;
      int i;
      SuperE(sqlite3_stmt &stmt, int i) : stmt(stmt), i(i) {}
};

template<class T>
class E : public SuperE {
   friend class TupleIterator;
   E(sqlite3_stmt &stmt, int i) : SuperE(stmt, i) {
      throw NotImplementedError("This template funcion (or object) should be called with a template argument T specific. See the class TupleIterator.");
   }
};

template<>
class E<int> : public SuperE {
   friend class TupleIterator;
   E(sqlite3_stmt &stmt, int i) : SuperE(stmt, i) {}
   operator int(){
      return sqlite3_column_int(&stmt, i);
   }
};

template<>
class E<double> : public SuperE {
   friend class TupleIterator;
   E(sqlite3_stmt &stmt, int i) : SuperE(stmt, i) {}
   operator double(){
      return sqlite3_column_double(&stmt, i);
   }
};


template<>
class E<const unsigned char*> : public SuperE {
   friend class TupleIterator;
   E(sqlite3_stmt &stmt, int i) : SuperE(stmt, i) {}
   operator const unsigned char*() {
      return sqlite3_column_text(&stmt, i);
   }
};

template<>
class E<const void*> : public SuperE {
   friend class TupleIterator;
   E(sqlite3_stmt &stmt, int i) : SuperE(stmt, i) {}
   operator const void*(){
      return sqlite3_column_blob(&stmt, i);
   }
};
#endif
