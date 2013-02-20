START_PORT = 9000
END_PORT = 9900

BEACON_SERVICE = 8083

assert END_PORT > START_PORT
assert BEACON_SERVICE not in range(START_PORT, END_PORT+1)

import random
import socket as S
from socket import AF_INET, SOCK_DGRAM, SOL_SOCKET, SO_BROADCAST, getaddrinfo
import time

def bind(socket, hostname):
   i = 0
   while i < 300:
      port = random.randint(START_PORT, END_PORT)
      try:
         socket.bind((hostname, port))
         return socket, str(port)
      except S.error, e:
         if hasattr(e, 'errno') and e.errno == 98:
            time.sleep(0.1)
            i += 0.1
         else:
            raise

   raise "None address in the local host is valid or free."

def discovery():
   services_running = []
   for port in range(START_PORT, END_PORT+1):
      socket = S.socket(AF_INET, SOCK_DGRAM)
      try:
         socket.bind(("", port))
      except S.error, e:
         if hasattr(e, 'errno') and e.errno == 98:
            services_running.append(port)
         else:
            raise
      finally:
         socket.close()

   return services_running

