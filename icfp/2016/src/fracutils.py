from fractions import Fraction

def qscale (a, s):
  return qmul(a, Fraction(s))

def qadd (a, b):
  return Fraction(a.numerator*b.denominator + a.denominator*b.numerator,a.denominator*b.denominator)

def qsub (a, b):
  return qadd (a, qscale(b, -1))

def qmul(a,b):
  return Fraction (int(a.numerator*b.numerator), int(a.denominator*b.denominator))

def qdiv(a,b):
  return Fraction (int(a.numerator*b.denomninator), int(a.denominator*b.numerator))

