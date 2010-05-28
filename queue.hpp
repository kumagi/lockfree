#ifndef LOCKFREE_QUEUE
#define LOCKFREE_QUEUE

#include "atomics.h"
#include <unistd.h> // usleep
#include <stdlib.h> // rand

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
	T deq(){ // it returns false if there is nothing to deque.
		while(1){
			const Node* const first = mHead;
			const Node* const last = mTail;
			const Node* const next = first->mNext;

			if(first != mHead){ continue;}

			if(first == last){
				if(next == NULL){
					while(mHead->mNext == NULL){
						usleep(1);
					}
					continue;
					//return false;
				}
				compare_and_set(&mTail,last,next);
			}else{
				T result = next->mValue;
				if(compare_and_set(&mHead, first, next)){
					delete first;
					return result;
				}
			}
		}
	}

	bool deq_delete(){ // it destroys dequed item. but fast.
		while(1){
			const Node* const first = mHead;
			const Node* const last = mTail;
			const Node* const next = first->mNext;

			if(first != mHead){ continue;}

			if(first == last){
				if(next == NULL){
					return false;
				}
				compare_and_set(&mTail,last,next);
			}else{
				if(compare_and_set(&mHead, first, next)){
					delete first;
					return true;
				}
			}
		}
	}
	bool empty() const{
		return mHead->mNext == NULL;
	}
	bool size() const{
		const Node* it = mHead;
		int num;
		while(it != NULL){
			it = it->mNext;
			num++;
		}
		return num-1;
	}

	~queue(){
		while(deq_delete());
		delete mHead;
	}
};

}; // namepace lockfree

#endif
