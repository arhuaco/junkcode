import math
from fractions import Fraction



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
  return a.x*b.x+a.y*b.y

def norm (v):
  return math.sqrt(dot(v,v))

def sub (a,b):
  return point(a.x - b.x, a.y - b.y)

def normalize(a):
  na = norm(a)
  return point(a.x/na, a.y/na)

def scale (a, scalar):
  return point(a.x*scalar, a.y*scalar)

def add (a,b):
  return point(a.x + b.x, a.y+b.y)

def mirror_p_vw (v,w,p):
  a = sub (w,v)
  b = sub (p,v)
  x = scale (normalize(a), dot(a,b)/norm(a))
  z = sub (x, b)
  q = add(x, z)
  return add(q,v);

