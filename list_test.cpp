#include "list.hpp"
#include <set>
#include <stdlib.h>
#include <time.h>

class tester{
private:
	lockfree::list<int> mlist;
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
		while(it != reg.end()){
			int test = *it;
			assert(mlist.contains(test) == true);
			++it;
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
		// erase all
		it = reg.begin();
		while(it != reg.end()){
			int test = *it;
			assert(mlist.erase(test) == true);
			++it;
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
		assert(mlist.erase(2) == false);
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
