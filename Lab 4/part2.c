#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <mraa/aio.h>
#include <math.h>
#include <ctype.h>

#define BUFSIZE 1024

// GLOBAL VARIABLES 
// variables to set up socket and server connection
int sockfd, n;
int portno = 16000; 
// use this to reference the socket's elements
struct sockaddr_in serveraddr;
// keep information related to host
struct hostent *server;
char *hostname = "r01.cs.ucla.edu";
char buf[BUFSIZE];

// variables needed to read temperature
mraa_aio_context tempSensor;
char* UID = "204564235";  // "device identifier" AKA my UID

// variables needed to get the info for the time
time_t timer;
char timeBuffer[9];
struct tm* timeInfo; 

// variables that can be changed by commands from the server
int sleepTime = 3;
int isCelsius = 0;  // by default use fahrenheit
int isStopped = 0;  

// open output file
FILE* outputFile;

// wrapper for perror
void error(char *msg) {
	perror(msg);
	exit(1);
}

// continously print out the temperature
void printTemperature() {
	// constants
	const int B = 4275;  // B value of the thermistor

	// declare temperature sensor as an analog I/O context
	tempSensor = mraa_aio_init(0);	
	int rawTemperature;

	// loop forever to continuously read in the temperature
	while(1) {
		// read the temperature sensor value once per second
		rawTemperature = mraa_aio_read(tempSensor);
		// calculate the temperature (in Celsius)
		double R = 1023.0/((double)rawTemperature) - 1.0;
		R = 100000.0*R;
		double celsius  = 1.0/(log(R/100000.0)/B + 1/298.15) - 273.15;
		// convert from Celsius to Fahrenheit
		double fahrenheit = celsius * 9/5 + 32;

		// calculate the current time
		time(&timer);
		timeInfo = localtime(&timer);
		// store the time string in the buffer
		strftime(timeBuffer, 9, "%H:%M:%S", timeInfo);

		// put temperature into a char buffer
		char temperatureOutput[64];
		if (isCelsius) {
			if (snprintf(temperatureOutput, 64, "%s TEMP=%.1f\n", UID, celsius) < 0) {
				error("ERROR writing temperature to buffer");
			}
		}
		else {
			if (snprintf(temperatureOutput, 64, "%s TEMP=%.1f\n", UID, fahrenheit) < 0) {
				error("ERROR writing temperature to buffer");
			}
		}

		
		if (!isStopped) {
			// write to server 
			if (write(sockfd, temperatureOutput, strlen(temperatureOutput)+1) < 0) {
	  			error("ERROR writing device ID to server");
	  		}
	  		// print output to log file
			if (isCelsius)
				fprintf(outputFile, "%s %.1f\n", timeBuffer, celsius);
			else
				fprintf(outputFile, "%s %.1f\n", timeBuffer, fahrenheit);
	  		// make sure output is printed to the log file
			fflush(outputFile);
  		}

		sleep(sleepTime);
	}
}

int main(int argc, char **argv) {
	// SET UP THE TCP CONNECTION TO THE REMOTE SERVER
	// create the socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("ERROR opening socket");
		exit(1);
	}

	// get the server's DNS entry
	server = gethostbyname(hostname);
	if (server == NULL) {
		fprintf(stderr, "ERROR no such as %s\n", hostname);
		exit(1);
	}

	// build the server's Internet address
	// clear the struct to all 0s to avoid junk results
	memset((char *) &serveraddr, 0, sizeof(serveraddr));
	// set the address family (usually AF_INET for Internet-based apps)
	serveraddr.sin_family = AF_INET;
	// get the IP address of the server
	memcpy((char *)&serveraddr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
	// converts and stores port number to the right format (ie. Endianess) so we can use it
	serveraddr.sin_port = htons(portno);

	// create a connection with the server 
    if (connect(sockfd, (struct sockaddr*) &serveraddr, sizeof(serveraddr)) < 0) {
    	error("ERROR connecting");
    	exit(1);
    }

 	// ACTUAL CODE BEGINS HERE
    // open the output file
    outputFile = fopen("lab4_2.log", "w");

  	// write my UID as the device identifier
  	if (write(sockfd, UID, strlen(UID)+1) < 0) {
  		error("ERROR writing device ID to server");
  	}

  	// use new thread to write the temperature continuously to the server
  	pthread_t temperatureThread;
  	if (pthread_create(&temperatureThread, NULL, (void *)printTemperature, NULL) < 0) {
  		error("ERROR cannot create thread");
  		exit(1);
  	}

  	// need main thread to read in the commands from the server and execute them
  	while (1) {
  		// buffer to hold commands from the server
  		char commandBuffer[1024] = {0};
  		// read in the commands with read()
  		// read returns the number of bytes it has read in (positive = command has been read in)
  		if (read(sockfd, commandBuffer, 1024) > 0) {
  			if (strcmp(commandBuffer, "OFF") == 0) {
		  		fprintf(outputFile, "OFF\n");
		  		printf("OFF\n");
  				fflush(outputFile);
		  		// turn the device off
		  		exit(0);
  			}
  			else if (strcmp(commandBuffer, "STOP") == 0) {
  				// stop the device from logging the temperature
  				isStopped = 1;
  				fprintf(outputFile, "STOP\n");
  				printf("STOP\n");
  				fflush(outputFile);
  			}
  			else if (strcmp(commandBuffer, "START") == 0) {
  				// start logging the temperature
  				isStopped = 0;
  				fprintf(outputFile, "START\n");
  				printf("START\n");
  				fflush(outputFile);
  			}
  			else if (strcmp(commandBuffer, "SCALE=F") == 0) {
  				isCelsius = 0;
  				fprintf(outputFile, "SCALE=F\n");
  				printf("SCALE=F\n");
  				fflush(outputFile);
  			}
  			else if (strcmp(commandBuffer, "SCALE=C") == 0) {
  				isCelsius = 1;
  				fprintf(outputFile, "SCALE=C\n");
  				printf("SCALE=C\n");
  				fflush(outputFile);
  			}
  			else if (strcmp(commandBuffer, "DISP Y") == 0 || strcmp(commandBuffer, "DISP N") == 0) {
  				// LOL not gonna do the extra credit rip
  				continue;
  			}
  			else {
  				// check if is the period command
  				// check if the first 7 chars are "PERIOD="
  				char subBuff[7+1];
				memcpy(subBuff, &commandBuffer[0], 7);
				subBuff[7] = '\0';
				int isValidPeriod = 1;  // assume that the period is valid

				if (strcmp(subBuff, "PERIOD=") == 0) {
					// check for the value of the period is valid number
					char periodBuff[1024-7+1];
					memcpy(periodBuff, &commandBuffer[7], 1024-7);
					periodBuff[1024-7] = '\0';
					int i = 0;
					while (periodBuff[i] != '\0') {
						// check for non-numbers and also negative numbers
						if (!isdigit(periodBuff[i])) {
							isValidPeriod = 0;
							break;
						}
						i++;
					}
					// period is valid so get its value
					if (isValidPeriod) {
						// convert the string to int
						int periodTime = atoi(periodBuff);
						// invalid period range so break and print invalid input
						if (periodTime > 3600 || periodTime < 1) {
							isValidPeriod = 0;
						}
						else {
							sleepTime = periodTime;
							// since valid sleep time, then print the command to output
							fprintf(outputFile, "PERIOD=%d\n", sleepTime);
							printf("PERIOD=%d\n", sleepTime);
	  						fflush(outputFile);
							// move onto the next command 
							continue;
						}
					}
				}
				// if invalid command passed in
				if (!isValidPeriod) {
					// print out the invalid command
					fprintf(outputFile, "%s I\n", commandBuffer);
					printf("%s I\n", commandBuffer);
					fflush(outputFile);
				}
  			}
  		}
  	}

  	// close the temperature sensor
  	mraa_aio_close(tempSensor);
  	// close the socket
  	close(sockfd);

    return 0;
}
