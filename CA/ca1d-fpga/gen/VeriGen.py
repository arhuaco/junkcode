#!/usr/bin/env python

import sys, re

"""
We allow to use helper functions with parameters. To The file
has to have a dash (-). So, if you wanted to generate the
file ac-cells.vv, you'll send it parameters like this:

`include "ac-cells.vv" // CA_WIDTH INITIAL_SIZE 22

We will find the parameters (we don't resolve parameter overloads yet).

You'll need to crate a file ac-cells.vv.py in this case.

"""

class VeriGen:

  def scan_father(self):

    prm_re = re.compile('^\s*parameter\s+(\S+)\s*=\s*(\S+)\s*;')
    module_re = re.compile('^\s*module');
    inc_re = re.compile('^\s*`include\s+"([^"]+)"\s*(//(.*))?')

    self.defines = {}

    f = open(self.father, 'r')

    line = f.readline()
    line_count = 0
    found_include = False
    self.params = ''
    while len(line) > 0:
      line_count += 1

      m = module_re.match(line)
      if m != None:
        if found_include: break
        else: self.defines = {}

      m = inc_re.match(line)
      if m != None:
        if m.group(1) == self.son:
          if self.verbose:
            print >>sys.stderr, "Included in line: %d" % (line_count)
          self.params = m.group(3)
          if self.params != None:
            if self.verbose:
              print >>sys.stderr, "Parameters: %s" % (self.params)
          found_include = True
  
      m = prm_re.match(line)
      if m != None:
        self.defines[m.group(1)] = m.group(2)
      line = f.readline()

    if not found_include:
      print >>sys.stderr, 'Could not `include "%s" in %s' % (self.son, self.father)
      sys.exit(1)

    number_re = re.compile('^(\d+)$') # Only decimals right now

    prm = []

    for p in  self.params.split():
      if number_re.match(p):
        if self.verbose:
          print >>sys.stderr, 'Param', p, ' => int ', p
        prm.append(int(p))
      else:
        if self.defines.has_key(p):
          if self.verbose:
            print >>sys.stderr, 'Param', p, ' => int ', self.defines[p]
          prm.append(int(self.defines[p]))
        else:
          print >>sys.stderr, 'Error: parameter', p, 'is not defined'
          sys.exit(1)

    self.func_params = prm

  def __init__ (self, funcs, verbose = False):

    self.verbose = verbose

    dash_loc = sys.argv[0].rfind ('-')
    if dash_loc == -1:
      print >> sys.stderr, "Error: No dash in python filename\n"
      sys.exit(1)

    start_loc = sys.argv[0].rfind ('/')
    if start_loc == -1:
      start_loc = 0
    else:
      start_loc = start_loc + 1

    self.father = sys.argv[0][start_loc:dash_loc] + '.v'
    self.son = sys.argv[0][start_loc:-3]

    if self.verbose:
      print >> sys.stderr, 'Generating', self.son, 'used in', self.father

    self.scan_father()

    for f in funcs:
      f(self.func_params)

if __name__ == "__main__":
  print >> sys.stderr, "Error(%s): You are not supposed to call me directly" % (sys.argv[0])
