all: user calculator

calculator: calculator.c MessageQueueWrapper.o Vector.o PriorityQueue.o MedianHeap.o Chrono.o
	gcc -o calculator calculator.c MessageQueueWrapper.o Vector.o PriorityQueue.o MedianHeap.o Chrono.o

user: user.c MessageQueueWrapper.o Vector.o PriorityQueue.o MedianHeap.o Chrono.o
	gcc -o user user.c MessageQueueWrapper.o Vector.o PriorityQueue.o MedianHeap.o Chrono.o

Chrono.o: Chrono.h Chrono.c
	gcc -c Chrono.c

MessageQueueWrapper.o: MessageQueueWrapper.h MessageQueueWrapper.c
	gcc -c MessageQueueWrapper.c

Vector.o: Vector.c Vector.h 
	gcc -c Vector.c

PriorityQueue.o: PriorityQueue.c PriorityQueue.h 
	gcc -c PriorityQueue.c

MedianHeap.o: MedianHeap.c MedianHeap.h
	gcc -c MedianHeap.c

binaries = user calculator main
clean:
	rm -f $(binaries) *.o