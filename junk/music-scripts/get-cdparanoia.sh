#!/bin/sh
# See <http://bachue.com/svnwiki/mp3-encode> for details.
cdparanoia -B 1-
normalize-audio -vb track*.cdda.wav
for file in track*.cdda.wav; \
  do lame -h -V2 -b32 -mj -q1 $file $(echo $file | sed 's/track\(..\).cdda.wav/\1.mp3/') \
     && rm $file; \
done
