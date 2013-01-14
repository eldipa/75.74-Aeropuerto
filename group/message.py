import struct

_format = '=l%is'
_header_size = struct.calcsize('=l')

def pack(payload, type):
   return struct.pack(_format % len(payload), type, payload)

def unpack(package):
   return struct.unpack(_format % (len(package) - _header_size), package)
