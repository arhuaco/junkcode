#!/usr/bin/python
from cStringIO import StringIO
import commands, os
import time, sys

if len(sys.argv) == 2:
    while True:
      ps = commands.getoutput('ps up ' + sys.argv[1])
      all = ps.split()
      print all[13], all[14], all[15] # CPU, MEM, RAM
      time.sleep(1)
      sys.stdout.flush()
else:
    print """Usage: python program PID"""
