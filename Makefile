CC = g++44
OPTS= -O0 -g
WARNS = -Wall -Wextra -Wformat=2 -Wstrict-aliasing=2 -Wcast-qual -Wcast-align \
	-Wwrite-strings -Wfloat-equal -Wpointer-arith -Wswitch-enum
LDOPT= -pthread

#target:list
target:queue

list:list_test.o list.o Makefile
	$(CC) list.o list_test.o -o list $(OPTS) $(WARNS) $(LDOPT)
queue:queue_test.o Makefile
	$(CC) queue_test.o -o queue $(OPTS)  $(WARNS) $(LDOPT)

stack:stack_test.o Makefile
	$(CC) stack_test.o -o stack $(OPTS) $(WARNS) $(LDOPT)


queue_test.o:queue_test.cpp queue.hpp
	$(CC) queue_test.cpp -o queue_test.o -c $(OPTS) $(WARNS)
stack_test.o:stack_test.cpp stack.hpp
	$(CC) stack_test.cpp -o stack_test.o -c $(OPTS) $(WARNS)
list_test.o:list_test.cpp list.hpp atomics.h
	$(CC) list_test.cpp -o list_test.o -c  $(OPTS) $(WARNS)


.SUFFIXES: .cpp .o
.c.o:
	$(CC) $< $(OPTS) -c