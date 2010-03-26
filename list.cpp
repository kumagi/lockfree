#include "atomics.h"
#include <stdio.h>
#include <boost/shared_ptr.hpp>

template<typename T>
class list{
private:
	class Node{
	public:
		T item;
		markable_ptr<Node> next; // it contains deletion mark in &1
		atomic_int shared_cnt;
		Node(const T t):item(t),next(NULL),shared_cnt(0){ }
		Node():item(),next(NULL),shared_cnt(1){ }// sentinel node
			
	};
	typedef markable_ptr<Node> NodePtr;
	Node *dummy;
	Node head;
	
	list(const list&);
	list& operator=(const list&);
public:
	class iterator{
	public:
		const Node* const head;
		Node *pred,*curr,*succ;
		int cnt;
		iterator& operator++(){
			while(1){
				bool next_mark = curr->next.check_mark();
				succ = curr->next;
				while(next_mark){
					if(!compare_and_set(&pred->next,curr->get_pointer(),succ)){
						int counter = 0;
						pred = head;
						curr = pred.next;
					}
					curr = succ;
					succ = curr->next;
					next_mark = curr->next.check_mark();
				}
				pred.shared_cnt--;
				pred = curr;
				succ.shared_cnt++;
				curr = succ;
			}
		}
		Node& operator*(){return *curr;}
		Node* operator->(){return curr;}
		iterator(const Node *h, Node *p, Node *c, Node *s, int cn):head(h),pred(p),curr(c),succ(s),cnt(cn){
			pred->shared_cnt++;
			curr->shared_cnt++;
		}
		iterator(const iterator& it):head(it.head),pred(it.pred),curr(it.curr),succ(it.succ),cnt(it.cnt){
			if(pred) {pred->shared_cnt++;}
			if(curr) {curr->shared_cnt++;}
			if(succ) {succ->shared_cnt++;}
		}
		~iterator(){
			pred->shared_cnt--;
			curr->shared_cnt--;
			succ->shared_cnt--;
		}
	};
	list(void):dummy(),head() {
	}
	iterator begin(){
		return iterator(&head,&head,head.next.get_pointer(),NULL,0);
	};
};





class tester{
private:
	list<int> mlist;
	list<int>::iterator it;
	int state;
public:
	tester(void):mlist(),it(mlist.begin()),state(0){ }
	void run(){ }
	bool success(){ return true; }
};

int main(void){
	tester test;
	test.run();
	if(test.success()){
		printf("Valid!\n");
	}else{
		printf("Invalid!\n");
	}
}
