#include "atomics.h"
#include <stdio.h>
#include <assert.h>


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
			mptr pred,curr;
			find(t, &mHead, &pred, &curr);
			assert(pred != NULL && !pred->next.is_marked());
			
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
			assert(pred != NULL && !pred->next.is_marked());
			
			if(curr.get() == NULL){ return false;}
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
			fprintf(stderr,"%d,",ptr->item);
			ptr = ptr->next;
		}
		fprintf(stderr,"\n");
	}
private:
	void find(const T& t, node* head, mptr* _pred, mptr* _curr){
		while(1){
			mptr pred = head;
			mptr curr = pred->next;
			if(pred != head) continue;
			while(1){
				if(curr.get() == NULL){
					*_pred = pred;
					*_curr = curr;
					return;
				}
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
						*_pred = pred;
						*_curr = curr;
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
		for(int i=0; i<1024; i++){// insert * 1024
			int test = rand();
			std::set<int>::iterator it = reg.find(test);
			if(it == reg.end()){
				assert(mlist.insert(test) == true);
			}else{
				assert(mlist.insert(test) == false);
			}
			reg.insert(test);
		}
		for(int i=0; i<1024; i++){// erase * 1024
			int test = rand();
			std::set<int>::iterator it = reg.find(test);
			if(it == reg.end()){
				assert(mlist.erase(test) == false);
			}else{
				assert(mlist.erase(test) == true);
			}
			reg.erase(test);
		}
		// contains
		std::set<int>::iterator it = reg.begin();
		while(it == reg.end()){
			int test = *it;
			assert(mlist.contains(test) == true);
		}
		for(int i=0; i<2048; i++){// contains * 2048
			int test = rand();
			std::set<int>::iterator it = reg.find(test);
			if(it == reg.end()){
				assert(mlist.contains(test) == false);
			}else{
				assert(mlist.contains(test) == true);
			}
			reg.erase(test);
		}
		mlist.dump();
		state = true;
	}
	void minitest(){
		assert(mlist.insert(10) == true);
		assert(mlist.insert(1) == true);
		assert(mlist.insert(4) == true);
		assert(mlist.insert(59) == true);
		assert(mlist.contains(10) == true);
		assert(mlist.contains(1) == true);
		assert(mlist.contains(10) == true);
		assert(mlist.contains(4) == true);
		assert(mlist.contains(59) == true);
		assert(mlist.insert(10) == false);
		assert(mlist.erase(4) == true);
		assert(mlist.contains(4) == false);
		
		state = true;
	}
	bool success(){ return state; }
};

int main(void){
	tester test;
	test.run();
	if(test.success()){
		printf("valid.\n");
	}else{
		printf("invalid.\n");
	}
}
