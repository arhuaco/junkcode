class FULogger:

  VERBOSE_NOISE = 70
  VERBOSE_SILLY = 50
  VERBOSE_INFO  = 30
  VERBOSE_ERROR = 10
  VERBOSE_OFF = 0

  def __init__ (self, verbose_level):
     
     self.level = verbose_level

     self.msg = {}

     self.msg[self.VERBOSE_NOISE] = 'NOISE'
     self.msg[self.VERBOSE_SILLY] = 'SILLY'
     self.msg[self.VERBOSE_INFO]  = 'INFO'
     self.msg[self.VERBOSE_ERROR] = 'ERROR'
     self.msg[self.VERBOSE_OFF] = 'OFF'

  def verbose(self, str, level):
    if level <= self.level:
      print self.msg[level] + ':', str

#l = FULogger(FULogger.VERBOSE_INFO);
#l.verbose('Prueba', FULogger.VERBOSE_INFO)
