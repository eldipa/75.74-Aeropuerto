%module ipc
%include "typemaps.i"
%include "std_string.i"

%{
#include "messagequeue.h"
#include <string>
#include <vector>
%}

%feature("autodoc", "0");

class MessageQueue {
    public:
        MessageQueue(const char *absolute_path, char proj_id, 
              int permissions = 0664, bool create = false);

        ~MessageQueue() throw();
};

%extend MessageQueue {
   void push(const std::string &msg) {
      $self->push(msg.data(), msg.size());
   }
   std::string pull(long type = 0) {
      std::vector<char> v;
      v.reserve(8192);
      ssize_t s = $self->pull(&v[0], 8192, type);
      
      return std::string(&v[0], s);
   }
};
