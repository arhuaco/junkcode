#!/usr/bin/python

import ltg
import random
import sys
import time

# random.seed(100)


def GetRandom(i):
  return random.randint(0, i)

cards = [ 'I',
          'zero',
          'succ',
          'dbl',
          'get',
          'put',
          'S',
          'K',
          'inc',
          'dec',
          'attack',
          'help',
          'copy',
          'revive',
          'zombie' ]


def GenMove():
  is_right = True if GetRandom(1) else False
  card = cards[GetRandom(14)]
  slot = GetRandom(255)
  return (is_right, card, slot)

time.sleep(0.1)

try_run = False
ltg.print_debug = False
ltg.print_debug_more = False

game = ltg.Game('alt')

if sys.argv[1] == '1':
  if ltg.print_debug_more:
    sys.stderr.write('I am player 1\n')
  (is_right, card, slot) = ltg.ReadFromStdin()
  game.Perform(is_right, card, slot)
else:
  if ltg.print_debug_more:
    sys.stderr.write('I am player 0\n')

# while game.HasEnded() == -1:
while True:
  (is_right, card, slot) = GenMove()
  ltg.WriteToStdout(is_right, card, slot)
  game.Perform(is_right, card, slot)
  # if game.HasEnded() != -1:
  #   break
  if not try_run:
    (is_right, card, slot) = ltg.ReadFromStdin()
    game.Perform(is_right, card, slot)

if ltg.print_debug_more:
  sys.stderr.write('Player %r won\n' % game.WhoWins())
