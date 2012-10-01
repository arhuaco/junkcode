#!/usr/bin/python

# With this file with can compute the winning odds of the mafia in the
# mafia game. See the paper:
# A mathematical model of the Mafia game: http://arxiv.org/abs/1009.1031

# n : citizens
# m : mafia members
def MafiaWinsMemo(n, m):
  # if there are no mafia members left, mafia won.
  if m == 0:
    return 0.0
  # if there are more mafia members, mafia wins
  if m > n - m:
    return 1.0
  return (float(n - m)/float(n)) * MafiaWinsMemo(n - 2, m) + \
         float(m)/float(n) * MafiaWinsMemo(n - 2, m - 1)


print MafiaWinsMemo(7, 2)
