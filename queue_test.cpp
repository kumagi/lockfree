
#include "queue.hpp"
#include <stdio.h>
#include "thread_pool.hpp"
#include <stdlib.h>
#include <queue>


queue<int> lfqueue;

/*
int main(void){
	lfqueue.enq(new item(1,2,3));
	lfqueue.deq();
	return 0;
}

/*/
std::queue<int> g_stlqueue;

#define MAX 20000000
void enque(int i){
	lfqueue.enq(i);
}
void deque(int){
	for(int i = 0;i<100;i++){
		int i = lfqueue.deq();
		i = i;
	}
}


pthread_mutex_t enq_mutex;
pthread_mutex_t deq_mutex;


void enque_mutex(int i){
	pthread_mutex_lock(&enq_mutex);
	
	g_stlqueue.push(i);
	
	pthread_mutex_unlock(&enq_mutex);
}
void deque_mutex(int){
	for(int i = 0;i<100;i++){
		int dequed; 
		pthread_mutex_lock(&deq_mutex);
		do{
			dequed = g_stlqueue.front();
		}while(dequed == 0);
		g_stlqueue.pop();
	
		pthread_mutex_unlock(&deq_mutex);
	}
}

//#define TEST
#include "gettime.h"

pthread_mutex_t printmutex;
int main (void)
{
	// random seed
	srand((unsigned int)time(NULL));
	
	double enquestart,enqueend,dequestart,dequeend;
	threadpool<int> threads(20);
	
	printf("%d thread initialized, %d items  \n",20, MAX);
	
	printf("lockfree\t|");
	// lockfree queue start
	enquestart = gettime();
	/*
	for(int i = 0; i < MAX; i++){
		threads.run(enque,i);
	}
	threads.wait();
	//*/
	for(int i = 0; i < MAX; i++){
		lfqueue.enq(i);
	}
	//*/
	enqueend = gettime();
	printf("  enque: %lf    ", enqueend - enquestart);
	
	dequestart = gettime();
	for(int i = 0; i < MAX/100; i++){
		threads.run(deque,0);
	}
	threads.wait();
	dequeend = gettime();
	printf(" deque: %lf\n", dequeend - dequestart);
	
	
	
	// mutex with stl start
	pthread_mutex_init(&printmutex,0);
	pthread_mutex_init(&enq_mutex,0);
	pthread_mutex_init(&deq_mutex,0);
	
	printf("STL(mutex)\t|");
	
	enquestart = gettime();
	/*
	for(int i = 0; i < MAX; i++){
		threads.run(enque_mutex,i+1);
	}
	//*/
	for(int i = 1; i <= MAX; i++){
		g_stlqueue.push(i);
	}
	//*/
	enqueend = gettime();
	printf("  enque: %lf    ", enqueend - enquestart);
	
	dequestart = gettime();
	for(int i = 0; i < MAX/100; i++){
		threads.run(deque_mutex,0);
	}
	threads.wait();
	dequeend = gettime();
	printf(" deque: %lf\n", dequeend - dequestart);
}

//*/
 
