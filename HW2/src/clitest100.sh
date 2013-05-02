#!/bin/bash

server=uw1-320-11.uwb.edu

for (( i = 1 ; i <= 5 ; i++ )) do

	echo 2 | ./hw2 $server > ./data/100mbpsStW$i.dat

	sleep 10	

	echo 3 | ./hw2 $server > ./data/100mbpsSW$i.dat

	sleep 10

done

