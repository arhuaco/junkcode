from fractions import Fraction
import argparse
from fracutils import *
from fracvecutils import *

def generate_points(n):
  s = '%d\n'%((n+1)*(n+1))
  for y in range(n+1):
    for x in range(n+1):
      s += str(Fraction(x, n))
      s+=','
      s += str(Fraction(y, n))
      s+='\n'
  return s

def generate_facets(n):
  s = '%d\n'%(n*n)
  k = 0
  for y in range(n):
    for x in range(n):
      s += '4 %d %d %d %d\n'%(k, k+1, k+n+2, k+n+1)
      k += 1
    k += 1
  return s

def rotate (v, R):
  return point (qadd (qmul(R[0][0], v.x), qmul(R[0][1], v.y)),
                qadd (qmul(R[1][0], v.x), qmul(R[1][1], v.y)))

def generate_redirections (n, T, R):
  M = {0:Fraction(0,n), 1:Fraction(1,n)}
  s = ''
  for y in range(n+1):
    for x in range(n+1):
      x1 = M[x%2]
      y1 = M[y%2]
      r = add (rotate (point(x1,y1), R), T)
      s += str(r.x)
      s += ','
      s += str(r.y)
      s += '\n'
  return s
    
def parse_arguments():
  parser = argparse.ArgumentParser()
  parser.add_argument('n', help='number of cells')
  parser.add_argument('matrix_index', help ='index of the matrix to use')
  parser.add_argument('tx', help='Coordinate x of translation')
  parser.add_argument('ty', help='Coordinate y of translation')
  return parser.parse_args()
def generate_grid(n, R, T):
  return generate_points(n) + generate_facets(n) + generate_redirections(n, T, R)


pythagorean_triples = [(5, 12, 13),	(8, 15, 17),	(7, 24, 25),(20, 21, 29),
                       (12, 35, 37),	(9, 40, 41),	(28, 45, 53),
                       (11, 60, 61),	(16, 63, 65),	(33, 56, 65),	(48, 55, 73),
                       (13, 84, 85),	(36, 77, 85),	(39, 80, 89),	(65, 72, 97)]


rotation_matrices = [ [ [Fraction(1), Fraction(0)], [Fraction(0), Fraction(1)] ], 
                      [ [Fraction(4,5), Fraction(-3,5)], [Fraction(3,5), Fraction(4,5)] ] ]

for t in pythagorean_triples:
  rotation_matrices.append ([[Fraction(t[1],t[2]), Fraction(-t[0],t[2])], [Fraction(t[0],t[2]), Fraction(t[1],t[2])]])

def main():
  args = parse_arguments()
  n = int(args.n)
  R = rotation_matrices[int(args.matrix_index)]
  tx = Fraction(args.tx)
  ty = Fraction(args.ty)
  print (generate_grid(n, R, point (tx, ty) ))
if __name__ == '__main__':
  main()
  
