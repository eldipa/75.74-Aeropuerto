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
#include <netdb.h>
#include "oserror.h"
#include "valueerror.h"
#include "notimplementederror.h"
#include "log.h"
#include <memory>
#include <errno.h>
#include <cstring>

class Socket {
   private:
      int fd;
      bool isstream;
      bool isassociated;

      struct sockaddr_storage peer_addr;
      socklen_t peer_addr_len;

   public:
      /*
       * Create an end point for communication between process, even if they are in separated nodes.
       * Two kinds of sockets can be created:
       *    - connection-based, stream oriented (if the parameter isstream is True)
       *    - connectionless, datagram oriented (if isstream is False)
       * 
       * The first kind is implemented with the TCP protocol meanwhile the second with the UDP protocol.
       * See man udp(7) and man tcp(7)
       *
       * For the first case, the socket can acts as an active or passive end point.
       * The first, will try to connect to a passive socket and stablish a channel of communication.
       * To acomplish that, the active socket will call the method 'destination' to start the process.
       *
       * In the other side, a socket play a passive role when call the method 'listen' and waits to someone
       * wants to connect to him.
       * When this happen, the passive socket can create a new socket that will be the other side of the channel.
       *
       *  Active                                        Passive
       *  act = Socket(True)                            pas = Socket(True)
       *  act.source(...) //optional                    pas.source("PasService") //almost a must
       *                            
       *                                                other_side = pas.listen()
       *                                                                  ^--------- wait until someone is connected
       *
       *  act.destionation("Passive", "PasService")
       *            ------------------------(unblock)-> other_side = pas.listen() // the connection is stablished
       *                                                                          // the first socket "pas" can still be 
       *                                                                          // used to accept new connections
       *  
       *  act.sendsome(...)         -------------->     other_side.receivesome(...) // act and other_side are indistingibles
       *  act.receivesome(...)      <--------------     other_side.sendsome(...)    // note how each know to where send the messages
       *
       *  act.~Socket() // finish                       other_side.~Socket() // finish
       *
       *  When you use the second version of sockets, connectionless, datagram oriented,
       *  there are not active or passive sockets.
       *
       *  One side                                      Other side
       *  one = socket(False)
       *  one.source(...) //optional
       *
       *  one.destination("Other side", "OtherService")
       *
       *  one.sendsome(...)         -------------->     other_side.receivesome(...)
       *                                                other_side.from_who(Host, Service) // save the origins of the previous message
       *                                                
       *                                                other_side.destination(Host, Service) // set that address so you can response
       *  one.receivesome(...)      <--------------     other_side.sendsome(...) 
       *
       *  one.sendsome(...)         -------------->     other_side.receivesome(...) // you don't need set again the destination if 
       *  one.receivesome(...)      <--------------     other_side.sendsome(...) // you don't change the interlocutor.
       *
       *  one.destionation("Third", "ThirdService") // because this is connectionless, you can talk with others hosts
       *     ...
       *  one.disassociate()  // if you can receive message from unknow sources, you need to call this.
       *  
       *
       * */
      explicit Socket(bool isstream=true) : 
         isstream(isstream),
         isassociated(false) {
         fd = socket(AF_INET, isstream? SOCK_STREAM : SOCK_DGRAM, 0);
         if(fd == -1)
            throw OSError("The socket cannot be created.");
      }

      void destination(const char* host, const char* service) {
         if(not host or not service)
            throw ValueError("The argument host [which is %s] and the service [which is %s] must not be null.", host? "not null" : "null", service? "not null" : "null");
         
         disassociate();
         struct addrinfo *result = resolve(host, service);
         try {
            if(::connect(fd, result->ai_addr, result->ai_addrlen) == -1)
               throw OSError("The connection to the host '%s' and the service '%s' has failed.", host, service);

            freeaddrinfo(result);

         }catch(...) {
            freeaddrinfo(result);
            throw;
         }
         
         isassociated = true;
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

      std::auto_ptr<Socket> listen(int backlog) {
         if(not isstream)
            throw NotImplementedError("A socket connectionless (datagram oriented) cannot be blocked to wait for a connection.");

         if(::listen(fd, backlog) == -1)
            throw OSError("The socket cannot stablish a queue of size %i for the comming connections.", backlog);
         
         clean_from_who();
         int other_side = ::accept(fd, (struct sockaddr *) &peer_addr, &peer_addr_len);
         if(other_side == -1) 
            throw OSError("The socket was trying to accept new connections but this has failed.");

         return std::auto_ptr<Socket>(new Socket(other_side));

      }

      ssize_t sendsome(const void *buf, size_t data_len) {
         ssize_t count = ::send(fd, buf, data_len, MSG_NOSIGNAL);
         if(count == -1) 
            throw OSError("The message length %i cannot be sent.", data_len);
         
         return count;
      }

      /*bool sendall(const void *buf, size_t data_len) {
         ssize_t count = 0;
         ssize_t burst = 0;
         while(data_len > count and burst > 0)
            count += (burst = sendsome(buf + count, data_len - count));
         
         return burst > 0;
      }*/

      ssize_t receivesome(void *buf, size_t buf_len) {
          clean_from_who();
          ssize_t count = ::recvfrom(fd, buf, buf_len, 0, (struct sockaddr *) &peer_addr, &peer_addr_len);
          if(count == -1) 
            throw OSError("The message cannot be received (of length least or equal to %i).", buf_len);

          return count;
      }

      /*bool receiveall(void *buf, size_t data_len) {
          ssize_t count = 0;
          ssize_t burst = 0;
          while(data_len > count and burst > 0) 
              count += (burst = receivesome(buf + count, data_len - count));

          return burst > 0;
      }*/

      void from_who(char *host, size_t host_length, char *service, size_t service_length) {
         int status = getnameinfo((struct sockaddr *) &peer_addr, peer_addr_len, 
                 host, host_length, service, service_length, NI_NAMEREQD | (isstream? 0 : NI_DGRAM));
         if(status != 0) {
            if(status != EAI_SYSTEM)
               errno = 0; //The error code is not in the errno (it has garbage)
            throw OSError("The name of the host and the service cannot be obtained: %s", gai_strerror(status));
         }
      }

      void disassociate() {
         if(isassociated) {
            if(isstream) {
               if(shutdown(fd, SHUT_RDWR) == -1)
                  throw OSError("The socket cannot be disassociated (shutdown).");
            }
            else {
               struct sockaddr reset;
               memset(&reset, 0, sizeof(reset));
               reset.sa_family = AF_UNSPEC;
               if(::connect(fd, &reset, sizeof(reset)) == -1)
                  throw OSError("The socket cannot be disassociated (connect to unspecified address).");
            }
         }
      }

      ~Socket() {
         if(isassociated and isstream) {
            if(shutdown(fd, SHUT_RDWR) == -1)
               Log::crit("An exception happend during the course of a destructor:\n%s", OSError(
                        "The socket cannot be disassociated (shutdown).").what());
         }

         if(close(fd) == -1)
            Log::crit("An exception happend during the course of a destructor:\n%s", OSError(
                     "The socket cannot be closed.").what());
      }

   private:
      explicit Socket(int other_side) : fd(other_side), isstream(true), isassociated(true) {}

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

      void clean_from_who() {
          memset(&peer_addr, 0, sizeof(struct sockaddr_storage));
          memset(&peer_addr_len, 0, sizeof(socklen_t));

          peer_addr_len = sizeof(peer_addr);
      }

};


#endif
