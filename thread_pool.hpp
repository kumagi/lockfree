#ifndef THREAD_POOL
#define THREAD_POOL
#include <pthread.h>
#include <stdlib.h> // malloc
#include <unistd.h> // sleep,usleep
#include <boost/shared_ptr.hpp>
 
#include <stdio.h>
 
namespace{
template<typename arg>
void* pool(void* o);
};
template<typename arg>
class threadpool{
private:
	int head;
	const int max;
	pthread_mutex_t* mutexs;
 
	int tmpid;
	volatile int workers;
	pthread_mutex_t tmpid_mutex;
 
	struct thread{
		pthread_t t;
		void (*funcname)(arg);
		arg argument;
		int available;
	}* threads;
 
	// pool function
	friend void* (::pool<arg>)(void*);
 
public:
	threadpool(const int nthreads):head(0),max(nthreads),tmpid(0),workers(0){
		pthread_mutex_init(&tmpid_mutex,NULL);
		pthread_mutex_lock(&tmpid_mutex);
 
		this->mutexs = (pthread_mutex_t*)malloc(nthreads*sizeof(pthread_mutex_t));
		this->threads = (thread*)malloc(nthreads*sizeof(thread));
		for(int i=0;i<nthreads;i++){
			tmpid = i;
			pthread_mutex_lock(&mutexs[i]);
			pthread_create(&threads[i].t, NULL, (::pool<arg>), this);
			pthread_mutex_lock(&tmpid_mutex); // wait for tmpid received
			threads[i].available = true;
		}
	}
	~threadpool(void){
		for(int i=0;i<max;i++){
			pthread_cancel(threads[i].t);
		}
	}
	int run(void (*func)(arg),const arg& param){
		int success;
		int it = head;
 
		// decide thread id
		while(1){
			if(__sync_bool_compare_and_swap(&threads[it].available, true, false)) // only one thread can get lock
				break;
			it = (it+1)%(max-1);
			usleep(1); // avoid busy
		}
 
		// set parameter
		threads[it].funcname = func;
		threads[it].argument = param;
 
		while(!__sync_bool_compare_and_swap(&workers,workers,workers+1));
 
		pthread_mutex_unlock(&mutexs[it]);
		head = (it+1)%(max-1);
	}
	void wait(void) const {
		while(workers != 0){
			usleep(1);
		}
	}
};
namespace{
	template<typename arg>
	void* pool(void* o){
		threadpool<arg>* obj = (threadpool<arg>*)o;
		int myid = obj->tmpid;
		pthread_mutex_unlock(&obj->tmpid_mutex); // tell that tmpid is received
		while(1){
			pthread_mutex_lock(&obj->mutexs[myid]);
			(obj->threads[myid].funcname)(obj->threads[myid].argument);
 
			while(!__sync_bool_compare_and_swap(&obj->workers,obj->workers,obj->workers-1));
			obj->threads[myid].available = true;
		}
	}
};
 
#endif
