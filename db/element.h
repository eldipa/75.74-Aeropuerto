#ifndef ELEMENT_H_
#define ELEMENT_H_

#include <sqlite3.h>
#include "notimplementederror.h"

class TupleIterator;
class Statement;

namespace Element {
    //Supported types
    typedef int integer;
    typedef double real;
    typedef const unsigned char* text;
    typedef const void* blob;

    namespace _private {
        class SuperE {
            protected:
                sqlite3_stmt &stmt;
                int i;
                SuperE(sqlite3_stmt &stmt, int i) : stmt(stmt), i(i) {}
        };


        template<class T> class E : public SuperE {
            friend class ::TupleIterator;
            friend class ::Statement;
            E(sqlite3_stmt &stmt, int i) : SuperE(stmt, i) {
                throw NotImplementedError("This template funcion (or object) should be called with a template argument T specific. See the class TupleIterator.");
            }
        };


        template<> class E<int> : public SuperE {
            friend class ::TupleIterator;
            friend class ::Statement;
            E(sqlite3_stmt &stmt, int i) : SuperE(stmt, i) {}
            operator int(){
                return sqlite3_column_int(&stmt, i);
            }
            void set(int val) {
               sqlite3_bind_int(&stmt, i, val);
            }
        };

        template<> class E<double> : public SuperE {
            friend class ::TupleIterator;
            friend class ::Statement;
            E(sqlite3_stmt &stmt, int i) : SuperE(stmt, i) {}
            operator double(){
                return sqlite3_column_double(&stmt, i);
            }
            void set(double val) {
               sqlite3_bind_double(&stmt, i, val);
            }
        };


        template<> class E<text> : public SuperE {
            friend class ::TupleIterator;
            friend class ::Statement;
            E(sqlite3_stmt &stmt, int i) : SuperE(stmt, i) {}
            operator text() {
                return sqlite3_column_text(&stmt, i);
            }
            void set(text val) {
               //Mismatch. SQLite use "const char*" and "const unsigned char*" as 'text'
               sqlite3_bind_text(&stmt, i, (const char*)val, -1, SQLITE_TRANSIENT);
            }
        };

        template<> class E<blob> : public SuperE {
            friend class ::TupleIterator;
            friend class ::Statement;
            E(sqlite3_stmt &stmt, int i) : SuperE(stmt, i) {}
            operator blob(){
                return sqlite3_column_blob(&stmt, i);
            }
            void set(blob val, int size) {
               sqlite3_bind_blob(&stmt, i, (const char*)val, size, SQLITE_TRANSIENT);
            }
        };
    }
}
#endif
