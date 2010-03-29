CC = g++44
OPTS= -pthread -Wall -O4
OBJS=queue.o list.o
target:queue
target:stack

list:list.o Makefile
	$(CC) list.o -o list $(OPTS)

queue:queue_test.o Makefile
	$(CC) queue_test.o -o queue $(OPTS)

stack:stack_test.o Makefile
	$(CC) stack_test.o -o stack $(OPTS)


list.o:list.cpp thread_pool.hpp atomics.h
	$(CC) list.cpp -o list.o -c $(OPTS)
queue_test.o:queue_test.cpp queue.hpp
	$(CC) queue_test.cpp -o queue_test.o -c $(OPTS)
stack.o:stack_test.cpp stack.hpp
	$(CC) stack_test.cpp -o stack_test.o -c $(OPTS)


.SUFFIXES: .cpp .o
.c.o:
	$(CC) $< $(OPTS) -c