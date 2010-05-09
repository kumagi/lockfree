#include "atomics.h"
#include <stdio.h>
#include <boost/shared_ptr.hpp>

template<typename T>
class list{
	/* requirement
	   default and copy constructor of T
	   bool operator<(const T&);
	*/
private:// typedef
	class node{
	public:
		const T item;
		markable_ptr<node> next; // it contains deletion mark in &1
	public:
		node(const T t):item(t),next(NULL){ }
		node(const T t,markable_ptr<node> n):item(t),next(n){ }
		node():item(),next(NULL){ }// sentinel node	
	};
	typedef markable_ptr<node> mptr;
public:// variants
	node mHead;
public:// public methods
	list():mHead(){}
	
	bool insert(const T& t){
		while(1){
			mptr pred = find(t, &mHead);
			assert(pred != NULL && !pred->next.is_marked());
			mptr curr = pred->next;
			
			if(curr.get() == NULL || curr->item != t){
				node* newnode = new node(t,curr);
				if(compare_and_set(&pred->next, curr.get(), newnode)){
					return true; 
				}else{
					delete newnode;
					continue;
				}
			}else{
				return false; // already exists
			}
		}
	}
	bool erase(const T& t){
		while(1){
			mptr pred = find(t, &mHead);
			assert(pred != NULL && !pred->next.is_marked());
			mptr curr = pred->next;
			if(curr.get() == NULL){ return true;}
			if(curr->item == t){
				mptr succ = curr->next;
				if(curr->next.attempt_mark()){
					compare_and_set(&pred->next, curr.get(), succ.get());
					return true;
				}else{
					continue;
				}
			}else{
				return false;
			}
		}
	}
	void dump(void)const{
		mptr ptr = mHead.next;
		while(ptr != NULL){
			fprintf(stderr,"%d,",ptr->item);
			ptr = ptr->next;
		}
		fprintf(stderr,"\n");
	}
private:
	mptr find(const T& t, node* head){
		while(1){
			mptr pred = head;
			mptr curr = pred->next;
			if(pred != head ) continue;
			while(1){
				if(curr.get() == NULL){return pred;}
				mptr succ = curr->next;
				if(curr.is_marked()){
					if(compare_and_set(&pred->next, curr.get(), succ.get())){
						curr = succ;
						continue;
					}else{ // pred is modified, back to head
						break;
					}
				}else{
					if(curr->item < t){
						pred = curr;
						curr = succ;
					}else{
						return pred;
					}
				}
			}
		}
	}
private:// forbidden methods
	list(const list&);
	list& operator=(const list&);
};

	

#include <set>
#include <stdlib.h>
#include <time.h>
class tester{
private:
	list<int> mlist;
	std::set<int> reg;
	int state;
public:
	tester(void):mlist(),state(false){
		srand((double)time(NULL));
	}
	void run(){
		for(int i=0; i<1024*1024; i++){
			int test = rand();
			std::set<int>::iterator it = reg.find(test);
			if(it == reg.end()){
				assert(mlist.insert(test) == true);
			}else{
				assert(mlist.insert(test) == false);
			}
			reg.insert(test);
		}
		mlist.dump();
		state = true;
	}
	void trying(){
		assert(mlist.insert(10) == true);
		assert(mlist.erase(10) == true);
		assert(mlist.insert(10) == true);
		
		state = true;
	}
	bool success(){ return state; }
};

int main(void){
	tester test;
	test.trying();
	if(test.success()){
		printf("valid.\n");
	}else{
		printf("invalid.\n");
	}
}
