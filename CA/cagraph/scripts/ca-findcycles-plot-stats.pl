#!/usr/bin/perl -w

use Fcntl;
use POSIX qw(tmpnam);

my $GNUPLOT="/usr/bin/gnuplot";
my $tmpfile;

do { $tmpfile = tmpnam() }
  until sysopen(TMP, $tmpfile, O_RDWR|O_CREAT|O_EXCL);

#print "$size,$rule,$max_cycle,$avg_cycle,$ncycles\n";
#5,2589284954,5,2.33333333333333,3

my $last_size = -1;

my %max_max; # Maximum of all maximums
my %max_avg; # Average Maximum
my %avg_avg; # Average length
my %count; # Average Maximum

while (<>)
{
  my ($size, $rule, $max_cycle, $avg_cycle, $ncycles) = split /,/;

  if (not defined($max_max{$size}))
  {
    $max_max{$size} = $max_cycle;
    $max_avg{$size} = $max_cycle;
    $avg_avg{$size} = $avg_cycle;

    $count{$size} = 1;
  }
  else
  {
    if ($max_cycle > $max_max{$size})
    {
      $max_max{$size} = $max_cycle
    }

    $max_avg{$size} += $max_cycle;
    $avg_avg{$size} += $avg_cycle;

    $count{$size} ++;
  }

  # TODO: print states
  sub print_row ()
  {
     $max_avg{$last_size} /=  $count{$last_size};
     $avg_avg{$last_size} /=  $count{$last_size};

     print TMP "$last_size $max_max{$last_size} $max_avg{$last_size} $avg_avg{$last_size}\n";
     #print "$last_size $max_max{$last_size} $max_avg{$last_size} $avg_avg{$last_size}\n";
  }

  if ($size != $last_size)
  {
    if ($last_size != -1)
    {
      print_row;
    }

    $last_size = $size;
  }
}

print_row;

open(PLOT, "| $GNUPLOT -persist -") || die ("Can't start $GNUPLOT: $!");

print PLOT 'set title "Cyclic behavior of good 1d r=2 CA PRNG"'."\n";

# Uncomment the following line to send the plot to a file.
print PLOT 'set terminal png; set output "cycles.png";';


print PLOT "set xlabel \"Number of cells in the automaton\";";
print PLOT "set ylabel \"size of the cycle (number of states)\";";
print PLOT "set key outside below\nset autoscale\n";
print PLOT "plot  '$tmpfile' using 1:2 t \"Maximum attained cycle length\" with lines,".
           "'' using 1:3 t \"Average Maximum length\" with lines,".
           "'' using 1:4 t \"Average cycle length\" with lines\n";

close(PLOT);
close(TMP);
unlink($tmpfile);
