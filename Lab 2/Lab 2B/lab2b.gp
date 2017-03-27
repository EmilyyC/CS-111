#! /usr/local/cs/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab_2b_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#
# output:
#	lab2b_1.png ... throughput vs number of threads for mutex and spin-lock synchronized adds and list operations.
#	lab2b_2.png ... mean time per mutex wait and mean time per operation for mutex-synchronized list operations.
#	lab2b_3.png ... number of successful iterations for each synchronization method.
#	lab2b_4.png ... throughput vs number of threads for mutexes with partitioned lists.
#	lab2b_5.png ... throughput vs number of threads for spin-locks with partitioned lists.
#
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep to select only the data they want.
#

# general plot parameters
set terminal png
set datafile separator ","

# GRAPH 1
# how many threads/iterations we can run without failure (w/o yielding)
set title "Lab2B-1: Throughput vs Number of Threads for Mutex and Spin-Lock Synchronization"
set xlabel "Number of Threads"
set logscale x 2
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_1.png'

# grep out the mutex synchronized adds, spin-lock synchronized adds, mutex-synchronized list operations, and spin-lock synchronized list operations
# to get the throughput, divide one Billion (number of nanoseconds in a second) by the time per operation (in nanoseconds).  
plot \
    "< grep add-m lab_2b_list.csv" using ($2):(1000000000/($6)) \
	title 'mutex synchronized adds' with points lc rgb 'green', \
     "< grep add-s lab_2b_list.csv" using ($2):(1000000000/($6)) \
	title 'spin-lock synchronized adds' with points lc rgb 'red', \
     "< grep list-none-m lab_2b_list.csv" using ($2):(1000000000/($7)) \
	title 'mutex synchronized list operations' with points lc rgb 'violet', \
     "< grep list-none-s lab_2b_list.csv" using ($2):(1000000000/($7)) \
	title 'spin-lock synchronized list operations' with points lc rgb 'blue'


# TODO THE SECOND TO FIFTH GRAPH

# GRAPH 2
set title "Lab2B-2: Wait-for-Lock Time and Average Time per Operation"
set xlabel "Number of threads"
set logscale x 2
set ylabel "Wait-for-Lock Time and Average Time per Operation"
set logscale y 10
set output 'lab2b_2.png'

# grep out the wait-for-lock time and the average time per operations for 1000 iterations and 1, 2, 4, 8, 16, 24 threads 
plot \
     "< grep list-none-m lab_2b_list.csv" using ($2):($7) \
	title 'average time per operation' with points lc rgb 'green', \
     "< grep list-none-m lab_2b_list.csv" using ($2):($8) \
	title 'average wait-for-mutex time' with points lc rgb 'blue'

# GRAPH 3
set title "Lab2B-3: Unprotected and Protected Iterations that Run Without Failure"
set xlabel "Number of Threads"
set logscale x 2
set ylabel "Successful Iterations"
set logscale y 10
set output 'lab2b_3.png'

# grep out the iterations with "--yield=id"
plot \
     "< grep list-id-none lab_2b_list.csv" using ($2):($3) \
	title 'unprotected (no synchronization)' with points lc rgb 'green', \
     "< grep list-id-s lab_2b_list.csv" using ($2):($3) \
	title 'spin lock synchronization' with points lc rgb 'blue', \
	 "< grep list-id-s lab_2b_list.csv" using ($2):($3) \
	title 'mutex synchronization' with points lc rgb 'red'

# GRAPH 4
set title "Lab2B-4: Throughput with Mutex Synchronization"
set xlabel "Number of Threads"
set logscale x 2
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_4.png'

# grep out the iterations containing "list-none-m"
plot \
     "< grep 'list-none-m,.*,1000,1,' lab_2b_list.csv" using ($2):(1000000000/($7)) \
	title '1 list' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,.*,1000,4,' lab_2b_list.csv" using ($2):(1000000000/($7)) \
	title '4 lists' with linespoints lc rgb 'green', \
     "< grep 'list-none-m,.*,1000,8,' lab_2b_list.csv" using ($2):(1000000000/($7)) \
	title '8 lists' with linespoints lc rgb 'blue', \
     "< grep 'list-none-m,.*,1000,16,' lab_2b_list.csv" using ($2):(1000000000/($7)) \
	title '16 lists' with linespoints lc rgb 'orange'

# GRAPH 5
set title "Lab2B-5: Throughput with Spin Lock Synchronization"
set xlabel "Number of Threads"
set logscale x 2
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_5.png'

# grep out the iterations containing "list-none-s"
plot \
     "< grep 'list-none-s,.*,1000,1,' lab_2b_list.csv" using ($2):(1000000000/($7)) \
	title '1 list' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,.*,1000,4,' lab_2b_list.csv" using ($2):(1000000000/($7)) \
	title '4 lists' with linespoints lc rgb 'green', \
     "< grep 'list-none-s,.*,1000,8,' lab_2b_list.csv" using ($2):(1000000000/($7)) \
	title '8 lists' with linespoints lc rgb 'blue', \
     "< grep 'list-none-s,.*,1000,16,' lab_2b_list.csv" using ($2):(1000000000/($7)) \
	title '16 lists' with linespoints lc rgb 'orange'