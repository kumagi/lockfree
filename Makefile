CC = g++44
OPTS= -pthread -Wall -g
OBJS=queue.o
target:queue
queue:$(OBJS) Makefile
	$(CC) $(OBJS) -o queue $(OPTS) -fwhole-program

queue.o:queue.cpp thread_pool.hpp atomics.h
	g++44 queue.cpp -o queue.o -c $(OPTS)
.SUFFIXES: .cpp .o
.c.o:
	$(CC) $< $(OPTS) -c