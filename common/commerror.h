/*
 * GenericError.h
 *
 *  Created on: 06/02/2013
 *      Author: gonzalo
 */

#ifndef COMMERROR_H_
#define COMMERROR_H_

#include <typeinfo>

#define CommError_LEN_BUFF_ERROR  1024

class CommError : public std::exception {
   private:
      char msg_error[CommError_LEN_BUFF_ERROR];

   public:
      explicit CommError(const char *format, ...) throw();
      virtual const char *what() const throw();
      virtual ~CommError() throw() {}
};

#endif /* GENERICERROR_H_ */
