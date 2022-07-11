# Thread and Time project

open folder containing "Thread_Time.sh" and "Time_Interval_Thread.c"

run ./Thread_Time.sh

Time and offset data will be saved to time_and_interval.txt file

plotting process using offset_data.txt and plot_offset.plt need further developed

- - - -
#### 1. Time_Interval_Thread.c ####

C program that create 3 thread Sample, Input and Logging described in instructions.

- - - -
#### 2. Thread_Time.sh ####

Bash scripts which
- Compile Time_Interval_Thread.c program
- Change the X value in freq.txt every x seconds (x can be modified)

- - - -
#### 3. freq.txt ####

Store frequency value X (ns)

- - - -
#### 4. time_and_interval.txt ####

Store date, time and offset value to previous captured time

- - - -
#### 5. offset_data.txt and plot_offset.plt (uncompleted) ####

offset_data.txt store time offset value help further ploting proccess

plot_offset.plt is a bash scripts using gnuplot to plot the offset data above
