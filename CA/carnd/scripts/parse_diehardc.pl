#! /usr/bin/perl -w

# Parse the diehardc output.
# Note : The name of the output file MUST NOT contain any digit
# Nelson Castillo

$debug      = 0;
$debug_bs   = 0;    # bitstream test
$debug_opso = 0 ; # OPSO test
$debug_oqso = 0 ; # OQSO test
$debug_dna  = 0 ;  # DNA test

@exp = qw{
    ^\D*\d+\D*(.*)$  
    ^\D*(.*)$
    ^\D*[0-9.]*\D*\d\D*(.*)$
    ^\D*[0-9.]*\D*[0-9.]*\D*(.*)$
    ^\D*[0-9.]*\D*(.*)$
};

sub add_test {
  $index[$count]= $_[0];
  $text [$count]=   $_[1];
  $use  [$count++]= $_[2];
}

$count = 0; # test counter

# add_test (line, text, regex to use);

add_test(23,"OVERLAPPING SUMS",0);
add_test(44,"RUNS UP 1",0);
add_test(45,"RUNS DOWN 1",0);
add_test(47,"RUNS UP 2",0);
add_test(48,"RUNS DOWN 2",0);
add_test(87,"3DSPHERES",0);
add_test(129,"PARKING LOT",0);
add_test(273,"BIRTHDAY SPACINGS TEST",0);
add_test(549, "BINARY RANK 6x8 MATRICES",1);
add_test(569, "BINARY RANK 31x31 MATRICES",2);
add_test(588, "BINARY RANK 32x32 MATRICES",2);
add_test(614, "COUNT-THE-1's IN SUCCESSIVE BYTES 1",3);
add_test(615, "COUNT-THE-1's IN SUCCESSIVE BYTES 2",3);
add_test(705, "CRAPS No. OF WINS",1);
add_test(706, "CRAPS THROWS/GAME",1);
add_test(748, "MINIMUM DISTANCE",1);
add_test(769, "OVERLAPPING PERMUTATION 1",3);
add_test(772, "OVERLAPPING PERMUTATION 2",3);
add_test(930, "SQUEEZE",4);
$index [$count]  =0; # the last one


$actual = 0; # wich one
$passed = 0; # count passed tests
$bs_passed = 0; # count the passed tests (bitstream)
$opso_passed = 0 ; # count the passed tests (OPSO)
$oqso_passed = 0 ; # count the passed tests (OQSO)
$dna_passed = 0 ; # count the passed tests (DNA)

while (<>){
    if ($index[$actual] eq $. ){
        print "\nparsing line $. exp : $exp[$use[$actual]]\n$_" if $debug;
        if (/$exp[$use[$actual]]/){
             print "$text[$actual] : $1 : ";
            if ($1 > 0.001 and $1 < 0.999) {
                print "P\n";
                $passed++;
            }else{
                print "F\n";
            }
            ++$actual;
        }      
    }
    elsif ( $. >= 641 and $. <= 660 ){ # BITSTREAM
        print "\nparsing line $.\n$_" if $debug_bs;
        if( /^\D*\d+\D*\d+\D*[0-9.]*\D*(.*)$/ ){
            print "Parsed $1\n" if $debug_bs;
            (++ $bs_passed) if ($1 > 0.001 and $1 < 0.999);
         } 
    }
    elsif ( $. >= 815 and $. <= 837 ){ # OPSO
        print "\nparsing line $.\n$_" if $debug_opso;
        if( /^\D*\d+\D*\d+\D*\d+\D*[0-9.]*\D*(.*)$/ ){
            print "Parsed $1\n" if $debug_opso;
           (++ $opso_passed) if ($1 > 0.001 and $1 < 0.999);
        }
    } 
    elsif ( $. >= 841 and $. <= 868 ){ # OQSO
        print "\nparsing line $.\n$_" if $debug_oqso;
        if( /^\D*\d+\D*\d+\D*\d+\D*[0-9.]*\D*(.*)$/ ){
            print "Parsed $1\n" if $debug_oqso;
           (++ $oqso_passed) if ($1 > 0.001 and $1 < 0.999);
        }
    }
    elsif ( $. >= 872 and $. <= 902 ){ # DNA
        print "\nparsing line $.\n$_" if $debug_dna;
        if( /^\D*\d+\D*\d+\D*\d+\D*[0-9.]*\D*(.*)$/ ){
            print "Parsed $1\n" if $debug_dna;
           (++ $dna_passed) if ($1 > 0.001 and $1 < 0.999);
        }
    }elsif(/^A KSTEST of those values yields ([0-9.]+)$/){
      $KS=$1;
    }

}

print "\nPassed : $passed/$count : \n";
print "BITSTREAM : $bs_passed/20 : \n";
print "OPSO : $opso_passed/23 : \n";
print "OQSO : $oqso_passed/28 : \n";
print "DNA : $dna_passed/31 : \n\n";

$Score = $passed + $bs_passed/20 + $opso_passed/23 + $oqso_passed/28 + $dna_passed/31;
$_=$Score + 0.0005;
if(/(\d+\.*\d{0,3}).*/){
  $Score=$1;
}

print "Score=$Score\n";
print "KS=$KS\n";
