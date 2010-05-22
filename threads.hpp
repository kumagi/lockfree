#ifndef THREAD_POOL
#define THREAD_POOL

#include <pthread.h>
#include <stdlib.h> // malloc
#include <unistd.h> // sleep,usleep
#include "queue.hpp"
#include <assert.h>
#include <sys/resource.h> // rlimit
 
namespace{
template<typename arg>
void* pool(void* o);
};

template<typename arg>
class threads{
	class task{
	public:
		void (*func)(arg);
		arg param;
		task(void (*_func)(arg), const arg& _param):func(_func),param(_param){}
		task(const task& _task){
			func = _task.func;
			param = _task.param;
		}
		task():func(NULL),param(){}
		task& operator=(const task& rhs){
			func = rhs.func;
			param = rhs.param;
		}
	};
	class thread_t{
	public:
		pthread_t t;
		pthread_mutex_t mutex;

		task* mytask;
		threads<arg>* const master;
		thread_t(threads<arg>* _master):master(_master){
			pthread_mutex_init(&mutex,0);
			pthread_mutex_lock(&mutex);
			pthread_create(&t, NULL, (::pool<arg>), this);
		};
		thread_t():master(NULL){
		};
		~thread_t(){
			if(master){
				pthread_cancel(t);
				pthread_mutex_destroy(&mutex);
			}
		}
		void dispatch(task* _task){
			mytask = _task;
			pthread_mutex_unlock(&mutex);
		}
	};
private:
	unsigned int max;
	// pool function
	friend void* (::pool<arg>)(void* o);
	lockfree::queue<thread_t*> thread_pool;
	lockfree::queue<task*> task_pool;
	volatile unsigned int count;

	pthread_mutex_t count_mut;

	friend void* (::pool)(void* o);
public:
	threads(const int _max):max(_max),count(0) {
		pthread_mutex_init(&count_mut,0);
		struct rlimit limit;
		getrlimit(RLIMIT_NPROC, &limit);
		if(max > limit.rlim_cur){
			max = limit.rlim_cur;
		}

		for(unsigned int i=0; i<max; i++){
			thread_pool.enq(new thread_t(this));
		}

	}
	~threads(){
		while(!thread_pool.empty()){
			thread_t* obj=NULL;
			thread_pool.deq(&obj);
			delete obj;
		}
		while(!task_pool.empty()){
			task* obj=NULL;
			task_pool.deq(&obj);
			delete obj;
		}
		pthread_mutex_destroy(&count_mut);
	}
	void run(void (*func)(arg),const arg& param){
		task* newtask = new task(func,param);
		if(!thread_pool.empty()){
			thread_t* obj;
			if(thread_pool.deq(&obj)){
				obj->dispatch(newtask);
			}else{
				task_pool.enq(newtask);
			}
		}else{
			task_pool.enq(newtask);
		}
	}
	void add(int num){
		for(unsigned int i=0; i<num; i++){
			thread_pool.enq(new thread_t(this));
		}
	}

	void wait(void) const {
		while(count || !task_pool.empty()){
			usleep(1);
		}
	}
private:
	void count_inc(void){
		pthread_mutex_lock(&count_mut);
		count++;
		pthread_mutex_unlock(&count_mut);
	}
	void count_dec(void){
		pthread_mutex_lock(&count_mut);
		count--;
		pthread_mutex_unlock(&count_mut);
	}
};
namespace{
template<typename arg>
void* pool(void* o){
	typename threads<arg>::thread_t* obj = (typename threads<arg>::thread_t*)o;
	while(1){
		pthread_mutex_lock(&obj->mutex);
		obj->master->count_inc();

		(*obj->mytask->func)(obj->mytask->param);
		delete obj->mytask;

		// search for suspended work
		while(!obj->master->task_pool.empty()){
			typename threads<arg>::task* nexttask;
			if(obj->master->task_pool.deq(&nexttask)){
				(nexttask->func)(nexttask->param);
				delete nexttask;
			}
		}

		obj->master->count_dec();
		obj->master->thread_pool.enq(obj);
	}
	return NULL;
}
};
#endif


