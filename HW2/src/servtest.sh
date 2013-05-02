#!/bin/bash

for (( i = 1 ; i <= 5 ; i++ )) do
	echo 2 | ./hw2

	sleep 2

	echo 3 | ./hw2
	
	sleep 2

done
