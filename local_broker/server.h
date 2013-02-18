#ifndef SERVER_H_
#define SERVER_H_

#include "socket.h"

class Server {

public:
	Server(const char* service);
	void run();
	virtual ~Server();

   virtual void client_handler(Socket&) = 0;

private:
   std::string service;
   bool continue_listening;
   int cant_childs;
};

#endif /* SERVER_H_ */

