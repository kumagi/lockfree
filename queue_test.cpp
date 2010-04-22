
#include "queue.hpp"
#include <stdio.h>
#include "thread_pool.hpp"
#include <stdlib.h>
#include <queue>
using namespace lockfree;;

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
#define CORE 4

void enque(int offset){
	for(int i=0 ;i<MAX/CORE; i++){
		lfqueue.enq(i+offset);
	}
}
void deque(int){
	for(int i = 0;i<MAX/CORE;i++){
		lfqueue.deq();
	}
}

class mutex{
private:
	mutex(void);
	mutex(const mutex&);
private:
	pthread_mutex_t *target;
public:
	mutex(pthread_mutex_t* _target):target(_target){
		lock();
	}
	~mutex(){
		unlock();
	}
	inline void lock(void){
		pthread_mutex_lock(target);
	}
	inline void unlock(void){
		pthread_mutex_unlock(target);
	}
};

pthread_mutex_t enq_mutex;
pthread_mutex_t deq_mutex;

void enque_mutex(int i){
	for(int i = 0;i<MAX/CORE;i++){
		mutex lock(&enq_mutex);
		g_stlqueue.push(i);
	}
}
void deque_mutex(int){
	for(int i = 0;i<MAX/CORE;i++){
		mutex lock(&deq_mutex);
		int dequed; 
		do{
			dequed = g_stlqueue.front();
		}while(dequed == 0);
		g_stlqueue.pop();
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
	//*
	for(int i = 0; i < CORE; i++){
		threads.run(enque,i*MAX/CORE);
	}
	threads.wait();
	enqueend = gettime();
	printf("  enque: %lf    ", enqueend - enquestart);
	
	dequestart = gettime();
	for(int i = 0; i < CORE; i++){
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
	//*
	for(int i = 0; i < MAX/CORE; i++){
		threads.run(enque_mutex,i*MAX/CORE);
	}
	/*/
	for(int i = 1; i <= MAX; i++){
		g_stlqueue.push(i);
	}
	//*/
	enqueend = gettime();
	printf("  enque: %lf    ", enqueend - enquestart);
	
	dequestart = gettime();
	for(int i = 0; i < MAX/CORE; i++){
		threads.run(deque_mutex,0);
	}
	threads.wait();
	dequeend = gettime();
	printf(" deque: %lf\n", dequeend - dequestart);
}

//*/
 
