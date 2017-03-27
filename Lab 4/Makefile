CC = gcc
.SILENT = clean

build:
	$(CC) -lmraa -lm -o lab4_1 part1.c
	$(CC) -lmraa -lm -pthread -o lab4_2 part2.c
	$(CC) -lmraa -lssl -lcrypto -lm -pthread -o lab4_3 part3.c

dist:
	tar -czvf lab4-204564235.tar.gz part1.c part2.c part3.c lab4_1.log lab4_2.log lab4_3.log Makefile README

clean:
	rm lab4_1 lab4_2 lab4_3
