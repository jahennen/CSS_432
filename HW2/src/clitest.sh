#!/bin/bash

for (( i = 1 ; i <= 5 ; i++ )) do

	echo 2 | ./hw2 uw1-320-11.uwb.edu > ./data/100mbpsStW$i.dat

	sleep 2	

	echo 3 | ./hw2 uw1-320-11.uwb.edu > ./data/100mbpsSW$i.dat

	sleep 2

done

