#include "atomics.h"
#include <stdio.h>
#include <boost/shared_ptr.hpp>

template<typename T>
class list{
private:
	class Node{
	public:
		boost::shared_ptr<T> item;
		Node* next; // it contains deletion mark in &1
		int* shared_cnt;
		Node(boost::shared_ptr<T> i):item(i),next(NULL),counter((int*)malloc(sizeof(int))){ }
	private:
		Node(){ }
	};
	boost::shared_ptr<T> dummy;
	Node head,tail;
	
	list(const list&);
	list& operator=(const list&);
public:
	list(void):head(dummy),tail(dummy) {
		head.next = &tail;
		tail.next = 0;
	}
	
	void enque(boost::shared_ptr<T> obj){
		
	}
	boost::shared_ptr<T> pop(void){
		
	}
};





class tester{
private:
	list<int> list;
	int state;
public:
	tester(void):list(),state(0){ }
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
