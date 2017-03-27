#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include "SortedList.h"


// global variables
int opt_yield = 0;  // extern variable initially declared in SortedList.h, reset the yield flag at the beginning
// global variables that must be passed to the threads
int numThreads = 1;
int numIterations = 1;
int numLists = 1;  
char syncOption = 'a';
SortedList_t *head;  // head of the list
SortedList_t **subLists;  // each element of the array is the header pointer of one of the sublists
SortedListElement_t **allElements;  // elements we generated for the list
int numListElements;  // number of elements in the list
long* mutexAcquireTimes;  // how long it took all threads to acquire the mutex (for all threads)
int exitCode = 0;  // final exit code for the program

// djb2 by Dan Bernstein
// use as hash function for list element keys
// unsigned vs signed char?
unsigned int hash(const unsigned char *str)
{
    unsigned int hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

// used to generate random keys for the list elements
char* generateRandomKey() {
	// C-string containing all possible characters for our key
	char *alphanum = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuv123456789";
	int alphanumLength = 61;  // 26 + 26 + 10 = 62
	// allocate memory to hold our key
	int keyLength = 10;
	char *key = malloc(keyLength*sizeof(char));  // key will have 10 characters (9 real and 1 nullbyte at the end)
	// initialize each character for the key
	int i;
	for (i = 0; i < keyLength -1; ++i) {
		key[i] = alphanum[rand() % alphanumLength];
	}
	key[keyLength-1] = '\0';   // terminate the key with a nullbyte
	return key;
}

// global spin lock is open initially
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

// global mutex lock to be used by all threads
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *threadIterations(void *threadArg) {
	// get the argument needed 
	int threadID = * (int *) threadArg;  // cast the thread argument and dereference it

	if (syncOption == 'm') {
		// time before acquiring the lock
		struct timespec startTimeMutex;  // used for one thread at a time
		// note (high resolution) starting time for the run 
		if (clock_gettime(CLOCK_MONOTONIC, &startTimeMutex)) {   // check to see if there was an error 
			perror("Error with clock_gettime()");
			exitCode = 1;
		}

		pthread_mutex_lock(&lock);  // acquire the lock 

		// time after acquiring the lock
		struct timespec endTimeMutex;
		// note (high resolution) end time for the run
		if (clock_gettime(CLOCK_MONOTONIC, &endTimeMutex)) {   // check to see if there was an error 
			perror("Error with clock_gettime()");
			exitCode = 1;
		}

		// compute the lock acquisition time for this thread (nanoseconds)
		long mutexAcquireTime = ((long)endTimeMutex.tv_sec - (long)startTimeMutex.tv_sec)*1000000000 + ((long)endTimeMutex.tv_nsec - (long)startTimeMutex.tv_nsec);
		// store this in the array for lock acquisition times for all threads
		mutexAcquireTimes[threadID] = mutexAcquireTime;
	}

	if (syncOption == 's')
		lockSpinLock(&spinLock);  // acquire the spin lock

	// insert this thread's designated elements into the sub lists
	int i;
	for (i = threadID; i < numListElements; i += numThreads) {
		// use hash function to determine while sublist the element should go into
		unsigned int whichSubList = hash(allElements[i]->key) % numLists;
		// insert into the correct sublist
		SortedList_insert(subLists[whichSubList], allElements[i]);
	}

	// get the total list length by adding up the lengths of each of the sublists
	// don't need to worry about race conditions (ie. thread writing to element but reading before) because we acquired the lock so only the current thread can be executing 
	int listLength = 0;
	for (i = 0; i < numLists; i++) {
		// list is corrupted
		if (SortedList_length(subLists[i]) == -1) {
			perror("Error: Cannot get list length because list is corrupted.");
			exit(1);  
		}
		// list is fine, so increment the total list length
		else {
			listLength += SortedList_length(subLists[i]);
		}
	}

	// lookup and delete each of the keys previously inserted
	SortedListElement_t *currElement;
	for (i = threadID; i < numListElements; i += numThreads) {
		// look up the element by first finding which sublist it is in, and then calling SortedList_lookup()
		int whichSubList = hash(allElements[i]->key) % numLists;
		currElement = SortedList_lookup(subLists[whichSubList], allElements[i]->key);
		if (currElement == NULL) {
			perror("Error: Cannot find previously inserted list element");
			exit(1);   // there was an error when looking up the previously inserted element
		}
		// delete the element
		if (SortedList_delete(currElement) != 0) {  
			perror("Error: Cannot delete list element due to corrupted element pointers");
			exit(1);
		}
	}

	if (syncOption == 'm')
		pthread_mutex_unlock(&lock);  // release the lock 

	if (syncOption == 's') 
		unlockSpinLock(&spinLock);  // release the spin lock

	// exit to re-join the parent thread
	pthread_exit(NULL);
}

int main(int argc, char **argv) {
	int c; 
	int option_index;
	pthread_t *threads;
	struct timespec startTime;
	struct timespec endTime;

	struct option longOptions[] = 
	{
		{"threads", optional_argument, 0, 1},
		{"iterations", optional_argument, 0, 2},
		{"yield", required_argument, 0, 3},
		{"sync", required_argument, 0, 4},
		{"lists", required_argument, 0, 5},
		{0, 0, 0, 0}
	};

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
			// --yield 
			case 3: 
			{
				int i = 0;
				while(optarg[i] != '\0') {
					switch(optarg[i]) {
						case 'i':
							opt_yield |= INSERT_YIELD;
							break;
						case 'd':
							opt_yield |= DELETE_YIELD;
							break;
						case 'l':
							opt_yield |= LOOKUP_YIELD;
							break;
					}
					++i;
				}
				break;
			}
			// --sync
			case 4: 
			{
				// set the --sync option
				syncOption = *optarg;
				break;
			}
			// --lists
			case 5: 
			{
				numLists = atoi(optarg);
				break;
			}
		}
	}

	// create and initialize the required number of list elements
	numListElements = numThreads * numIterations;
	allElements = malloc(numListElements * sizeof(SortedListElement_t*));

	// initialize the list elements with random keys
	int j;
	for (j = 0; j < numListElements; ++j) {
		allElements[j] = malloc(sizeof(SortedListElement_t));
		allElements[j]->prev = NULL;
		allElements[j]->next = NULL;
		allElements[j]->key = generateRandomKey();
	}

	// allocate memory for the array of header pointers to sublists 
	subLists = malloc(numLists * sizeof(SortedList_t*));
	// initialize an empty list (make the dummy node) for each sublist
	for (j = 0; j < numLists; j++) {
		subLists[j] = malloc(sizeof(SortedList_t));
		subLists[j]->prev = subLists[j];
		subLists[j]->next = subLists[j];
		subLists[j]->key = NULL;
	}

	// allocate memory for an array to hold the mutex wait times for each thread
	mutexAcquireTimes = malloc(numThreads * sizeof(long));

	// note (high resolution) starting time for the run 
	if (clock_gettime(CLOCK_MONOTONIC, &startTime)) {   // check to see if there was an error 
		perror("Error with clock_gettime()");
		exitCode = 1;
	}

	// create the specified number of threads and have them insert/delete elements of the list
	threads = malloc((sizeof(pthread_t)*numThreads));
	// create an array to hold the ID's of each thread
	int* threadIDs = malloc(sizeof(int)*numThreads);
	int t;
	for (t = 0; t < numThreads; ++t) {
		// add the threadID to the array
		threadIDs[t] = t;
		int ret = pthread_create(&threads[t], NULL, threadIterations, (void *) (threadIDs+t));
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

	// check the length of the list to confirm that it's empty
	int i;
	for (i = 0; i < numLists; i++) {
		if (SortedList_length(subLists[i]) != 0) {
			perror("Error: The list is not empty.");
			exit(1);  
		}
	}

	// print the name of the test
	printf("list-");
	// yield options
	if (opt_yield & INSERT_YIELD)
		printf("i");
	if (opt_yield & DELETE_YIELD)
		printf("d");
	if (opt_yield & LOOKUP_YIELD)
		printf("l");
	if (opt_yield == 0)  // no yield options specified
		printf("none");
	// sync options
	printf("-");
	if (syncOption == 'a')   // did not specify syncOption so has default value of 'a'
		printf("none");
	else
		printf("%c", syncOption);
	printf(",");

	// perform calculations to output statistics of the run
	long numTotalOperations = numThreads * numIterations * 3;  // since each thread does insert(), lookup(), and delete()
	long runTime = ((long)endTime.tv_sec - (long)startTime.tv_sec)*1000000000 + ((long)endTime.tv_nsec - (long)startTime.tv_nsec);   // in nanoseconds
	long avgTimePerOperation = runTime / numTotalOperations;  // in nanoseconds
	// print the statistics 
	printf("%ld,%ld,%ld,%ld,%ld,%ld", numThreads, numIterations, numLists, numTotalOperations, runTime, avgTimePerOperation);
	// if used mutex synchronization, print additional column containing average wait-for-lock time
	// the total number of locking operations is equal to the number of threads since each thread acquires the lock once
	if (syncOption == 'm') {
		int i;
		long totalMutexAcquireTime = 0;
		// get the total time that the threads waited to acquire the mutex
		for (i = 0; i < numThreads; i++) {
			totalMutexAcquireTime += mutexAcquireTimes[i];
		}
		long averageMutexWait = totalMutexAcquireTime / numThreads;
		printf(",%ld", averageMutexWait);
	}
	// flush the buffer and print a newline
	printf("\n");

	exit(exitCode);
}