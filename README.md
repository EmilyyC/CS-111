# UCLA CS 111 (Winter 2017)

Assignments for UCLA [CS 111](http://web.cs.ucla.edu/classes/winter17/cs111/) (Winter 2017) taught by Dr. Paul Eggert.



### Lab 0: [Warm-Up](http://web.cs.ucla.edu/classes/winter17/cs111/labs/project0.html)

"Completely trivial" warm-up involving command line argument processing that should take "20 minutes of work".    

 

### Lab 1: [Simpleton Shell](http://web.cs.ucla.edu/classes/winter17/cs111/assign/lab1.html)

A simple, stripped down shell implemented in C that takes in as command line arguments which files to access, which pipes to create, and which subcommands to invoke. It then creates or accesses all the files and creates all the pipes processes needed to run the subcommands, and reports the processes's exit statuses as they exit.



### Lab 2: [Races and Synchronization](http://web.cs.ucla.edu/classes/winter17/cs111/labs/CS111newProject2A.html) / [Lock Granularity and Performance](http://web.cs.ucla.edu/classes/winter17/cs111/labs/CS111newProject2B.html)

Multithreaded C applications that deal with conflicting read-modify-write operations on single variables and ordered linked lists. Then, the applications' performance was analyzed by plotting the results using gnuplot.



### Lab 3: [File System Dump](http://web.cs.ucla.edu/classes/winter17/cs111/assign/cs111_project3A.html) / [Analysis](http://web.cs.ucla.edu/classes/winter17/cs111/assign/cs111_project3B.html)

C program to analyze file systems and diagnose corruption; the program reads the image of a EXT2 file system, analyzes it, and summarizes its contents in several csv files. A Python program was implemented to analyze these csv files to diagnose problems in the provided file system.



### Lab 4: [Embedded Systems](http://web.cs.ucla.edu/classes/winter17/cs111/labs/Project4.html)

C programs for the Intel Edison to measure temperature using the Grove Temperature Sensor and act as a client for interactions with a remote server through either TCP or SSL/TLS.

