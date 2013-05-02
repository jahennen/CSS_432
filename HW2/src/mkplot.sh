#!/bin/bash

echo set terminal postscript landscape
echo set nolabel
echo set xlabel \"Window Size\"
echo set xrange [0:30]
echo set ylabel \"usec\"
echo set yrange [0:25000000]
echo set output \"udp.ps\"
echo plot \"100mbpsSW.dat\" title \"100mbps sliding window\" with linespoints, \"1gbpsSW.dat\" title \"1gbps sliding window\" with linespoints, `cat 100mbpsStW.dat` title \"100mbs stop\&wait\" with line, `cat 1gbpsStW.dat` title \"1gbps stop\&wait\" with line

gnuplot udp.plt
ps2pdf udp.ps
