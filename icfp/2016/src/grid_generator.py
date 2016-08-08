from fractions import Fraction
import argparse

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

def generate_redirections (n):
  s = ''
  for y in range(n+1):
    for x in range(n+1):
      s += str(Fraction(x%2,n))
      s += ','
      s += str(Fraction(y%2,n))
      s += '\n'
  return s
    
def parse_arguments():
  parser = argparse.ArgumentParser()
  parser.add_argument('n', help='number of cells')
  return parser.parse_args()
def generate_grid(n):
  return generate_points(n) + generate_facets(n) + generate_redirections(n)

def main():
  args = parse_arguments()
  print (generate_grid(int(args.n)))
if __name__ == '__main__':
  main()
  
