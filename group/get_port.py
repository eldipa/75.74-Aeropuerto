
import socket as S

import config
_c = config.Configuration("config.ini")

START_PORT = _c.getint("port pool", "start port pool")
END_PORT = _c.getint("port pool", "end port pool")

BEACON_SERVICE = _c.get("beacon service", "service")
try:
   BEACON_SERVICE = S.getservbyname(BEACON_SERVICE)
except S.error:
   BEACON_SERVICE = int(BEACON_SERVICE)


assert END_PORT > START_PORT
assert BEACON_SERVICE not in range(START_PORT, END_PORT+1)

import random
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
   
   #full scan
   for port in range(START_PORT, END_PORT+1):
      try:
         socket.bind((hostname, port))
         return socket, str(port)
      except S.error, e:
         if hasattr(e, 'errno') and e.errno == 98:
            pass
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

