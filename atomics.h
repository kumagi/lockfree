
#ifndef ATOMICS
#define ATOMICS

#if __x86_64__ || _WIN64
#define _64BIT
#define _ptr long long
#else
#define _32BIT
#define _ptr int
#endif


inline bool compare_and_set(const void* const pos, const void* const oldval, const void* const newval) __attribute__((always_inline));

#ifdef _64BIT
inline bool compare_and_set(const void* const pos, const void* const oldval, const void* const newval) {
	return __sync_bool_compare_and_swap((long long*)pos,oldval,newval);
}
#else
inline bool compare_and_set(const void* pos, const void* const oldval, const void* const newval) {
	return __sync_bool_compare_and_swap((int*)pos,oldval,newval);
}
#endif


class atomic_int{
public:
	int a;
	atomic_int():a(0){ }
	atomic_int(const int param):a(param){}
	operator int()const {return a;}
	int& operator++(){
		__sync_fetch_and_add(&a,1);
		return a;
	}
	int& operator--(){
		__sync_fetch_and_sub(&a,1);
		return a;
	}
	int operator++(int){
		return __sync_fetch_and_add(&a,1);
	}
	int operator--(int){
		return __sync_fetch_and_sub(&a,1);
	}
	int& operator=(int integer){
		a = integer;
		return a;
	}
};


template<typename T>
class markable_ptr{
public:
  T* ptr;
  markable_ptr():ptr(0){}
  markable_ptr(const markable_ptr<T>& origin):ptr(origin.ptr){}
  markable_ptr(T* const _ptr):ptr(_ptr){}
  T& operator*() const {return *(T*)(ptr&(~1));}
  T* operator->() const {return (T*)(ptr)&(~1));}
  bool operator==(const T* const rhs)const{
    return ptr == rhs;
  }
  bool operator!=(const T* const rhs)const{
    return ptr != rhs;
  }
  bool attempt_mark(){return compare_and_swap(&ptr,ptr,ptr|1);}
  T* get_pointer() const {return (T*)(ptr&(~1));}
  bool check_mark() const {return ptr&1;}
  markable_ptr<T>& operator=(const markable_ptr<T>& origin){
    ptr = origin.ptr;
  }
  markable_ptr<T>& operator=(const T* origin){
    ptr = origin;
  }
};
#endif
