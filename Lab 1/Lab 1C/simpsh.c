#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

int max(int a, int b) {
	return (a > b ? a : b);
}

void signalHandler(int n) {
	fprintf(stderr, "%d caught", n);
	exit(n);
}

int main(int argc, char** argv) {
	int c;
	int optionIndex;
	int verboseFlag = 0;
	int rdonlyFlag = 0;  // O_RDONLY = 0x0000
	int wronlyFlag = 0;  // O_WRONLY = 0x0001
	int rdwrFlag = 0; // O_RDWR = 0x0002
	int fileFlags = 0;   // OR all of the different flags into this variable for each switch case
	int profileFlag = 0;
	int exitStatus = 0;  // must update this to reflect if all commands exited correctly or not
	int* fileDescriptors = NULL;  // will definitely have at least 3 open files
	int numOpenFiles = 0;
	pid_t* processIDs = NULL;
	char** processCommandArgs = NULL;
	int numProcesses = 0;

	struct rusage usage; 	// need to use for --profile 
	double startUserTime, endUserTime, userTime;
	double startSystemTime, endSystemTime, systemTime;

	struct option longOptions[] = 
	{
		{"append", no_argument, 0, 1},
		{"cloexec", no_argument, 0, 2},
		{"creat", no_argument, 0, 3},
		{"directory", no_argument, 0, 4},
		{"dsync", no_argument, 0, 5},
		{"excl", no_argument, 0, 6},
		{"nofollow", no_argument, 0, 7},
		{"nonblock", no_argument, 0, 8},
		{"rsync", no_argument, 0, 9},
		{"sync", no_argument, 0, 10},
		{"trunc", no_argument, 0, 11},
		{"rdonly", required_argument, &rdonlyFlag, 1},
		{"wronly", required_argument, &wronlyFlag, O_WRONLY},
		{"rdwr", required_argument, &rdwrFlag, O_RDWR},
		{"pipe", no_argument, 0, 12},  // TODO
		{"command", required_argument, 0, 13},
		{"wait", no_argument, 0, 14},  
		{"close", required_argument, 0, 15},  
		{"verbose", no_argument, &verboseFlag, 1},
		{"profile", no_argument, 0, 16},  // TODO
		{"abort", no_argument, 0, 17}, 
		{"catch", required_argument, 0, 18},
		{"ignore", required_argument, 0, 19},  
		{"default", required_argument, 0, 20},  
		{"pause", no_argument, 0, 21},  
		{0, 0, 0, 0}
	};

	// go through the command line arguments 
	while(1) {
		c = getopt_long(argc, argv, "", longOptions, &optionIndex);
		
		// end of options
		if (c == -1) 
			break;

		// see which option was selected
		switch(c) {
			// --append
			case 1:
			{
				if (verboseFlag) 
					printf("--append\n");
				fileFlags = fileFlags | O_APPEND;
				break;
			}
			// --cloexec
			case 2:
			{
				if (verboseFlag) 
					printf("--cloexec\n");
				fileFlags = fileFlags | O_CLOEXEC;
				break;
			}
			// --creat
			case 3:
			{
				if (verboseFlag) 
					printf("--creat\n");
				fileFlags = fileFlags | O_CREAT;
				break;
			}
			// --directory
			case 4:
			{
				if (verboseFlag) 
					printf("--directory\n");
				fileFlags = fileFlags | O_DIRECTORY;
				break;
			}
			// --dsync
			case 5:
			{
				if (verboseFlag) 
					printf("--dsync\n");
				fileFlags = fileFlags | O_DSYNC;
				break;
			}
			// --excl
			case 6:
			{
				if (verboseFlag) 
					printf("--excl\n");
				fileFlags = fileFlags | O_EXCL;
				break;
			}
			// --nofollow
			case 7:
			{
				if (verboseFlag) 
					printf("--nofollow\n");
				fileFlags = fileFlags | O_NOFOLLOW;
				break;
			}
			// --nonblock
			case 8:
			{
				if (verboseFlag) 
					printf("--nonblock\n");
				fileFlags = fileFlags | O_NONBLOCK;
				break;
			}
			// --rsync
			case 9:
			{
				if (verboseFlag) 
					printf("--rsync\n");
				fileFlags = fileFlags | O_RSYNC;
				break;
			}
			// --sync
			case 10:
			{
				if (verboseFlag) 
					printf("--sync\n");
				fileFlags = fileFlags | O_SYNC;
				break;
			}			
			// --trunc
			case 11:
			{
				if (verboseFlag) 
					printf("--trunc\n");
				fileFlags = fileFlags | O_TRUNC;
				break;
			}
			// --pipe
			case 12:
			{
				// get the start time
				if (profileFlag) {
					// TODO: error checking for return value
					int returnVal = getrusage(RUSAGE_SELF, &usage);
					startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
				}

				if (verboseFlag) 
					printf("--pipe\n");
				// create the pipes
				int pipeFileDescriptors[2];
				int pipeCreation = pipe(pipeFileDescriptors);
				// allocate more space for the pipes
				fileDescriptors = realloc(fileDescriptors, (numOpenFiles+2)*sizeof(int));
				fileDescriptors[numOpenFiles] = pipeFileDescriptors[0];
				fileDescriptors[numOpenFiles+1] = pipeFileDescriptors[1];
				// increment the number of open files 
				numOpenFiles += 2;

				// get the end time
				if (profileFlag) {
					int returnVal = getrusage(RUSAGE_SELF, &usage);
					endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
					// calculate the difference between start and end time
					userTime = endUserTime - startUserTime;
					systemTime = endSystemTime - startSystemTime;
					printf("Usage: (user) %fs | (system) %fs \n", userTime, systemTime);
				}
				break;
			}
			// --command i o e cmd args
			case 13:
			{
				// get the start time
				if (profileFlag) {
					// TODO: error checking for return value
					int returnVal = getrusage(RUSAGE_SELF, &usage);
					startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
				}

				// optind contains the index of where getopt_long() thinks the next option is (current index in argv + 2), so need to decrement optind to get the index of the first command argument
				int argIndex = optind - 1;

				// save the standard input i, standard output o, and standard error e into an array
				int ioFileDescriptors[3];
				int i = 0;
				for ( ; i < 3; i++) {
					ioFileDescriptors[i] = atoi(argv[argIndex++]);  
					// check to see if the file descriptor is valid
					if ((ioFileDescriptors[i] >= numOpenFiles) || (fileDescriptors[ioFileDescriptors[i]] == -1)) {
						fprintf(stderr, "Invalid file descriptor value specified.\n");
					}
				}

				// first, count the number of arguments for the command (including the command name)
				int numCommandArgs = 0;
				int j = argIndex;
				for ( ; j < argc; j++) {
					// check to see if at the end of the 
					if(argv[j][0] == '-' && argv[j][1] == '-') {
						break;
					}
					numCommandArgs++;
				}

				// save the rest of the arguments into an array of pointers to C-strings
				char* commandArgs[numCommandArgs+1];
				int k = 0;
				for ( ; k < numCommandArgs; k++) {
					commandArgs[k] = argv[argIndex++];
				}

				// must terminate list of arguments with NULL pointer to run execvp() later
				commandArgs[numCommandArgs] = (char*) NULL;

				// create a single string out of the command 
				int l = 0;
				char* commandString = NULL;
				int numChars = 0;
				for ( ; l < numCommandArgs; l++) {
					int j = 0;
					while(commandArgs[l][j] != '\0') {
						commandString = realloc(commandString, (numChars+1)*sizeof(char));
						commandString[numChars] = commandArgs[l][j++];
						numChars++;
					}
					// add a space between the arguments
					commandString = realloc(commandString, (numChars+1)*sizeof(char));
					commandString[numChars] = ' ';
					numChars++;
				}
				// make the last char a null-byte (since it is currently an extraneous space)
				commandString[numChars-1] = '\0';

				// must output option to standard output
				if (verboseFlag) {
					printf("--command %d %d %d ", ioFileDescriptors[0], ioFileDescriptors[1], ioFileDescriptors[2]);
					int i = 0;
					while(commandString[i] != '\0')
						printf("%c", commandString[i++]);
					printf("\n");
					// flush the buffer
					fflush(stdout);
				}

				// execute the command using a child process
				// allocate more space in the array to hold the command args
				processCommandArgs = realloc(processCommandArgs, (numProcesses+1)*sizeof(char*));
				processCommandArgs[numProcesses] = commandString;
				
				// allocate more space in the array to hold the process IDs
				processIDs = realloc(processIDs, (numProcesses+1)*sizeof(pid_t));
				processIDs[numProcesses] = fork();
				
				if (processIDs[numProcesses] == 0) {    // child process
					// need to do redirection for file descriptors 0, 1, 2 based on passed in arguments
					int i = 0;
					for ( ; i < 3; i++) {
						close(i);
						dup2(fileDescriptors[ioFileDescriptors[i]], i); 
						close(fileDescriptors[ioFileDescriptors[i]]);
					}

					// close all file descriptors from 3 and upwards
					for ( ; i < numOpenFiles; i++) {
						close(i+3); 
					}

					int execvpReturn = execvp(commandArgs[0], commandArgs);
					if (execvpReturn == -1) {
						printf("Error in calling execvp()!");
					}
				}
				else if (processIDs[numProcesses] > 0) {    // parent process
					numProcesses++;
					// get the end time
					if (profileFlag) {
					int returnVal = getrusage(RUSAGE_SELF, &usage);
					endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
					// calculate the difference between start and end time
					userTime = endUserTime - startUserTime;
					systemTime = endSystemTime - startSystemTime;
					printf("Usage: (user) %fs | (system) %fs \n", userTime, systemTime);
					}
				}
				else {    // error because fork() failed

				}
				break;
			}
			// --wait
			case 14: 
			{
				// get the start time
				if (profileFlag) {
					// TODO: error checking for return value
					int returnVal = getrusage(RUSAGE_SELF, &usage);
					startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
				}

				if (verboseFlag) 
					printf("--wait\n");

				// close all file descriptors
				// TODO
				int i = 0;
				for ( ; i < numOpenFiles; i++) {
					close(i+3);   // need to add 3 because we want to keep stdin (0), stdout (1), and stderr (2)
				}

				// we can assume that wait is the last option
				// keep looping until the all child processes have terminated (all array entries = 0)
				int numProcessesTerminated = 0;
				while (1) {
					int wstatus;
					// wait for any child process
					pid_t finishedProcess = waitpid(-1, &wstatus, 0);
					int childExitStatus = WEXITSTATUS(wstatus);
					
					// print out the child process's exit status
					printf("%d ", childExitStatus);

					// update the final exit status to be maximum of all children's exit status
					if (childExitStatus > exitStatus)
						exitStatus = childExitStatus;
					
					// print out the command for the child process
					// first, iterate through all the PIDs that we have saved to see which child process has just terminated (use the same index for the PID and the command args)
					int indexForArgs;
					int i = 0;
					for ( ; i < numProcesses; i++) {
						if (processIDs[i] == finishedProcess) {
							indexForArgs = i;
							break;
						}
					}
					// actually print out the command
					int j = 0;
					while (processCommandArgs[indexForArgs][j] != '\0') {
						printf("%c", processCommandArgs[indexForArgs][j++]);
					}
					printf("\n");

					// increment the number of terminated child processes
					numProcessesTerminated++;

					// check to see if all child processes have finished
					if (numProcessesTerminated == numProcesses) {
						break;
					}
				}

				// get the end time
				if (profileFlag) {
					int returnVal = getrusage(RUSAGE_SELF, &usage);
					endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
					// calculate the difference between start and end time
					userTime = endUserTime - startUserTime;
					systemTime = endSystemTime - startSystemTime;
					printf("Usage: (user) %fs | (system) %fs \n", userTime, systemTime);

					// also need to print out usage by children processes	
					returnVal = getrusage(RUSAGE_CHILDREN, &usage);
					endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
					// print out the total time used by the program
					printf("Children Usage: (user) %fs | (system) %fs \n", endUserTime, endSystemTime);
				}
				break;
			}
			// --close N
			case 15:
			{
				// get the start time
				if (profileFlag) {
					// TODO: error checking for return value
					int returnVal = getrusage(RUSAGE_SELF, &usage);
					startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
				}

				if (verboseFlag) {
					printf("--close ");
					int i = 0;
					while (optarg[i] != '\0')
						printf("%c", optarg[i++]);
					printf("\n");
				}
				// close the Nth file opened by a file-opening option
				close(fileDescriptors[atoi(optarg)]);
				// invalidate the file descriptor in the array
				fileDescriptors[atoi(optarg)] = -1;

				// get the end time
				if (profileFlag) {
					int returnVal = getrusage(RUSAGE_SELF, &usage);
					endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
					// calculate the difference between start and end time
					userTime = endUserTime - startUserTime;
					systemTime = endSystemTime - startSystemTime;
					printf("Usage: (user) %fs | (system) %fs \n", userTime, systemTime);
				}
				break;
			}
			// --profile
			case 16: 
			{
				if (verboseFlag) 
					printf("--profile\n");
				// set the profile flag
				profileFlag = 1;
				break;
			}
			// --abort
			case 17: 
			{
				// get the start time
				if (profileFlag) {
					// TODO: error checking for return value
					int returnVal = getrusage(RUSAGE_SELF, &usage);
					startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
				}

				if (verboseFlag) 
					printf("--abort\n");
				// generate a segmentation violation to crash the shell
				int* a = NULL;
				*a = 1;

				// get the end time
				if (profileFlag) {
					int returnVal = getrusage(RUSAGE_SELF, &usage);
					endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
					// calculate the difference between start and end time
					userTime = endUserTime - startUserTime;
					systemTime = endSystemTime - startSystemTime;
					printf("Usage: (user) %fs | (system) %fs \n", userTime, systemTime);
				}

				break;
			}
			// --catch N
			case 18:
			{
				// get the start time
				if (profileFlag) {
					// TODO: error checking for return value
					int returnVal = getrusage(RUSAGE_SELF, &usage);
					startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
				}

				if (verboseFlag) {
					printf("--catch ");
					int i = 0;
					while (optarg[i] != '\0')
						printf("%c", optarg[i++]);
					printf("\n");
				}
				// set the new signal handler 
				struct sigaction new_action;
				new_action.sa_handler = signalHandler;
				int signalNumber = atoi(optarg);
				sigaction(signalNumber, &new_action, NULL);

				// get the end time
				if (profileFlag) {
					int returnVal = getrusage(RUSAGE_SELF, &usage);
					endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
					// calculate the difference between start and end time
					userTime = endUserTime - startUserTime;
					systemTime = endSystemTime - startSystemTime;
					printf("Usage: (user) %fs | (system) %fs \n", userTime, systemTime);
				}
				break;
			}
			// --ignore N
			case 19:
			{
				// get the start time
				if (profileFlag) {
					// TODO: error checking for return value
					int returnVal = getrusage(RUSAGE_SELF, &usage);
					startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
				}

				if (verboseFlag) {
					printf("--ignore ");
					int i = 0;
					while (optarg[i] != '\0')
						printf("%c", optarg[i++]);
					printf("\n");
				}
				// set the new signal handler 
				struct sigaction new_action;
				new_action.sa_handler = SIG_IGN;
				int signalNumber = atoi(optarg);
				sigaction(signalNumber, &new_action, NULL);

				// get the end time
				if (profileFlag) {
					int returnVal = getrusage(RUSAGE_SELF, &usage);
					endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
					// calculate the difference between start and end time
					userTime = endUserTime - startUserTime;
					systemTime = endSystemTime - startSystemTime;
					printf("Usage: (user) %fs | (system) %fs \n", userTime, systemTime);
				}

				break;
			}
			// --default N
			case 20:
			{
				// get the start time
				if (profileFlag) {
					// TODO: error checking for return value
					int returnVal = getrusage(RUSAGE_SELF, &usage);
					startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
				}

				if (verboseFlag) {
					printf("--default ");
					int i = 0;
					while (optarg[i] != '\0')
						printf("%c", optarg[i++]);
					printf("\n");
				}
				// set the new signal handler 
				struct sigaction new_action;
				new_action.sa_handler = SIG_DFL;
				int signalNumber = atoi(optarg);
				sigaction(signalNumber, &new_action, NULL);

				// get the end time
				if (profileFlag) {
					int returnVal = getrusage(RUSAGE_SELF, &usage);
					endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
					// calculate the difference between start and end time
					userTime = endUserTime - startUserTime;
					systemTime = endSystemTime - startSystemTime;
					printf("Usage: (user) %fs | (system) %fs \n", userTime, systemTime);
				}
				break;
			}
			// --pause
			case 21: 
			{
				// get the start time
				if (profileFlag) {
					// TODO: error checking for return value
					int returnVal = getrusage(RUSAGE_SELF, &usage);
					startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
				}

				if (verboseFlag) 
					printf("--pause\n");
				pause();

				// get the end time
				if (profileFlag) {
					int returnVal = getrusage(RUSAGE_SELF, &usage);
					endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
					// calculate the difference between start and end time
					userTime = endUserTime - startUserTime;
					systemTime = endSystemTime - startSystemTime;
					printf("Usage: (user) %fs | (system) %fs \n", userTime, systemTime);
				}
				break;
			}

		}

		// flush the buffer
		fflush(stdout);

		// open file since file-opening option specified
		if (wronlyFlag || rdonlyFlag || rdwrFlag) {

			// get the start time
			if (profileFlag) {
				// TODO: error checking for return value
				int returnVal = getrusage(RUSAGE_SELF, &usage);
				startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
				startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
			}

			// must output option to stdin
			if (verboseFlag) {
				if (wronlyFlag) {
					printf("--wronly ");
				}
				else if (rdonlyFlag) {
					printf("--rdonly ");
				}
				else if (rdwrFlag) {
					printf("--rdwr ");
				}
				int i = 0;
				while (optarg[i] != '\0')
					printf("%c", optarg[i++]);
				printf("\n");
				fflush(stdout);
			}

			// adding more files, so must realloc() to get more room in the array
			fileDescriptors = realloc(fileDescriptors, (numOpenFiles+1)*sizeof(int));
			if (fileDescriptors == NULL) {
				// TODO
			}

			// open the file with the correct flags
			fileDescriptors[numOpenFiles] = open(optarg, (fileFlags | wronlyFlag | rdwrFlag), 0644);

			// error occurred if open returns -1
			if (fileDescriptors[numOpenFiles] == -1) {
				fprintf(stderr, "ERROR: Unable to open the specified input file.\n");
				perror(NULL);
				// must update exit status if currently 0
				if (exitStatus == 0) {
					// indicate there was an error opening the file
					exitStatus = 1;
				}
			}

			// increment the number of open files
			numOpenFiles++;

			// reset the file and permission flags for the next file to be created
			fileFlags = 0;
			rdonlyFlag = 0;
			rdwrFlag = 0;
			wronlyFlag = 0;

			// get the end time
			if (profileFlag) {
				int returnVal = getrusage(RUSAGE_SELF, &usage);
				endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
				endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
				// calculate the difference between start and end time
				userTime = endUserTime - startUserTime;
				systemTime = endSystemTime - startSystemTime;
				printf("Usage: (user) %fs | (system) %fs \n", userTime, systemTime);
			}
		}
	}

		// print out total time by calling getrusage
		if (profileFlag) {
			int returnVal = getrusage(RUSAGE_SELF, &usage);
			endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
			// print out the total time used by the program
			printf("Total Usage: (user) %fs | (system) %fs \n", endUserTime, endSystemTime);
		}

		exit(exitStatus);
}