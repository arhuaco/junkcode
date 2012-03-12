./rng1 2794871142
./diehardc out.rnd out.txt 1111111111111111
cat out.txt | ./parse_diehardc.pl
