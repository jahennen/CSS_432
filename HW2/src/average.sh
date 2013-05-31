#!/bin/bash


expr `cat ./data/100mbpsStW*.dat | paste -sd+ | bc` / 5 > ./100mbpsStW.dat

for file in ./data/100mbpsSW*.dat
do
	cat $file
	echo
done > ./data/tempSW.dat

rm ./100mbpsSW.dat
for i in `seq 1 30` 
do
	sumelapsed=$(cat ./data/tempSW.dat | 
	awk 'NR % 31 == '"$i"' { print $2 }' |
	paste -sd+ | bc)
	elapsed=`expr $sumelapsed / 5`
	echo $i $elapsed >> ./100mbpsSW.dat
	
done 

rm ./data/tempSW.dat

# 1gbps average below

expr `cat ./data/1gbpsStW*.dat | paste -sd+ | bc` / 5 > ./1gbpsStW.dat

for file in ./data/1gbpsSW*.dat
do
	cat $file
	echo
done > ./data/tempSW.dat

rm ./1gbpsSW.dat
for i in `seq 1 30` 
do
	sumelapsed=$(cat ./data/tempSW.dat | 
	awk 'NR % 31 == '"$i"' { print $2 }' |
	paste -sd+ | bc)
	elapsed=`expr $sumelapsed / 5`
	echo $i $elapsed >> ./1gbpsSW.dat
	
done 

rm ./data/tempSW.dat


# awk 'NR % 30 == '"$i"' { print NR " " $1; n++; sum += $1 } ; END { print "avg = " sum/n}'
