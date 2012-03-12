import copy
import os
import sys

sys.setrecursionlimit(2000)
print_debug = False
print_debug_more = False


class LTGException(Exception):
  """Class used to track game exceptions."""
  def __init__(self, value):
    self.value = value

  def __str__(self):
    return str(self.value)

  def Print(self, slot):
    if not print_debug:
      return
    msg = str(self)
    if msg.find('limit') == -1:
      msg = 'Native.Error'
    else:
      msg = 'Native.AppLimitExceeded'
    sys.stderr.write('Exception: %s\nslot %r reset to I\n' % (msg, slot))


def IsSlotValue(i):
  return i >= 0 and i <= 255


def IncIters(iters):
  # print 'Changing iter from %r' % iters[0]
  iters[0] = iters[0] + 1
  if iters[0] > 1000:
    raise LTGException('Error, limit reached')


def PrintTurnTail(is_right, player, slot, action):
  if not print_debug:
    return
  sys.stderr.write('(1) apply card to slot, or (2) apply slot to card?\n')
  if is_right:
    sys.stderr.write('slot no?\n')
    sys.stderr.write('card name?\n')
  else:
    sys.stderr.write('card name?\n')
    sys.stderr.write('slot no?\n')
  if is_right:
    sys.stderr.write(
        'player %r applied slot %r to card %s\n' % (player, slot, action))
  else:
    sys.stderr.write(
        'player %r applied card %s to slot %r\n' % (player, action, slot))


def ReadFromStdin():
  """Reads an input produced by the enemy.

  Returns:
    (is_right, card, slot): The content of the 3 read lines.
  """
  n1 = raw_input()
  if print_debug_more:
    sys.stderr.write('$1: %r\n' % n1)
  n2 = raw_input()
  if print_debug_more:
    sys.stderr.write('$2: %r\n' % n2)
  n3 = raw_input()
  if print_debug_more:
    sys.stderr.write('$3: %r\n' % n3)
  left_right = int(n1)
  if left_right == 1:
    return (False, n2, int(n3))  # is_right, card, slot
  return (True, n3, int(n2))  # is_right, card, slot


def WriteToStdout(is_right, card, slot, prefix=''):
  if is_right:
    res = '%s%r\n' % (prefix, 2)  # Right application
    res += '%s%r\n' % (prefix, slot)
    res += '%s%s\n' % (prefix, card)
  else:
    res = '%s%r\n' % (prefix, 1)  # Left application
    res += '%s%s\n' % (prefix, card)
    res += '%s%r\n' % (prefix, slot)
  sys.stdout.write(res)
  if print_debug_more:
    sys.stderr.write(res)
  sys.stdout.flush()
  # os.flush()


class Game(object):

  def __init__(self, mode):
    self._vitality = [ [10000] * 256, [10000] * 256 ]
    self._field = [ [], [] ]
    self._live = [256, 256]
    for i in xrange(256):
      o1 = Obj()
      o1.SetNewFunction('I')
      self._field[0].append(o1)
      o2 = Obj()
      o2.SetNewFunction('I')
      self._field[1].append(o2)
    self._mode = mode  # alt, only
    self._current_turn = 0
    self._next_player = 0
    self._error = False

  def HasEnded(self):
    # if self._current_turn == 100000 and self._next_player == 0:
    #   return self.WhoWins()
    if self._live[0] == 0 and self._live[1] == 0:
      return 2  # A tie
    if self._live[0] == 0:
      return 1  # Player 1 wins
    if self._live[1] == 0:
      return 0  # Player 0 wins
    return -1  # Still playing
    
  def WhoWins(self):
    if self._live[1] > self._live[0]:
      return 1
    if self._live[0] > self._live[1]:
      return 0
    return 2  # A tie

  def IsDeadSlot(self, i, player):
    return self._vitality[player][i] <= 0

  def CheckLive(self, v_old, i, player):
    if v_old <= 0 and not self.IsDeadSlot(i, player):
      # Revived
      self._live[player] += 1
    elif v_old > 0 and self.IsDeadSlot(i, player):
      # Killed
      self._live[player] -= 1
    # Nothing changes

  def IncVitality(self, i, player, inc):
    v = self._vitality[player][i]
    self._vitality[player][i] = v + inc
    if self._vitality[player][i] > 65535:
      self._vitality[player][i] = 65535
    self.CheckLive(v, i, player)

  def DecVitality(self, i, player, dec):
    # Note: only zombies can get vitality -1
    v = self._vitality[player][i]
    self._vitality[player][i] = v - dec
    if self._vitality[player][i] < 0:
      self._vitality[player][i] = 0
    self.CheckLive(v, i, player)

  def ReviveSlot(self, i, player):
    v = self._vitality[player][i]
    self._vitality[player][i] = 1
    self.CheckLive(v, i, player)

  def SetZombieVitality(self, i, player):
    v = self._vitality[player][i]
    self._vitality[player][i] = -1
    self.CheckLive(v, i, player)

  def GetSlot(self, i, player):
    return copy.deepcopy(self._field[player][i])

  def SetSlot(self, i, player, new_field):
    self._field[player][i] = copy.deepcopy(new_field)

  # public
  def IsFunc(self, i, player):
    return self._field[player][i]._is_func

  # public, only makes sense if !IsFunc
  def GetValue(self, i, player):
    return self._field[player][i]._value

  # public, check that the function is alone
  def IsAlone(self, i, player):
    return self._field[player][i].IsAlone()

  #public
  def GetVitality(self, player, slot):
    return self._vitality[player][slot]

  def GreaterVitality(self, n, player, slot):
    return n > self._vitality[player][slot]

  def CheckZombies(self, player):
    for i in xrange(256):
      if self._vitality[player][i] < 0:
        # print 'Running zombie on slot %r' % i
        try:
          # print '%r={%r,%r}' % (i, v, self._field[player][i].Print())
          if not self._field[player][i]._is_func:
            # This should be already covered by one below, but...
            raise LTGException('Error applying right not function card %r' % self._field[player][i].Print())
          f = self.GetSlot(i, player)
          o = Obj()
          o.SetNewFunction('I')
          # print 'Applying zombie slot %r, field %r on I' % (i, f.Print())
          iters = [0]
          f.ApplyRight(o, player, iters, True, self)  # Zombie call
          # print 'Used %r applications' % iters[0]
        except LTGException as e:
          # print 'ExceptionZ: %r' % str(e)
          e.Print(i)
        # Zombie always replaces
        self._field[player][i].SetNewFunction('I')
        v = self._vitality[player][i]
        self._vitality[player][i] = 0
        self.CheckLive(v, i, player)

  def Perform(self, is_right, action, slot):
    player = self._next_player
    self.CheckZombies(player)
    if player == 0:
      self._current_turn += 1
    self.PrintCurrentStatus(player)
    self._next_player = (1 - player) if self._mode == 'alt' else 0
    try:
      PrintTurnTail(is_right, player, slot, action)
      if is_right:
        self.PerformRight(action, slot, player)
      else:
        self.PerformLeft(action, slot, player)
    except LTGException as e:
      # print 'Exception: %r\nReplacing slot %r with I' % (str(e), slot)
      e.Print(slot)
      self._field[player][slot].SetNewFunction('I')

  def PerformLeft(self, action, slot, player):
    o = Obj()
    o.SetNewFunction(action)
    if not o._is_func:  # This should be already covered by one below, but...
      raise LTGException('Error applying left not function card %r' % action)
    if self.IsDeadSlot(slot, player):
      raise LTGException('Error applying left in dead slot %r' % slot)
    f = self.GetSlot(slot, player)
    # print 'Applying %r on slot %r, field %r' % (action, slot, f.Print())
    iters = [0]
    self._field[player][slot] = o.ApplyRight(f, player, iters, False, self)
    # print 'Result %r' % self._field[player][slot].Print()
    # print 'Used %r applications' % iters[0]
   
  def PerformRight(self, action, slot, player):
    if self.IsDeadSlot(slot, player):
      raise LTGException('Error applying right in dead slot %r' % slot)
    if not self._field[player][slot]._is_func:
      # This should be already covered by one below, but...
      raise LTGException('Error applying right not function card %r' % self._field[player][slot].Print())
    f = self.GetSlot(slot, player)
    o = Obj()
    o.SetNewFunction(action)
    # print 'Applying slot %r, field %r on %r' % (slot, f.Print(), action)
    iters = [0]
    self._field[player][slot] = f.ApplyRight(o, player, iters, False, self)
    # print 'Result %r' % self._field[player][slot].Print()
    # print 'Used %r applications' % iters[0]

  def PrintCurrentStatus(self, player):
    # print '*** player %r\'s turn, with slots:' % player
    if not print_debug:
      return
    if self._next_player == 0:
      sys.stderr.write('###### turn %r\n' % self._current_turn)
    sys.stderr.write('*** player %r\'s turn, with slots:\n' % player)
    for i in xrange(256):
      f = self._field[player][i]
      v = self._vitality[player][i]
      if v != 10000 or f._value != 'I' or not f._next_right is None:
        # print '%r={%r,%r}' % (i, v, f.Print())
        sys.stderr.write('%r={%r,%s}\n' % (i, v, f.Print()))
    sys.stderr.write('(slots {10000,I} are omitted)\n')


class Obj(object):

  def __init__(self):
    self._value = None
    self._params = None
    self._is_func = None
    self._next_right = None
    self._zero_card = False

  def IsAlone(self):
    return (self._next_right is None and (
        self._params is None or not self._params or len(self._params) == 0))

  def SetConstant(self, num):
    self._value = num
    self._params = None
    self._is_func = False
    self._next_right = None
    self._zero_card = False

  def SetNewFunction(self, which):
    if which == 'zero':
      self.SetConstant(0)
      self._zero_card = True
      return
    self._value = which
    self._params = []
    self._is_func = True
    self._next_right = None
    self._zero_card = False

  def Print(self):
    if self._zero_card:
      return 'zero'
    elif not self._is_func:
      return str(self._value)
    ret = self._value
    if not self._params is None:
      for p in self._params:
        ret += '(' + p.Print() + ')'
    if not self._next_right is None:
      ret += '(' + self._next_right.Print() + ')'
    return ret

  def ApplyRight(self, other_object, player, iters, is_zombie, game):
    if self._value is None:
      game._error = True
      sys.stderr.write('DIE self with no value!')
      return None
      # print 'DIE self with no value!'
      # sys.exit(1)

    # print '-Starting %r' % self.Print()
    if not other_object is None:
      # print '-other_object: %r' % other_object.Print()
      if not self._next_right is None:
        # Lets eval more to the right
        if not self._is_func:
          raise LTGException('Cannot evaluate constant in other object')
        self._next_right = self._next_right.ApplyRight(
            other_object, player, iters, is_zombie, game)
      else:
        self._next_right = other_object.ApplyRight(
            None, player, iters, is_zombie, game)
    # if not self._next_right is None:
    #   print '-NR: %r' % self._next_right.Print()

    # print '-self: %r' % self.Print()
    if self._value == 'I':
      if self._next_right is None:
        self._is_func = True
        return self
      IncIters(iters)
      # return copy.deepcopy(self._next_right)
      return self._next_right
    elif self._value == 'succ':
      if self._next_right is None:
        self._is_func = True
        return self
      if self._next_right._is_func:
        raise LTGException('Error succ on not int %r' % self._next_right._value)
      n = self._next_right._value + 1
      if n > 65535:
        n = 65535
      self.SetConstant(n)
      IncIters(iters)
      return self
    elif self._value == 'dbl':
      if self._next_right is None:
        self._is_func = True
        return self
      if self._next_right._is_func:
        raise LTGException('Error, dbl in not integer')
      n = self._next_right._value * 2
      if n > 65535:
        n = 65535
      self.SetConstant(n)
      IncIters(iters)
      return self
    elif self._value == 'get':
      if self._next_right is None:
        self._is_func = True
        return self
      if self._next_right._is_func:
        raise LTGException('Error get of function')
        # self._is_func = True
        # return self
      i = self._next_right._value
      if not IsSlotValue(i):
        raise LTGException('Error get of no slot value')
      if game.IsDeadSlot(i, player):  # proponent
        raise LTGException('Error get of dead slot')
      IncIters(iters)
      return game.GetSlot(i, player)  # proponent
    elif self._value == 'put':
      if self._next_right is None:
        self._is_func = True
        return self
      if not self._next_right._next_right is None:
        IncIters(iters)
        return self._next_right._next_right
      self.SetNewFunction('I')
      IncIters(iters)
      return self
    elif self._value == 'S':
      if self._next_right is None:
        self._is_func = True
        return self
      if len(self._params) == 0:
        # Add next_right as the first parameter f.
        self._params.append(copy.deepcopy(self._next_right))
        self._next_right = None
        self._is_func = True
        IncIters(iters)
        return self
      if len(self._params) == 1:
        # Add next_right as the first parameter g.
        self._params.append(copy.deepcopy(self._next_right))
        self._next_right = None
        self._is_func = True
        IncIters(iters)
        return self
      if len(self._params) > 2:
        game._error = True
        sys.stderr.write('DIE S with too many parameters')
        # print 'DIE S with too many parameters'
        # sys.exit(1)  # This code should not be reached!
      # S is ready to be evaluated with next parameter.
      f = self._params[0]
      g = self._params[1]
      x1 = copy.deepcopy(self._next_right)
      x2 = copy.deepcopy(self._next_right)
      if not f._is_func:
        raise LTGException('Error: S with f not function')
      h = f.ApplyRight(x1, player, iters, is_zombie, game)
      # Important: The exception raises only after evaluating f.
      if not g._is_func:
        raise LTGException('Error: S with g not function')
      y = g.ApplyRight(x2, player, iters, is_zombie, game)
      if not h._is_func:
        raise LTGException('Error: S with h not function')
      z = h.ApplyRight(y, player, iters, is_zombie, game)
      # z can be a function or a constant, it doesn't matter.
      IncIters(iters)
      return z
    elif self._value == 'K':
      if self._next_right is None:
        self._is_func = True
        return self
      if len(self._params) == 0:
        # Add next_right as the first parameter x.
        self._params.append(copy.deepcopy(self._next_right))
        self._next_right = None
        self._is_func = True
        IncIters(iters)
        return self
      if len(self._params) > 1:
        game._error = True
        sys.stderr.write('DIE K with too many parameters')
        # print 'DIE K with too many parameters'
        # sys.exit(1)  # This code should not be reached!
      # K is ready to be evaluated with next parameter.
      IncIters(iters)
      return self._params[0]
    elif self._value == 'inc':
      if self._next_right is None:
        self._is_func = True
        return self
      if self._next_right._is_func:
        raise LTGException('Error inc of function')
        # self._is_func = True
        # return self
      i = self._next_right._value
      if not IsSlotValue(i):
        raise LTGException('Error inc of no slot value')
      if not game.IsDeadSlot(i, player):
        if is_zombie:
          game.DecVitality(i, player, 1)  # proponent
        else:
          game.IncVitality(i, player, 1)  # proponent
      self.SetNewFunction('I')
      IncIters(iters)
      return self
    elif self._value == 'dec':
      if self._next_right is None:
        self._is_func = True
        return self
      if self._next_right._is_func:
        raise LTGException('Error dec of function')
        # self._is_func = True
        # return self
      i = self._next_right._value
      if not IsSlotValue(i):
        raise LTGException('Error dec of no slot value')
      if not game.IsDeadSlot(255 - i, player):
        if is_zombie:
          game.IncVitality(255 - i, 1 - player, 1)  # opponent
        else:
          game.DecVitality(255 - i, 1 - player, 1)  # opponent
      self.SetNewFunction('I')
      IncIters(iters)
      return self
    elif self._value == 'attack':
      if self._next_right is None:
        self._is_func = True
        return self
      if len(self._params) == 0:
        # Add next_right as the first parameter i.
        self._params.append(copy.deepcopy(self._next_right))
        self._next_right = None
        self._is_func = True
        IncIters(iters)
        return self
      if len(self._params) == 1:
        # Add next_right as the first parameter j.
        self._params.append(copy.deepcopy(self._next_right))
        self._next_right = None
        self._is_func = True
        IncIters(iters)
        return self
      if len(self._params) > 2:
        game._error = True
        sys.stderr.write('DIE attack with too many parameters')
        # print 'DIE attack with too many parameters'
        # sys.exit(1)  # This code should not be reached!
      # attack is ready to be evaluated with next parameter n.
      i = self._params[0]._value
      if not IsSlotValue(i):
        raise LTGException('Error: attack with i not slot number')
      if self._next_right._is_func:  # n is not integer.
        raise LTGException('Error: attack with non integer n')
      n = self._next_right._value
      if game.GreaterVitality(n, player, i):  # proponent
        raise LTGException('Error: attack with n > v')
      game.DecVitality(i, player, n)  # proponent
      j = self._params[1]._value
      if not IsSlotValue(j):
        raise LTGException('Error: attack with i not slot number')
      if not game.IsDeadSlot(255 - j, 1 - player):
        if is_zombie:
          game.IncVitality(
              255 - j, 1 - player, int((9 * n) / 10))  # opponent
        else:
          game.DecVitality(
              255 - j, 1 - player, int((9 * n) / 10))  # opponent
      self.SetNewFunction('I')
      IncIters(iters)
      return self
    elif self._value == 'help':
      if self._next_right is None:
        self._is_func = True
        return self
      if len(self._params) == 0:
        # Add next_right as the first parameter i.
        self._params.append(copy.deepcopy(self._next_right))
        self._next_right = None
        self._is_func = True
        IncIters(iters)
        return self
      if len(self._params) == 1:
        # Add next_right as the first parameter j.
        self._params.append(copy.deepcopy(self._next_right))
        self._next_right = None
        self._is_func = True
        IncIters(iters)
        return self
      if len(self._params) > 2:
        game._error = True
        sys.stderr.write('DIE help with too many parameters')
        # print 'DIE help with too many parameters'
        # sys.exit(1)  # This code should not be reached!
      # attack is ready to be evaluated with next parameter n.
      i = self._params[0]._value
      if not IsSlotValue(i):
        raise LTGException('Error: attack with i not slot number')
      if self._next_right._is_func:  # n is not integer.
        raise LTGException('Error: attack with non integer n')
      n = self._next_right._value
      if game.GreaterVitality(n, player, i):  # porponent
        raise LTGException('Error: attack with n > v')
      game.DecVitality(i, player, n)  # proponent
      j = self._params[1]._value
      if not IsSlotValue(j):
        raise LTGException('Error: attack with i not slot number')
      if not game.IsDeadSlot(j, 1 - player):
        if is_zombie:
          game.DecVitality(j, player, int((11 * n) / 10))  # proponent
        else:
          game.IncVitality(j, player, int((11 * n) / 10))  # proponent
      self.SetNewFunction('I')
      IncIters(iters)
      return self
    elif self._value == 'copy':
      if self._next_right is None:
        self._is_func = True
        return self
      if self._next_right._is_func:
        raise LTGException('Error copy of function')
        # self._is_func = True
        # return self
      i = self._next_right._value
      if not IsSlotValue(i):
        raise LTGException('Error copy of no slot value')
      IncIters(iters)
      return game.GetSlot(i, 1 - player)  # opponent
    elif self._value == 'revive':
      if self._next_right is None:
        self._is_func = True
        return self
      if self._next_right._is_func:
        raise LTGException('Error revive of function')
        # self._is_func = True
        # return self
      i = self._next_right._value
      if not IsSlotValue(i):
        raise LTGException('Error revive of no slot value')
      if game.IsDeadSlot(i, player):
        game.ReviveSlot(i, player)  # proponent
      self.SetNewFunction('I')
      IncIters(iters)
      return self
    elif self._value == 'zombie':
      if self._next_right is None:
        self._is_func = True
        return self
      if len(self._params) == 0:
        # Add next_right as the first parameter i.
        self._params.append(copy.deepcopy(self._next_right))
        self._next_right = None
        self._is_func = True
        IncIters(iters)
        return self
      if len(self._params) > 1:
        game._error = True
        sys.stderr.write('DIE zombie with too many parameters')
        # print 'DIE zombie with too many parameters'
        # sys.exit(1)  # This code should not be reached!
      # zombie is ready to be evaluated with next parameter x.
      if self._params[0]._is_func:
        raise LTGException('Error zombie using non value slot')
      i = self._params[0]._value
      if not IsSlotValue(i):
        raise LTGException('Error zombie of no slot value')
      if not game.IsDeadSlot(255 - i, 1 - player):  # opponent
        raise LTGException('Error zombie of no dead slot')
      game.SetSlot(255 - i, 1 - player, self._next_right)
      game.SetZombieVitality(255 - i, 1 - player)
      self.SetNewFunction('I')
      IncIters(iters)
      return self

    # This should be a numeric constant!
    if self._is_func:
      game._error = True
      sys.stderr.write('DIE ERROR NUMERIC CONSTANT NOT FOUND')
      # print 'DIE ERROR NUMERIC CONSTANT NOT FOUND'
      # sys.exit(1)
    return self
