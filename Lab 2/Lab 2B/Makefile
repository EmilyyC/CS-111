CC = gcc
CFLAGS = -g -lpthread -o
.SILENT: clean 

build:
	$(CC) $(CFLAGS) lab2_add lab2_add.c 
	$(CC) $(CFLAGS) lab2_list lab2_list.c SortedList.c

tests:
	# range of threads and iterations to see what it takes to cause a failure
	./lab2_add --threads=2  --iterations=100    >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=1000   >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=10000  >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=100000 >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=100    >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=1000   >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=10000  >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=100000 >> lab_2b_list.csv
	./lab2_add --threads=8  --iterations=100    >> lab_2b_list.csv
	./lab2_add --threads=8  --iterations=1000   >> lab_2b_list.csv
	./lab2_add --threads=8  --iterations=10000  >> lab_2b_list.csv
	./lab2_add --threads=8  --iterations=100000 >> lab_2b_list.csv
	./lab2_add --threads=12 --iterations=100    >> lab_2b_list.csv
	./lab2_add --threads=12 --iterations=1000   >> lab_2b_list.csv
	./lab2_add --threads=12 --iterations=10000  >> lab_2b_list.csv
	./lab2_add --threads=12 --iterations=100000  >> lab_2b_list.csv
	# range of threads and iterations to cause failure with yield
	./lab2_add --threads=2  --iterations=10     --yield >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=10     --yield >> lab_2b_list.csv
	./lab2_add --threads=8  --iterations=10     --yield >> lab_2b_list.csv
	./lab2_add --threads=12 --iterations=10     --yield >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=20     --yield >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=20     --yield >> lab_2b_list.csv
	./lab2_add --threads=8  --iterations=20     --yield >> lab_2b_list.csv
	./lab2_add --threads=12 --iterations=20     --yield >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=40     --yield >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=40     --yield >> lab_2b_list.csv
	./lab2_add --threads=8  --iterations=40     --yield >> lab_2b_list.csv
	./lab2_add --threads=12 --iterations=40     --yield >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=80     --yield >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=80     --yield >> lab_2b_list.csv
	./lab2_add --threads=8  --iterations=80     --yield >> lab_2b_list.csv
	./lab2_add --threads=12 --iterations=80     --yield >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=100    --yield >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=100    --yield >> lab_2b_list.csv
	./lab2_add --threads=8  --iterations=100    --yield >> lab_2b_list.csv
	./lab2_add --threads=12 --iterations=100    --yield >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=1000   --yield >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=1000   --yield >> lab_2b_list.csv
	./lab2_add --threads=8  --iterations=1000   --yield >> lab_2b_list.csv
	./lab2_add --threads=12 --iterations=1000   --yield >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=10000  --yield >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=10000  --yield >> lab_2b_list.csv
	./lab2_add --threads=8  --iterations=10000  --yield >> lab_2b_list.csv
	./lab2_add --threads=12 --iterations=10000  --yield >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=100000 --yield >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=100000 --yield >> lab_2b_list.csv
	./lab2_add --threads=8  --iterations=100000 --yield >> lab_2b_list.csv
	./lab2_add --threads=12 --iterations=100000 --yield >> lab_2b_list.csv
	# look at cost per operation vs number of iterations
	./lab2_add --threads=1  --iterations=100         >> lab_2b_list.csv
	./lab2_add --threads=1  --iterations=1000        >> lab_2b_list.csv
	./lab2_add --threads=1  --iterations=10000       >> lab_2b_list.csv
	./lab2_add --threads=1  --iterations=100000      >> lab_2b_list.csv
	./lab2_add --threads=1  --iterations=1000000     >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=100         >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=1000        >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=10000       >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=100000      >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=1000000     >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=100         >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=1000        >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=10000       >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=100000      >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=1000000     >> lab_2b_list.csv
	./lab2_add --threads=1  --iterations=100     --yield >> lab_2b_list.csv
	./lab2_add --threads=1  --iterations=1000    --yield >> lab_2b_list.csv
	./lab2_add --threads=1  --iterations=10000   --yield >> lab_2b_list.csv
	./lab2_add --threads=1  --iterations=100000  --yield >> lab_2b_list.csv
	./lab2_add --threads=1  --iterations=1000000 --yield >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=100     --yield >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=1000    --yield >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=10000   --yield >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=100000  --yield >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=1000000 --yield >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=100     --yield >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=1000    --yield >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=10000   --yield >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=100000  --yield >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=1000000 --yield >> lab_2b_list.csv
	# demonstrate the efficacy of each of the protection mechanisms
	./lab2_add --threads=2  --iterations=10000 --yield --sync=m >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=10000 --yield --sync=m >> lab_2b_list.csv
	./lab2_add --threads=8  --iterations=10000 --yield --sync=m >> lab_2b_list.csv
	./lab2_add --threads=12 --iterations=10000 --yield --sync=m >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=10000 --yield --sync=c >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=10000 --yield --sync=c >> lab_2b_list.csv
	./lab2_add --threads=8  --iterations=10000 --yield --sync=c >> lab_2b_list.csv
	./lab2_add --threads=12 --iterations=10000 --yield --sync=c >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=10000 --yield --sync=s >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=10000 --yield --sync=s >> lab_2b_list.csv
	./lab2_add --threads=8  --iterations=1000  --yield --sync=s >> lab_2b_list.csv
	./lab2_add --threads=12 --iterations=1000  --yield --sync=s >> lab_2b_list.csv
	# generate time per operation numbers
	./lab2_add --threads=1  --iterations=10000 --sync=m >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=10000 --sync=m >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=10000 --sync=m >> lab_2b_list.csv
	./lab2_add --threads=8  --iterations=10000 --sync=m >> lab_2b_list.csv
	./lab2_add --threads=12 --iterations=10000 --sync=m >> lab_2b_list.csv
	./lab2_add --threads=1  --iterations=10000 --sync=c >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=10000 --sync=c >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=10000 --sync=c >> lab_2b_list.csv
	./lab2_add --threads=8  --iterations=10000 --sync=c >> lab_2b_list.csv
	./lab2_add --threads=12 --iterations=10000 --sync=c >> lab_2b_list.csv
	./lab2_add --threads=1  --iterations=10000 --sync=s >> lab_2b_list.csv
	./lab2_add --threads=2  --iterations=10000 --sync=s >> lab_2b_list.csv
	./lab2_add --threads=4  --iterations=10000 --sync=s >> lab_2b_list.csv
	./lab2_add --threads=8  --iterations=10000 --sync=s >> lab_2b_list.csv
	./lab2_add --threads=12 --iterations=10000 --sync=s >> lab_2b_list.csv

	# generate time per operation vs number of iterations
	./lab2_list --threads=1  --iterations=10	      >> lab_2b_list.csv
	./lab2_list --threads=1  --iterations=100	      >> lab_2b_list.csv
	./lab2_list --threads=1  --iterations=1000	      >> lab_2b_list.csv
	./lab2_list --threads=1  --iterations=10000	      >> lab_2b_list.csv
	./lab2_list --threads=1  --iterations=20000	      >> lab_2b_list.csv
	# conflicts w/o yielding
	-./lab2_list --threads=2  --iterations=10             >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=100            >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=1000           >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=10             >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=100            >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=1000           >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=10             >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=100            >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=1000           >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=10             >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=100            >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=1000           >> lab_2b_list.csv
	# generate insert conflicts
	-./lab2_list --threads=2  --iterations=2   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=4   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=8   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=16  --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=32  --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=2   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=4   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=8   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=16  --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=32  --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=2   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=4   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=8   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=16  --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=32  --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=2   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=4   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=8   --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=16  --yield=i  >> lab_2b_list.csv
	-./lab2_list --threads=12  --iterations=32  --yield=i  >> lab_2b_list.csv
	# generate delete conflicts
	-./lab2_list --threads=2  --iterations=2   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=4   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=8   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=16  --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=32  --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=2   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=4   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=8   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=16  --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=32  --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=2   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=4   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=8   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=16  --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=32  --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=2   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=4   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=8   --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=16  --yield=d  >> lab_2b_list.csv
	-./lab2_list --threads=12  --iterations=32  --yield=d  >> lab_2b_list.csv
	# generate insert/lookup conflicts
	-./lab2_list --threads=2  --iterations=2   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=4   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=8   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=16  --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=32  --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=2   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=4   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=8   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=16  --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=32  --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=2   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=4   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=8   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=16  --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=32  --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=2   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=4   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=8   --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=16  --yield=il >> lab_2b_list.csv
	-./lab2_list --threads=12  --iterations=32  --yield=il >> lab_2b_list.csv
	# generate delete/lookup conflicts
	-./lab2_list --threads=2  --iterations=2   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=4   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=8   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=16  --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=2  --iterations=32  --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=2   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=4   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=8   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=16  --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=4  --iterations=32  --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=2   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=4   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=8   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=16  --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=8  --iterations=32  --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=2   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=4   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=8   --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=12 --iterations=16  --yield=dl >> lab_2b_list.csv
	-./lab2_list --threads=12  --iterations=32  --yield=dl >> lab_2b_list.csv
	# demonstrate the efficacy of mutex and spin-lock synchronization
	./lab2_list --threads=12 --iterations=32 --yield=i  --sync=m >> lab_2b_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=d  --sync=m >> lab_2b_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=il --sync=m >> lab_2b_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=dl --sync=m >> lab_2b_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=i  --sync=s >> lab_2b_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=d  --sync=s >> lab_2b_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=il --sync=s >> lab_2b_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=dl --sync=s >> lab_2b_list.csv
	# measure the performance
	./lab2_list --threads=1  --iterations=1000          >> lab_2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab_2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s >> lab_2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s >> lab_2b_list.csv
	# check correctness of sublist implementation
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=1 >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=2 >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=4 >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=8 >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=16 >> lab_2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=4 --iterations=1 >> lab_2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=4 --iterations=2 >> lab_2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=4 --iterations=4 >> lab_2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=4 --iterations=8 >> lab_2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=4 --iterations=16 >> lab_2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=8 --iterations=1 >> lab_2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=8 --iterations=2 >> lab_2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=8 --iterations=4 >> lab_2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=8 --iterations=8 >> lab_2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=8 --iterations=16 >> lab_2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=1 >> lab_2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=2 >> lab_2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=4 >> lab_2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=8 >> lab_2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=16 >> lab_2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=1 >> lab_2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=2 >> lab_2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=4 >> lab_2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=8 >> lab_2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=16 >> lab_2b_list.csv
	# check that sub lists work correctly with spinlocks
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=10 --sync=s >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=20 --sync=s >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=40 --sync=s >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=80 --sync=s >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=10 --sync=s >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=20 --sync=s >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=40 --sync=s >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=80 --sync=s >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=10 --sync=s >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=20 --sync=s >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=40 --sync=s >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=80 --sync=s >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=10 --sync=s >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=20 --sync=s >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=40 --sync=s >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=80 --sync=s >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=10 --sync=s >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=20 --sync=s >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=40 --sync=s >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=80 --sync=s >> lab_2b_list.csv
	# check that sub lists work correctly with mutexes
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=10 --sync=m >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=20 --sync=m >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=40 --sync=m >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=80 --sync=m >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=10 --sync=m >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=20 --sync=m >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=40 --sync=m >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=80 --sync=m >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=10 --sync=m >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=20 --sync=m >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=40 --sync=m >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=80 --sync=m >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=10 --sync=m >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=20 --sync=m >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=40 --sync=m >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=80 --sync=m >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=10 --sync=m >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=20 --sync=m >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=40 --sync=m >> lab_2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=80 --sync=m >> lab_2b_list.csv
	# run synchronized versions without yields
	./lab2_list --iterations=1000 --threads=1 --lists=1 --sync=s >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=1 --lists=4 --sync=s >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=1 --lists=8 --sync=s >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=1 --lists=16 --sync=s >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=2 --lists=1 --sync=s >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=2 --lists=4 --sync=s >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=2 --lists=8 --sync=s >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=2 --lists=16 --sync=s >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=4 --lists=1 --sync=s >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=4 --lists=4 --sync=s >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=4 --lists=8 --sync=s >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=4 --lists=16 --sync=s >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=8 --lists=1 --sync=s >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=8 --lists=4 --sync=s >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=8 --lists=8 --sync=s >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=8 --lists=16 --sync=s >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=12 --lists=1 --sync=s >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=12 --lists=4 --sync=s >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=12 --lists=8 --sync=s >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=12 --lists=16 --sync=s >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=1 --lists=1 --sync=m >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=1 --lists=4 --sync=m >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=1 --lists=8 --sync=m >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=1 --lists=16 --sync=m >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=2 --lists=1 --sync=m >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=2 --lists=4 --sync=m >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=2 --lists=8 --sync=m >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=2 --lists=16 --sync=m >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=4 --lists=1 --sync=m >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=4 --lists=4 --sync=m >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=4 --lists=8 --sync=m >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=4 --lists=16 --sync=m >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=8 --lists=1 --sync=m >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=8 --lists=4 --sync=m >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=8 --lists=8 --sync=m >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=8 --lists=16 --sync=m >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=12 --lists=1 --sync=m >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=12 --lists=4 --sync=m >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=12 --lists=8 --sync=m >> lab_2b_list.csv
	./lab2_list --iterations=1000 --threads=12 --lists=16 --sync=m >> lab_2b_list.csv
	
graphs:
	chmod +x lab2b.gp
	./lab2b.gp 

tarball:
	tar -czvf lab2b-204564235.tar.gz lab2_add.c SortedList.h SortedList.c lab2_list.c Makefile lab_2b_list.csv lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png README.txt lab2b.gp profile.gperf

clean:
	rm lab2_add lab2_list lab2b-204564235.tar.gz raw.gperf

profile:
	LD_PRELOAD=/u/cs/ugrad/karenl/lib/libprofiler.so CPUPROFILE=./raw.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text ./lab2_list ./raw.gperf > profile.gperf
	pprof --list=threadIterations ./lab2_list ./raw.gperf >> profile.gperf
