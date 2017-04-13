

#include <cstdint>
#include <algorithm>
#include <cstdio>
//#include "alloca.h"
#include <malloc.h>

using u64 = uint64_t;

struct memnd { void *cur, *nxt; };

#define BEGIN {volatile void* __memnd=alloca(sizeof(memnd));
#define END }
#define MALLOC(sz) malloc(sz);

#define STK_PTR(var) \
 volatile void* var = nullptr; \
 __asm { \
 leaq rbx, var \
 movq [rbx]var, rbp  \
 );

 //__asm__ ( \
 //"movq rbp, %0 \n" \
 //: "+rm" (var) \
 //);

#define PREV_STK_PTR(var) \
 volatile void** __base = nullptr; \
 __asm__ ( \
 "movq rbp, %0 \n" \
 : "+rm" (__base) \
 ); \
 volatile void* var = nullptr; \
 var = *__base;


extern "C" {

//[[clang::optnone]]
volatile void* make_mem(int sz)
BEGIN
  //PREV_STK_PTR(prev);
  //return prev;
  return nullptr;
END

}

//[[clang::optnone]]
int main()
{
  //volatile auto m = alloca(16);
  auto prev = make_mem(8);
  STK_PTR(cur); 
  printf("prev: %lu cur: %lu \n", (u64)prev,  (u64)cur );
  
  return 0;
}

// volatile void** base = nullptr;
// __asm__ (
//   "movq rbp, %0 \n"
//   : "+rm" (base)
// );
// volatile void* prev = nullptr;
// prev = *base;

//volatile auto m = alloca(16);
//volatile char sixteen[16];
//volatile long long sz2 = sz;
//return malloc(sz);


