#!/bin/bash

> time_and_interval.txt
gcc -pthread -o Thread Time_Interval_Thread.c
# timeout 10 ./Thread "1"

# X period
interval=1

#run 5 times
for i in {6..2}
do
	X_fake=$((10 ** $i))
    echo "$X_fake" > freq.txt
    echo "X frequency: $X_fake"

    # run program for (interval) seconds
    timeout $interval ./Thread "1"
    # sleep 10
done
