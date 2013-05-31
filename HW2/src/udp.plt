set terminal postscript landscape
set nolabel
set xlabel "Window Size"
set xrange [0:30]
set ylabel "usec"
set yrange [0:25000000]
set output "udp.ps"
plot "100mbpsSW.dat" title "100mbps sliding window" with linespoints, "1gbpsSW.dat" title "1gbps sliding window" with linespoints, 16734666 title "100mbs stop&wait" with line, 2647294 title "1gbps stop&wait" with line
