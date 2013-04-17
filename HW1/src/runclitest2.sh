#!/bin/bash

HOST=uw1-320-12.uwb.edu
outfile=./results/CliOut.txt

for nbuf in 15 30 60
do
	bufsize=`expr 1500 / $nbuf`
	for type in 1 2 3
	do
		echo >> $outfile
		echo
		echo Type $type, nbuf $nbuf, bufsize $bufsize >> $outfile
		for (( i = 0 ; i < 10 ; i++ ))
		do
			./client 38028 20000 $nbuf $bufsize $HOST $type 2>> $outfile
			sleep 1
		done
	done
done
