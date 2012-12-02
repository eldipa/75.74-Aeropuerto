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
#include <netdb.h>
#include <memory>
#include <string>


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
      explicit Socket(bool isstream); 

      void destination(const std::string &host, const std::string &service);
      void disassociate();
      void from_who(std::string &host, std::string &service);

      void source(const std::string &service);

      std::auto_ptr<Socket> listen(int backlog);

      ssize_t sendsome(const void *buf, size_t data_len);
      ssize_t receivesome(void *buf, size_t buf_len);

      ~Socket();

   private:
      explicit Socket(int other_side);
      struct addrinfo* resolve(const char* host, const char* service);

      void clean_from_who();
};


#endif
