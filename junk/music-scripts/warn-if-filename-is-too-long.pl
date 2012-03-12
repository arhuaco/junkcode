#!/usr/bin/perl -w

use strict;
my $max_file_len = 64;

while (<>)
{
  chomp;
  
  my $full = "$_\n";
  
  s/^.*?([^\/]+)+$/$1/;
 
  print $full if (length() > $max_file_len)
}
