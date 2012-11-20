#ifndef TUPLE_H_
#define TUPLE_H_

#include "stmt.h"
#include "element.h"
#include "yasper.h"

class sqlite3;
class sqlite3_stmt;

class TupleIterator {
   private:
      sqlite3 &db;
      sqlite3_stmt &stmt;
      bool done;
      bool must_be_reset;

      TupleIterator(sqlite3 &db, sqlite3_stmt &stmt, bool done=false);
      TupleIterator(const TupleIterator&);
      TupleIterator& operator=(const TupleIterator&);
      
      void check(int i) const;

      friend yasper::ptr<TupleIterator> Statement::begin();
      friend yasper::ptr<TupleIterator> Statement::end();
   public:

      bool operator!=(const TupleIterator& t);
      TupleIterator& operator++();
    
      /*
       * This method return the number of columns of each tuple iterated.
       * */     
      int size() const;
        
      /*
       * With this method, the iterator can access to one column and interpret it according the template argument.
       * For example, to read the second column which is an integer you should execute:
       *    it.at<int>(1)
       * 
       * As you can see, the first column has the index 0.
       * The specific types that you can use are in Element:: namespaces and are:
       *    Element::integer (or int)
       *    Element::real    (or double)
       *    Element::text    (or const unsigned char*)
       *    Element::blob    (or const void*)
       * 
       * Other type may cause an exception in runtime.
       *
       * If you use the wrong type, the underlying engine will perform a conversion.
       *    An Element::  | when is readed as an Element::   | then you will obtain
       *      integer     |              real                | the real version (double) of the integer (int)
       *  integer or real |           text or blob           | the same number but as a string like "12"
       *       real       |             integer              | the number truncated
       *   text or blob   |         integer or real          | the conversion of the string using atoi/atof (beware!)
       *       text       |              blob                | no change
       *       blob       |              text                | add a nul character at the end if it is needed
       *
       * If the readed value is NULL this will be interpreted as a 0 (for Element::integer and Element::real) or as a null pointer
       * (for Element::text and Element::blob)
       *
       * The types Element::text and Element::blob are pointers to an area that is managed by the internal engine. 
       * You must not use free(), delete, or any like that.
       *
       * It is responsibility of the upper layers know the size of the Element::text and the Element::blob. There is
       * no way to obtain that sizes.
       * */
      template<typename T> T at(int i) {
         check(i);
         return Element::_private::E<T>(stmt, i);
      }

      ~TupleIterator() throw();
};


#endif
