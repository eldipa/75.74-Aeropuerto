#ifndef MESSAGE_BROKER_ERROR_H_
#define MESSAGE_BROKER_ERROR_H_

#include <typeinfo>

#define MessageBrokerError_LEN_BUFF_ERROR  1024

class MessageBrokerError : public std::exception {
   private:
      char msg_error[MessageBrokerError_LEN_BUFF_ERROR];

   public:
      explicit MessageBrokerError(const char *format, ...) throw();
      virtual const char *what() const throw();
      virtual ~MessageBrokerError() throw() {}
};

#endif /* MESSAGE_BROKER_ERROR_H_ */
