import hashlib
import random
import sys
import time

def get_random_nonce():
  return str(random.random()) + '-' + str(random.random())

def compute_difficulty(n_zeros):
  return eval("0b" + "0" * n_zeros + "1" * (256 - n_zeros))

def find_hash(text, difficulty):
  while True:
    m = hashlib.sha256()
    m.update(text + ' ')
    m.update(get_random_nonce())
    if int(m.hexdigest(), 16) < difficulty:
      return m.hexdigest()

def hash_n(n_times, text):
  """compute n times to get average time - return one hash"""
  time_start = time.time()
  for i in range(n_times):
    h = find_hash('Gloria', int_diff)
  total_time = time.time() - time_start
  return h, total_time / n_times

for diff in range(0, 257):
  int_diff = compute_difficulty(diff)
  h, total_time = hash_n(10, 'Gloria')
  print diff, "%.4f" % total_time, h
