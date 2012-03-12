#!/usr/bin/python

def find_distances(number):
  DIST = (number + 1) * [None]
  def fill_distances(n, dist):
    while n >= 0:
      if DIST[n] != None and DIST[n] < dist:
        return
      DIST[n] = dist
      if n > 1 and n % 2 == 0:
        fill_distances(n / 2, dist + 1)
      dist += 1
      n -= 1
  fill_distances(number, 0)
  return DIST

