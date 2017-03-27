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
lab2_add.csv -- contains all of your results for all of the Part-1 tests.
lab2_list.csv -- contains all of your results for all of the Part-2 tests.
lab2_add-1.png -- threads and iterations required to generate a failure (with and without yields).
lab2_add-2.png -- average time per operation with and without yields.
lab2_add-3.png -- average time per (single threaded) operation vs. the number of iterations.
lab2_add-4.png -- threads and iterations that can run successfully with yields under each of the three synchronization methods.
lab2_add-5.png -- average time per (multi-threaded) operation vs. the number of threads, for all four versions of the add function.
lab2_list-1.png -- average time per (single threaded) unprotected operation vs. number of iterations (illustrating the correction of the per-operation cost for the list length).
lab2_list-2.png -- threads and iterations required to generate a failure (with and without yields).
lab2_list-3.png -- iterations that can run (protected) without failure.
lab2_list-4.png -- (corrected) average time per operation (for unprotected, mutex, and spin-lock) vs. number of threads.
README.txt -- contains descriptions of included files and answers to lab qeustions.
lab2_add.gp -- script that generates graphs for lab2_add using gnuplot
lab2_list.gp -- script that generates graphs for lab2_list using gnuplot

Testing Methodology
===================
I performed various tests on lab2_add and lab2_list within my "tests" section in my Makefile, which generated the CSV data needed for the graphs. In order to draw the graphs, I used two scripts provided in the spec: lab2_add.gp and lab2_list.gp. 

Question Responses
==================
2.1.1) It started to consistently make errors with 2 threads and 10000 iterations. It takes many iterations before errors are seen because because the time cost for creating a thread is very expensive, so if you are only using a small number of iterations, then one thread might finish running before the second thread is even created. Thus, the threads will not be running together at the same time, so there's no race condition since the multiple threads are not interacting. This means that for small numbers of iterations the value of the counter will likely be 0 (the correct value) since there will be little possiblity of race conditions. On the other hand, for larger number of iterations, more threads are running at the same time so there is a greater chance of race conditions due to them accessing and writing to the same counter variable simultaneously, so the value of the counter will likely be incorrect and not equal to 0.

2.1.2) The --yield runs are slower because when you call sched_yield(), you have a context switch that goes into kernel mode and put your thread into the waiting queue, so this adds a lot of time to the final runtime of your thread. Since multiple yield functions may run at the same time, it's not possible to get valid per-operation timings because we are collecting the wall time and we can't get the portion of the wall time that is devoted to yield and the part that isn't.

2.1.3) The time cost to start a new thread is very expensive, but over many iterations, the cost of creating the new thread will be amortized over each iteration. Thus, the average cost per optation drops with increasing iterations. Since the cost per iteration is a function of the number of iterations, in order to figure out how many iterations to run, keep running the iterations until the line is no longer sloping down and is just stable; then you know that the cost of creating a thread is amortized to each iteration and you can get the true time cost for each iteration.

2.1.4) The options all perform similarly for low numbers of threads becausee there's only a few threads so they will likely be able to succeed and acquire the lock right away. As the number of threads rises, the three protected operations slow down because it is more likely that some other thread will hold the lock. Thus, a thread will need to wait until the other thread gives up the lock before it can acquire the lock, slowing down the protected operations. Spinlocks are expensive for large number of threads because if a thread cannot acquire the lock, the thread continues polling and continues to use the CPU resource the whole time while waiting for the lock.

2.2.1) For mutex-protected operations in Part-1 and Part-2, the correct times per operation appear to be much lower for the linked list operations. This is because we are dividing by the cost of synchronization (which is once per list operation) by the number of elements in the list, which we should not be doing. After making this correction, we still see that mutex synchronization goes up more quickly for lists than for adds. This is because the linked list locks are held much longer than the corresponding add locks, thus increasing the probability of conflict and having to pay the cost of blocking a thread.

2.2.2) For the mutex, the same issues discussed above in #2.2.1 still apply. Moreover, for few number of threads, the costs of spin-locks are almost the same as that of mutexes. However, as the number of threads increases, the costs of the spin-locks grows greater than that of the mutexes. This is due to the fact that the more threads there are, the more likely the threads will be competing for the same lock. Since the spin lock causes the thread to just spin and waste CPU cycles while waiting to acquire the lock, the spin lock's costs is greater than that of the mutex, which puts the thread to sleep so the CPU can do useful work while the thread is waiting for the lock.
