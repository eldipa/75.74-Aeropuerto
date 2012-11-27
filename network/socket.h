#ifndef SOCKET_H_
#define SOCKET_H_

/*************************************************************************
 *                                                                       *
 *                        This work is licensed under a                  *
 *   CC BY-SA        Creative Commons Attribution-ShareAlike             *
 *                           3.0 Unported License.                       *
 *                                                                       * 
 *  Author: Di Paola Martin Pablo, 2012                                  *
 *                                                                       *
 *************************************************************************/

/*******************************************************************************
 *                                                                             *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS        *
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT          *
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A    *
 *  PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER  *
 *  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,   *
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,        *
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR         *
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF     *
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING       *
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS         *
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               *
 *                                                                             *
 *******************************************************************************/
#include <sys/socket.h>
#include <sys/types.h>
#include "oserror.h"

class Socket {
   private:
      int fd;
      bool isstream;
      bool isconnected;

   public:
      Socket(bool isstream=true) : 
         isstream(isstream),
         isconnected(false) {
         fd = socket(AF_INET, isstream? SOCK_STREAM : SOCK_DGRAM, 0);
         if(fd == -1)
            throw OSError("The socket cannot be created.")
      }

      void destination(const char* host, const char* service) {
         if(not host or not service)
            throw ValueError("The argument host [which is %s] and the service [which is %s] must not be null.", host? "not null" : "null", service? "not null" : "null");

         if(isconnected) {
            if(shutdown(fd, SHUT_RDWR) == -1)
               throw OSError("The socket cannot be disconnected (shutdown).").what();
         }

         struct addrinfo *result = resolve(host, service);
         try {
            if(::connect(fd, result->ai_addr, result->ai_addrlen) == -1)
               throw OSError("The connection to the host '%s' and the service '%s' has failed.", host, service);

            freeaddrinfo(result);

         }catch(...) {
            freeaddrinfo(result);
            throw;
         }
         
         isconnected = true;
      }

      void source(const char* service) {
         if(not service)
            throw ValueError("The argument service must not be null.");

         struct addrinfo *result = resolve(0, service);
         try {
            if(::bind(fd, result->ai_addr, result->ai_addrlen) == -1)
               throw OSError("The socket was not bound to the local address and the service '%s'.", service);

            freeaddrinfo(result);

         }catch(...) {
            freeaddrinfo(result);
            throw;
         }
      }

      void listen(int backlog) {
         if(::listen(fd, backlog) == -1)
            throw OSError("The socket was bound to the local address and the service '%s' but cannot stablish a queue of size %i for the comming connections.", service, backlog);
      }

      ssize_t sendsome(const void *buf, size_t data_len) {
         ssize_t count = ::send(fd, buf, data_len, MSG_NOSIGNAL);
         if(count == -1) 
            throw OSError("The message length %i cannot be sent.", data_len);
         
         return count
      }

      bool sendall(const void *buf, size_t data_len) {
         ssize_t count = 0;
         ssize_t burst = 0;
         while(data_len > count and burst > 0)
            count += (burst = sendsome(fd, buf + count, data_len - count));
         
         return burst > 0;
      }

      ~Socket() {
         if(isconnected) {
            if(shutdown(fd, SHUT_RDWR) == -1)
               Log::crit("An exception happend during the course of a destructor:\n%s", OSError(
                        "The socket cannot be disconnected (shutdown).").what());
         }

         if(close(fd) == -1)
            Log::crit("An exception happend during the course of a destructor:\n%s", OSError(
                     "The socket cannot be closed.").what());
      }

   private:
      struct addrinfo* resolve(const char* host, const char* service) {
         struct addrinfo hints;
         struct addrinfo *result = 0;
         int status = 0;

         memset(&hints, 0, sizeof(struct addrinfo));
         hints.ai_family = AF_INET;
         hints.ai_socktype = isstream? SOCK_STREAM : SOCK_DGRAM; 
         hints.ai_flags = host? 0 : AI_PASSIVE;
         hints.ai_protocol = 0;

         status = getaddrinfo(host, service, &hints, &result);
         if(status != 0) {
            if(status != EAI_SYSTEM)
               errno = 0; //The error code is not in the errno (it has garbage)
            throw OSError("The address cannot be obtained for the host '%s' and the service '%s': %s", host, service, gai_strerror(status));
         }

         if(not result) {
            errno = 0;
            throw OSError("The address cannot be obtained for the host '%s' and the service '%s', however no error was explicity generated.", host, service);
         }
         
         return result;
      }

};


#endif
