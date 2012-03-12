set terminal png
set output 'bench.png'
set   autoscale                        # scale axes automatically
set logscale x  
set xtics (16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536)
set ytic auto                          # set ytics automatically
set title "Floating Point Benchmarck"
set ylabel "MFLOP/S"
set xlabel "N"
plot "bench.csv" u 1:2 w l title 'dot-prod(C)', "bench.csv" u 1:3 w l title 'dot-prod(SSE 4-reg)', "bench.csv" u 1:4 w l title 'dot-prod(SSE 16-reg)', "bench.csv" u 1:5 w l title 'euclidean-dist(C)', "bench.csv" u 1:6 w l title 'euclidean-dist(SSE 4-reg)', "bench.csv" u 1:7 w l title 'euclidean-dist(SSE 4-reg)'
