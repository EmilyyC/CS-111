#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <sched.h>
#include <pthread.h>

// must be global so we can use inside of threadIterations()
struct threadIterationsArgs {   // argument for threadIterations()
	long long *counter;
	int numIterations;
	char *syncOption;
};

int opt_yield;  // specify whether threads should immediately yield
void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    if (opt_yield)
    	sched_yield();
    *pointer = sum;
}

// create global mutex lock to be used by all threads
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
// protected by a pthread_mutex
void mutexAdd(long long *pointer, long long value) {
	pthread_mutex_lock(&lock);  // acquire the lock 
	// start of critical section
	long long sum = *pointer + value;
	if (opt_yield)
    	sched_yield();
	*pointer = sum;
	// end of critical section
	pthread_mutex_unlock(&lock);  // release the lock
}

// the spin lock is open initially
int spinLock = 0;  // 0: unlocked, 1: locked
// lock the spin lock
void lockSpinLock(int *spinLock) {
	while(__sync_lock_test_and_set(spinLock, 1))
		;  // do nothing while the lock is held AKA spin wait
	// the function will return when the lock returns 0, so it is open, then you write 1 to it so you acquire it
}
// unlock the spin lock
void unlockSpinLock(int *spinLock) {
	__sync_lock_release(spinLock);   // will write 0 to the spinLock so that it is considered open to threads
}
// protected by a spin-lock
void spinLockAdd(long long *pointer, long long value) {
	lockSpinLock(&spinLock);
	// start of critical section
	long long sum = *pointer + value;
	if (opt_yield)
    	sched_yield();
	*pointer = sum;
	// end of critical section
	unlockSpinLock(&spinLock);
}

// performs the add using __sync_val_compare_and_swap 
void compareAndSwapAdd(long long *pointer, long long value) {
	int temp, temp2;
	do {
		temp = *pointer;
		if (opt_yield)
    		sched_yield();
		temp2 = temp + value;  // cannot use *counter because the value of *counter could have changed
	} while(__sync_val_compare_and_swap(pointer, temp, temp2) != temp); 
}

void *threadIterations(void *threadArgs) {
	// get the arguments needed out of the struct
	struct threadIterationsArgs *args = (struct threadIterationsArgs *) threadArgs; 
	int numIterations = args->numIterations;
	long long *counter = args->counter;
	char *syncOption = args->syncOption;
	// add 1 to the counter 
	int i;
	for (i = 0; i < numIterations; ++i) {
		if (syncOption == NULL)
			add(counter, 1);
		else if (*syncOption == 'm')
			mutexAdd(counter, 1);
		else if (*syncOption == 's')
			spinLockAdd(counter, 1);
		else if (*syncOption == 'c')
			compareAndSwapAdd(counter, 1);
	}
	// subtract 1 from the counter
	int j;
	for (j = 0; j < numIterations; ++j) {
		if (syncOption == NULL)
			add(counter, -1);
		else if (*syncOption == 'm')
			mutexAdd(counter, -1);
		else if (*syncOption == 's')
			spinLockAdd(counter, -1);
		else if (*syncOption == 'c')
			compareAndSwapAdd(counter, -1);
	}
	// exit to re-join the parent thread
	pthread_exit(NULL);
}

int main(int argc, char **argv) {
	int c; 
	int option_index;
	long long counter = 0;   // counter for the threads; should be 0 if no race conditions
	pthread_t *threads;
	int numThreads = 1;
	int numIterations = 1;
	struct timespec startTime;
	struct timespec endTime;
	char *syncOption = NULL;
	struct threadIterationsArgs args;
	int exitCode = 0;

	struct option longOptions[] = 
	{
		{"threads", optional_argument, 0, 1},
		{"iterations", optional_argument, 0, 2},
		{"yield", no_argument, &opt_yield, 1},
		{"sync", required_argument, 0, 3},
		{0, 0, 0, 0}
	};

	// go through the command line arguments
	while (1) {
		c = getopt_long(argc, argv, "", longOptions, &option_index);
		// end of options
		if (c == -1)
			break;
		// see which option was selected
		switch(c) {
			// --threads
			case 1:
			{
				// argument passed
				if (optarg != NULL) 
					numThreads = atoi(optarg);
				break;
			}
			// --iterations
			case 2:
			{
				// argument passed
				if (optarg != NULL) 
					numIterations = atoi(optarg);
				break;
			}
			// --sync
			case 3:
			{
				// set the --sync option
				if (optarg != NULL) {
					syncOption = malloc(sizeof(char));
					*syncOption = *optarg;
				}
				break;
			}
		}
	}

	// note (high resolution) starting time for the run 
	if (clock_gettime(CLOCK_MONOTONIC, &startTime)) {   // check to see if there was an error 
		perror("Error with clock_gettime()");
		exitCode = 1;
	}

	// initialize arguments for threadIterations
	args.counter = &counter;
	args.numIterations = numIterations;
	args.syncOption = syncOption;  // specify which type of add to perform

	// create specified number of threads and run add(+1) and add(-1) for each thread
	threads = (pthread_t *) malloc((sizeof(pthread_t)*numThreads));
	int t;
	for (t = 0; t < numThreads; ++t) {
		// check which version of the add function to run
		int ret = pthread_create(&threads[t], NULL, threadIterations, (void *) &args);
		if (ret != 0) {
			perror("Error with pthread_create()");
			exitCode = 1;
		}
	}

	// wait for all of the spawned threads to finish
	for (t = 0; t < numThreads; ++t) {
		int ret = pthread_join(threads[t], NULL);
		if (ret != 0) {
			perror("Error with pthread_join()");
			exitCode = 1;
		}
	}

	// note (high resolution) end time for the run
	if (clock_gettime(CLOCK_MONOTONIC, &endTime)) {   // check to see if there was an error 
		perror("Error with clock_gettime()");
		exitCode = 1;
	}

	// perform calculations to output statistics of the run
	long totalNumOps = numThreads * numIterations * 2;  // need to *2 since we add(+1) and add(-1)
	long runTime = ((long)endTime.tv_sec - (long)startTime.tv_sec)*1000000000 + ((long)endTime.tv_nsec - (long)startTime.tv_nsec);   // in nanoseconds
	long averageTimePerOp = runTime/totalNumOps;  // in nanoseconds

	// print statistics to stdout in CSV format
	// print the name of the test
	if (opt_yield) {
		if (syncOption != NULL)
			printf("add-yield-%c,", *syncOption);
		else
			printf("add-yield-none,");
	}
	else {
		if (syncOption != NULL)
			printf("add-%c,", *syncOption);
		else
			printf("add-none,");
	}
	printf("%d,%d,%d,%d,%d,%d\n", numThreads, numIterations, totalNumOps, runTime, averageTimePerOp, counter);

	exit(exitCode);
}