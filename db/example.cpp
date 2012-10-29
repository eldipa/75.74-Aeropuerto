#include <iostream>
#include <memory>
#include "database.h"
#include "stmt.h"
#include "tuple.h"

int main(int , char *[]) try {
   Database db("scripts/aeropuerto", true);
   std::auto_ptr<Statement> s = db.statement("select * from FechaHora;");
   std::auto_ptr<TupleIterator> p_it = s->begin();
   std::auto_ptr<TupleIterator> p_end = s->end();
   
   TupleIterator &it = *p_it;
   TupleIterator &end = *p_end;
   for(; it != end ; ++it) {
      std::cout << "Id: " << it.at<int>(0) << " Fecha: " << it.at<const unsigned char*>(1) << std::endl;
   }

   return 0;
}
catch(...) {
}
