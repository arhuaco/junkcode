#import logging
import pprint

def get_area (list_of_coords):
  a = 0
  n = len(list_of_coords)
  for i in range(n):
    current_point = list_of_coords[i]
    previous_point = list_of_coords[i-1]
    dy = current_point[1] - previous_point[1]
    dx = previous_point[0] + current_point[0]
    a += dx * dy
  return a/2

def get_center (list_of_coords):
  n = len(list_of_coords)
  if n == 0:
    return (0,0)
  sx = 0
  sy = 0
  for p in list_of_coords:
    sx += p[0]
    sy += p[1]
  return (sx/n, sy/n)

def get_envelope (list_of_coords):
  mx = list_of_coords[0][0]
  my = list_of_coords[0][1]
  Mx = list_of_coords[0][0]
  My = list_of_coords[0][1]
  for p in list_of_coords[1:]:
    if p[0] < mx:
      mx = p[0]
    if p[1] < my:
      my = p[1]
    if p[0] > Mx:
      Mx = p[0]
    if p[1] > My:
      My = p[1]
  l = [ (mx, my), (Mx, my), (Mx, My), (mx, My)]

  #logging.debug ("[%f,%f;%f %f;%f %f; %f %f]"%(mx, my, Mx, my, Mx, My, mx, My))
  return l
   
