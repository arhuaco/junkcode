import hashlib
import random
import sys
import time

def get_random_nonce():
  return str(random.random()) + '-' + str(random.random())

def compute_difficulty(n_zeros):
  return eval("0x" + "0" * n_zeros + "f" * (64 - n_zeros))

def find_hash(text, difficulty):
  while True:
    m = hashlib.sha256()
    m.update(text + ' ')
    m.update(get_random_nonce())
    if int(m.hexdigest(), 16) < difficulty:
      return m.hexdigest()

for diff in range(0, 33):
  int_diff =  compute_difficulty(diff)
  time_start = time.time()
  h = find_hash('Gloria', int_diff)
  total_time = time.time() - time_start
  print total_time, diff, int_diff,  h
