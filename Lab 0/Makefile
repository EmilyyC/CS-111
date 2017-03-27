CC = gcc
CFLAGS = -g -o
.SILENT: check

all:
	$(CC) $(CFLAGS) lab0 lab0.c

check:
	# correctly implements --input and --output and returns 0
	echo "testing testing 123" > inputFile.txt
	./lab0 --input=inputFile.txt --output=outputFile.txt
	if [ $$? -eq 0 ] ; then \
		echo "Correct handling of --input and --output."; \
	else \
		echo "Failed handling of --input and --output."; \
	fi

	# check to see if correctly copies input to output
	diff inputFile.txt outputFile.txt
	if [ $$? -eq 0 ] ; then \
		echo "Correctly copied input to output."; \
	else \
		echo "Failed copying of input to output."; \
	fi 

clean:
	rm lab0 lab0-204564235.tar.gz inputFile.txt outputFile.txt

dist:
	tar -czvf lab0-204564235.tar.gz lab0.c Makefile nullptr.png backtrace.png README