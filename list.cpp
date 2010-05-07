#include "atomics.h"
#include <stdio.h>
#include <boost/shared_ptr.hpp>

template<typename T>
class list{
  /* requirement
    copy constructor of T
    bool operator<(const T&);
   */
private:// typedef
  class node{
  public:
    const T item;
    markable_ptr<node> next; // it contains deletion mark in &1
  public:
    Node(const T t):item(t),next(NULL),shared_cnt(0){ }
    Node():item(),next(NULL),shared_cnt(1){ }// sentinel node	
  };
  typedef markable_ptr<Node> node_ptr;
public:// variants
  node mHead;
public:// public methods
  list():mHead(NULL){}
  bool insert(const T& t){
    node_ptr prev = mHead;
    node_ptr curr = prev->next;
    while(curr != NULL){
      const node_ptr succ = curr->next;
      if(curr.check_mark()){
        if(compare_and_swap(&prev->next, curr, succ)){
          curr = succ;
          continue;
        }else{// prev is updated, return to head
          prev = mHead;
          curr = prev->next;
          continue;
        }
      }else{
        if(curr->item < t){
          prev = curr;
          curr = succ;
          continue;
        }else if(curr->item == t){
          return false; // already exists
        }else{
          node* newnode = new node(t);
          node->next = succ;
          if(compare_and_swap()
          return true;
        }
      }
    }
  }
  bool erase(const T& t){
    
  }
  bool find(const T& t)const{
    
  }
private:// forbidden methods
  list(const list&);2
  list& operator=(const list&);
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
    printf("valid.\n");
  }else{
    printf("invalid.\n");
  }
}
