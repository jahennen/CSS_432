#!/bin/bash

outfile=./results/ServOut.txt

for nbuf in 15 30 60
do
	bufsize=`expr 1500 / $nbuf`
	for type in 1 2 3
	do
		echo >> $outfile
		echo
		echo Type $type, nbuf $nbuf, bufsize $bufsize		
		echo Type $type, nbuf $nbuf, bufsize $bufsize >> $outfile
		for (( i = 0 ; i < 10 ; i++ ))
		do
			./server 38028 20000 2>> $outfile
			sleep 1
		done
	done
done
