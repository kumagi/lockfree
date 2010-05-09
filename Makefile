CC = g++44
OPTS= -O0 -g
WARNS = -Wall -Wextra -Wformat=2 -Wstrict-aliasing=2 -Wcast-qual -Wcast-align \
	-Wwrite-strings -Wfloat-equal -Wpointer-arith -Wswitch-enum
LDOPT= -pthread

target:list
#target:stack

list:list.o Makefile
	$(CC) list.o -o list $(OPTS) $(WARNS) $(LDOPT)

queue:queue_test.o Makefile
	$(CC) queue_test.o -o queue $(OPTS)  $(WARNS) $(LDOPT)

stack:stack_test.o Makefile
	$(CC) stack_test.o -o stack $(OPTS) $(WARNS) $(LDOPT)


list.o:list.cpp thread_pool.hpp atomics.h
	$(CC) list.cpp -o list.o -c $(OPTS) $(WARNS)
queue_test.o:queue_test.cpp queue.hpp
	$(CC) queue_test.cpp -o queue_test.o -c $(OPTS) $(WARNS)
stack.o:stack_test.cpp stack.hpp
	$(CC) stack_test.cpp -o stack_test.o -c $(OPTS) $(WARNS)


.SUFFIXES: .cpp .o
.c.o:
	$(CC) $< $(OPTS) -c