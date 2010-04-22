#ifndef LOCKFREE_QUEUE_H_
#define LOCKFREE_QUEUE_H_

#include "atomics.h"
#include "thread_pool.hpp"
#include <stdlib.h>

namespace lockfree{

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
} // namespace lockfree
#endif
