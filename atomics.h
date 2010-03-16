#if __x86_64__ || _WIN64
#define _64BIT
#else
#define _32BIT
#endif

#ifndef ATOMICS
#define ATOMICS

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

#endif
