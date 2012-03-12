#!/usr/bin/perl -w

use strict;

use IO::Handle;


my $GNUPLOT="gnuplot";

my $TO_FILE = 1;

open(PLOT, "| $GNUPLOT -persist -") || die ("Can't start $GNUPLOT: $!");
PLOT->autoflush(1);

print PLOT 'set title "Reflow!"'."\n";
print PLOT "set mouse;\n";
print PLOT "set key outside below\nset autoscale;\n";


while(1)
{
  
  print PLOT "set term x11;";
  if ($TO_FILE)
  {
    print PLOT "set term png;";
    print PLOT "set output 'tmp.png';";
  }
  print PLOT "plot 'splinedata' with linespoints, ";
  print PLOT "'oven1.log' using 1:3 t 'controlling - top' with lines, ";
  print PLOT "'oven1.log' using 1:5 t 'bottom' with lines, ";
  print PLOT "'oven1.log' using 1:6 t 'up' with lines,";
  print PLOT "'oven1.log' using 1:7 t 'down' with lines;\n";

  exit 1
    if ($TO_FILE);

  sleep 4
}

close(PLOT);
