#!/bin/bash


cat ./data/100mbpsStW*.dat | paste -sd+ | bc > ./100mbpsStW.dat

cat ./data/100mbpsSW*.dat > ./data/tempSW.dat

for i in `seq 0 29` 
do
	cat $i >> 100mbpsSW.dat
	cat ./data/tempSW.dat | 
	awk 'NR % 30 + '"$i"' { print $2 }' |
	paste -sd+ | bc 
	
done 

rm ./data/temp.dat
# awk 'NR % 30 == '"$i"' { print NR " " $1; n++; sum += $1 } ; END { print "avg = " sum/n}'
