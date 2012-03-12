import sys
import find_distances
from ltg import IsSlotValue

RIGHT = True
LEFT = False
CARDS = set(['I', 'zero', 'succ', 'dbl', 'get', 'put', 'S', 'K',
            'inc', 'dec', 'attack', 'help', 'copy', 'revive', 'zombie'])

class WriteNumber:

  def __init__(self, where, number, who):
    self.number = number
    self.where = where
    self.who = who
    self.distances = find_distances.find_distances(self.number)

  def get_move(self, game):
    if (not game.IsFunc(self.where, self.who)
        and game.GetValue(self.where, self.who) == self.number):
      return None
    if game.IsDeadSlot(self.where, self.who):
      print >> sys.stderr, 'CELL died WriteNumber'
      return None
    # We don't have he number in the right position.
    best_option = None
    def consider_option(best_option, dist, op):
      if best_option == None or dist < best_option[0]:
        best_option = [dist, op]
      return best_option
    # get the dbl, and succ distances
    if not game.IsFunc(self.where, self.who):
      val = int(game.GetValue(self.where, self.who))
      if val * 2 < len(self.distances):
        best_option = consider_option(best_option, self.distances[val * 2] + 1, [LEFT, 'dbl', self.where])
      if val + 1 < len(self.distances):
        best_option = consider_option(best_option, self.distances[val + 1] + 1, [LEFT, 'succ', self.where])
    else:  # if game.IsFunc(self.where, self.who):
      val = game.GetValue(self.where, self.who)
      if val == 'I':
        best_option = consider_option(best_option, self.distances[0] + 1, [RIGHT, 'zero', self.where])
    # Next line always executes.
    # It considers how expensive it is to put I in the slot to start from zero in the next step.
    best_option = consider_option(best_option, self.distances[0] + 2, [LEFT, 'put', self.where]) 
    return best_option[1]

# When this is set, we're making an assignation.
# Ugly but works in case of sabotage.

Assignator = None

def IsInt(s):
  try: 
    int(s)
    return True
  except ValueError:
    return False


def AssertValid(dest, card):
  assert(IsSlotValue(dest))
  assert(card in CARDS)

REVIVING = None

class Engine:

  def parse(self, text):
    program = []
    for line in [i.strip() for i in text.split('\n')]:
      if len(line) and line[0] != '#':
        text = line.split()
        if text[0] == 'set':
          assert(len(text) == 3)
          if IsInt(text[2]):  # set the integer value
            program.append(('set-int', int(text[1]), int(text[2])))
          elif text[2][0] == "*":  # set with pointer
            program.append(('set-int', int(text[1]), int(text[2][1:])))
            program.append(('left', int(text[1]), 'get'))
          else: # set a card
            program.append(('set-card', int(text[1]), text[2]))
        elif text[0] == 'right':
          assert(len(text) == 3)
          if IsInt(text[2]):  # set the integer value
            assert(text[2] == "0" or text[2] == "1")
            program.append(('left', int(text[1]), 'K'))
            program.append(('left', int(text[1]), 'S'))
            if (text[2] == "0"):
              assert(IsSlotValue(int(text[1])))
              program.append(('right', int(text[1]), 'I'))
              program.append(('right', int(text[1]), 'zero'))
            else: #1
              program.append(('right', int(text[1]), 'succ'))
              program.append(('right', int(text[1]), 'zero'))
          elif text[2][0] == "*":
            src_addr = int(text[2][1:])
            sys.stderr.flush()
            assert(IsSlotValue(src_addr))
            if src_addr == 0:
              program.append(('left', int(text[1]), 'K'))
              program.append(('left', int(text[1]), 'S'))
              program.append(('right', int(text[1]), 'get'))
              program.append(('right', int(text[1]), 'zero'))
            else:
              program.append(('left', int(text[1]), 'K'))
              program.append(('left', int(text[1]), 'S'))
              program.append(('right', int(text[1]), 'get'))
              program.append(('left', int(text[1]), 'K'))
              program.append(('left', int(text[1]), 'S'))
              program.append(('right', int(text[1]), 'get'))
              program.append(('set-int', 0, src_addr))
              program.append(('right', int(text[1]), 'zero'))
          else:
            assert(0)  # expected, 0, 1 or valid *N
        elif text[0] == 'left':
          assert(len(text) == 3)
          program.append(('left', int(text[1]), text[2]))
        elif text[0] == 'rewind':
          program.append(('goto', "0"))
        else:
          print >> sys.stderr, '******** ERROR', line
          sys.exit(1)
    return program

  def __init__(self, code):
    self.code = self.parse(code)
    self.ip = 0  # instuction pointer
    self.ended = False

  def play(self, game, player):
    global REVIVING
    while self.ip < len(self.code):
      # REVIVE START
      # if one of our cells got killed, try to revive it ASAP.
      if REVIVING != None:
        assert(len(REVIVING) == 2)
        m = REVIVING[0].get_move(game)
        if not m is None:  # we have a step to execute.
          return m
        ret = (LEFT, "revive", REVIVING[1])
        REVIVING = None
        return ret
      for i in xrange(256):
        if game.IsDeadSlot(i, player):
          # Use any cell over 100 with the most vitality for revival, that is not the same cell.
          revive_slot = None # better than nothing.
          revive_max = -1
          for j in xrange(40, 255):
            if game.GetVitality(player, j) > revive_max:
              revive_slot = j
              revive_max = game.GetVitality(player, revive_slot)
          if revive_slot != None: #revive i!
            REVIVING = [WriteNumber(revive_slot, i, player), revive_slot]
          break # revive the first cell and that's it. If we don't have a slot now we
                # won't have it in the next iteration.
                # FIXME: Here we should enter in panic move and only revive things...
      if REVIVING != None:
        #print >> sys.stderr, '************* REVIVING NOW'
        continue
      #REVIVE END
      if self.code[self.ip][0] == 'set-int':
        global Assignator
        if Assignator == None:
          Assignator = WriteNumber(self.code[self.ip][1], int(self.code[self.ip][2]), player)
        m = Assignator.get_move(game)
        if not m is None:  # we have a step to execute.
          return m
        # else:  # we're done generating the number
        Assignator = None
        self.ip += 1
      elif self.code[self.ip][0] == 'set-card':
        dest = int(self.code[self.ip][1])
        card = self.code[self.ip][2]
        AssertValid(dest, card)
        if game.IsFunc(dest, player) and game.GetValue(dest, player) == card and game.IsAlone(dest, player):
          # we're done
          self.ip += 1
        elif game.IsFunc(dest, player) and game.GetValue(dest, player) == "I" and game.IsAlone(dest, player):
          return (RIGHT, card, dest)  # copy the symbol
        else:
          return (LEFT, 'put', dest)  # clear so that we can copy next
      elif self.code[self.ip][0] == 'left':
        dest = int(self.code[self.ip][1])
        card = self.code[self.ip][2]
        AssertValid(dest, card)
        self.ip += 1
        return (LEFT, card, dest)
      elif self.code[self.ip][0] == 'right':
        dest = int(self.code[self.ip][1])
        card = self.code[self.ip][2]
        AssertValid(dest, card)
        self.ip += 1
        return (RIGHT, card, dest)
        # program.append(('left', int(text[1]), 'get'))
      elif self.code[self.ip][0] == 'goto':
        self.ip = int(self.code[self.ip][1])
      else:    
        assert(0)
    # default NOOP 
    # FIXME(nel): Try to revive zero for ever.
    self.ended = True
    return (LEFT, 'put', 0)
