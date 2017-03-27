CC = gcc
CFLAGS = -g -o
.SILENT: clean

build: 
	$(CC) $(CFLAGS) lab3a lab3a.c

check:
	diff super.csv solutions/super.csv
	diff group.csv solutions/group.csv
	diff bitmap.csv solutions/bitmap.csv
	diff inode.csv solutions/inode.csv
	diff directory.csv solutions/directory.csv
	diff indirect.csv solutions/indirect.csv

clean: 
	rm *.csv lab3a lab3a-204564235.tar.gz

dist:
	tar -czvf lab3a-204564235.tar.gz lab3a.c Makefile README
