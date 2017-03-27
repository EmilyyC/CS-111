#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <errno.h>

void segFaultHandler() {
	fprintf(stderr, "ERROR: Segmentation fault caught by SIGSEGV handler.\n");
	exit(3);
}

int main(int argc, char **argv) {
	int c;
	int option_index;
	int isSegFault = 0;
	struct option long_options[] = 
		{
			{"input", required_argument, 0, 'a'},
			{"output", required_argument, 0, 'b'},
			{"segfault", no_argument, &isSegFault, 1},
			{"catch", no_argument, 0, 'c'},
			{0, 0, 0, 0}
		};

	// go through the command line arguments
	while (1) {
		c = getopt_long(argc, argv, "", long_options, &option_index);
		// end of options
		if (c == -1)
			break;
		// see which option was selected
		switch(c) {
			case 'a':
			{
				// use the specified file as standard input
				int ifd = open(optarg, O_RDONLY);
				if (ifd >= 0) {
					close(0);
					// 0 is a copy of ifd, so the input is redirected
					dup(ifd);
					// can close ifd because it is now redundant
					close(ifd);
				}
				// error occurred if open returns -1
				else {
					fprintf(stderr, "ERROR: Unable to open the specified input file.\n");
					perror(NULL);
					exit(1);
				}
				break;
			}
			case 'b':
			{
				// create the specified field and use it as standard output
				// all people have read and write privileges for the file
				int ofd = creat(optarg, 0666);
				// TODO: error check for close
				if (ofd >= 0) {
					close(1);
					// 1 is a copy of ofd, so the output is redirected
					dup(ofd);
					// can close ofd because it is now redundant
					close(ofd);
				}
				// error occured if creat returns -1
				else {
					fprintf(stderr, "ERROR: Unable to create the specified output file.\n");
					perror(NULL);
					exit(2);
				}
				break;
			}
			case 'c':
			{
				// register SIGSEGV handler to catch the segmentation fault
				signal(SIGSEGV, segFaultHandler);
				break;
			}
		}
	}

	// force a segmentation fault if necessary
	if (isSegFault == 1) {
		char* segFaultPtr = NULL;
		*segFaultPtr = 'a';
	}

	// copy standard input to standard output
	// read() from file descriptor 0 (until EOF) and write() to file descriptor 1
	// read() returns 0 when EOF
	char currChar;
	int readStatus;
	while (1) {
		readStatus = read(0, &currChar, sizeof(char));
		// error occurred, so exit with the error code
		if (readStatus == -1) {
			exit(errno);
		}
		// reached EOF
		else if (readStatus == 0) {
			break;
		}
		else {
			write(1, &currChar, sizeof(char));
		}
	}

	// no errors encountered
	exit(0);
}
