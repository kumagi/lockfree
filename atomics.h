#ifndef ATOMICS
#define ATOMICS


inline bool compare_and_set(void* pos, void* oldval, void* newval){
	return __sync_bool_compare_and_swap((int*)pos,(int)oldval,(int)newval);
}

#endif
