START_PORT = 8000
END_PORT = 8900

import random
import socket as S
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
