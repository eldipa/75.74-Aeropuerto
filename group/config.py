import ConfigParser

def Configuration(config_filename):
   p = ConfigParser.RawConfigParser()
   with open(config_filename) as f:
      p.readfp(f)
   
   return p

