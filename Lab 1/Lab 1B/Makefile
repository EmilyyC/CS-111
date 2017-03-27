CC = gcc
CFLAGS = -g -o
.SILENT: check

all:
	$(CC) $(CFLAGS) simpsh simpsh.c 

clean:
	rm simpsh lab1-KarenLi.tar.gz *.txt

check:
	# TEST CASE 1: the contents out input.txt should be copied to out.txt
	touch input1.txt input2.txt input3.txt out.txt err.txt foo.txt;
	echo "test case #1" > input1.txt; 
	./simpsh --rdonly input1.txt --wronly out.txt --wronly err.txt --ignore 2 --command 0 1 2 cat; 
	diff input1.txt out.txt; \
	if [ $$? -eq 0 ] ; then \
		echo "Test case #1 passed."; \
	else \
		echo "Test case #1 failed."; \
	fi ; \

	# TEST CASE 2: the contents of input.txt should be copied to foo.txt
	echo "test case #2" > input2.txt; 
	./simpsh --wronly out.txt --wronly err.txt --rdonly input2.txt --wronly foo.txt --pipe --command 2 3 1 cat; 
	diff input2.txt foo.txt; \
	if [ $$? -eq 0 ] ; then \
		echo "Test case #2 passed."; \
	else \
		echo "Test case #2 failed."; \
	fi ; \

	# TEST CASE 3: the contents of input.txt should be copied to err.txt
	echo "test case #3" > input3.txt; 
	./simpsh --wronly out.txt --wronly foo.txt --wronly err.txt --default 1 --command 1 2 0 cat input3.txt; 
	diff input3.txt err.txt; \
	if [ $$? -eq 0 ] ; then \
		echo "Test case #3 passed."; \
	else \
		echo "Test case #3 failed."; \
	fi 

dist:
	tar -czvf lab1-KarenLi.tar.gz simpsh.c Makefile README