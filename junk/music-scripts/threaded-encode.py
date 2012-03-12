#!/usr/bin/env python

# This script allows you to use multiple CPUs to encode
# a group of files faster. It gets the files from the
# CD first.
#
# It will be useful for multicore PCs.
#
# Add NFS and SSH and you'll get a
# parallel ripping machine :)
#
# By
# arhuaco@freaks-unidos.net

# set to the concurrency level you want
NTHREADS = 2

import time
import thread
import glob
import sys
import re
import os

# get the tracks
os.system('cdparanoia -B 1-')

# normalize the volume
os.system('normalize-audio -vb track*.cdda.wav')

biglock = thread.allocate_lock()

# files to process
file_stack = glob.glob('./track*.cdda.wav') 

# how to get the track number
track_re = re.compile('./track(\d+).cdda.wav')

n_busy = NTHREADS 

def encoder_func(lock, *args):
    global n_busy

    while len(file_stack) > 0:

        lock.acquire() 

        file = file_stack.pop()
        track = track_re.match(file).group(1)
        cmd = 'nice lame -S -h -V2 -b32 -mj -q1 ' + file + ' ' + track + '.mp3'
        print cmd
        
        lock.release()

        os.system(cmd)
        os.unlink(file)

    lock.acquire() 
    
    n_busy -= 1

    if n_busy == 0:
      biglock.release()

    lock.release()

if __name__=="__main__":

    lock = thread.allocate_lock()

    biglock.acquire() # Released when there are no more files to process

    thread.start_new_thread(encoder_func, (lock, 0))
    thread.start_new_thread(encoder_func, (lock, 0))

    biglock.acquire()  #waiting for biglock.release()
