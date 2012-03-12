#!/usr/bin/perl -w

#print "$size,$rule,$max_cycle,$avg_cycle,$ncycles\n";
#5,2589284954,5,2.33333333333333,3

my $last_size = -1;
my %max_max; # Maximum of all maximums
my %max_rule; # Maximum of all maximums

sub print_results()
{
  my $nstates = 2 ** $last_size;
  my $nbytes = $last_size * $max_max{$last_size};

  print "Size:$last_size ($nstates states) max_cyle_len:$max_max{$last_size} ($nbytes bytes) rule:$max_rule{$last_size}\n";
}

while (<>)
{
  my ($size, $rule, $max_cycle, $avg_cycle, $ncycles) = split /,/;

  if (not defined($max_max{$size}))
  {
    $max_max{$size} = $max_cycle;
    $max_rule{$size} = $rule;
  }
  else
  {
    if ($max_cycle > $max_max{$size})
    {
      $max_max{$size}  = $max_cycle;
      $max_rule{$size} = $rule;
    }
  }

  if ($size != $last_size)
  {
    if ($last_size != -1)
    {
      print_results;
    }

    $last_size = $size;
  }
}

print_results;
