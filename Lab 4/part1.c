#include <stdio.h>
#include <unistd.h>
#include <mraa/aio.h>
#include <sys/time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

int main() {
	// constants
	const int B = 4275;  // B value of the thermistor

	// declare temperature sensor as an analog I/O context
	mraa_aio_context tempSensor;
	tempSensor = mraa_aio_init(0);	
	int rawTemperature;

	// variables needed to get the info for the time
	time_t timer;
	char timeBuffer[9];
	struct tm* timeInfo; 

	// open output file
	FILE* outputFile = fopen("lab4_1.log", "w");

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
	
		// print output to output file
		fprintf(outputFile, "%s %.1f\n", timeBuffer, fahrenheit);

		// print output to shell
		printf("%.1f\n", fahrenheit);

		// make sure output is printed to the log file and shell
		fflush(outputFile);
		fflush(stdout);
		sleep(1);
	}

	mraa_aio_close(tempSensor);
	
	return 0;
}
