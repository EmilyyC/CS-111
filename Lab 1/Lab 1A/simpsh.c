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

int max(int a, int b) {
	return (a > b ? a : b);
}

int main(int argc, char** argv) {
	int c;
	int optionIndex;
	int verboseFlag = 0;
	int rdonlyFlag = 0;  // O_RDONLY = 0x0000
	int wronlyFlag = 0;  // O_WRONLY = 0x0001
	int fileFlags = 0;   // OR all of the different flags into this variable for each switch case
	int exitStatus = 0;  // must update this to reflect if all commands exited correctly or not
	int numOpenFiles = 0;

	struct option longOptions[] = 
	{
		{"rdonly", required_argument, &rdonlyFlag, 1},
		{"wronly", required_argument, &wronlyFlag, O_WRONLY},
		{"command", required_argument, 0, 16},
		{"verbose", no_argument, &verboseFlag, 1},
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
			// --command i o e cmd args
			case 16:
			{
				// optind contains the index of where getopt_long() thinks the next option is (current index in argv + 2), so need to decrement optind to get the index of the first command argument
				int argIndex = optind - 1;

				// save the standard input i, standard output o, and standard error e into an array
				int ioFileDescriptors[3];
				int i = 0;
				for ( ; i < 3; i++) {
					ioFileDescriptors[i] = atoi(argv[argIndex++]);  
					// check to see if the file descriptor is valid
					if (ioFileDescriptors[i] >= numOpenFiles) {
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

				// must output option to standard output
				if (verboseFlag) {
					printf("--command %d %d %d ", ioFileDescriptors[0], ioFileDescriptors[1], ioFileDescriptors[2]);
					int i = 0;
					for ( ; i < numCommandArgs; i++) {
						int j = 0;
						while(commandArgs[i][j] != '\0') {
							printf("%c", commandArgs[i][j++]);
						}
						printf(" ");
					}
					printf("\n");
					// flush the buffer
					fflush(stdout);
				}

				// execute the command using a child process
				pid_t child_pid = fork();
				if (child_pid == 0) {    // child process
					// need to do redirection for file descriptors 0, 1, 2 based on passed in arguments
					int i = 0;
					for ( ; i < 3; i++) {
						close(i);
						dup2(ioFileDescriptors[i]+3, i);  // must add 3 because the first file descriptor starts at file descriptor 3 (since 0, 1, and 2 reserved for stdin, stdout, and stderr)
						close(ioFileDescriptors[i]+3);
					}
					int execvpReturn = execvp(commandArgs[0], commandArgs);
					if (execvpReturn == -1) {
						printf("Error in calling execvp()!");
					}
				}
				else if (child_pid > 0) {    // parent process

				}
				else {    // error because fork() failed

				}

				break;
			}
		}

		// open file since file-opening option specified
		if (wronlyFlag || rdonlyFlag) {

			// must output option to stdin
			if (verboseFlag) {
				if (wronlyFlag) {
					printf("--wronly ");
				}
				else if (rdonlyFlag) {
					printf("--rdonly ");
				}
				int i = 0;
				while (optarg[i] != '\0')
					printf("%c", optarg[i++]);
				printf("\n");
				fflush(stdout);
			}

			// open the file with the correct flags
			int newFile = open(optarg, fileFlags | wronlyFlag);

			// error occurred if open returns -1
			if (newFile == -1) {
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
			wronlyFlag = 0;
			rdonlyFlag = 0;
		}
	}

	exit(exitStatus);
}
