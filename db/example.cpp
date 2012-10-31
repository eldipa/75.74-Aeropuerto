#include <iostream>
#include <memory>
#include "database.h"
#include "stmt.h"
#include "tupleiter.h"
#include "log.h"

int main(int , char *[]) try {
   //Primero, como en todo base de datos, hay que conectarse a esta indicando el nombre del archivo
   //que contiene la bd. Por ahora solo se soporta un unico archivo por Database y debe ser local
   Database db("scripts/aeropuerto", false);

   //Ahora construimos un statement o instruccion SQL. Puede ser una query, un insert, un update, ...
   //pero nunca puede contener un COMMIT.
   std::auto_ptr<Statement> ins = db.statement("insert into FechaHora (fecha, hora, minuto) values (date('2012-10-29'), :hora, :minutos)");
   std::auto_ptr<Statement> query = db.statement("select * from FechaHora");

   //Notese como la instruccion "ins" tiene componentes variables.
   //Para setearles valores particulares basta con:
   ins->set(":hora", 10);
   ins->set(":minutos", 45);

   //La instruccion SQL esta construida pero no es ejecutada.
   //Para "comenzar" a ejecutarla se debe llamar a begin
   //Es importante aclarar que cada llamada a begin REINICIA la ejecucion de ese statement haciendo 
   //que otros iteradores queden invalidos.
   //
   //Para este caso, la instruccion insert no devuelve ningun listado de resultados por lo que con
   //llamar a begin alcanza con ejecutar y terminar la instruccion
   ins->begin();
    
   //Para el caso de una query, esta puede retornar varios resultados. Para poder tener accesso a
   //ellos basta con usar el iterador retornado por begin
   std::auto_ptr<TupleIterator> p_it = query->begin();
   std::auto_ptr<TupleIterator> p_end = query->end();
   
   //Estas dos lineas no son mas que unos alias
   TupleIterator &it = *p_it;
   TupleIterator &end = *p_end;

   //El mitico idiom de C++
   for(; it != end ; ++it) {
      //Para derefenciar una columna (o elemento de la tupla retornada), basta con llamar a "at" 
      //Dado que la tupla contiene varios tipos de elementos, es necesario especificarlo como un
      //argumento de template.
      std::cout << "Id: " << it.at<int>(0)  
          << " Fecha: " << it.at<Element::text>(1)  //Element::text == const unsigned char * (puf!)
          << " Hora: " << it.at<int>(2) << ":" << it.at<int>(3) << std::endl;
   }
    
   //Es importante aclarar que TupleIterator representa una instruccion de SQL en curso y que si no
   //es destruido, puede dejar lockeada toda la base de datos.
   //Destruir explicitamente los iteradores o bien, dejar que auto_ptr<TupleIterator> se vaya de scope
   //y sea auto_ptr que los destruya automaticamente.
   return 0;
} catch(const std::exception &e) {
   Log::crit("%s", e.what());
} catch(...) {
   Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}
