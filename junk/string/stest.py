#!/usr/bin/python
from cStringIO import StringIO
import commands, os
import time, sys

# I did not write this code
# It was downloaded from: http://www.skymind.com/~ocrow/python_string/
# I modified it so that it runs with Python 2.4 / Python 2.5
# TODO: use the timeit module

# The results reported in the article are no longer valid.

with_psyco = True

try:
  import psyco
  psyco.full()
except ImportError:
  print >> sys.stderr, 'No Psyco available! please install it!'
  with_psyco = False

def method1():
  out_str = ''
  for num in xrange(loop_count):
    out_str += `num`
  ps_stats()
  return out_str

def method2():
    from UserString import MutableString
    out_str = MutableString()
    for num in xrange(loop_count):
        out_str += `num`
    ps_stats()
    return out_str

def method3():
    from array import array
    char_array = array('c')
    for num in xrange(loop_count):
        char_array.fromstring(`num`)
    ps_stats()
    return char_array.tostring()

def method4():
    str_list = []
    for num in xrange(loop_count):
        str_list.append(`num`)
    out_str = ''.join(str_list)
    ps_stats()
    return out_str

def method5():
    file_str = StringIO()
    for num in xrange(loop_count):
        file_str.write(`num`)
    out_str = file_str.getvalue()
    ps_stats()
    return out_str

def method6():
    out_str = ''.join([`num` for num in xrange(loop_count)])
    ps_stats()
    return out_str

def ps_stats():
    global process_size
    global pid
    ps = commands.getoutput('ps up ' + `pid`)
    process_size = ps.split()[15]

def call_method(num):
    global process_size
    t0 = time.time()
    eval('method' + str(num) + '()')
    t1 = time.time()
    print 'method:', num, ':seconds', (t1 -t0),
    print 'process size:', process_size
    print 'psycho killer:', with_psyco, # I like "Talking heads" :-)
    print 'loop_count:', loop_count 
  
loop_count = 0
pid = os.getpid()

if len(sys.argv) == 3:
    loop_count = int(sys.argv[2])
    call_method(sys.argv[1])
else:
    print """Usage: python stest.py <n> <count>
          where n is the method number to test
          and count is the limit of the counter"""
