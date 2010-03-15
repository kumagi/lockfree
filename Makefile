CC = g++44
OPTS= -pthread -Wall -O2
OBJS=queue.o
target:queue
queue:$(OBJS) Makefile
	$(CC) $(OBJS) -o queue $(OPTS)

queue.o:queue.cpp thread_pool.hpp atomics.h
	g++44 queue.cpp -o queue.o -c
.SUFFIXES: .cpp .o
.c.o:
	$(CC) $< $(OPTS) -c