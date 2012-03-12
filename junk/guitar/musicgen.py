#!/usr/bin/python

# This is a file that helps me generate partitures for
# the program songwrite2. Don't think of this as a generic
# library.

TPL0 = """<?xml version="1.0" encoding="utf-8"?>

<song version="0.2.1" lang="en" printsize="20">
	<title>N's song</title>
	<authors>N</authors>
	<copyright>Copyright 2009 by N</copyright>
	<comments></comments>
	<bars>"""
#<bar rythm="4/4" tempo="90" syncope="0"/>

END_TPL0 = """</bars>
	<playlist>
	</playlist>"""

TPL_GUITAR_HARMONICS = """
	<partition volume="255" muted="0" print_with_staff_too="0" g8="1" chorus="0" instrument="31" capo="0" reverb="0" tonality="C">
		<header>Guitar</header>
		<view type="tablature">
			<strings>
				<string pitch="64"/>
				<string pitch="59"/>
				<string pitch="55"/>
				<string pitch="50"/>
				<string pitch="45"/>
				<string pitch="40"/>
			</strings>
		</view>
		<notes>"""

TPL_GUITAR = """
	<partition volume="255" muted="0" print_with_staff_too="0" g8="1" chorus="0" instrument="26" capo="0" reverb="0" tonality="C">
		<header>Guitar</header>
		<view type="tablature">
			<strings>
				<string pitch="64"/>
				<string pitch="59"/>
				<string pitch="55"/>
				<string pitch="50"/>
				<string pitch="45"/>
				<string pitch="40"/>
			</strings>
		</view>
		<notes>"""
# <note pitch="76" time="21.0" duration="1.0" volume="204" string="0"/>
TPL_BASS = """
<partition volume="255" muted="0" print_with_staff_too="0" g8="1" chorus="0" instrument="19" capo="0" reverb="0" tonality="C">
		<header>Bass</header>
		<view type="tablature">
			<strings>
				<string pitch="43"/>
				<string pitch="38"/>
				<string pitch="33"/>
				<string pitch="28"/>
			</strings>
		</view>
		<notes>"""
TPL_END_INSTRUMENT = """
		</notes>
	</partition>"""

TPL_END = "</song>"

# These are the notes of the guitar. It's a scale.
# The first pair is the 6th string, the fitst E (lowest note), Then F, G.
# Then we have A, B, C, in the 5th string.
# Then D in the 4th string.. and so on.
GUITAR = [(40, 5), (41, 5), (43, 5), (45, 4), (47, 4), (48, 4), (50, 3), (52, 3), (53, 3), (55, 2), (57, 2), (59, 1), (60, 1), (62, 1), (64, 0), (65, 0), (67, 0), (69, 0), (71, 0), (73, 0), (74, 0), (76, 0)]

BASS = [(28, 3), (29, 3), (31, 3), (33, 2), (35, 2), (36, 2), (38, 1), (40, 1), (41, 1), (43, 0), (45, 0)]

def index_notes(INSTRUMENT):
  """ I want to make C = 0. Thus we have this table
       E F G A B C D
       2 3 4 5 6 0 1 """

  indexed = []
  idx = 2
  for note in INSTRUMENT:
    indexed.append((idx, note))
    idx = (idx + 1) % 7
  return indexed

def next_note(IDX_NOTES, current, next):
  """ Loook for the closes note in the guitar. """
  #print 'current, next', (current, next)
  assert(current >= 0 and current < len(IDX_NOTES))
  assert(next >= 0 and next < len(IDX_NOTES))
  up, down = current, current
  while IDX_NOTES[down][0] != next and IDX_NOTES[up][0] != next:
    down = max(0, down - 1)
    up   = min(len(IDX_NOTES) - 1, up + 1)
  if IDX_NOTES[down][0] == next:
    return down
  return up

INSTRUMENTS = {}
INSTRUMENTS["GUITAR_HARMONICS"] = [index_notes(GUITAR), TPL_GUITAR_HARMONICS]
INSTRUMENTS["GUITAR"] = [index_notes(GUITAR), TPL_GUITAR]
INSTRUMENTS["BASS"] = [index_notes(BASS), TPL_BASS]

def write_header(file,  NOTES):
  print >>file, TPL0
  for i in xrange(len(NOTES) / 4 + 1):
    print >> file, '<bar rythm="4/4" tempo="120" syncope="0"/>'
  print >> file, END_TPL0

def add_instrument(file, instrument_name, seq, strategy, duration, block_size = 32):
  idx = [] #index to what the instrument makes available
  instrument = INSTRUMENTS[instrument_name][0]
  print >> file, INSTRUMENTS[instrument_name][1]
  if strategy == "inc-dec": #increment and decrement
    notes = []
    current = 0
    prev = 0
    for i in seq: # map to C, D, E, F, G, A, B
      notes.append(current)
      if prev < i:
        current = (current + 1) % 7
      else:
        current = (current - 1 + 7) % 7
      prev = i 
    # Find the closest note in the given scale for the changes
    current_idx = 0
    for n in notes:
      current_idx = next_note(instrument, current_idx, n) # closest
      idx.append(current_idx)
  elif strategy == "map": # map to the available notes in blocks.
    while len(seq):
      m = max(seq[:block_size]) + 1
      for n in seq[:block_size]: 
        idx.append(int((float(n) / m) * len(instrument)))
      seq = seq[block_size:] # not efficient at all.
  else:
    assert(False) # Error, unknown strategy.
  # now the output 
  t = 0
  for i in idx:
    pitch, string = instrument[i][1:][0]
    if t % duration == 0:
      print >> file, ('<note pitch="%d" time="%d.0" duration="%d.0" volume="204" string="%d"/>' % (pitch, t, duration, string))
    t += 1
  print >> file, TPL_END_INSTRUMENT 

def write_end(file):
  print >> file, TPL_END
