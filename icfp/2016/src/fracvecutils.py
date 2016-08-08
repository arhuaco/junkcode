import math
from fractions import Fraction
from fracutils import *

def frac_str (x):
  return str(Fraction(x).limit_denominator())
  
class point:
  def __init__(self,x,y):
    self.x = x
    self.y = y
  def __str__ (self):
    return '' + str(self.x) + ',' + str(self.y)
  def to_frac (self):
    return (Fraction(self.x), Fraction(self.y))
  def to_frac_str(self):
    return '' + frac_str(self.x) + ',' + frac_str(self.y)

def dot (a,b):
  return qadd(qmul(a.x, b.x), qmul(a.y,b.y))

def norm (v):
  n = dot(v,v)
  return math.sqrt(n.numerator/n.denominator)

def sub (a,b):
  return point(qsub(a.x, b.x), qsub(a.y, b.y))

def normalize(a):
  na = norm(a)
  if (na == 0):
    return point (Fraction(0), Fraction(0))
  return point(qscale(a.x, 1/na), qscale(a.y,1/na))

def scale (a, scalar):
  return point(qscale(a.x, scalar), qscale(a.y,scalar))

def add (a,b):
  return point(qadd(a.x, b.x), qadd(a.y,b.y))

def mirror_p_vw (v,w,p):
  a = sub (w,v)
  b = sub (p,v)
  ab = dot(a,b)
  x = scale (normalize(a), ab.numerator/norm(a)/ab.denominator)
  z = sub (x, b)
  q = add(x, z)
  return add(q,v);

def segments_intersect (a,b,c,d,e,f,g,h):
  '''Determines if (a,b)->(c,d) intersects with the segment
     (e,f)->(g,h) '''
  disc = (e-g)*(b-d)-(f-h)*(a-c)
  if disc == 0: 
    return False
  v1 = ((b-d)*(a-g)+(c-a)*(b-h)) / disc
  v2 = ((h-f)*(a-g)+(e-g)*(b-h)) / disc
  return v1>=0 and v1<=1 and v2>=0 and v2 <=1
