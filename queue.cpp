#include "atomics.h"
#include "thread_pool.hpp"
#include <stdlib.h>

template<typename T>
class queue{
private:
	class Node{
	public:
		const T mValue;
		Node* mNext;
		Node(const T& v):mValue(v),mNext(NULL){}
		Node():mValue(),mNext(NULL){};
	private:
		Node(const Node&);
		Node& operator=(const Node&);
	};	
	Node *mHead,*mTail;
	
	// uncopyable
	queue(const queue&);
	queue& operator=(const queue&);
public:
	queue():mHead(new Node()),mTail(mHead){ } // sentinel node
	
	void enq(const T& v){
		const Node* const node = new Node(v);
		while(1){
			const Node* const last = mTail;
	 		const Node* const next = last->mNext;
			if(last != mTail){ continue; }
			if(next == NULL){
				if(compare_and_set(&last->mNext, next, node)){
					compare_and_set(&mTail, last, node);
					return;
				}
			}else{
				compare_and_set(&mTail, last, next);
			}
		}
	}
	T deq(void){
		unsigned int backoff = 2;
		while(1){
			const Node* const first = mHead;
			const Node* const last = mTail;
			const Node* const next = first->mNext;

			if(first != mHead){ continue;}
			
			if(first == last){
				if(next == NULL){
					usleep(rand()&(backoff - 1));
					backoff <<= 1;
					continue;
				}
				compare_and_set(&mTail,last,next);
			}else{
				const T v = next->mValue;
				if(compare_and_set(&mHead, first, next)){
					delete first;
					return v;
				}
			}
		}
	}
	
	T deq_throws(void){
		while(1){
			const Node* const first = mHead;
			const Node* const last = mTail;
			const Node* const next = first->mNext;

			if(first != mHead){ continue;}
			
			if(first == last){
				if(next == NULL){
					throw 1;
				}
				compare_and_set(&mTail,last,next);
			}else{
				const T v = next->mValue;
				if(compare_and_set(&mHead, first, next)){
					delete first;
					return v;
				}
			}
		}
	}
	~queue(){
		try{
			while(1){
				deq_throws();
			}
		}
		catch(int i){};
	}
};


#include <stdio.h>
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

void enque(int i){
	lfqueue.enq(i);
}


pthread_mutex_t enq_mutex;
pthread_mutex_t deq_mutex;


void enque_mutex(int i){
	pthread_mutex_lock(&enq_mutex);
	
	g_stlqueue.push(i);
	
	pthread_mutex_unlock(&enq_mutex);
}
void deque_mutex(int){
	pthread_mutex_lock(&deq_mutex);
	int dequed; 
	do{
		dequed = g_stlqueue.front();
	}while(dequed == 0);
	g_stlqueue.pop();
	
	pthread_mutex_unlock(&deq_mutex);
}

//#define TEST
#include "gettime.h"

pthread_mutex_t printmutex;
void deque(int){
	int i = lfqueue.deq();
	i = i;
}
#define THREADS 30
#define MAX 100000
int main (void)
{
	// random seed
	srand((unsigned int)time(NULL));
	
	// mutex
	pthread_mutex_init(&printmutex,0);
	pthread_mutex_init(&enq_mutex,0);
	pthread_mutex_init(&deq_mutex,0);
	
	threadpool<int> threads(THREADS);
	
	printf("%d thread initialized\n", THREADS);
	
	// lockfree queue bench
	const double start = gettime();
	for(int i = 0; i < MAX; i++){
		threads.run(enque,i);
		threads.run(deque,NULL);
	}
	threads.wait();
	const double end = gettime();
	printf("lockfree\t| %d items enque and dequed  time: %lf\n", MAX, end - start);
	
	
	// stl with single thread start
	std::queue<int> stlqueue;
	const double stlstart = gettime();
	for(int i = 0; i < MAX; i++){
		stlqueue.push(i);
		int p = stlqueue.front();
		p = p;
		stlqueue.pop();
	}
	const double stlend = gettime();
	printf("STL\t\t| %d items enque and dequed  time: %lf\n", MAX, stlend - stlstart);
	
	
	// mutex with stl start
	const double mutexstart = gettime();
	for(int i = 0; i < MAX; i++){
		threads.run(enque_mutex,i+1);
	}
	for(int i = 0; i < MAX; i++){
		threads.run(deque_mutex,NULL);
	}
	threads.wait();
	const double mutexend = gettime();
	printf("STL MT\t\t| %d items enque and dequed  time: %lf\n", MAX, mutexend - mutexstart);
}

//*/
