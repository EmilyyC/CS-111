Karen Li, UID: 204564235
CS 111 - Lab 2A: Races and Synchronization
==========================================

Files Included
==============
lab2_add.c -- C program that implements and tests a shared variable add function, implements the (below) specified command line options, and produces the (below) specified output statistics.
SortedList.h -- header file (supplied by us) describing the interfaces for linked list operations.
SortedList.c -- C module that implements insert, delete, lookup, and length methods for a sorted doubly linked list (described in the provided header file, including correct placement of yield calls).
lab2_list.c -- C program that implements the (below) specified command line options and produces the (below) specified output statistics.
Makefile -- build the deliverable programs, output, graphs, and tarball.
profile.gperf -- execution profiling report showing where cycles are being spent for the spin-lock list test (1000 iterations, 12 threads).
lab_2b_list.csv -- containing your results for all of the performance tests.
lab2b.gp -- script that generates all of the graphs using gnuplot
lab2b_1.png -- throughput vs number of threads for mutex and spin-lock synchronized adds and list operations.
lab2b_2.png -- mean time per mutex wait and mean time per operation for mutex-synchronized list operations.
lab2b_3.png -- number of successful iterations for each synchronization method.
lab2b_4.png -- throughput vs number of threads for mutexes with partitioned lists.
lab2b_5.png -- throughput vs number of threads for spin-locks with partitioned lists.
README.txt -- descriptions of each of the included files, additional information, and answers to each of the questions.

Testing Methodology
===================
I performed various tests on lab2_add and lab2_list within my "tests" section in my Makefile, which generated the CSV data needed for the graphs and stored them into lab_2b_list.csv. In order to draw the graphs, I used a script called lab2b.gp.

profile.gperf contains my execution profiling report, showing where cycles are being spent for the spin-lock list test (1000 iterations, 12 threads).

Question Responses
==================
2.3.1) In the 1 and 2-thread tests for add, since we only have a few threads, most of the time will be spent in the add function itself because we will not have many threads trying to acquire the lock at the same time. Similarly, for the 1 and 2-thread tests for list, most of the cycles will be spent in the list operations. These are the most expensive parts of the code if you only have a few threads because since you won't be switching between many threads, you will just spend most of the time performing the actual useful work of the program.
For the high-thread spin-lock tests, most of the time will be spent spinning since we will have many threads trying to acquire one lock, and thus, they must spin wait until the lock is freed and they can acquire it to enter the critical section. 
For the high-thread mutex tests, most of the time will be spent making context switches when a thread finds that a lock is already held, and thus uses a system call to give up the CPU and go to sleep so another thread can run. Context switches between the kernel and user mode are expensive, so most of the cycles will be spent doing this between the different threads contending for a mutex.

2.3.2) When run with a large number of threads, the code to acquire the spin lock (in the function lockSpinLock()) consumes most of the cycles when the spin-lock version of the list exerciser is run with a large number of threads; in fact, approximately 94.4% of them to be exact according to profile.gperf. 
Acquiring the spin lock becomes really expensive with large numbers of threads because since only one thread can execute in the critical section, all of the other threads must spin wait until they can acquire the lock and enter the critical section. Since threads that are spin waiting continue to have control of the CPU and waste CPU time, then the lockSpinLock() operation becomes very time consuming and expensive.

2.3.3) The average lock-wait time rises dramatically with the number of contending threads because the more threads there are, the more likely they are to compete with each other in order to acquire the lock. Thus, more threads must wait while another thread is inside of the critical section, so the lock-wait time will rise dramatically since they will just be sleeping up the thread inside the critical section completes its work and wakes up the next thread in the queue.
The completion time per operation is just the time to complete the operation and doesn't include the time for all threads while they are waiting; completion time is just the wall time of the process and not the sum of the run times of all threads. Thus, the completion time per operation rises less dramatically with the number of contending threads because it does not include the time for all threads while they are waiting, which is where most of the time goes when the number of threads is high.
The wait time per operation can go up faster or higher than the completion time per operation because the wait time includes the time for all threads and is a sum of the run and wait times of all threads. Thus, since wait time includes the wait for lock time for all threads, wait time can be higher than the completion time, which does not include the wait time for all threads.

2.3.4) Since in my implementation the thread acquires the lock before performing any of the list operations for the linked list, then increasing the number of lists does not affect the amount of time that the threads spend waiting to acquire the lock. Thus, the throughput increases with the number of lists because a greater number of lists means that the lists will be shorter on average, and so the time required to perform the list operations will be smaller. For deletion, the run time is O(1), so this is unaffected by the number of lists. However, for insertion, length, and lookup, the run times are all O(n), so a large number of sublists means shorter sub lists overall and higher throughput. Using sublists does include the overhead of hashing to list element's key to decide which sublist the element is inside, but hashing is very cheap and has O(1) time.
Whether or not the throughput should continue increasing as we have even more lists depends on the CPU, number of iterations, and number of threads. If we have many iterations, then more sublists will allow throughput to continue to increase since we will be operating on shorter sublists. However, eventually we will have so many sublists that the sublists will always be very short and so there will be no more benefit in increasing the number of sublists. At this point, the throughput will no longer increase.
In the above curves, it does not appear to be true that the throughput of a N-way partitioned list should be equivalent to the throughput of a single list with fewer (1/N) threads. This is because the throughput depends on the numbers of competing threads and how long they each spend in the critical section. Thus, since the throughput for the N-way partitioned lists is higher than that of a single list with fewer threads, this indicates that even though the N-way partitioned lists have more threads, the threads spend much less time in the critical section, which allows them to have an overall higher throughput. 