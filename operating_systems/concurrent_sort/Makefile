CSORT: semun.h CSORT.c semWrapper.o sharedMemoryWrapper.o
	gcc -w -o CSORT CSORT.c semWrapper.o sharedMemoryWrapper.o

semWrapper.o: semWrapper.c semWrapper.h
	gcc -c semWrapper.c

sharedMemoryWrapper.o: sharedMemoryWrapper.c sharedMemoryWrapper.h
	gcc -c sharedMemoryWrapper.c