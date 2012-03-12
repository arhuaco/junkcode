#!/usr/bin/perl -w

my $max_file_len = 64;
my $DEBUG = 1;

use strict;

while (<>)
{
  chomp;
 
  my $old = $_;
  my $base = $_;

  $base = s/^(.*?\/)[^\/]+$//;
  
  # Change unwanted chars and trim spaces.

  s/\&/and/;
  s/ /_/mg;
  s/[^\w|\d|\.|\/|\+|\_|\=]/-/mg;
  
  my $ext='';
  
  if (/(\.[^\.|\/]+)$/)
  {
    $ext = $1
  }

  print "ext = $ext\n"
    if $DEBUG;
  
  # Max file length (without the extension)
  # TODO: We're in trouble if length($ext) >= $max_file_len

  my $maxlen = length() - length($ext);

  
  if ($maxlen + length($ext) > $max_file_len)
  {
    $maxlen = $max_file_len - length($ext)
  }
  
  print "maxlen = $maxlen\n"
    if $DEBUG;
  
  s/^(.{0,$maxlen}).*?(\.[^\.|\/]+)$/$1$2/;
  
  print "$_\n"
    if $DEBUG;
    
  if (not rename($old,$_))
  {
    print "$_\n"
  }
}
