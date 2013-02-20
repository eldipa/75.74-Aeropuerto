
import sys
import syslog

sys.path.append("../ipc")

from ipc import MessageQueue
from subprocess import Popen

import passage
import ring
import struct
import message
from invalid import *
import traceback
import time
import socket
from socket import AF_INET, SOCK_DGRAM, SOL_SOCKET, SO_BROADCAST, getaddrinfo

import stop
import get_port

import signal
def handler(signum, frame):
   raise KeyboardInterrupt("Interrupted by a TERM signal.")

signal.signal(signal.SIGTERM, handler)

DISCOVERY_TIME = 5

if __name__ == '__main__':
   if len(sys.argv[1:]) != 1:
      print "Usage: beacon_service.py localhost_name"
      print "  - localhost_name: the name of this host viewed by other nodes."
      sys.exit(1)

   localhost_name, = sys.argv[1:]
   
   syslog.openlog("beacon_service")
   syslog.syslog(syslog.LOG_INFO, "Init 'beacon_service' process. Arguments: Localhost: %s " % (
      localhost_name))

   datagram_socket = socket.socket(AF_INET, SOCK_DGRAM)
   try:
      try:
         datagram_socket.bind(("", get_port.BEACON_SERVICE))
         clients = get_port.discovery()
         last = time.time()
         while True:
            try:
               msg, peer = datagram_socket.recvfrom(ring.BEACON_BUF_MAX_SIZE)
               type, = struct.unpack('>4s', msg[:4])

               if type not in ('OPEN', 'FIND'):
                  raise InvalidNetworkMessage("The message has a wrong type", msg, peer)

               peer = str(peer)
               s = struct.pack(">H%is%is" % (len(peer), len(msg)), len(peer), peer, msg)

               for i in clients:
                  datagram_socket.sendto(s, (localhost_name, i))
               
               if time.time() - last > DISCOVERY_TIME:
                  t = time.time()
                  syslog.syslog(syslog.LOG_INFO, "Discovering ...")
                  _clients = get_port.discovery()
                  last = time.time()
                  syslog.syslog(syslog.LOG_INFO, "Found %i open services in %s time. Added: %i Removed: %i" % (len(_clients), str(last-t), len(set(_clients)-set(clients)), len(set(clients)-set(_clients)) ))
                  clients = _clients

            except InvalidNetworkMessage, e:
               syslog.syslog(syslog.LOG_CRIT, "%s\n%s" % (traceback.format_exc(), str(e)))
      
      except KeyboardInterrupt:
         syslog.syslog(syslog.LOG_INFO, "Interruption:\n%s" % traceback.format_exc())
         sys.exit(0)
      except Exception, e:
         syslog.syslog(syslog.LOG_CRIT, "Critical exception (will shutdown everything) %s\n%s" % (traceback.format_exc(), str(e)))
         sys.exit(2)
   
   finally:
      syslog.syslog(syslog.LOG_INFO, "Shutdown 'beacon_service'.")
      datagram_socket.close()
      time.sleep(0.1)
