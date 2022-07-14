#!/bin/bash

# clear or remove data form file
> time_and_interval.txt
rm *offset_data_*

# Compile C program
gcc -pthread -o Thread Time_Interval_Thread.c

# get time with X period for (interval) seconds, can be changed for better surveying
interval=30

#run 5 times
for i in {6..2} 
do
	X_fake=$((10 ** $i))

    # > offset_data_$(X_fake).txt

    echo "$X_fake" > freq.txt
    echo "X frequency: $X_fake" >> time_and_interval.txt
    echo "X frequency: $X_fake" >> offset_data_$(X_fake).txt

    # run program for (interval) seconds
    timeout $interval ./Thread "$X_fake"
    # sleep 10
done
