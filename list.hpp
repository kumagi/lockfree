#ifndef LOCKFREE_LIST_H_
#define LOCKFREE_LIST_H_
#include "atomics.h"
#include <stdio.h>
#include <assert.h>

namespace lockfree{

template<typename T>
class list{
	/* requirement
	   T()
	   T(const T&)
	   bool operator<
	   bool operator==
	*/
private:// typedef
	class node{
	public:
		const T item;
		markable_ptr<node> next; // it contains deletion mark in &1
	public:
		node(const T& t):item(t),next(NULL){ }
		node(const T& t,markable_ptr<node> n):item(t),next(n){ }
		node():item(),next(NULL){ }// sentinel node	
	};
	typedef markable_ptr<node> mptr;
public:// variants
	node mHead;
public:// public methods
	list():mHead(){}
	
	bool insert(const T& t){
		while(1){
			mptr pred,curr;
			find(t, &mHead, &pred, &curr);
			
			if(curr.get() == NULL || curr->item != t){
				node* newnode = new node(t,curr);
				if(compare_and_set(&pred->next, curr.get(), newnode)){
					return true; 
				}else{
					continue;
				}
			}else{
				return false; // already exists
			}
		}
	}
	bool erase(const T& t){
		while(1){
			mptr pred,curr;
			find(t, &mHead, &pred, &curr);
			
			if(curr.get() == NULL){ return false;} // no item
			if(curr->item == t){
				mptr succ = curr->next;
				if(curr->next.attempt_mark()){
					compare_and_set(&pred->next, curr.get(), succ.get()); // although it failed, no problem
					return true;
				}else{
					continue; // curr is changed, retry.
				}
			}else{
				return false; // no item
			}
		}
	}
	bool contains(const T& t)const{
		mptr ptr = mHead.next;
		while(ptr != NULL){
			if(ptr->item < t){
				ptr = ptr->next;
				continue;}
			else{break;}
		}
		if(ptr.get() == NULL){return false;}
		if(ptr->item == t){
			if(ptr->next.is_marked()){ // marked object is invalid
				return false;
			}else{
				return true;
			}
		}else {return false;}
	}
	void dump(void)const{
		mptr ptr = mHead.next;
		while(ptr != NULL){
			printf("%d,",ptr->item);
			ptr = ptr->next;
		}
	}
private:
	void find(const T& t, node* head, mptr* pred, mptr* curr){
		while(1){
			*pred = head;
			*curr = (*pred)->next;
			if(*pred != head) continue;
			while(1){
				if(curr->get() == NULL){return;}
				mptr succ = (*curr)->next;
				if(curr->is_marked()){ // check deleting mark
					if(compare_and_set(&(*pred)->next, curr->get(), succ.get())){
						*curr = succ;
						continue;
					}else{ // pred is modified, back to head
						break;
					}
				}else{
					if((*curr)->item < t){ // iterate
						*pred = *curr;
						*curr = succ;
					}else{
						return;
					}
				}
			}
		}
	}
private:// forbidden methods
	list(const list&);
	list& operator=(const list&);
};

}// namespace lockfree	

#endif
