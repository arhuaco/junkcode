#!/usr/bin/python 
import httplib, thread, time, random, md5

from fulogger import *

class CheckPages:

  def __init__(self, pages, bot):
    self.log = FULogger(FULogger.VERBOSE_INFO);
    self.log.verbose('Starting checker', FULogger.VERBOSE_INFO)

    self.check_interval =  2
    self.random_interval = 5 
    self.each_page_interval = 5 

    self.page = []
    self.hash = []
    self.status = []
    for p in pages:
      self.page.append(p)
      self.hash.append('unknown')
      self.status.append('unknown')

    thread.start_new_thread(self.check_thread, ())

  def check_page(self, page):
    """Fetches one page"""
    try:
      self.log.verbose('Downloading page', FULogger.VERBOSE_INFO)
      conn = httplib.HTTPConnection(page[0]) # server
      conn.request("GET", page[1]) # page: ie: /index.html
      r = conn.getresponse()
      m = md5.new()
      m.update(r.read())
      print repr(m.hexdigest()), r.status
    except:
      return 'error', 'error'

  def check_thread(self):
    while True:
      time.sleep(self.check_interval + self.random_interval * random.random())
      self.log.verbose('Checking for updates', FULogger.VERBOSE_INFO)
      #self.each_page_interval = 5 
      for i in xrange(0, len(self.page)):
        self.check_page(self.page[0])
