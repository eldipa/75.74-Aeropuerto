/*
 * GenericError.h
 *
 *  Created on: 06/02/2013
 *      Author: gonzalo
 */

#ifndef GENERICERROR_H_
#define GENERICERROR_H_

#include <typeinfo>

#define GenericError_LEN_BUFF_ERROR  1024

class GenericError : public std::exception {
   private:
      char msg_error[GenericError_LEN_BUFF_ERROR];

   public:
      explicit GenericError(const char *format, ...) throw();
      virtual const char *what() const throw();
      virtual ~GenericError() throw() {}
};

#endif /* GENERICERROR_H_ */
