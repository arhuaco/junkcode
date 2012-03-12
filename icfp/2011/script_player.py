#!/usr/bin/python

import ltg
import random
import sys
import time
import engine

# LOC is a numbrer ranging form 0 to 255 (inclusive).
# number is a value from 0 to 65535
# command (valid card)


# **** set
# set LOC number     # copies the number
# set LOC card       # copies the card
# set LOC *number    # copies the contents of another location

# *** right

# right LOC 0       # appends 0 to the right of the current LOC
# right LOC 1       # appends 1 to the right of the current LOC

# right LOC *0      # appends contents of LOC 0 to the right of the current LOC
                    # FIXME: 0 needs to be alive?

# right LOC *N      #appends contents of LOC N to the right of the current LOC
                    # This will overwrite 0!
                    # less efficient than loc 0
                    # FIXME: 0 needs to be alive?

# left LOC card     # Low level command, that applies a card to a slot.

# rewind            # starts from the first instruction.

start_str = """
# matar celda 0
set 6 255
set 7 4000
"""
i = 10
curr = 0
while i < 240:
  start_str += 'set 6 %r\n' % (255 - curr)
  start_str += 'set %r %r\nleft %r attack\nright %r *6\nright %r *7\n' % (i, i, i, i, i)
  i += 1
  start_str += 'set %r %r\nleft %r attack\nright %r *6\nright %r *7\n' % (i, i, i, i, i)
  i += 1
  start_str += 'set %r %r\nleft %r attack\nright %r *6\nright %r *7\n' % (i, i, i, i, i)
  i += 1
  curr += 1
i = 10
while i < 240:
  start_str += 'set 6 %r\n' % (255 - curr)
  start_str += 'set %r %r\nleft %r attack\nright %r *6\nright %r *7\n' % (i, i, i, i, i)
  i += 1
  start_str += 'set %r %r\nleft %r attack\nright %r *6\nright %r *7\n' % (i, i, i, i, i)
  i += 1
  start_str += 'set %r %r\nleft %r attack\nright %r *6\nright %r *7\n' % (i, i, i, i, i)
  i += 1
  curr += 1

sys.stderr.write(start_str)

normal_engine = engine.Engine(start_str)

def GenMove(game, player):
  return normal_engine.play(game, player)

time.sleep(0.1)

try_run = False
ltg.print_debug = False
ltg.print_debug_more = False

# sys.stdin = open('./run 1 |')

game = ltg.Game('alt')
if sys.argv[1] == '1':
  player = 1
  if ltg.print_debug_more:
    sys.stderr.write('I am player 1\n')
  (is_right, card, slot) = ltg.ReadFromStdin()
  game.Perform(is_right, card, slot)
else:
  player = 0
  if ltg.print_debug_more:
    sys.stderr.write('I am player 0\n')

# while game.HasEnded() == -1:
while True:
  (is_right, card, slot) = GenMove(game, player)
  ltg.WriteToStdout(is_right, card, slot)
  game.Perform(is_right, card, slot)
  # if game.HasEnded() != -1:
  #   break
  # Debug de Cristian.
  if not try_run:
    (is_right, card, slot) = ltg.ReadFromStdin()
    game.Perform(is_right, card, slot)

if ltg.print_debug_more:
  sys.stderr.write('Player %r won\n' % game.WhoWins())
