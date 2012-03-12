#!/usr/bin/perl -w

 use strict;

 use CDDB_get qw( get_cddb );
 use MP3::Tag;

 my %config;

 # following variables just need to be declared if different from defaults

 $config{CDDB_HOST}="freedb.freedb.org";        # set cddb host
 $config{CDDB_PORT}=8880;                       # set cddb port
 $config{CDDB_MODE}="cddb";                     # set cddb mode: cddb or http
 $config{CD_DEVICE}="/dev/cdrom";               # set cd device

 # user interaction welcome?

 $config{input}=1;   # 1: ask user if more than one possibility
                     # 0: no user interaction

 # get it on

 my %cd=get_cddb(\%config);

 unless(defined $cd{title}) {
   die "no cddb entry found";
 }


 print "artist: $cd{artist}\n";
 print "title: $cd{title}\n";
 print "category: $cd{cat}\n";
 print "cddbid: $cd{id}\n";
 print "trackno: $cd{tno}\n";

 my $n=1;

 open (RENAME, ">rename.sh");

 foreach my $song_title ( @{$cd{track}} ) {

   print "track $n: $song_title\n";

   my $file = $n < 10 ? "0$n" : $n;

   $file .= '.mp3';

   my $mp3 = MP3::Tag->new($file) or die;

   $mp3->new_tag("ID3v2");

   $mp3->{ID3v2}->add_frame('TIT1', $cd{cat})
     if exists $cd{cat};
     
   $mp3->{ID3v2}->add_frame('TIT2', $song_title);

   $mp3->{ID3v2}->add_frame('TPE1', $cd{artist})
     if exists $cd{artist};
     
   $mp3->{ID3v2}->add_frame('TCON', $cd{genre})
     if exists $cd{genre};

   $mp3->{ID3v2}->add_frame('TALB', $cd{title})
     if exists $cd{title};
   
   $mp3->{ID3v2}->add_frame('TRCK', "$n");

   $mp3->{ID3v2}->add_frame('TYER', $cd{year})
     if exists $cd{year};

   #How to I set this tag? Let's keep it in comments for now.
   $mp3->{ID3v2}->add_frame('COMM', 'ENG', 'Short text', 'CDDB DISCID: ' . $cd{id});
   
   #I couldn't set these tags.
   #$mp3->{ID3v2}->add_frame("TENC", 'Short text', "LAME");
   #$mp3->{ID3v2}->add_frame("TSSE", "-h -V2 -b32 -mj -q1");

   $mp3->{ID3v2}->write_tag();

   $mp3->close();

   my $new = $song_title;

   $new =~ s/\s+/_/g; 
   $new =~ s/á/a/g;
   $new =~ s/é/e/g;
   $new =~ s/í/i/g;
   $new =~ s/ó/o/g;
   $new =~ s/ú/u/g;
   $new =~ s/â/a/g;
   $new =~ s/ê/e/g;
   $new =~ s/ô/o/g;
   $new =~ s/ã/a/g;
   $new =~ s/õ/o/g;
   $new =~ s/ç/c/g;
   $new =~ s/Á/A/g;
   $new =~ s/É/E/g;
   $new =~ s/Í/I/g;
   $new =~ s/Ó/O/g;
   $new =~ s/Ú/U/g;
   
   $new =~ s/\W//g; 

   print RENAME "mv \"$file\" $new.mp3\n";

   $n++;
 }

 close RENAME;
