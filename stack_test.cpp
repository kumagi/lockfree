#include "stack.hpp"
#include "thread_pool.hpp"
#include "gettime.h"
#include <stdio.h>
#define MAX 20000000

stack<int> st;


void popper(int id){
	for(int i=0;i<MAX/4;i++){
		int data;
		st.atomic_pop(&data);
		//printf("%d:%d,",id,data);
	}
}

int main(void){
	for(int i=0;i<MAX;i++){
		st.push(i);
	}
	
	threadpool<int> threads(4);
	printf("%d items push and pop\n ",MAX);
	
	double start = gettime();
	
	threads.run(popper,1);
	threads.run(popper,2);
	threads.run(popper,3);
	threads.run(popper,4);
	
	threads.wait();
	double end = gettime();
	
	
	printf("time:%f\n ",end-start);
}
