CC = g++44
OPTS= -pthread -Wall -O4
OBJS=queue.o list.o
target:list

list:list.o Makefile
	$(CC) list.o -o list $(OPTS) -fwhole-program

queue:$(OBJS) Makefile
	$(CC) queue.o -o queue $(OPTS) -fwhole-program

list.o:list.cpp thread_pool.hpp atomics.h
	g++44 list.cpp -o list.o -c $(OPTS)
queue.o:queue.cpp thread_pool.hpp atomics.h
	g++44 queue.cpp -o queue.o -c $(OPTS)


.SUFFIXES: .cpp .o
.c.o:
	$(CC) $< $(OPTS) -c