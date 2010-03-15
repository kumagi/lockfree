#include "atomics.h"
#include "thread_pool.hpp"
template<typename T>
class queue{
private:
	class Node{
	public:
		T* mValue;
		Node* next;
		Node(T* v):mValue(v),next(NULL){}
	};	
	Node *head,*tail;
public:
	queue():head(new Node(NULL)),tail(head){ } // sentinel node
	
	void enq(T* const v){
		Node* const node = new Node(v);
		while(1){
			Node* last = tail;
			Node* next = last != NULL ? last->next : NULL;
			if(last != tail){ continue; }
			if(next == NULL){
				if(compare_and_set(&last->next, next, node)){
					compare_and_set(&tail,last,node);
					return;
				}
			}else{
				compare_and_set(tail, last, next);
			}
		}
	}
	T* deq(void){
		
	}
};



#include <stdio.h>
#include <stdlib.h>
class item{
private:
	int a,b,c;
	item();
public:
	item(int x,int y, int z):a(x),b(y),c(z){ }
	void print(void)const {printf("Item - a:%d b:%d c:%d\n",a,b,c);}
};
queue<item> lfqueue;

void enque(item* node){
	lfqueue.enq(node);
}


pthread_mutex_t printmutex;
void deque(item* node){
	item* dequed = lfqueue.deq();
	
	pthread_mutex_lock(&printmutex);
	dequed->print();
	pthread_mutex_unlock(&printmutex);
	
	delete dequed;
}

int main (void)
{
	// random seed
	FILE* devrandom = fopen("/dev/random","r");
	int randomseed;
	fread(&randomseed,sizeof(int),1,devrandom);
	
	// mutex
	pthread_mutex_init(&printmutex,0);
	
	srand(randomseed);
	threadpool<item*> threads(100);
	for(int i = 0; i < 1000; i++){
		threads.run(enque,new item(i,rand()%21,rand()%433+23));
	}
	
	for(int i = 0; i < 1000; i++){
		threads.run(deque,NULL);
	}
}
