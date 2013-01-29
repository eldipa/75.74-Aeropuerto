
class RingException(Exception):
   def __init__(self, *args):
      self.args = args

   def __str__(self):
      return "\n".join(map(str, self.args))


class InvalidMessage(RingException):
   def __init__(self, description, message, *args):
      if isinstance(message, basestring):
         message = " ".join(map(lambda c: hex(ord(c)), message))
      else:
         message = str(message)
      
      RingException.__init__(self, description, message, *args)


class InvalidNetworkMessage(InvalidMessage):
   def __init__(self, description, message, peer):
      InvalidMessage.__init__(self, description, message, peer)


class InvalidApplicationMessage(InvalidMessage):
   def __init__(self, description, message):
      InvalidMessage.__init__(self, description, message)


class UnstableChannel(RingException):
   def __init__(self, description, peer):
      RingException.__init__(self, description, peer)

