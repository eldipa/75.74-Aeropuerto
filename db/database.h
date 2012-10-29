#ifndef DATABASE_H_
#define DATABASE_H_

#include <string>
#include <memory>

class Statement;
class sqlite3;

class Database {
   private:
      std::string filename;
      sqlite3 *database;

   public:
      Database(const char* filename, bool only_for_read);

      std::auto_ptr<Statement> statement(const char* sql_stmt);

      ~Database() throw();
};

#endif
