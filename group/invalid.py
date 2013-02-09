
import traceback

class RingException(Exception):
   def __init__(self, *args):
      self.args = filter(None, args)

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
   def __init__(self, description, peer, exception=None):
      if exception:
         description += "\nChannel Error: '%s'\n%s" % (str(exception), traceback.format_exc())
      RingException.__init__(self, description, peer)

