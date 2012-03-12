function run
{
  ./checkmany 15 10000000 1024 $1 0 15-10M-1024-$1-0 1 > check$1.log &
}  

run 32
run 64
run 128
run 255
run 256
run 257
