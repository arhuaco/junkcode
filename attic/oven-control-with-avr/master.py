#! /usr/bin/python

import sys, serial, threading, time, thread, os, string, decimal

def adc2t(adc):
  return  -0.66473988439313 + (99.4219653179191 * (0.00532974427994848 + (adc * 0.0025706940874036)))

def read_sensor(id):
   ok = False
   while (not ok):
     try:  # don't lock here. Ping can interrupt. It's OK to fail here.
       s.write(repr(id) + "r") 
       value0 = int(s.readline().strip())
       s.write(repr(id) + "r") 
       value1 = int(s.readline().strip())
       s.write(repr(id) + "r") 
       value2 = int(s.readline().strip())
       ok = True
     except: 
       ok = False # Dummy
       print >> sys.stderr, "Read failed. Nice :)"

   return (value0 + value1 + value2) / 3


def do_cmd(cmd):
    write_lock.acquire() 
    s.write(cmd)
    write_lock.release() 

# state
t0 = time.time()
write_lock = thread.allocate_lock()
did_end = False
status0 = 'Undef'
status1 = 'Undef'

def aim_for (yprev, tprev, ynext, tnext, deltat):
   global status

   slope = (ynext - yprev) / (tnext - tprev)
   const = ynext - slope * tnext

   tnow = time.time() - t0

   while tnow <= tnext:
     time.sleep(deltat)

     ynow0 = adc2t(read_sensor(0))
     ynow1 = adc2t(read_sensor(1))

     tnow = time.time() - t0

     guess = slope * (tnow + deltat * 3)  + const # line

     if ynow0 > guess:
       do_cmd("0-")
       status0 = '25'
     else: # just an else
       do_cmd("0+")
       status0 = '35'

     if ynow1 > guess or ynow1 > 200:
       do_cmd("1-")
       status1 = '10'
     else: # just an else
       do_cmd("1+")
       status1 = '20'

     # Log
     log = repr(tnow)[0:5] + ' ' + repr(tnext)[0:5] + ' ' + repr(ynow0)[0:5] + ' ' + repr(ynext)[0:5]  + ' ' + \
           repr(ynow1)[0:5] + ' ' + status0 + ' ' + status1
     info = '  t0   t1 realt goal_t port1(broken) down up'
     sys.stdout.write(log + "\n")
     sys.stdout.flush()

     print >> sys.stderr, info
     print >> sys.stderr, log 

     sys.stdout.flush()

def read_curve(filename):

    f = open(filename, 'r')
    v = []

    l = f.readline().strip()
    while len(l):
      if l.find('#') == -1:
        v.append(map(float,l.rsplit(' ')))
      l = f.readline().strip()

    f.close()
    return v

def controller():
    global did_end 
    """loop forever and copy serial->console"""

    #plot = os.popen('gnuplot -persist -', 'w')
    #plot.write("set term x11\n")

    #def make_graph():
    #  plot.write("set mouse\n")
    #  plot.write("set key outside below\nset autoscale\n")
    #  plot.write("show terminal\n")

    #  plot.write("plot '-' using 1:2 t 'Naive Program' with lines\n")
    #  for v in read_curve('owen-program.txt'):
    #    plot.write(v[0] + ' ' + v[1] + "\n")
    #    plot.flush()
    #  plot.write("EOF\n")

    #make_graph()
    #make_graph()

    program = read_curve('splinedata')
    time_before = float(program[0][0])
    temp_before = float(program[0][1])

    for v in program[1:]:
      aim_for (temp_before, time_before, v[1], v[0], 0.1) 
      time_before = v[0]
      temp_before = v[1]

    print >> sys.stderr, "Goal reached. End."
    did_end = True

    plot.close()

    sys.exit(0)

def letsping():
    """ Ping the slace each 10 secs.. this will damage some reads :) """

    # The ping is disabled now !

    count = 10
    while not did_end:
      time.sleep(1)
#      count -= 1
#      if count < 0:
#        write_lock.acquire() 
        # Send a ping. The pong (<) will damage some sensor readings. 
        # We need this in case some reads becomes stuck.
#        s.write(">") 
#        write_lock.release() 
#        count = 10

if __name__ == '__main__':


    # serial
    port  = 0
    baudrate = 9600
    
    try:
        s = serial.Serial(port, baudrate, rtscts=0, xonxoff=0)
    except:
        sys.stderr.write("Could not open port\n")
        sys.exit(1)
    #start serial->console thread
    r = threading.Thread(target=controller)
    r.setDaemon(1)
    r.start()
    #and enter console->serial loop
    letsping()
