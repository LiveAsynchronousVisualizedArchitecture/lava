
// -todo: put in structure of extra error checking for shared library loading
// -todo: once tbl is switched to not be an array, this might not need to be a template - tbl is now untemplated
// -todo: allocation template parameters might mean that a template is still neccesary - tbl has allocation functions on the stack
// -todo: make convenience functions to iterate through packets
// -todo: make a main node that takes command line arguments or have a way to designate a starting node - might not need a main node since message nodes will be run anyway
// -todo: clean up LavaOut and LavaVal to make outputing easier - just utility function for now
// -todo: make Lava convenience function to make a tbl with the correct allocators
// -todo: make a Lava convenience function to make a tbl from a packet
// -todo: separate input slots and output slots into two different indices

// todo: make sure an error still runs the packet callback? does there need to be an error callback?
// todo: put in more error states into LavaInst
// todo: fill in error checking on shared library loading - need to make sure that the errors from nodes end up making it into their instances and ultimatly the GUI
// todo: give LavaNode struct a description string
// todo: write about design of LavaQ including that it is lock free, uses contiguous memory, does not rely on pointers and small allocations, and doesn't need versions since the start and end only increment - when a reader is reading a value, it can be sure that the buffer underneath hasn't been switched twice, because that would require inserting more values, which would increment end.... but end isn't atomicly linked to the start index - does switching buffers need to add the absolute capacity to both start and end ? 
// todo: use the capacity as a power of two exponent directly so that the modulo operator is avoided - would this mean masking with ~(0xFF << cap) to isolate the bits below the exponent as 0, then flipping all the bits so only the bit below the exponent are 1s, then applying bitwise AND to have only those bits from st and m_end ?

// idea: make pop into a template that can try to pull out a variable amount of values at one time
// idea: try using realloc with LavaQ 
//       | windows has a thread local heap have a realloc function
//       | the realloc function would need to be optional and there will need to be two different paths depending on whether the realloc function is nullptr or not 
//       | the realloc copying could be tricky, might need to fill the new slots in the buffer 
//       | if a new buffer is double the size of the old, the wrapped part of the buffer could simply be appended, but what does it mean for the only increasing m_end and st variables? 
//       | there should be an offset of the st and m_end wrapped around the previous buffer to the same numbers wrapped around the current buffer - after the all the elements are in one contiguous line, if the span moves backwards, start from the st and move it backwards, if the span moves everything forwards, start at the end and move it forwards - would the C lib memmove work?

#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __LAVAFLOW_HEADERGUARD_HPP__
#define __LAVAFLOW_HEADERGUARD_HPP__

#include <cstdint>
#include <bitset>
#include <chrono>
#include <atomic>
#include <memory>
#include <thread>
#include <mutex>
#include <array>
#include <string>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <regex>
#include <experimental/filesystem>

//#include "flat_lockfree_map.hpp"
#include "../no_rt_util.h"   // temp
#include "../tbl.hpp"        // temp

#if defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #define WIN32_EXTRA_LEAN
  #define NOMINMAX
  #include <Windows.h>
#elif defined(__APPLE__) || defined(__MACH__) || defined(__unix__) || defined(__FreeBSD__) // || defined(__linux__) ?    // osx, linux and freebsd
  #include <unistd.h>
#endif

#define LAVA_ARG_COUNT 512

#if defined(_MSC_VER)
  //namespace fs = std::tr2::sys;                                                             // todo: different compiler versions would need different filesystem paths
  namespace fs = std::experimental::filesystem;
#endif

#if defined(_WIN32)
  #define LAVA_SLEEP_MS(x) Sleep( (u32)(x) )               // windows sleep is in milliseconds
#elif defined(__APPLE__) || defined(__MACH__) || defined(__unix__) || defined(__FreeBSD__) // || defined(__linux__) ?    // osx, linux and freebsd
  #define LAVA_SLEEP_MS(x) sleep( (u32)((x)/1000.0) )      // unix sleep is in seconds
#endif

template<class T> struct LavaQ
{
// single producer, multi-consumer queue
// Design: Two buffers with a single 64 bit atomic that contains which buffer to use as well as the start and end of the queue
// Two function pointers contain the allocation and deallocation functions to use so that the data structure can cross and shared library boundary

  using      Tptr = T*;
  using      aPtr = std::atomic<T*>;
  using       u64 = uint64_t;
  using     abool = std::atomic<bool>;
  using       au8 = std::atomic<u8>;
  using      ai32 = std::atomic<i32>;
  using      au64 = std::atomic<u64>;
  using AllocFunc = void*(*)(size_t);
  using  FreeFunc = void(*)(void*);

  union     StBuf
  {
    struct { u64 useA : 1; u64 cap : 6; u64 st : 57; };
    u64 asInt = 0;
  };
  union    RefMem
  {
    struct{
      u64 m_addr : 48;
      u64  m_ref : 16;
    };
    mutable au64  atmInt;
    u64            asInt = 0;

    static const u64 DELETED = 0xFFFF;

    u64        ref() const 
    {
      RefMem ret;
      ret.asInt = atmInt.load();
      return ret.m_addr? ret.m_ref  :  0; // if m_addr is nullptr, return that there are 0 references
    }
    T*        addr() const
    {
      RefMem ret;
      ret.asInt = atmInt.load();
      return (T*)ret.m_addr;
    }
    void      addr(void* p) // todo: change this to a simple atomic store, since it will only be done by the single writing thread
    {
      RefMem nxt, prev;
      do{
        prev.asInt = nxt.asInt = atmInt.load();
        nxt.m_addr = (u64)p;
      }while( !atmInt.compare_exchange_strong(prev.asInt, nxt.asInt) );
    }
    T*  addrIncRef() const // todo: make this const
    {
      RefMem nxt, prev;
      do{
        prev.asInt  = nxt.asInt  = atmInt.load();
        if(nxt.m_addr)
          nxt.m_ref  += 1;
        else return nullptr;
      }while( !atmInt.compare_exchange_strong(prev.asInt, nxt.asInt) );

      return (T*)nxt.m_addr;
    }
    void    subRef() const
    {
      RefMem nxt, prev;
      do{
        prev.asInt  = nxt.asInt  = atmInt.load();
        if(nxt.m_addr)
          nxt.m_ref  -= 1;
        else return;
      }while( !atmInt.compare_exchange_strong(prev.asInt, nxt.asInt) );
    }
  };

//private:
  const u32 INITIAL_CAPACITY = 1;

  AllocFunc    m_alloc = nullptr;
  FreeFunc      m_free = nullptr;
  au64           m_end = 0;
  RefMem        m_memA;
  RefMem        m_memB;
  StBuf          m_buf;

  #ifndef NDEBUG
    std::thread::id  writeThrd;
  #endif

  void init()
  {
    m_buf.useA = 0;
    m_buf.st   = 0;
    m_buf.cap  = 0;

    #ifndef NDEBUG
      writeThrd = std::this_thread::get_id();
    #endif
  }

//public:
  LavaQ() : 
    m_alloc( malloc ),
    m_free( free )
  {
    init();
  }
  LavaQ(AllocFunc a, FreeFunc f) : m_alloc(a), m_free(f)
  {
    init();
  }
  ~LavaQ()
  {
    RefMem memA, memB;
    memA.asInt = m_memA.atmInt.load();
    memB.asInt = m_memB.atmInt.load();

    assert(memA.m_ref == 0);
    assert(memB.m_ref == 0);

    if(memA.m_addr)
      m_free( (T*)memA.m_addr );
    if(memB.m_addr)
      m_free( (T*)memB.m_addr );
  }

  StBuf         loadBuf()          const
  {
    StBuf ret;
    ret.asInt = ((au64*)&m_buf.asInt)->load();
    return ret;
  }
  u64              size(StBuf buf) const
  { 
    if(buf.cap == 0) return 0;
 
    auto cap = Capacity(buf.cap);
    auto  st = buf.st;
    auto  en = m_end.load();

    if(en >= st){
      return en - st;
    }else{
      return cap - st + en;
    }
  }
  u64              size()          const
  { 
    return size(loadBuf());
  }
  bool              pop(T& ret)    const
  {
    // This will make sure the index (which will only increase) hasn't changed while incrementing the start
    // Also makes sure the capacity hasn't changed, since that will dictate the other side of the modulo operator
    // Ultimatly the index is made from buf.st and buf.cap
    StBuf prev, buf;
    au64* abuf = (au64*)&m_buf.asInt;
    auto    en = m_end.load();
    prev = buf = loadBuf();

    if( size(buf) == 0 ) return false;

    u64 idx = Idx(buf);
    if( buf.useA ){
      ret = atA(idx);
    }else{ 
      ret = atB(idx);
    }

    buf.st += 1;
    bool ok = abuf->compare_exchange_strong(prev.asInt, buf.asInt);

    return ok;
  }
  T                 atA(u64 i)     const
  {
    // mem should never be nullptr, since it is updated to the new pointer atomically, and never set to nullptr as an intermediate 
    // the buffers are only nullptr on initialization

    auto mem = m_memA.addrIncRef();
      T ret = mem[i];
    m_memA.subRef();
    return ret;
  }
  T                 atB(u64 i)     const
  {
    // mem should never be nullptr, since it is updated to the new pointer atomically, and never set to nullptr as an intermediate 
    // the buffers are only nullptr on initialization

    auto mem = m_memB.addrIncRef();
      T ret = mem[i];
    m_memB.subRef();
    return ret;
  }
  void            clear()          const
  {
    T val;
    while(size()>0)          // there is probably a better way, though it would have to be thread safe
      pop(val);
  }

  StBuf   switchBuffers(u64 nxtCapExp)
  {
    // It doesn't matter if the start and end have changed when copying the memory from one buffer to the other, since the reading threads will only increment the start
    StBuf prev, buf;
    au64* abuf = (au64*)&m_buf.asInt;
    do{
      prev = buf =   loadBuf();
      buf.useA   =  !buf.useA;
      buf.cap    =   nxtCapExp;
    }while( !abuf->compare_exchange_strong(prev.asInt, buf.asInt) );

    return prev;
  }
  u64            incEnd()
  {
    assert(std::this_thread::get_id() == writeThrd);
    return m_end.fetch_add(1);
  }
  u64            expand(StBuf buf, RefMem& prevMem, RefMem& nxtMem)
  {
    using namespace std;

    auto prevCap   = Capacity(buf.cap);
    auto nxtCapExp = buf.cap + 1;
    auto nxtCap    = Capacity( nxtCapExp );
    void* nxtAlloc = m_alloc( nxtCap * sizeof(T) );       // the capacity functions will get the absolute value from the power of 2 that is stored in the 1 byte capacity variables

    while( nxtMem.ref() != 0 ){
      this_thread::sleep_for( 0ms );                       // todo: check if yield holds any advantages here
    }
    T* oldAddr = nxtMem.addr();                            // make sure the references are 0 - they can't go up here, since any new read would use the other buffer
    nxtMem.addr( nxtAlloc );                               // 1
    if(oldAddr) m_free(oldAddr);

    T* nxtAddr = (T*)nxtAlloc;
    T* prvAddr = prevMem.addr();
    auto    en = m_end.load();
    for(u64 i=buf.st; i<en; ++i){                          // other threads can increment buf.st, but that should only mean some of the copied elements have already been popped and not cause an actual problem
      auto prevIdx = i % prevCap;
      auto  nxtIdx = i % nxtCap;
      nxtAddr[nxtIdx] = prvAddr[prevIdx];                  // only this thread can change anything, so no other thread should interfere with the actual data
    }

    return nxtCapExp;
  }
  void           expand()
  {
    assert(std::this_thread::get_id() == writeThrd);

    // 0. assume that the opposite buffer pointer is nullptr
    // 1. make another allocation 
    // 2. copy the data
    // 3. switch the buffers
    // 4. deallocate the previous allocation

    using namespace std;

    auto       buf = loadBuf();
    auto        en = m_end.load();
    u64  nxtCapExp = 0;
    if( m_memA.addr()==nullptr && m_memB.addr()==nullptr ){
      nxtCapExp = INITIAL_CAPACITY;
      m_memA.addr( m_alloc( Capacity(nxtCapExp) *sizeof(T)) );              // initial allocation
    }else if( buf.useA ){
      nxtCapExp = expand(buf, m_memA, m_memB);
    }else{
      nxtCapExp = expand(buf, m_memB, m_memA);
    }

    switchBuffers(nxtCapExp);                                       // this has to make sure that the start and end stay the same, but the current thread is the only one that can change the buffer bit or the end. 
  }
  StBuf            push(T const& val)
  {
    assert(std::this_thread::get_id() == writeThrd);

    // This increments the end m_end variable, which sits at one beyond the last index, just like STL iterators
    // T must be dealt with by value and can be arbitrary sized because there won't be multiple writing threads - writes don't need to be atomic, just write into the unused slot and increment m_end
    // if the capacity is not high enough, use a secondary buffer to enlarge it
    // this should work well, since only the owning thread can make it larger, while any thread can pop() items from the queue
    
    auto buf = loadBuf();
    if(buf.cap==0 || size(buf) >= Capacity(buf.cap)-1 ){
      expand();
      buf = loadBuf();
    }

    T*   mem = buf.useA? m_memA.addr() : m_memB.addr();
    auto idx = m_end.load() % Capacity(buf.cap);
    mem[idx] = val;

    incEnd();

    return buf;
  }

  static u64   Capacity(u64 capExp)          // capExp is capacity exponent
  {
    return (1 << capExp) >> 1;
  }
  static u64        Idx(StBuf buf)
  {
    return buf.st % Capacity(buf.cap);
  }
};

static inline u64 popcount64(u64 x)
{
  /*
  * libpopcnt.h - C/C++ library for counting the number of 1 bits (bit
  * population count) in an array as quickly as possible using
  * specialized CPU instructions e.g. POPCNT, AVX2.
  *
  * Copyright (c) 2016 - 2017, Kim Walisch
  * Copyright (c) 2016 - 2017, Wojciech Mula
  *
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions are met:
  *
  * 1. Redistributions of source code must retain the above copyright notice, this
  *    list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
  * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  */

  u64 m1 = 0x5555555555555555ll;
  u64 m2 = 0x3333333333333333ll;
  u64 m4 = 0x0F0F0F0F0F0F0F0Fll;
  u64 h01 = 0x0101010101010101ll;

  x -= (x >> 1) & m1;
  x = (x & m2) + ((x >> 2) & m2);
  x = (x + (x >> 4)) & m4;

  return (x * h01) >> 56;

  // GCC built in intrinsic - int  __builtin_popcount(unsigned int)
  // End libpopcnt
}

// static data segment data
#if defined(_WIN32)
  //static const std::string  liveExt(".live.dll");                            // todo: change this to const char* - don't want static intialization functions running
  static const char* liveExt = ".live.dll";                                    // todo: change this to const char* - don't want static intialization functions running
#endif

//static __declspec(thread)       void*   lava_thread_heap = nullptr;          // thread local handle for thread local heap allocations
thread_local       void*   lava_thread_heap = nullptr;       // thread local handle for thread local heap allocations
// end data segment data

// function declarations
BOOL WINAPI DllMain(
  _In_  HINSTANCE     hinstDLL,
  _In_  DWORD        fdwReason,
  _In_  LPVOID     lpvReserved
);

// allocator definitions
inline void    LavaHeapDestroyCallback(void* heapHnd)
{
  if(heapHnd)
    HeapDestroy(heapHnd);
}
inline void*      LavaHeapInit(size_t initialSz = 0)
{
  if(!lava_thread_heap) {
    lava_thread_heap = HeapCreate(HEAP_NO_SERIALIZE, initialSz, 0);
  }
  return lava_thread_heap;
}
inline void*     LavaHeapAlloc(size_t sz)
{
  void* thread_heap = LavaHeapInit(sz);

  void* ret = nullptr;
  if(thread_heap)
    ret = HeapAlloc(thread_heap, HEAP_NO_SERIALIZE, sz);

  return ret;
}
inline void*   LavaHeapReAlloc(void* memptr, size_t sz)
{
  void* thread_heap = LavaHeapInit(sz);

  // HeapReAlloc has exception codes that can be checked and put in to a thread local error code
  void* ret = nullptr;
  if(thread_heap)
    ret = HeapReAlloc(thread_heap, HEAP_REALLOC_IN_PLACE_ONLY|HEAP_NO_SERIALIZE, memptr, sz);

  return ret;
}
inline void       LavaHeapFree(void* memptr)
{
  void* thread_heap = LavaHeapInit();

  //void* ret = nullptr;
  bool ret = false;
  if(thread_heap && memptr) {
    ret = HeapFree(thread_heap, HEAP_NO_SERIALIZE, memptr);
    //return ret;
  }//else{ return 0; }
}

template <class T> struct  ThreadAllocator
{
  using value_type  =  T;

  template<class U> 
  struct rebind
  {
    using other = ThreadAllocator<U>;
  };

  ThreadAllocator() /*noexcept*/ {}   // default ctor not required by STL

                                      // A converting copy constructor:
  template<class U> __declspec(noinline) ThreadAllocator(const ThreadAllocator<U>&) /*noexcept*/ {}
  template<class U> __declspec(noinline) bool operator==(const ThreadAllocator<U>&) const /*noexcept*/
  {
    return true;
  }
  template<class U> __declspec(noinline) bool operator!=(const ThreadAllocator<U>&) const /*noexcept*/
  {
    return false;
  }
  __declspec(noinline) T*    allocate(const size_t n)     const;
  __declspec(noinline) void  deallocate(T* const& p, size_t)    const;

  template<class U, class... ARGS>
  __declspec(noinline) void construct(U* p, ARGS&&... args)
  {
    ::new ((void*)p) U(std::forward<ARGS>(args)...);
  }
  template<class U>
  __declspec(noinline) void destroy(U* p)
  {
    p->~U();
  }
};
template <class T> T*      ThreadAllocator<T>::allocate(const size_t n)  const
{
  if(n==0) return nullptr;

  if( n > static_cast<size_t>(-1)/sizeof(T) ) {
    throw std::bad_array_new_length();
  }

  //void* p = ThreadAlloc( n * sizeof(T) );
  void* p = LavaHeapAlloc( n * sizeof(T) );
  if(!p) { throw std::bad_alloc(); }

  return static_cast<T*>(p);
}
template <class T> void    ThreadAllocator<T>::deallocate(T* const& p, size_t) const
{
  LavaHeapFree(p);
}
// end allocator definitions

// data types
struct       LavaNode;
struct     LavaParams;
struct      LavaFrame;
struct        LavaVal;
struct        LavaOut;
union          LavaId;
class       LavaGraph;
struct        LavaMem;
struct     LavaPacket;
struct      LavaConst;

enum class LavaControl { STOP=0, GO=1 };

using str                =  std::string;
using lava_handle        =  HMODULE;                                                      // maps handles to the LavaFlowNode pointers contained in the shared libraries
using lava_paths         =  std::vector<std::string>;
using lava_hndlNodeMap   =  std::unordered_multimap<lava_handle, LavaNode*>;              // maps handles to the LavaFlowNode pointers contained in the shared libraries
using lava_libHndls      =  std::unordered_map<LavaNode*, lava_handle>;                   // todo: need to change this depending on OS
using lava_hndlvec       =  std::vector<lava_handle>;                                     // todo: need to change this depending on OS
using lava_pathHndlMap   =  std::unordered_map<std::string, lava_handle>;                 // maps LavaFlowNode names to their OS specific handles
using lava_flowNodes     =  std::unordered_multimap<std::string, LavaNode*>;              // maps LavaFlowNode paths to their pointers
using lava_nidMap        =  std::unordered_multimap<std::string, uint64_t>;               // maps LavaFlowNode names to their ids 
using lava_flowPtrs      =  std::unordered_set<LavaNode*>;                                // LavaFlowNode pointers referenced uniquely by address instead of using an id
using lava_ptrsvec       =  std::vector<LavaNode*>;
using lava_nameNodeMap   =  std::unordered_map<std::string, LavaNode*>;                   // maps the node names to their pointers
using lava_threadQ       =  LavaQ<LavaOut>;

extern "C" using       LavaAllocFunc  =  void* (*)(uint64_t);                             // custom allocation function passed in to each node call
extern "C" using     LavaReallocFunc  =  void* (*)(void*, uint64_t);                      // custom allocation function passed in to each node call
extern "C" using        LavaFreeFunc  =  void  (*)(void*);                                // custom allocation function passed in to each node call
extern "C" using  GetLavaFlowNodes_t  =  LavaNode*(*)();                                               // the signature of the function that is searched for in every shared library - this returns a LavaFlowNode* that is treated as a sort of null terminated list of the actual nodes contained in the shared library 
extern "C" using            FlowFunc  =  uint64_t (*)(LavaParams const*, LavaFrame const*, lava_threadQ*);   // node function taking a LavaFrame in
extern "C" using       ConstructFunc  =  void(*)();
//extern "C" using      PacketCallback  =  LavaControl (*)();
extern "C" using      PacketCallback  =  LavaControl (*)(LavaPacket* pkt);

struct   AtomicBitset
{
  using   u64 = uint64_t;
  using  au64 = std::atomic<u64>;

  struct Hndl
  {
    AtomicBitset*  p;
    u8           bit;
    
    operator bool()
    {
      au64* abits = (au64*)(&p->bits);
      u64    bits = abits->load();
      return (bool) ((bits >> bit) & 0x1);
    }
    Hndl& operator=(bool b)
    {
      u64 prevBits, nxtBits;
      au64*  abits = (au64*)(&p->bits);
      do{
        prevBits = nxtBits = abits->load();
        if(b) nxtBits |=    (u64)0x1 << bit;            // or with the proper bit set to 1
        else  nxtBits &= ~( (u64)0x1 << bit);           // and with the proper bit set to 0 and everything else set to 1
      }while( !abits->compare_exchange_strong(prevBits, nxtBits) );
      
      return *this;
    }
  };
  struct  Str
  {
    char bitstr[65];
    operator char*(){ return (char*)bitstr; }
  };

  u64   bits = 0;

  Hndl operator[](u64 bit)
  {
    Hndl h;
    h.p   = this;
    h.bit = (u8)bit;

    return h;
  }
  bool operator[](u64 bit) const
  {
    au64* abits = (au64*)(&bits);
    u64    bits = abits->load();
    return (bool) ((bits >> bit) & 0x1);
  }
  u8        count()        const  // make x an atomic load
  {
    auto x = bits;
    u64 m1 = 0x5555555555555555ll;
    u64 m2 = 0x3333333333333333ll;
    u64 m4 = 0x0F0F0F0F0F0F0F0Fll;
    u64 h01 = 0x0101010101010101ll;

    x -= (x >> 1) & m1;
    x = (x & m2) + ((x >> 2) & m2);
    x = (x + (x >> 4)) & m4;

    return (x * h01) >> 56;
    // return popcount64(bits);
  }
  Str       toStr()
  {
    Str ret;
    for(i32 i=0; i<64; ++i){
      ret.bitstr[63-i] = (*this)[i]?  '1'  :  '0';
    }
    ret.bitstr[64] = '\0';

    return ret;
  }
};
union     LavaArgType{ 
  enum { NONE=0, END, DATA_ERROR, STORE, MEMORY, PASSTHRU, SEQUENCE, ENUMERATION };                // todo: does this need store sequence and memory sequence? // PASSTHROUGH is unfortunatly taken up by the windows gdi header
  u8 asInt;
};
union          LavaId                                            // this Id serves as both a nodeId and LavaFlowSlot index, since a LavaFlowSlot index will alway coordinate with only one node 
{    
  static  const u64 NODE_NONE = 0xFFFFFFFFFFFF;                  // 48 bits all set
  static  const u64 SLOT_NONE = 0xFFFF;                          // 16 bits all set

  struct { 
    u64  isIn :  1;
    u64  sidx : 16;                                       // idx is the index of the LavaFlowSlot - for a node this is 0 - todo: convert to SLOT_NONE
    u64   nid : 47;                                       // id is the node id number - This is a unique number for each node that doesn't change. It can refer back to a node since it doesn't represent an ordering or an index into an array 
  };
  u64 asInt;

  LavaId() : nid(NODE_NONE), sidx(SLOT_NONE) {}
  LavaId(u64 _id, u64 _idx=SLOT_NONE, u64 _isIn=1) : 
    nid(_id), sidx(_idx), isIn(_isIn) {}

  bool   operator==(LavaId  r) const { return nid==r.nid && sidx==r.sidx && isIn==r.isIn; }
  bool    operator<(LavaId const& r)   const {
    if(nid==r.nid) return sidx < r.sidx;
    else         return nid  < r.nid;
  }
  size_t operator()(LavaId const& _id) const {
    return std::hash<u64>()(_id.nid) ^ std::hash<u64>()(_id.sidx);
  }
};
struct     LavaParams
{
  u32                    inputs;
  u64                     cycle;
  LavaId                     id;

  LavaAllocFunc       ref_alloc;
  LavaReallocFunc   ref_realloc;
  LavaFreeFunc         ref_free;

  LavaAllocFunc     local_alloc;
  LavaReallocFunc local_realloc;
  LavaFreeFunc       local_free;

  FILE*             lava_stdout;
  FILE*              lava_stdin;
  FILE*             lava_stderr;
};
struct        LavaVal
{
  u64      type :  3;          // ArgType
  u64     value : 61;

  operator void*(){ return (void*)value; }
};
struct        LavaOut
{
  LavaVal val;

  union {
    struct { u64 cycle; u32 slot; u32 listIdx; };
    u8 bytes[16];
  }key;

  LavaOut() : key{0,0,0}, val{0,0} {}
  LavaOut(u32 slot, u64 value) : key{0,slot,0}, val{LavaArgType::MEMORY,value}
  {}
  LavaOut(u32 slot, u64 value, u64 type) : key{0,slot,0}, val{type,value}
  {}

  // type(0), value(0) {}
  // value(val), type(LavaArgType::MEMORY)
  // value(val), type(_type)
};
struct     LavaPacket
{
  //u64    ref_count;                               // todo: is this used? 
  u64        cycle : 55;
  u64       framed :  1;
  u64     priority :  8;
  u64    dest_node : 48;
  u64    dest_slot : 16;
  u64     src_node : 48;
  u64     src_slot : 16;
  u64   rangeStart;
  u64     rangeEnd;
  u64     sz_bytes;                               // the size in bytes can be used to further sort the packets so that the largets are processed first, possibly resulting in less memory usage over time
  u64           id;
  LavaVal      val;
  //LavaMsg      msg;

  bool operator<(LavaPacket const& r) const
  {    
    if(cycle    != r.cycle)    return cycle    > r.cycle;               // want lowest frame numbers to be done first 
    if(priority != r.priority) return priority < r.priority;            // want highest priority to be done first
    if(sz_bytes != r.sz_bytes) return sz_bytes < r.sz_bytes;            // want largest sizes to be done first
    if(framed   != r.framed)   return framed   < r.framed;              // want framed packets to be done first so that their dependencies can be resolved

    return id > r.id;
  }
};
struct      LavaFrame
{
  enum FRAME { ERR_FRAME = 0xFFFFFFFFFFFFFFFE, NO_FRAME = 0xFFFFFFFFFFFFFFFF };
  static const u64 PACKET_SLOTS = 16;

  using       Slots = AtomicBitset;
  using PacketArray = std::array<LavaPacket, PACKET_SLOTS>;

  u64                dest = LavaId::NODE_NONE;             // The destination node this frame will be run with
  u64               cycle = 0;                             // The numer of this frame - lowest frame needs to be run first
  u64           src_frame = 0;                             // Does this need to come from the message node?
  u64          dest_frame = 0;                             // should this come from the node instance?
  Slots          slotMask;                                 // The bit mask respresenting which slots already have packets in them

  u16               slots = 0;                             // The total number of slots needed for this frame to be complete 
  PacketArray     packets;
  //LavaPacket      packets[PACKET_SLOTS];

  bool          putSlot(u64 sIdx, LavaPacket const& pkt)
  {
    if( slotMask[sIdx] ) return false;

    slotMask[sIdx] = true;
    packets[sIdx] = pkt;

    return true;
  }
  u64         slotCount()         const
  {
    //return popcount64(slotMask.to_ulong());
    return slotMask.count();
  }
  bool        allFilled()         const
  {
    if( slotCount() >= slots ) return true;
    return false;
  }

};
struct       LavaNode
{
  enum Type { NONE=0, FLOW=1, MSG=2, CONSTANT=3, GENERATOR=4, JOINT=5, NODE_ERROR=0xFFFFFFFFFFFFFFFF };                              // this should be filled in with other node types like scatter, gather, transform, generate, sink, blocking sink, blocking/pinned/owned msg - should a sink node always be pinned to it's own thread

  //FlowFunc               func = nullptr;      // todo: change to a union that also holds the memory pointer
  //ConstructFunc   constructor = nullptr;      // todo: make into a union that also holds the constant node file length
  //ConstructFunc    destructor = nullptr;      // todo: change to a union that also holds the file handle

  union{
    FlowFunc               func;                // todo: change to a union that also holds the memory pointer
    void*               filePtr = nullptr;
  };
  union{
    ConstructFunc   constructor = nullptr;      // todo: make into a union that also holds the constant node file length
    uint64_t           fileSize;
  };
  union{
    ConstructFunc    destructor = nullptr;      // todo: change to a union that also holds the file handle
    #ifdef _WIN32
      void*      fileHndl;
    #elif defined(__APPLE__) || defined(__MACH__) || defined(__unix__) || defined(__FreeBSD__) // || defined(__linux__) ?    // osx, linux and freebsd
      int        fileHndl;
    #endif
  };
  
  Type              node_type = NONE;
  const char*            name = nullptr;
  const char**       in_types = nullptr;
  const char**       in_names = nullptr;
  const char**      out_types = nullptr;
  const char**      out_names = nullptr;
  const char*     description = nullptr;
  uint64_t            version = 0;
};
struct       LavaInst
{
  using au32  =  std::atomic<uint32_t>;
  using au64  =  std::atomic<uint64_t>;

  enum State { NORMAL=0, OUTPUT_ERROR, RUN_ERROR, LOAD_ERROR, COMPILE_ERROR };              // OUTPUT_ERROR is for nodes who output a value that contains a nullptr instead of a valid memory allocation

  LavaId         id;
  LavaNode*    node;
  u32        inputs;
  u32       outputs;
  union{ State state = NORMAL; u32 stateU32; };

  mutable u64   cycle = 0;  // todo: make this atomic 
  mutable u64    time = 0;  // todo: make this atomic / make functions to add time and get the current time

  bool operator<(LavaInst const& lval){ return id < lval.id; }

  u64   fetchIncFrame() const
  {
    return std::atomic_fetch_add( (au64*)(&cycle), 1);
  }
  void       setState(LavaInst::State s) // -> LavaInst::State
  {
    ((au32*)(&state))->store(s);
    //return this->state;
  }
  auto       getState() -> LavaInst::State
  {
    stateU32 = ((au32*)(&stateU32))->load();
    return state;
  }
  u64         addTime(u64 t)
  {
    return ((au64*)(&time))->fetch_add(t);
  }
  u64       clearTime()
  {
    return ((au64*)(&time))->exchange(0);
  }
};
struct   LavaFlowSlot
{ 
  enum State { NORMAL=0, HIGHLIGHTED, SELECTED, SLOT_ERROR };  // todo: highlighted and selected should be moved to the UI, NORMAL and SLOT_ERROR should stay

  LavaId id; bool in=false; State state=NORMAL;

  LavaFlowSlot(){}
  LavaFlowSlot(LavaId Id, bool In=false) : id(Id), in(In), state(NORMAL) {}
};
struct  LavaMemHeader
{
  using au64 = std::atomic<uint64_t>;

  au64  refCount;
  u64  sizeBytes;
};
struct        LavaMem
{
  using au64 = std::atomic<uint64_t>;
   
  void*  ptr = nullptr;

  uint64_t    refCount()const{ return ((uint64_t*)ptr)[0]; }
  uint64_t   sizeBytes()const{ return ((uint64_t*)ptr)[1]; }
  uint64_t&   refCount()     { return ((uint64_t*)ptr)[0]; }
  uint64_t&  sizeBytes()     { return ((uint64_t*)ptr)[1]; }
  void*           data()     { return ((uint64_t*)ptr)+2;  }

  uint64_t      incRef()     { return ((au64*)ptr)->fetch_add( 1); }
  uint64_t      decRef()
  { 
    au64* ap = (au64*)ptr;
    assert(ap->load() != 0);
    return ap->fetch_add(-1);
  }

  static LavaMem fromDataAddr(uint64_t addr)                                       // create a LavaMem struct from the address of the start of the data (after the reference count and sizeBytes at the start of the allocation)
  {
    LavaMem lm;
    lm.ptr =  (void*)(addr - sizeof(uint64_t)*2);
    assert(lm.refCount() < 1000);
    return lm;
  }
};
struct    LavaCommand
{
  enum Command { ADD_NODE=0, ADD_SLOT, TGL_CNCT, DEL_NODE, DEL_CNCT };

  union Arg { 
    Arg() : id() {} 
    LavaId id; u64 val; LavaNode* ndptr; bool slotDest;
  };

  Command cmd; 
  union { Arg A; Arg dest; Arg nd; };
  union { Arg B; Arg  src; };
};

const        LavaNode LavaNodeListEnd = {nullptr, nullptr, nullptr, LavaNode::NONE, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, 0};

struct      LavaConst
{
  //struct ConstMem  //{  //};
  //LavaConst(LavaNode const& n, str const& typeStr)

  LavaNode* node = nullptr; 

  const char** makeStrLst(str const& s)
  {
    u64 strMemLen   =  (s.length()+1) * sizeof(char);
    u64 ptrMemLen   =  sizeof(void*) * 2;
    void** strLst   =  (void**)malloc(ptrMemLen + strMemLen);                                     // allocate memory for the string + two pointers, since the node types are a nullptr terminated list of C strings
    strLst[0]       =  &(strLst[2]);                                                              // set the first 8 bytes to be a pointer to the start of the first type string (which skips over the null pointer that ends the list
    strLst[1]       =  nullptr;                                                                   // make the second 8 bytes be nullptr (0) so that the list of string is nullptr terminated
    memcpy(strLst[0], s.c_str(), strMemLen); 

    return (const char**)strLst;
  }
  LavaConst& mv(LavaConst&& rval)
  {
    node      = rval.node;
    rval.node = nullptr;
    return *this;
  }

  LavaConst() : node(nullptr) {}
  //LavaConst(void* cnstMem, str const& nameStr, str const& typeStr)
  LavaConst(str const& nameStr, str const& typeStr)
  {
    // the node pointer and all of the node's pointers need to point to heap memory so that the addresses don't change

    node             =  (LavaNode*)malloc(sizeof(LavaNode) * 2);                                    // because there is no LavaNode struct as a static embedded in a shared library
    node[0]          =  LavaNode();                                                                 // make sure the LavaNode is initialized to default values 
    node[1]          =  LavaNodeListEnd;
    //node->filePtr    =  cnstMem;
    node->node_type  =  LavaNode::CONSTANT;
    node->out_types  =  makeStrLst(typeStr);

    char* name       =  (char*)malloc( nameStr.length()+1 );
    strcpy(name, nameStr.c_str());
    node->name       =  name;
  }
  ~LavaConst()
  {
    if(node)
    {
      if(node->name)
        free( (void*)node->name );

      if(node->out_types)
        free(node->out_types);

      free(node);
    }
  }

  LavaConst(LavaConst&& rval){ mv(std::move(rval)); }
  LavaConst& operator=(LavaConst&& rval){ return mv(std::move(rval)); }

  LavaConst(LavaConst const& lval) = delete;
  void operator=(LavaConst& lval)  = delete;
};
// end data types

extern "C" __declspec(dllexport) LavaNode* GetLavaFlowNodes();   // prototype of function to return static plugin loading struct
// end function declarations

using lava_memvec = std::vector<LavaMem, ThreadAllocator<LavaMem> >;

class       LavaGraph                  // LavaGraph should specifically be about the connections between nodes
{
public:
  enum       Cmd { ADD_NODE=0, ADD_SLOT, TGL_CNCT, DEL_NODE, DEL_CNCT };
  union      Arg { LavaId id; u64 val; LavaNode* ndptr; };
  struct Command { Cmd cmd; Arg A; Arg B; };

  struct NodeInstance { uint64_t id; LavaNode* nd; };                  // not used? - a struct used for returning an instance of a node - the Nodes map of ids and LavaFlowNode pointers  

  using abool         =  std::atomic<bool>;
  using au32          =  std::atomic<uint32_t>;
  using NodeInsts     =  std::unordered_map<uint64_t, LavaInst>;       // maps an id to a LavaFlowNode struct
  using Slots         =  std::multimap<LavaId, LavaFlowSlot>;          // The key is a node id, the value is the index into the slot array.  Every node can have 0 or more slots. Slots can only have 1 and only 1 node. Slots have their node index in their struct so getting the node from the slots is easy. To get the slots that a node has, this multimap is used
  using CnctMap       =  std::unordered_map<LavaId, LavaId, LavaId>;   // maps connections from their single destination slot to their single source slot - Id is the hash function object in the third template argument
  using SrcMap        =  std::multimap<LavaId, LavaId>;                // maps connections from their single source slot to their one or more destination slots
  using vec_insts     =  std::vector<LavaInst>;                        // list of node instances - Id and pointer pairs
  using vec_nptrs     =  std::vector<LavaInst>;                        // lists used for returning from reloading functions
  using vec_cnptrs    =  std::vector<LavaNode const*>;
  using vec_ids       =  std::vector<LavaId>;
  using GenIds        =  std::unordered_set<LavaId, LavaId>;           // LavaId has an operator() to hash itself
  using GenCache      =  std::vector<LavaId>;                          // LavaId has an operator() to hash itself
  using NormalizeMap  =  std::map<uint64_t, uint64_t>;
  using CmdQ          =  std::queue<LavaCommand>;
  using RetStk        =  std::stack<LavaCommand::Arg>;
  using ArgVec        =  std::vector<LavaCommand::Arg>;

private:
  mutable abool         m_useA;

  uint64_t             m_nxtId;               // nxtId is next id - a counter for every node created that only increases, giving each node a unique id
  CmdQ                  m_cmdq;
  RetStk                 m_stk;

  NodeInsts           m_nodesA;
  Slots             m_inSlotsA;
  Slots            m_outSlotsA;
  CnctMap             m_cnctsA;
  SrcMap          m_destCnctsA;
  GenIds           m_genNodesA;
  GenCache         m_genCacheA;

  NodeInsts           m_nodesB;
  Slots             m_inSlotsB;
  Slots            m_outSlotsB;
  CnctMap             m_cnctsB;
  SrcMap          m_destCnctsB;
  GenIds           m_genNodesB;
  GenCache         m_genCacheB;

public:
  NodeInsts&            curNodes(){ return m_useA.load()?  m_nodesA     : m_nodesB;     }
  Slots&              curInSlots(){ return m_useA.load()?  m_inSlotsA     : m_inSlotsB;     }
  Slots&             curOutSlots(){ return m_useA.load()?  m_outSlotsA     : m_outSlotsB;     }
  CnctMap&              curCncts(){ if(m_useA.load()) return m_cnctsA; else return m_cnctsB; }
  SrcMap&           curDestCncts(){ return m_useA.load()?  m_destCnctsA : m_destCnctsB; }
  GenIds&            curGenNodes(){ return m_useA.load()?  m_genNodesA  : m_genNodesB;  }
  GenCache&          curMsgCache(){ return m_useA.load()?  m_genCacheA  : m_genCacheB;  }
  NodeInsts const&      curNodes()const{ return m_useA.load()?  m_nodesA     : m_nodesB;     }
  Slots     const&    curInSlots()const{ return m_useA.load()?  m_inSlotsA     : m_inSlotsB;     }
  Slots     const&   curOutSlots()const{ return m_useA.load()?  m_outSlotsA     : m_outSlotsB;     }
  CnctMap   const&      curCncts()const{ return m_useA.load()?  m_cnctsA     : m_cnctsB;     }
  SrcMap    const&  curDestCncts()const{ return m_useA.load()?  m_destCnctsA : m_destCnctsB; }
  GenIds    const&   curGenNodes()const{ return m_useA.load()?  m_genNodesA  : m_genNodesB;  }

  NodeInsts&            oppNodes(){ 
    return !m_useA.load()?  m_nodesA     : m_nodesB;     }
  Slots&              oppInSlots(){ return !m_useA.load()?  m_inSlotsA     : m_inSlotsB;     }
  Slots&             oppOutSlots(){ return !m_useA.load()?  m_outSlotsA     : m_outSlotsB;     }
  CnctMap&              oppCncts(){ if(!m_useA.load()) return m_cnctsA; else return m_cnctsB; }
  SrcMap&           oppDestCncts(){ return !m_useA.load()?  m_destCnctsA : m_destCnctsB; }
  GenIds&            oppGenNodes(){ return !m_useA.load()?  m_genNodesA  : m_genNodesB;  }
  GenCache&          oppMsgCache(){ return !m_useA.load()?  m_genCacheA  : m_genCacheB;  }
  NodeInsts const&      oppNodes()const{ return !m_useA.load()?  m_nodesA     : m_nodesB;     }
  Slots     const&    oppInSlots()const{ return !m_useA.load()?  m_inSlotsA   : m_inSlotsB;   }
  Slots     const&   oppOutSlots()const{ return !m_useA.load()?  m_outSlotsA  : m_outSlotsB;  }
  CnctMap   const&      oppCncts()const{ return !m_useA.load()?  m_cnctsA     : m_cnctsB;     }
  SrcMap    const&  oppDestCncts()const{ return !m_useA.load()?  m_destCnctsA : m_destCnctsB; }
  GenIds    const&   oppGenNodes()const{ return !m_useA.load()?  m_genNodesA  : m_genNodesB;  }


  void            init()
  { 
    m_useA.store(true);
    m_nxtId = 1;
  }
  void              mv(LavaGraph&& rval)
  {
    using namespace std;

    m_nodesA     = move(rval.m_nodesA); 
    //m_slotsA     = move(rval.m_slotsA); 
    m_inSlotsA   = move(rval.m_inSlotsA); 
    m_outSlotsA  = move(rval.m_outSlotsA); 
    m_cnctsA     = move(rval.m_cnctsA); 
    m_destCnctsA = move(rval.m_destCnctsA);

    m_nodesB     = move(rval.m_nodesB); 
    //m_slotsB     = move(rval.m_slotsB); 
    m_inSlotsB   = move(rval.m_inSlotsB); 
    m_outSlotsB  = move(rval.m_outSlotsB);
    m_cnctsB     = move(rval.m_cnctsB); 
    m_destCnctsB = move(rval.m_destCnctsB);

    //m_running   = rval.m_running;
    //m_ids       = move(rval.m_ids);
  }
  void              cp(LavaGraph const& lval)
  {
    using namespace std;

    m_nodesA     = lval.m_nodesA;
    //m_slotsA     = lval.m_slotsA; 
    m_inSlotsA   = lval.m_inSlotsA; 
    m_outSlotsA  = lval.m_outSlotsA; 
    m_cnctsA     = lval.m_cnctsA; 
    m_destCnctsA = lval.m_destCnctsA;

    m_nodesB     = lval.m_nodesB;
    //m_slotsB     = lval.m_slotsB;
    m_inSlotsB   = lval.m_inSlotsB; 
    m_outSlotsB  = lval.m_outSlotsB;
    m_cnctsB     = lval.m_cnctsB; 
    m_destCnctsB = lval.m_destCnctsB;
  }
  u64            nxtId(){ return m_nxtId++; }

  auto   nodeDestSlots(vec_nptrs const& nds) -> vec_ids
  {
    using namespace std;

    vec_ids sidxs;                                            // sidxs is slot indexes
    for(auto ni : nds){                                       // ni is Node Instance np is node pointer and nds is nodes
      auto si = lower_bound(ALL(curInSlots()), LavaId(ni.id.nid), [](auto a,auto b){ return a.first < b; } );          // si is slot iterator
      if(si != end(curInSlots())  &&  si->first.nid == ni.id.nid){
        LavaFlowSlot& s = si->second;
        sidxs.push_back(si->first);
      }

      //auto si = lower_bound(ALL(curSlots()), LavaId(ni.id.nid), [](auto a,auto b){ return a.first < b; } );          // si is slot iterator
      //if(si != end(curSlots())  &&  si->first.nid == ni.id.nid){
      //  LavaFlowSlot& s = si->second;
      //  if(s.in) sidxs.push_back(si->first);
      //}
    }
    return sidxs;                                        // RVO
  }
  auto       nodeSlots(vec_nptrs const& nds) -> vec_ids
  {
    using namespace std;

    vec_ids sidxs;                                            // sidxs is LavaFlowSlot indexes
    for(auto ni : nds)
    {                                       // np is node pointer and nds is nodes
      auto inSi = lower_bound(ALL(curInSlots()), LavaId(ni.id.nid), [](auto a,auto b){ return a.first < b; } );          // si is slot iterator
      if(inSi != end(curInSlots())  &&  inSi->first.nid == ni.id.nid){
        LavaFlowSlot& s = inSi->second;
        sidxs.push_back(inSi->first);
      }

      auto outSi = lower_bound(ALL(curOutSlots()), LavaId(ni.id.nid), [](auto a,auto b){ return a.first < b; } );          // si is slot iterator
      if(outSi != end(curOutSlots())  &&  outSi->first.nid == ni.id.nid){
        LavaFlowSlot& s = outSi->second;
        sidxs.push_back(outSi->first);
      }

      //auto si = lower_bound(ALL(curSlots()), LavaId(ni.id.nid), [](auto a,auto b){ return a.first < b; } );          // si is slot iterator
      //if(si != end(curSlots())  &&  si->first.nid == ni.id.nid){
      //  LavaFlowSlot& s = si->second;
      //  sidxs.push_back(si->first);        
      //}
    }
    return sidxs;                                        // RVO
  }
  auto       errorInst() -> LavaInst&
  { 
    static LavaInst ERR_INST = { LavaId(LavaNode::NODE_ERROR, LavaFlowSlot::SLOT_ERROR), nullptr, 0, 0 };
    return ERR_INST;
  }
  auto       errorInst() const -> LavaInst const&
  { 
    static LavaInst ERR_INST = { LavaId(LavaNode::NODE_ERROR, LavaFlowSlot::SLOT_ERROR), nullptr, 0, 0 };
    return ERR_INST;
  }
  
  u64        nxtInSlot(u64 nid) // non-const, part of writing, and needs to use the opposite buffer
  {
    auto si = nodeInSlots(nid);                   // si is slot iterator
    i64 cur = -1;
    while(si != end(oppInSlots())   && 
      si->first.nid  == nid && 
      si->first.sidx <= (u64)(cur+1) ){
      //cur = si->first.idx;
      ++cur; ++si; 
    }
    //return cur;

    return cur + 1;
  }
  u64       nxtOutSlot(u64 nid) // non-const, part of writing, and needs to use the opposite buffer
  {
    auto si = nodeOutSlots(nid);                   // si is slot iterator
    i64 cur = -1;
    while(si != end(oppOutSlots())   && 
      si->first.nid  == nid && 
      si->first.sidx <= (u64)(cur+1) ){
      //cur = si->first.idx;
      ++cur; ++si; 
    }
    //return cur;

    return cur + 1;
  }
  u32       countSlots(const char** typeList)  // todo: make this static
  {
    u32 cnt=0;
    for(; typeList  &&  *typeList; ++typeList){ 
      ++cnt; 
    }
    return cnt;
  }
  LavaInst    makeInst(u64 nid, LavaNode* ln)
  {
    LavaInst li;                                       // li is Lava Instance
    li.id      = nid;
    li.node    = ln;
    li.inputs  = countSlots(ln->in_types);
    li.outputs = countSlots(ln->out_types);

    return li;
  }

public:
  LavaGraph() { init(); }
  LavaGraph(LavaGraph&& rval){ mv(std::move(rval)); }
  LavaGraph& operator=(LavaGraph&& rval){ mv(std::move(rval)); return *this; }

  auto         operator[](uint64_t nid) -> decltype(curNodes()[0])
  {
    return curNodes()[nid];
  }

  // global
  auto   normalizeIndices() -> NormalizeMap
  {
    using namespace std;

    // create a mapping of old node Ids to new ones, new ones will be their position + 1
    // todo: make a sorted map here so that the node Ids don't get reordered, only moved back down to start from 1

    vector<LavaId> sortedNodeIds;
    SECTION(create a vector of node ids and sort them)
    {
      //u64 curSrtNid = 0;
      for(auto& kv : curNodes()){
        sortedNodeIds.push_back(kv.first);
      }
      sort( ALL(sortedNodeIds) );
    }

    NormalizeMap nids;
    SECTION(make a map of node ids moved to start at 1 and be continuous)
    {
      u64 cur = 1;
      for(auto const& id : sortedNodeIds){
        nids[id.nid] = cur++;
      }
    }

    // connections 
    CnctMap nxtCncts;
    for(auto kv : curCncts()){
      LavaId nxtDest = kv.first;
      LavaId nxtSrc  = kv.second;
      nxtDest.nid = nids[nxtDest.nid];
      nxtSrc.nid  = nids[nxtSrc.nid];
      nxtCncts.insert({nxtDest, nxtSrc});
    }
    curCncts() = move(nxtCncts);

    SrcMap nxtDestCncts;
    for(auto kv : curDestCncts()){
      LavaId nxtSrc   = kv.first;
      LavaId nxtDest  = kv.second;
      nxtSrc.nid   = nids[nxtSrc.nid];
      nxtDest.nid  = nids[nxtDest.nid];
      nxtDestCncts.insert({nxtSrc, nxtDest});
    }
    curDestCncts() = move(nxtDestCncts);

    SECTION(in slots)
    {
      Slots nxtSlots;
      for(auto& kv : curInSlots()){
        LavaId nxtId  = kv.first;
        nxtId.nid     = nids[nxtId.nid];
        LavaFlowSlot nxtS = kv.second;
        auto nxtSltId = nids[nxtS.id.nid];
        nxtS.id.nid   = nxtSltId;
        nxtS.id.sidx  = nxtS.id.sidx;
        nxtSlots.insert({nxtId, nxtS});
      }
      curInSlots() = move(nxtSlots);
    }
    SECTION(out slots)
    {
      Slots nxtSlots;
      for(auto& kv : curOutSlots()){
        LavaId nxtId  = kv.first;
        nxtId.nid     = nids[nxtId.nid];
        LavaFlowSlot nxtS = kv.second;
        auto nxtSltId = nids[nxtS.id.nid];
        nxtS.id.nid   = nxtSltId;
        nxtS.id.sidx  = nxtS.id.sidx;
        nxtSlots.insert({nxtId, nxtS});
      }
      curOutSlots() = move(nxtSlots);
    }

    // node ids 
    NodeInsts nxtNds;
    for(auto& kv : curNodes()){
      u64     nxtId = nids[kv.first];
      LavaInst inst = curNodes()[kv.first];
      inst.id       = nxtId;
      nxtNds.insert({nxtId, inst});
    }
    curNodes() = move(nxtNds);

    return nids;
  }
  void      setNextNodeId(u64 nxt){ m_nxtId = nxt; }
  u64           totalTime()
  {
    u64 total = 0;
    for(auto& kv : curNodes()){ 
      total += kv.second.time;
    }
    return total;
  }
  u64           clearTime()
  {
    u64 total = 0;
    for(auto& kv : curNodes()){ 
      total += kv.second.clearTime();
    }
    return total;
  }
  void              clear()
  {
    curNodes().clear();
    //curSlots().clear();
    curInSlots().clear();
    curOutSlots().clear();
    curCncts().clear();
    curDestCncts().clear();
    curGenNodes().clear();

    oppNodes().clear();
    oppInSlots().clear();
    oppOutSlots().clear();
    oppCncts().clear();
    oppDestCncts().clear();
    oppGenNodes().clear();

    while(m_cmdq.size()>0) m_cmdq.pop();
    while(m_stk.size()>0) m_stk.pop();
    
    init();
    
    //m_ids.clear();
  }
  u64                 put(LavaCommand::Command cmd, LavaCommand::Arg A = LavaCommand::Arg(), LavaCommand::Arg B = LavaCommand::Arg() )
  {
    m_cmdq.push({cmd, A, B});
    return m_cmdq.size();
  }
  auto               exec() -> ArgVec
  {
    using namespace std;
    
    ArgVec returned;
    auto sz = m_cmdq.size();
    if(sz > 0)
    {
      if(m_useA.load()){
        m_nodesB     = m_nodesA;
        //m_slotsB     = m_slotsA; 
        m_inSlotsB   = m_inSlotsA; 
        m_outSlotsB  = m_outSlotsA; 
        m_cnctsB     = m_cnctsA; 
        m_destCnctsB = m_destCnctsA;
        m_genNodesB  = m_genNodesA;
        m_genCacheB  = m_genCacheA;
      }else{
        m_nodesA     = m_nodesB;
        //m_slotsA     = m_slotsB; 
        m_inSlotsA   = m_inSlotsB; 
        m_outSlotsA  = m_outSlotsB; 
        m_cnctsA     = m_cnctsB; 
        m_destCnctsA = m_destCnctsB;
        m_genNodesA  = m_genNodesB;
        m_genCacheA  = m_genCacheB;
      }

      while(m_cmdq.size() > 0)
      {
        auto lc = m_cmdq.front();                 // lc is LavaCommand
        m_cmdq.pop();
        switch(lc.cmd)
        {
          case LavaCommand::TGL_CNCT:{
            this->toggleCnct(lc.src.id, lc.dest.id);
          }break;

          case LavaCommand::DEL_CNCT:{
            this->delCnct(lc.A.id);
          }break;

          case LavaCommand::ADD_NODE:{
            u64 nid = this->addNode(lc.A.ndptr, lc.B.val);
            LavaCommand::Arg ret;
            ret.id.nid = nid;
            m_stk.push(ret);
            returned.push_back(ret);
          }break;

          case LavaCommand::DEL_NODE:{
            this->delNode(lc.A.id.nid);
          }break;

          case LavaCommand::ADD_SLOT:{
            LavaFlowSlot s(m_stk.top().id.nid, lc.A.slotDest);
            m_stk.pop();
            LavaId sid = this->addSlot(s); // lc.A.id.sidx);
            LavaCommand::Arg ret;
            ret.id = sid;
            m_stk.push(ret);
            returned.push_back(ret);
          }break;

          default: break;
        };
      }

      while(m_stk.size()>0){ m_stk.pop(); }
      
      auto& mcache = oppMsgCache();                                        // mcache is message cache
      auto&    cur = oppGenNodes();
      mcache.clear();
      mcache.reserve( cur.size() );

      for(auto const& lid : cur){
        mcache.push_back(lid.nid);
      }

      m_useA.store( !m_useA.load() );                                      // this should be the only place where it is flipped, so the store is all that matters
    }

    return returned;
  }

  // opposite buffer changes
  uint64_t    addNode(LavaNode* ln, uint64_t nid)
  {
    LavaInst li = makeInst(nid, ln);

    if( ln->node_type==LavaNode::MSG        || 
        ln->node_type==LavaNode::GENERATOR  || 
        ln->node_type==LavaNode::CONSTANT   || 
        li.inputs==0 ){
      oppGenNodes().insert(nid);
    }

    return oppNodes().insert({nid, li}).first->first;                             // returns a pair that contains the key-value pair
  }
  uint64_t    addNode(LavaNode* ln) // bool newId=true)
  {
    u64 id = nxtId();
    return addNode(ln, id);
  }
  bool        delNode(uint64_t nid)
  {
    SECTION(in slots and destination connections)
    {
      auto     si = this->nodeInSlots(nid);                       // si is slot iterator
      auto siCnct = si; 
      for(; siCnct!=end(oppInSlots())  &&  siCnct->first.nid==nid; ++siCnct ){
        this->delDestCnct(nid);
      }
    }
    SECTION(out slots and source connections)
    {
      auto     si = this->nodeOutSlots(nid);                       // si is slot iterator
      auto siCnct = si; 
      for(; siCnct!=end(oppOutSlots())  &&  siCnct->first.nid==nid; ++siCnct ){
        this->delSrcCncts(nid);
      }
    }

    SECTION(delete input/destination slots)
    {
      auto si = this->nodeInSlots(nid);                       // si is slot iterator
      for(; si!=end(oppInSlots()) && si->first.nid==nid; ){
         si = oppInSlots().erase(si);
      }
    }

    SECTION(delete output/src slots)
    {
      auto si = this->nodeOutSlots(nid);                       // si is slot iterator
      for(; si!=end(oppOutSlots()) && si->first.nid==nid; ){
        si = oppOutSlots().erase(si);
      }
    }

    //auto  inSltCnt = oppInSlots().erase(nid);                         // slcnt is slot count
    //auto outSltCnt = oppOutSlots().erase(nid);                        // slcnt is slot count

    auto&  ndInsts = oppNodes();
    //ndInsts[nid].node->destructor();
    auto       cnt = ndInsts.erase(nid);

    //auto       cnt = oppNodes().erase(nid);

    oppGenNodes().erase(nid);

    return cnt > 0;                                  // return true if 1 or more elements were erased, return false if no elements were erasedm
    //return (cnt + inSltCnt + outSltCnt) > 0;                                  // return true if 1 or more elements were erased, return false if no elements were erasedm
  }
  LavaId      addSlot(LavaFlowSlot  s, u64 sidx=LavaId::SLOT_NONE)
  {
    LavaId id(s.id.nid, sidx, s.in);
    if(id.sidx==LavaId::SLOT_NONE){
      id.sidx =  s.in? nxtInSlot(s.id.nid)  :  nxtOutSlot(s.id.nid);
    }else{ id.sidx = sidx; }
    
    s.id = id;
    if(s.in) oppInSlots().insert({id, s});
    else     oppOutSlots().insert({id, s});


    //LavaId id(s.id.nid, sidx);
    //id.sidx = sidx? sidx  :  nxtSlot(s.id.nid);
    //s.id    = id;
    //oppSlots().insert({id, s});

    return id;
  }
  u32         delCnct(LavaId  src, LavaId  dest)
  {
    u32      cnt = 0;
    auto srcIter = oppCncts().find(dest);
    if(srcIter != oppCncts().end())
    {
      auto iter = oppDestCncts().find(src);
      for(; iter != oppDestCncts().end() && iter->first == src; ){
        auto cpy = iter++;          // ++iter;
        if(cpy->second == dest){
          oppDestCncts().erase(cpy);
          ++cnt;
        }
      }
      oppCncts().erase(dest);
    }

    return cnt;
  }
  void     toggleCnct(LavaId  src, LavaId  dest)
  {
    auto srcLiIter  = oppNodes().find(src.nid);
    if(srcLiIter==oppNodes().end() || !(srcLiIter->second.node) ){ return; }

    auto destLiIter  = oppNodes().find(dest.nid);
    if(destLiIter==oppNodes().end() || !(destLiIter->second.node) ){ return; }

    //LavaInst srcLi  = oppNodes().find(src.nid);    // node(src.nid);
    //LavaInst destLi = node(dest.nid);
    //if(!srcLi.node || !destLi.node){ return; } // if either of the nodes don't exist, don't make the connection

    u32    delcnt = 0;
    auto       di = oppCncts().find(dest);
    bool matchSrc = false;
    if( di != end(oppCncts()) ){
      matchSrc = src == di->second;
      delcnt   = delCnct(di->second,dest);
    }

    if( !(matchSrc && delcnt>0) ){
      oppCncts().insert({dest, src});
      oppDestCncts().insert({src, dest});
    }
  }

  // nodes
  auto           node(u64 id)  -> LavaInst&
  {
    auto nIter = curNodes().find(id);                                          // nIter is node iterator
    if(nIter == end(curNodes())) return errorInst();

    return nIter->second;
  }
  auto           node(u64 id) const -> LavaInst
  {
    auto nIter = curNodes().find(id);                                          // nIter is node iterator
    if(nIter == end(curNodes())) return errorInst();

    return nIter->second;
  }
  auto          nodes()       const -> vec_insts
  {
    vec_insts nds;
    nds.reserve(curNodes().size());
    for(auto& on : curNodes()){                                  // on is order and node - order is on.first    LavaInst is on.second
      nds.push_back(on.second);
    }
    return nds;
  }
  auto          nodes() -> vec_insts
  {
    vec_insts nds;
    nds.reserve(curNodes().size());
    for(auto& on : curNodes()){                                  // on is order and node - order is on.first    node is on.second
      nds.push_back(on.second);

      //NodeInstance inst;
      //inst.id = on.first;
      //inst.nd = on.second;
      //nds.push_back(inst);
    }
    return nds;
  }
  u64           maxId()
  {
    using namespace std;

    u64 mx = 0;
    for(auto kv : curNodes()){ mx = max(mx, kv.first); }

    return mx;

    //for(auto kv : m_ids){ mx = max(mx, kv.first); }
  }
  u64             nsz()       const { return curNodes().size(); }
  auto       msgNodes()       const -> GenIds const&
  {
    return curGenNodes();
  }
  auto       nodeFunc(uint64_t nid) const -> FlowFunc
  {
    auto ni = curNodes().find(nid);                            // ni is node iterator
    if(ni == end(curNodes()))
      return ni->second.node->func;
      //return ni->second->func;
    else
      return nullptr;
  }
  void       setState(u64 nid, LavaInst::State state) // -> LavaInst::State
  {
    //return this->node(nid).setState(nid, state);
    LavaInst& li = this->node(nid);
    li.setState(state);
  }
  auto       getState(u64 nid) -> LavaInst::State
  {
    //return this->node(nid).getState();
    LavaInst& li = this->node(nid);
    return li.getState();
  }

  // slots
  auto         inSlot(LavaId   id) -> LavaFlowSlot*
  {
    auto si = curInSlots().find(id);
    if(si == curInSlots().end()) 
      return nullptr;

    return &si->second;
  }
  auto        outSlot(LavaId   id) -> LavaFlowSlot*
  {
    auto si = curOutSlots().find(id);
    if(si == curOutSlots().end()) 
      return nullptr;

    return &si->second;
  }
  auto           slot(LavaId   id) -> LavaFlowSlot*
  {
    if(id.isIn) return  inSlot(id);
    else        return outSlot(id);
  }

  auto      nodeInSlots(u64     nid) const -> decltype(curInSlots().cbegin())
  {
    return lower_bound(ALL(curInSlots()), nid, [](auto a,auto b){ return a.first.nid < b; } );
  }
  auto      nodeInSlots(u64     nid) -> decltype(oppInSlots().begin())
  {
    return lower_bound(ALL(oppInSlots()), nid, [](auto a,auto b){ return a.first.nid < b; } );
  }
  auto     nodeOutSlots(u64     nid) const -> decltype(curOutSlots().cbegin())
  {
    return lower_bound(ALL(curInSlots()), nid, [](auto a,auto b){ return a.first.nid < b; } );
  }
  auto     nodeOutSlots(u64     nid) -> decltype(oppOutSlots().begin())
  {
    return lower_bound(ALL(oppOutSlots()), nid, [](auto a,auto b){ return a.first.nid < b; } );
  }
  auto          inSlots()  -> Slots& { return curInSlots(); }
  auto          inSlots() const -> Slots const& { return curInSlots();  }
  auto       getInSlots() -> Slots& { return curInSlots(); }
  auto         outSlots() -> Slots& { return curOutSlots(); }
  auto         outSlots() const -> Slots const& { return curOutSlots();  }
  auto      getOutSlots() -> Slots& { return curOutSlots(); }

  auto          srcSlot(LavaId destId) -> LavaFlowSlot* 
  {
    auto ci = curCncts().find(destId);
    if(ci != curCncts().end()){
      //LavaFlowSlot* s = slot(ci->second);
      LavaFlowSlot* s = outSlot(ci->second);
      return s;
    }else 
      return nullptr;
  }
  auto        destSlots(LavaId  srcId) -> decltype(curDestCncts().find(srcId))
  {
    return curDestCncts().find(srcId);
  }
  auto        inSlotEnd() -> decltype(curInSlots().end()) { return curInSlots().end(); }
  u64           inSltSz() const { return curInSlots().size(); }
  auto       outSlotEnd() -> decltype(curOutSlots().end()) { return curOutSlots().end(); }
  u64          outSltSz() const { return curOutSlots().size(); }

  // connection deletion
  u32     delSrcCncts(LavaId  src)
  {
    u32  cnt = 0;
    auto  di = oppDestCncts().find(src);
    for(; di!=oppDestCncts().end() && di->first==src; ++cnt, ++di){   // ++di,
      oppCncts().erase(di->second);
    }
    oppDestCncts().erase(src);

    return cnt;

    //di = m_destCncts.find(src);
  }
  bool    delDestCnct(LavaId dest)
  {
    auto iter = oppCncts().find(dest);
    if(iter==oppCncts().end()) return false;

    auto src = iter->second;
    oppCncts().erase(dest);

    auto srcIter = oppDestCncts().find(src);
    for(; srcIter!=end(oppDestCncts()) && srcIter->first==src && srcIter->second==dest; ){
      auto cpy = srcIter++;
      oppDestCncts().erase(cpy);
    }

    return true;
  }
  u32         delCnct(LavaId   id)
  {
    u32 cnt = 0;
    //LavaFlowSlot* s = this->slot(id);
    LavaFlowSlot* s =  id.isIn? inSlot(id)  :  outSlot(id);
    if(!s) return 0;

    if(s->in){
      auto iter = oppCncts().find(id);
      if(iter==oppCncts().end()){ return 0; }

      return delCnct(iter->second, iter->first);
    }else{
      auto iter = oppDestCncts().find(id);
      if(iter==oppDestCncts().end()){ return 0; }

      auto  idx = iter->first;
      while(iter != oppDestCncts().end() && iter->first == idx){
        oppCncts().erase(iter->second);
        iter = oppDestCncts().erase(iter);
        ++cnt;
      }
    }

    return cnt;
  }

  // connection reading
  auto      destCncts(LavaId  src) -> decltype(curDestCncts().begin()) // C++14 -> decltype(m_slots.find(Id(nid)))
  {
    return lower_bound(ALL(curDestCncts()), LavaId(src), [](auto a,auto b){ return a.first < b; } );
  }
  u64   destCnctCount(LavaId  src)
  {
    return curDestCncts().count(src);
  }
  auto    destCnctEnd() -> decltype(curDestCncts().end()) { return curDestCncts().end(); }
  auto        cnctEnd() -> decltype(curCncts().end())     { return curCncts().end(); }
  auto      cnctBegin() -> decltype(curCncts().begin())   { return curCncts().begin(); }
  auto          cncts() -> CnctMap& { return curCncts(); }
  auto          cncts() const -> CnctMap const& { return curCncts(); }
  u64          cnctsz() const { return curCncts().size(); }
  auto    srcCnctsMap() -> SrcMap&
  {
    return curDestCncts();
  }
};
struct       LavaFlow
{
public:
  using abool           =  std::atomic<bool>;
  using au64            =  std::atomic<uint64_t>;
  using PacketQueue     =  std::priority_queue<LavaPacket>;
  using FrameQueue      =  std::vector<LavaFrame>;
  using MsgNodeVec      =  std::vector<uint64_t>;
  using Mutex           =  std::mutex;
  //using PktCalbk        =  void (*)(LavaPacket pkt);
  using ConstMem        =  std::unordered_map<std::string, LavaConst>;
 
  enum FlowErr { NONE=0, RUN_ERR=0xFFFFFFFFFFFFFFFF };

  lava_pathHndlMap           libs;     // libs is libraries - this maps the live path of the shared libary with the OS specific handle that the OS loading function returns
  lava_nidMap                nids;     // nids is node ids  - this maps the name of the node to all of the graph node ids that use it
  lava_flowNodes             flow;
  lava_flowPtrs              ptrs;     // ptrs are the LavaFlowNode pointers - each one needs a unique id so they can be referenced elsewhere 
  lava_hndlNodeMap         ndptrs;     // ndptrs is node pointers - a map from each handle to the one (zero?) or more LavaFlowNode pointers the shared lib contains
  lava_nameNodeMap      nameToPtr;     // maps node names to their pointers 
  ConstMem               constMem;

//  mutable bool          m_running = false;            // todo: make this atomic
  mutable abool         m_running = false;            // todo: make this atomic
  mutable u64          m_curMsgId = 0;                // todo: make this atomic
  mutable u64             m_cycle = 0;                // todo: make this atomic
  mutable LavaId          m_curId = LavaNode::NONE;   // todo: make this atomic - won't be used as a single variable anyway
  mutable u64       m_threadCount = 0;                // todo: make this atomic
  mutable u32             version = 0;                // todo: make this atomic
  mutable Mutex            m_qLck;
  mutable Mutex       m_frameQLck;
  //mutable PktCalbk packetCallback;
  mutable PacketCallback packetCallback;
  mutable PacketQueue           q;
  mutable FrameQueue       frameQ;
  mutable au64         m_nxtMsgNd = 0;

  MsgNodeVec          m_genNodesA;
  LavaGraph                 graph;

  void  udpateMsgIdxCache() // is this used? should it be? todo: not thread safe - this takes the current hash map of message nodes 
  {
    auto& cur = graph.curGenNodes();
    m_genNodesA.clear();
    m_genNodesA.reserve( cur.size() );

    for(auto const& lid : cur){
      m_genNodesA.push_back(lid.nid);
    }
    //new (&m_msgNodesA) MsgNodeVec( ALL(graph.curMsgNodes()) );
  }
  u64      fetchIncNxtMsg() const                    // fetchIncNxtMsg is fetch increment next message (node index)
  {
    return m_nxtMsgNd.fetch_add(1);
  }
  u64            nxtMsgId()
  {
    auto&  cur = graph.curMsgCache();
    u64     id = LavaId::NODE_NONE;
    if(cur.size()>0){
      u64 idx = fetchIncNxtMsg() % cur.size();
      return cur[idx].nid;
    }
    return LavaId::NODE_NONE;
  }

  u64      incThreadCount()
  {
    return std::atomic_fetch_add( (au64*)&m_threadCount,  1);
  }
  u64      decThreadCount()
  {
    return std::atomic_fetch_add( (au64*)&m_threadCount, -1);
  }
  bool          nxtPacket(LavaPacket* outPkt)
  {
    using namespace std;
    
    // need to somehow find a full set of packets for the current frame - how to keep one frame from outrunning another? 
    // does each node's slot need its own queue? should packets be organized differently? one queue per frame? what determines a frame? one pass through all the message nodes?
    bool packetWritten = false;

    m_qLck.lock();             // lock mutex
      if(q.size() > 0){
        *outPkt = q.top();
        q.pop();
        packetWritten = true;
        m_curId = outPkt->dest_node;
      }
    m_qLck.unlock();           // unlock mutex

    return packetWritten;

    //lock_guard<Mutex> qLck(m_qLck);
    //LavaPacket pckt = q.top();
  }
  void          putPacket(LavaPacket     pkt)
  {
    m_qLck.lock();              // mutex lock
      //if( packetCallback )
        //packetCallback(pkt);
      q.push(pkt);              // todo: use a mutex here initially
    m_qLck.unlock();            // mutex unlock
  }

  // query 
  auto     getNxtPacketId() -> LavaId
  {
    using namespace std;
    
    LavaId ret;
    lock_guard<Mutex>  qLck(m_qLck);
      if(q.size() > 0){
        ret.nid  = q.top().dest_node;
        ret.sidx = q.top().dest_slot;
      }else{
        ret.nid  = LavaId::NODE_NONE;
        ret.sidx = LavaId::SLOT_NONE;
      }
      return ret;
    // implicit unlock
  }

  // execution
  void              start(){ m_running =  true; }
  void               stop()
  {
    m_running = false;                                 // this will make the 'running' boolean variable false, which will make the the while(running) loop stop, and the threads will end
    //for(auto& t : fd.flowThreads){
    //  t.join();
    //}
  }
  void     runDestructors(bool destructConstants=true)
  {
    assert(m_running == false);                               // all threads should be out of the loop by the time this function is called
    for(auto const& kv : nameToPtr){
      LavaNode* ln = kv.second;
      if(ln){
        if(ln->node_type==LavaNode::CONSTANT)
        {
          if(destructConstants){
            #ifdef _WIN32
              if(ln->filePtr){
                UnmapViewOfFile(ln->filePtr);
              }
              if(ln->fileHndl){
                CloseHandle(ln->fileHndl);
              }
            #elif defined(__APPLE__) || defined(__MACH__) || defined(__unix__) || defined(__FreeBSD__) || defined(__linux__)     // osx, linux and freebsd
              if(sm.hndlPtr){
                munmap(sm.hndlPtr, sm.size);  // todo: size here needs to be the total size, and errors need to be checked
              }
              remove(sm.path);
              // todo: deal with errors here as well
            #endif
          }
        }else if(ln->destructor){
          ln->destructor();
        }

      }
    }
  }
  void    runConstructors()
  {
    assert(m_running == false);                               // all threads should be out of the loop by the time this function is called
    for(auto const& kv : nameToPtr){
      LavaNode* ln = kv.second;
      if(ln && 
         ln->node_type!=LavaNode::CONSTANT && 
         ln->constructor)
      {
        ln->constructor();
      }
    }
  }

  //void          pauseLoop(){}
};

// Lava Helper Functions
template<class T> tbl LavaMakeTbl(LavaParams const* lp, u64 count, T initVal=T() )
{
  using namespace std;

  tbl t;
  t.m_alloc    =  lp->ref_alloc;
  t.m_realloc  =  nullptr;
  t.m_free     =  nullptr;

  t.resize(count, initVal);

  return move(t);
}
inline tbl            LavaMakeTbl(LavaParams const* lp)
{
  using namespace std;

  tbl t;
  t.m_alloc    =  lp->ref_alloc;
  //t.m_realloc  =  lp->ref_realloc;
  t.m_realloc  =  nullptr;
  t.m_free     =  nullptr;

  return move(t);
}
inline tbl           LavaLocalTbl(LavaParams const* lp)
{
  using namespace std;

  tbl t;
  t.m_alloc   = lp->local_alloc;
  t.m_realloc = lp->local_realloc;
  t.m_free    = lp->local_free;

  return move(t);
}
inline const tbl  LavaTblFromPckt(LavaParams const* lp, LavaFrame const* in, u32 slot)
{
  using namespace std;

  if( !in->slotMask[slot] ){ return tbl(); }

  assert( LavaMem::fromDataAddr(in->packets[slot].val.value).refCount() != 0 );

  void* valPtr = (void*)in->packets[slot].val.value;
  if( !tbl::isTbl(valPtr) ){ return tbl(); }

  tbl t(valPtr);
  t.m_alloc   = lp->ref_alloc;
  t.m_realloc = lp->ref_realloc;
  //t.m_free    = nullptr;

  return move(t);
}
inline str        LavaStrFromPckt(LavaFrame const* in, u32 slot)              // don't need the LavaParams struct since we will be using the node's std allocators
{
  using namespace std;

  auto inTxtPckt = in->packets[slot];

  str inTxt;
  inTxt.resize( inTxtPckt.sz_bytes );
  memcpy( (void*)inTxt.data(), (void*)inTxtPckt.val.value, inTxtPckt.sz_bytes);

  return move(inTxt);
}
inline LavaOut       LavaTblToOut(tbl const& t, u32 slot)
{
  LavaOut o;
  o.val.value = (u64)t.memStart();
  o.val.type  = LavaArgType::MEMORY;
  o.key.slot  = slot;

  return o;
}
inline LavaOut       LavaStrToOut(LavaParams const* lp, str const& s, u32 slot)
{
  void* mem = lp->ref_alloc(s.size() + 1);
  memcpy(mem, s.c_str(), s.size()+1);

  LavaOut o;
  o.val.value = (u64)mem;
  o.val.type  = LavaArgType::MEMORY;
  o.key.slot  = slot;

  return o;
}
inline LavaOut       LavaMemToOut(LavaMem lm, u32 slot)
{
  LavaOut o;
  o.val.value = (u64)lm.data();
  o.val.type  = LavaArgType::MEMORY;
  o.key.slot  = slot;

  return o;
}
inline bool           LavaNxtPckt(LavaFrame const* in, u32* currentIndex)
{
  while(*currentIndex < in->packets.size()){
    if( in->slotMask[ (*currentIndex)++ ] ){ return true; }
  }
  return false;

  //for( ; *currentIndex < in->packets.size() ; ++(*currentIndex) ){
  //  if( in->slotMask[*currentIndex] ){
  //    ++(*currentIndex);
  //    return true;
  //  }
  //}
  //return false;
}
inline LavaMem  LavaMemAllocation(LavaAllocFunc alloc, u64 sizeBytes)
{
  //const int lmHdrSz = sizeof(LavaMemHeader);
  assert( sizeof(LavaMemHeader) == 16 );
  void*     p = alloc(sizeof(LavaMemHeader) + sizeBytes);
  if(!p){ 
    return LavaMem{nullptr};
  }

  auto header = (LavaMemHeader*)p;
  header->refCount.store(0);
  header->sizeBytes = sizeBytes;

  LavaMem lm;
  lm.ptr = p;

  return lm;
}
// End Lava Helper Functions

#if defined(__LAVAFLOW_IMPL__)

#if defined(_WIN32)
  //int (WINAPIV * __vsnprintf)(char *, size_t, const char*, va_list) = _vsnprintf;
  //int (WINAPIV * _vsnprintf)(char *, size_t, const char*, va_list) = vsnprintf;
#endif

static thread_local lava_memvec*  lava_thread_ownedMem = nullptr;       // thread local handle for thread local heap allocations

// function implementations
BOOL WINAPI DllMain(
  _In_ HINSTANCE    hinstDLL,
  _In_ DWORD       fdwReason,
  _In_ LPVOID    lpvReserved
)
{
  //Println("dllmain called: ", fdwReason);
  switch(fdwReason)
  {
    //case DLL_PROCESS_ATTACH:
    //case DLL_THREAD_ATTACH:
    //  lava_thread_heap = nullptr;
    //  break;
  case DLL_PROCESS_DETACH:
  case DLL_THREAD_DETACH:
    //Println("Heap Destroy");
    LavaHeapDestroyCallback(lava_thread_heap);
    lava_thread_heap = nullptr;
    break;
  default:
    ;
  }
  return true;
}

namespace {

template<class T> u64 countPtrList(T* p)
{
  u64 cnt = 0;
  while(p && *p != 0){ 
    ++cnt; 
    ++p; 
  }
  return cnt;
}

#include "../str_util.hpp"
void                printdb(simdb const& db)
{
  using namespace std;
  
  Println("size: ", db.size());

  

  std::vector<i8> memv(db.memsize(), 0);
  memcpy( (void*)memv.data(), db.mem(), db.memsize() );

  Println("\n");

  u64 blksz = db.blockSize();
  TO(memv.size(),i){ 
    if(i % blksz == 0){
      putc('|', stdout);
    }
    putc(memv[i] ,stdout);
  }
}
void           PrintLavaMem(LavaMem lm)
{
  if(lm.ptr){
    printf("\n addr: %llu  data addr: %llu  ref count: %llu   size bytes: %llu \n", 
      (u64)(lm.ptr), (u64)(lm.data()), (u64)lm.refCount(), (u64)lm.sizeBytes() );
    assert(lm.refCount() < 1000);
  }
}
void       PrintMemMapError()
{
#ifdef _WIN32      // windows
  int      err = (int)GetLastError();
  LPSTR msgBuf = nullptr;
  /*size_t msgSz =*/ FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msgBuf, 0, NULL);
  win_printf("Lava Constant memory mapping error %d - %s", err, msgBuf);
  LocalFree(msgBuf);
#endif
}

LavaNode         MemMapFile(fs::path const& pth)
{
  using namespace std;

  LavaNode retNd;

  #ifdef _WIN32      // windows
    HANDLE createHndl=NULL, openMappingHndl=NULL, createMappingHndl=NULL;
    
    str     pthStr = pth.generic_string();
    LPSTR  pthCstr = (char*)pthStr.c_str();
    
    createHndl = CreateFileA(
                        pthCstr, 
                        //GENERIC_READ, 
                        GENERIC_READ | GENERIC_WRITE, 
                        //FILE_SHARE_READ, // | FILE_SHARE_WRITE, 
                        FILE_SHARE_READ | FILE_SHARE_WRITE, 
                        NULL, 
                        OPEN_EXISTING, 
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);

    LARGE_INTEGER  fsz;
    fsz.QuadPart = 0;
    PLARGE_INTEGER fszPtr = &fsz;
    auto fszOk = GetFileSizeEx(createHndl, fszPtr);
    if(fszOk==0){
      PrintMemMapError();
      return LavaNodeListEnd;
    }else
      retNd.fileSize = fszPtr->QuadPart;

    createMappingHndl = CreateFileMappingA(
                          createHndl, 
                          NULL,
                          //PAGE_READONLY, 
                          PAGE_READWRITE, 
                          0,
                          0,
                          NULL); 

    retNd.fileHndl = createMappingHndl;
    if(retNd.fileHndl != NULL && retNd.fileHndl != INVALID_HANDLE_VALUE)
    {
      retNd.filePtr = MapViewOfFile(retNd.fileHndl,   // handle to map object
        //FILE_MAP_READ, //| FILE_MAP_WRITE, // FILE_MAP_ALL_ACCESS,   // read/write permission
        FILE_MAP_READ | FILE_MAP_WRITE,        // read/write permission
        0,
        0,
        0);
    }else{
      PrintMemMapError();
    }

    if(retNd.filePtr==nullptr)
    { 
      PrintMemMapError();
      CloseHandle(retNd.fileHndl); 
      return LavaNodeListEnd;
    }

  #elif defined(__APPLE__) || defined(__MACH__) || defined(__unix__) || defined(__FreeBSD__) || defined(__linux__)  // osx, linux and freebsd
      sm.owner  = true; // todo: have to figure out how to detect which process is the owner

      sm.fileHndl = open(sm.path, O_RDWR);
      if(sm.fileHndl == -1)
      {
        sm.fileHndl = open(sm.path, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR |S_IRGRP|S_IWGRP | S_IROTH|S_IWOTH ); // O_CREAT | O_SHLOCK ); // | O_NONBLOCK );
        if(sm.fileHndl == -1){
          if(error_code){ *error_code = simdb_error::COULD_NOT_OPEN_MAP_FILE; }
        }
        else{
          //flock(sm.fileHndl, LOCK_EX);   // exclusive lock  // LOCK_NB
        }
      }else{ sm.owner = false; }

      if(sm.owner){  // todo: still need more concrete race protection?
        fcntl(sm.fileHndl, F_GETLK, &flock);
        flock(sm.fileHndl, LOCK_EX);              // exclusive lock  // LOCK_NB
                                                  //fcntl(sm.fileHndl, F_PREALLOCATE);
  #if defined(__linux__) 
  #else 
        fcntl(sm.fileHndl, F_ALLOCATECONTIG);
  #endif

        if( ftruncate(sm.fileHndl, sizeBytes)!=0 ){
          if(error_code){ *error_code = simdb_error::FTRUNCATE_FAILURE; }
        }
        if( flock(sm.fileHndl, LOCK_UN)!=0 ){
          if(error_code){ *error_code = simdb_error::FLOCK_FAILURE; }
        }
      }

      sm.hndlPtr  = mmap(NULL, sizeBytes, PROT_READ|PROT_WRITE, MAP_SHARED , sm.fileHndl, 0); // MAP_PREFAULT_READ  | MAP_NOSYNC
      close(sm.fileHndl);
      sm.fileHndl = 0;

      if(sm.hndlPtr==MAP_FAILED){
        if(error_code){ *error_code = simdb_error::COULD_NOT_MEMORY_MAP_FILE; }
      }
  #endif       

  return retNd;
}

auto       GetSharedLibPath() -> std::wstring
{
  using namespace std;
  
  #ifdef _WIN32
    HMODULE hModule = GetModuleHandleW(NULL);
    WCHAR path[MAX_PATH];
    GetModuleFileNameW(hModule, path, MAX_PATH);

    //return string(TEXT(path));
    return path;
  #endif
}
auto    GetSharedLibDirIter() -> fs::directory_iterator //( fs::path( GetSharedLibPath() ) )
{
  using namespace std;
  using namespace  fs;

  auto libPath = path( GetSharedLibPath() );
  libPath.remove_filename();
  path    root = libPath;

  vector<str> paths;
  auto    dirIter = directory_iterator(root);

  return dirIter;  
}
auto        GetRefreshPaths(bool force=false) -> lava_paths
{
  using namespace std;
  using namespace  fs;

  static const regex lavaRegex("lava_.*");
  static const regex  extRegex(".*\\.live\\.dll");

  auto libPath = path( GetSharedLibPath() );
  libPath.remove_filename();
  path    root = libPath;

  vector<str> paths;
  auto    dirIter = directory_iterator(root);
  for(auto& d : dirIter)                                 // iterate though the root directory looking for shared libraries and constants
  {
    auto   p = d.path();
    if(!p.has_filename()){ continue; }

    auto ext = p.extension().generic_string();                        // ext is extension

    if(ext==".dll"){
      str fstr = p.filename().generic_string();                       // fstr is file string
      if(  regex_match(fstr, extRegex) ){ continue; }
      if( !regex_match(fstr,lavaRegex) ){ continue; }
    }else{ continue; } // else if( ext!=".const" ){ continue; }

    auto livepth = p;
    livepth.replace_extension( liveExt );

    bool refresh = true;
    if( exists(livepth) ){
      auto liveWrite = last_write_time(livepth).time_since_epoch().count();         // liveWrite is live write time - the live shared library file's last write time 
      auto origWrite = last_write_time(p).time_since_epoch().count();               // origWrite is orginal write time - the original shared library file's last write time
      refresh = force || origWrite > liveWrite;                                     // original has to be newer, don't want to do anything if they are the same either - does the copied live file need its time set to match the original?
    }

    if(refresh)
      paths.push_back( p.generic_string() );

    //refresh = origWrite > liveWrite;
    //
    //if(origWrite > liveWrite)
    //  refresh = true;
  }

  return paths;
}
uint64_t    CopyPathsToLive(lava_paths       const& paths)
{
  using namespace std;
  using namespace  fs;

  uint64_t count = 0;
  for(auto const& p : paths){
    path livepth(p);
    livepth.replace_extension(".live.dll");
    
    bool doCopy = false;
    if( exists(livepth) ){
      doCopy = remove(livepth);
    }else{ doCopy = true; }
    
    if(doCopy){ 
      copy_file(p, livepth);
      ++count;
    }
  }

  return count;
}
uint64_t        RemovePaths(lava_paths       const& paths)
{
  using namespace std;
  using namespace  fs;

  uint64_t count = 0;
  for(auto const& p : paths){
    if( remove(path(p)) ){ ++count; }
  }

  return count;
}
auto               LoadLibs(lava_paths       const& paths) -> lava_hndlvec
{
  lava_hndlvec hndls(paths.size(), 0);

  TO(paths.size(), i){
    HMODULE lib = LoadLibrary(TEXT(paths[i].c_str()));
    hndls[i] = lib;
  }

  return hndls;
}
uint64_t           FreeLibs(lava_hndlvec     const& hndls)
{
  uint64_t count = 0;
  for(auto const& h : hndls){
    if( FreeLibrary(h) ){ ++count; }
  }
  return count;
}
auto           GetLivePaths(lava_paths       const& paths) -> lava_paths 
{
  using namespace std;
  using namespace  fs;

  lava_paths ret;
  for(auto const& p : paths){
    ret.emplace_back( path(p).replace_extension(liveExt).generic_string() );
  }

  return ret;
}
auto         GetLiveHandles(lava_pathHndlMap const& hndls, lava_paths const& paths) -> lava_hndlvec
{
  lava_hndlvec ret;
  for(auto const& p : paths){
    auto hi = hndls.find(p);         // hi is handle iterator
    if(hi != end(hndls)) ret.push_back(hi->second);
  }

  return ret;
}
auto       GetFlowNodeLists(lava_hndlvec     const& hndls) -> lava_ptrsvec
{
  lava_ptrsvec ret(hndls.size(), nullptr);
  TO(hndls.size(),i){
    auto h = hndls[i];
    if(h){
      auto  GetLavaFlowNodes = (GetLavaFlowNodes_t)GetProcAddress(h, TEXT("GetLavaNodes") );
      LavaNode* nodeList = GetLavaFlowNodes();
      ret[i] = nodeList;
    }
  }

  return ret;
}
void    ErrorCheckNodeLists(lava_ptrsvec* inout_ndLsts)
{
  auto& lsts = *inout_ndLsts;
  auto    sz = lsts.size();
  TO(sz,i)
  {
    LavaNode* lst = lsts[i];  
    if(!lst){  }  // erase this node list if is a nullptr

    for(; lst->func; ++lst)
    {
      if(!lst->func){}                                 // error because this node does not have a primary function
      if(!lst->name){}                                 // error because this node does not have a name
      
      if( countPtrList(lst->in_names) != countPtrList(lst->in_types) )        // count input types and input names to make sure they line up
      {}

      if( countPtrList(lst->out_names) != countPtrList(lst->out_types) )        // do the same for output types and names - count them both to make sure there are the same number of each
      {}
    }

  }
}

LavaInst::State exceptWrapper(FlowFunc f, LavaFlow& lf, LavaParams* lp, LavaFrame* inFrame, lava_threadQ* outArgs) // LavaOut* outArgs)
{
  LavaInst::State        ret = LavaInst::NORMAL;  // LavaFlow::NONE;
  uint64_t         winExcept = 0;
  __try{
    f(lp, inFrame, outArgs);
  }__except( (winExcept=GetExceptionCode()) || EXCEPTION_EXECUTE_HANDLER ){
    ret =  LavaInst::RUN_ERROR; 
    printf("\n windows exception code: %llu \n", winExcept);
  }

  //#ifndef _NDEBUG
  //  TO(outArgs->size(),i){
  //    LavaOut o = outArgs->m_buf.useA? outArgs->atA(i) : outArgs->atB(i);
  //    //LavaMem  mem = LavaMem::fromDataAddr(o.value);
  //    if(o.value == 0){ ret = LavaInst::OUTPUT_ERROR; }
  //  }
  //#endif

  return ret;
}

}

LavaNode*      AddFlowConst(fs::path const& pth, LavaFlow& inout_flow)
{
  using namespace std;

  SECTION(continue if the file does not have a const extension)
  {
    if(!pth.has_filename()){ return nullptr; }
    auto ext = pth.extension().generic_string();                        // ext is extension
    if(ext!=".const"){ return nullptr; }
  }

  str typeStr, nameStr;
  SECTION(get the type and name from the file path - name.type.const)
  {
    auto typePth = pth;
    typePth.replace_extension("");
    typeStr = typePth.extension().generic_string();
    typeStr = typeStr.substr(1, typeStr.length()-1);
    nameStr = pth.filename().generic_string();
    nameStr = nameStr.substr(0, nameStr.find('.'));
  }

  LavaNode    mmapNd = MemMapFile(pth);                                             // use the memory mapping from simdb

  LavaNode*  nodePtr = nullptr;
  SECTION(create LavaConst, use the node pointer in the graph and move the LavaConst into the constMem map)
  {
    str pstr = pth.generic_string();

    LavaConst lc(nameStr, typeStr);
    lc.node->filePtr  = mmapNd.filePtr;
    lc.node->fileSize = mmapNd.fileSize;
    lc.node->fileHndl = mmapNd.fileHndl;
    nodePtr = lc.node;
    inout_flow.constMem[pstr] = move(lc);                                     // have to do this last since it moves the LavaConst and sets the original version to a nullptr - this might overwrite a former LavaCont, which would deallocate its heap memory on deconstruction

    inout_flow.flow.erase(pstr);
    inout_flow.nameToPtr.erase(nameStr);
    inout_flow.nameToPtr.insert( {nameStr, nodePtr} );
    inout_flow.flow.insert( {pstr, nodePtr} );
  }

  return nodePtr;
}

void*             LavaAlloc(uint64_t sizeBytes)
{
  uint64_t* mem = (uint64_t*)LavaHeapAlloc(sizeBytes + sizeof(uint64_t)*2);
  mem[0]  =  0;              // reference count
  mem[1]  =  sizeBytes;      // number of bytes of main allocation

  LavaMem lm;
  lm.ptr  =  mem;

  lava_thread_ownedMem->push_back(lm);

  return (void*)(mem + 2);   // sizeof(uint64_t)*2);
}
void*           LavaRealloc(void* addr, uint64_t sizeBytes)
{
  uint64_t* realAddr = (uint64_t*)addr - 2;
  uint64_t prevSz    = realAddr[1];                                // get the previous sizeBytes so we know how much to copy
  if(realAddr[0] != 0)
    realAddr[0]--;                                                 // decrement the previous memory's references since we are no longer going to have it to decrement in the main loop 

  void*       nxtMem = LavaAlloc(sizeBytes);                       // make a new allocation that is the requested size
  memcpy(nxtMem, addr, prevSz);                                    // copy from the previous alloction to the new allocation
  return nxtMem;
}
void               LavaFree(void* addr)
{
  //void* p = (void*)( (u8*)addr - 16 );  // 16 bytes for the reference count and sizeBytes
  void* p = (void*)( (uint64_t*)addr - 2 );
  LavaHeapFree(p);
}

void               LavaInit()
{
  //new (&db)     simdb("lava_db", 128, 2<<4);
  //new (&vizdb)  simdb("viz_db",  128, 2<<4);
}
bool        RefreshFlowLibs(LavaFlow& inout_flow, bool force=false)
{
  using namespace std;
  
  bool     newlibs  =  false;
  auto       paths  =  GetRefreshPaths(force);
  if(paths.size() > 0){
    newlibs = true;
    ++inout_flow.version;
  }

  if(!newlibs){ return false; } // avoid doing anything including locking if there no new libraries

  auto   livePaths  =  GetLivePaths(paths);
  
  lock_guard<mutex> flowLck(inout_flow.m_qLck);  // todo: only locks the queue, which isn't good enough - need to switch the nodes in a manner that will work live - maybe atomically swap the function pointer to null, update the node, then swap the function pointer again

  SECTION(run destructor on the nodes given by the paths)
  {
    for(auto const& p : paths){
      auto ndIter = inout_flow.flow.find(p); // ndIter is node iterator
      for(; ndIter != end(inout_flow.flow) && ndIter->first==p; ++ndIter){
        LavaNode* nd = ndIter->second;
        if(nd->destructor){ nd->destructor(); }
      }
    }
  }

  // coordinate live paths to handles
  auto liveHandles  =  GetLiveHandles(inout_flow.libs, livePaths);

  // free the handles
  auto   freeCount  =  FreeLibs(liveHandles); 

  // delete the now unloaded live shared library files
  auto    delCount  =  RemovePaths(livePaths);

  // copy the refresh paths' files
  auto   copyCount  =  CopyPathsToLive(paths); 

  // load the handles
  auto loadedHndls  =  LoadLibs(livePaths);

  // put loaded handles into LavaFlow struct
  TO(livePaths.size(), i){
    auto h = loadedHndls[i];
    if(h){
      inout_flow.libs[livePaths[i]] = h;
    }
  }

  lava_ptrsvec flowNdLists = GetFlowNodeLists(loadedHndls);          // extract the flow node lists from the handles

  //auto ndErrs = ErrorCheckNodeLists( &flowndLists );
  ErrorCheckNodeLists( &flowNdLists );

  // extract the flow nodes from the lists and put them into the multi-map
  // todo: should this use and node creation commands? should nodes be made to swtich atomically?
  TO(livePaths.size(),i)
  {
    LavaNode* ndList = flowNdLists[i];
    if(ndList){
      auto const&  p = livePaths[i];
      
      inout_flow.flow.erase(p);                                     // delete the current node list for the livePath 
      for(; ndList->func!=nullptr; ++ndList){                       // insert each of the LavaFlowNodes in the ndList into the multi-map
        if(ndList->constructor){ ndList->constructor(); }
        inout_flow.nameToPtr.erase(ndList->name);
        inout_flow.nameToPtr.insert( {ndList->name, ndList} );
        inout_flow.flow.insert( {p, ndList} );
      }
    }else{
      // set the node's state to an error
    }
  }

  //inout_flow.m_qLck.lock();

  return newlibs; // flow lock should unlocked here
}
bool      RefreshFlowConsts(LavaFlow& inout_flow)
{
  bool anyLoaded = false; // todo: integrate this with the rest of the loading to make consts get moved to live versions 
  auto   dirIter = GetSharedLibDirIter();
  for(auto& d : dirIter)                                                        // iterate though the root directory looking for shared libraries and constants
  {
    //auto pth = d.path();
    LavaNode* np = AddFlowConst(d.path(), inout_flow);
    if(np) anyLoaded = true;
  }

  return anyLoaded;
}

void               LavaStop(LavaFlow& lf)
{
  //outQ.clear();                                                       // this will pop all output packets in a thread safe way so that when it is deconstructed there will be no more packets

  lf.m_running.store(false);
  lf.m_frameQLck.lock();                                               // lock queue mutex        
    lf.frameQ.clear();
    while( !lf.q.empty() ){
      auto& pckt = lf.q.top();
      auto    lm = LavaMem::fromDataAddr(pckt.val.value);
      //lm.decRef();
      lf.q.pop();
    }
  lf.m_frameQLck.unlock();                                            // unlock queue mutex
}

void               LavaLoop(LavaFlow& lf) noexcept
{
  using namespace std;
  using namespace std::chrono;

  lf.incThreadCount();

  lava_threadQ     outQ;                              // queue of the output arguments
  lava_memvec  ownedMem;
  LavaFrame     inFrame;
  LavaVal        inArgs[LAVA_ARG_COUNT]={};           // these will end up on the per-thread stack when the thread enters this function, which is what we want - thread specific memory for the function call
  LavaParams         lp;

  //#ifdef _WIN32
  //  freopen("CONOUT$", "w", stdout);
  //#endif

  SECTION(initialization at thread start before loop)
  {
    lava_thread_ownedMem = &ownedMem;                   // move the pointer out to a global scope for the thread, so that the allocation function passed to the shared library can add the pointer the owned memory of the thread
    LavaHeapInit();

    lp.ref_alloc      =   LavaAlloc;
    lp.ref_realloc    =   LavaRealloc;
    lp.ref_free       =   LavaFree;
    lp.local_alloc    =   LavaHeapAlloc;
    lp.local_realloc  =   LavaHeapReAlloc;
    lp.local_free     =   LavaFree;

    #if defined(_WIN32)
      lp.lava_stdout    =   stdout;
      lp.lava_stdin     =   stdin;
      lp.lava_stderr    =   stderr;
      //lp.lava_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    #else
      lp.lava_stdout    =   stdout;
      lp.lava_stdin     =   stdin;
      lp.lava_stderr    =   stderr;
    #endif
  }

  while(lf.m_running)
  {    
    LavaFrame    runFrm;
    LavaPacket     pckt;
    u64          nodeId = LavaId::NODE_NONE;
    bool         doFlow = false;
    SECTION(make a frame from a packet to run a node or run a generator if no full frames are available)
    {
      doFlow = lf.nxtPacket(&pckt);
      if(doFlow) SECTION(if there is a packet available, fit it into a existing frame or create a new frame)
      {
        u16 sIdx  =  pckt.dest_slot;
        SECTION(lock the frame queue)
        {
          lock_guard<mutex> frameQLck(lf.m_frameQLck);                      // need a mutex that automatically unlocks on scope exit so that a goto can be used - the goto is needed to break out of an inner loop

          bool frameFound = false;
          SECTION(loop outstanding frames for a match for this packet - determine if a frame was found)
          {
            TO(lf.frameQ.size(), i)                                       // loop through the current frames looking for one with the same frame number and destination slot id - if no frame is found to put the packet into, make a new one - keep track of the lowest full frame while looping and use that if no full frame is found?
            {
              auto& frm = lf.frameQ[i];
              if(frm.dest  != pckt.dest_node){ continue; }                 // todo: unify dest_node and dest_id etc. as one LavaId LavaPacket
              if(frm.cycle != pckt.cycle){     continue; }

              bool slotTaken = frm.slotMask[sIdx];
              if(!slotTaken){
                //frm.packets[sIdx] = pckt;
                frm.putSlot(sIdx, pckt);
                frameFound = true;
              }

              if( frm.allFilled() ){                                    // If all the slots are filled, copy the frame out and erase it
                runFrm = frm;
                lf.frameQ.erase(lf.frameQ.begin()+i);                   // can't use an index, have to use an iterator for some reason
                frameFound = true;
              }

              if(frameFound){ break; }
            }
          }
          if(!frameFound) SECTION(if a frame was not found for this packet, create one, then put it in the array if the frame has more than one slot)
          {
            LavaInst& ndInst   =  lf.graph.node(pckt.dest_node);
            LavaFrame    frm;
            frm.slots  =  ndInst.inputs;                               // find the number of input slots for the dest node
            frm.dest   =  pckt.dest_node;
            frm.cycle  =  pckt.cycle;
            //runFrm.frame        =  ndInst.fetchIncFrame();
            frm.putSlot(sIdx, pckt);
              
            if(frm.slots > 1){
              lf.frameQ.push_back(frm);                                // New frame that starts with the current packet put into it 
              continue;                                                // go back to the start of the loop - jumping out of this scope should unlock the lock_guard mutex above
            }else
              runFrm = frm;
          }
        } // mutex is unlocked

        nodeId = runFrm.dest;
      }else SECTION(try to run a single message node if there was no packet found){
        nodeId  =  lf.nxtMsgId();
        // todo: need to work out here if the message node is available - locking and lock free message nodes would come in to play
      }
    }

    LavaInst::State state = LavaInst::NORMAL;
    SECTION(RUN the node from id and pass it a frame, then put its output packets in the queue)
    {
      if(nodeId != LavaId::NODE_NONE)
      {
        LavaInst&  li  =  lf.graph[nodeId];
        FlowFunc func  =  li.node?  li.node->func : nullptr;                 //lf.graph[nid].node->func;
        if(func && nodeId!=LavaId::NODE_NONE)
        {
          if(li.node->node_type==LavaNode::CONSTANT){
            SECTION(copy the memory mapped file in the const node)
            {              
              void* outMem = LavaAlloc(li.node->fileSize);
              memcpy(outMem, li.node->filePtr, li.node->fileSize);
              LavaOut o;
              o.val.value = (uint64_t)outMem;
              o.val.type  = LavaArgType::MEMORY;
              o.key.slot  = 0;

              outQ.push(o);
            }
          }else{
            SECTION(create arguments and call function)
            {
              lp.inputs         =   1;
              lp.cycle          =   lf.m_cycle;
              lp.id             =   LavaId(nodeId);

              auto stTime = high_resolution_clock::now();
                state       = exceptWrapper(func, lf, &lp, &runFrm, &outQ);         // actually run the node here
                if(state != LavaInst::NORMAL){ outQ.clear(); }
              auto endTime = high_resolution_clock::now();
              duration<u64,nano> diff = (endTime - stTime);
              li.addTime( diff.count() );
            }
          }
          SECTION(take LavaOut structs from the output queue and put them into packet queue as packets)                 // this section will not be reached if there was an error
          {
            if(outQ.size()==0){
              LavaControl cntrl  =  lf.packetCallback? lf.packetCallback(nullptr) : LavaControl::GO;                                                 // because this is before putting the memory in the queue, it can't get picked up and used yet, though that may not make a difference, since this thread has to free it anyway
              if(cntrl==LavaControl::STOP) 
                LavaStop(lf);
            }else while(outQ.size() > 0)
              SECTION(use output packets)
              {
                LavaOut outArg; 
                LavaMem    mem;
                SECTION(get the next output value from the queue and continue if there is a problem)
                {
                  if( !outQ.pop(outArg) ){ continue; }
                  //if(outArg.val.value == 0){ state = LavaInst::OUTPUT_ERROR; continue; }
                
                  if(outArg.val.value==0){
                    mem.ptr = 0;
                    state   = LavaInst::OUTPUT_ERROR;
                  }else{ mem = LavaMem::fromDataAddr(outArg.val.value); }                                      // this will be used to increment the reference count for every packet created
                }

                LavaPacket basePkt, pkt;
                SECTION(create new base packet and initialize the main packet with the base)
                {
                  basePkt.cycle       =   lf.m_cycle;            // increment the frame on every major loop through both data and message nodes - how to know when a full cycle has passed? maybe purely by message nodes - only increment frame if data is created through a message node cycle
                  basePkt.framed      =   false;                 // would this go on the socket?
                  basePkt.rangeStart  =   0;
                  basePkt.rangeEnd    =   0;
                  basePkt.src_node    =   nodeId;
                  basePkt.src_slot    =   outArg.key.slot;
                  basePkt.id          =   0;
                  basePkt.val         =   outArg.val;
                  basePkt.sz_bytes    =   mem.ptr? mem.sizeBytes() : 0;
                  pkt                 =   basePkt;
                }
                LavaControl cntrl = lf.packetCallback? lf.packetCallback(&pkt) : LavaControl::GO;                                                 // because this is before putting the memory in the queue, it can't get picked up and used yet, though that may not make a difference, since this thread has to free it anyway
              
                if(cntrl==LavaControl::GO) 
                  SECTION(make a packet for each connection, increment their reference count and put in the main packet queue)
                {
                  // route the packet using the graph - the packet may be copied multiple times and go to multiple destination slots
                  LavaId     src  =  { nodeId, outArg.key.slot, false };
                  auto        di  =  lf.graph.destCncts(src);                         // di is destination iterator
                  auto     diCnt  =  di;                                              // diCnt is destination iterator counter - used to count the number of destination slots this packet will be copied to so that the reference count can be set correctly
                  auto      diEn  =  lf.graph.destCnctEnd();
                  for(; di!=diEn && di->first==src; ++di)
                  {                                                                   // loop through the 1 or more destination slots connected to this source
                    LavaId  pktId = di->second;
                    pkt           = basePkt;                                          // pkt is packet
                    pkt.dest_node = pktId.nid;
                    pkt.dest_slot = pktId.sidx;

                    mem.incRef();

                    lf.putPacket(pkt);                                                // putPacket contains a mutex for now, eventually will be a lock free queue
                  }
                }
                else if(cntrl==LavaControl::STOP)
                {
                  LavaStop(lf);
                  outQ.clear();                                                       // this will pop all output packets in a thread safe way so that when it is deconstructed there will be no more packets
                }
              }
          }
        }
        else state = LavaInst::LOAD_ERROR;
      }
    }
    SECTION(switch on the node state decided by trying to run a packet through the node)
    {
      switch(state){
        case LavaInst::OUTPUT_ERROR:
        case LavaInst::RUN_ERROR:{
          lf.graph.setState(nodeId, LavaInst::RUN_ERROR);
          //lf.putPacket(pckt);                                                            // if there was an error, put the packet back into the queue
        }break;
        case LavaInst::LOAD_ERROR:{
          lf.graph.setState(nodeId, LavaInst::RUN_ERROR);                                  // todo: should deal with this at load time and not here of course
        }break;
        case LavaInst::NORMAL:
        default:{                                                                          // if everything worked, decrement the references of all the packets in the frame
          if(doFlow){
            TO(LavaFrame::PACKET_SLOTS,i) if(runFrm.slotMask[i]){
             
              LavaMem mem = LavaMem::fromDataAddr(runFrm.packets[i].val.value);
              mem.decRef();
            }
          }
        }break;
      }
    }
    SECTION(dealloction - partition owned allocations and free those with their reference count at 0)
    {
      //for(auto const& lm : ownedMem){ PrintLavaMem(lm); }

      auto  zeroRef  =  partition(ALL(ownedMem), [](auto a){return a.refCount() > 0;} );                           // partition the memory with zero references to the end / right of the vector so they can be deleted by just cutting down the size of the vector
      auto freeIter  =  zeroRef;
      for(; freeIter != end(ownedMem); ++freeIter){                            // loop through the memory with zero references and free them
        LavaFree( freeIter->data() );                                          // LavaFree will subtract 16 bytes from the  pointer given to it - ->data() should give the address at the start of the memory meant to be used, then LavaFree will subtract 16 from that
      }
      ownedMem.erase(zeroRef, end(ownedMem));                                  // erase the now freed memory

      if(ownedMem.size() <= ownedMem.capacity()/4 ){
        ownedMem.shrink_to_fit();
      }
    }
  }

  SECTION(loop through allocations and wait for their ref counts to be zero before exiting the loop)
  {
    // will the allocations need to be freed here like the normal deallocation loop?
  }

  lf.decThreadCount();
}

// end function implementations



// priority queue of packets - sort by frame number, then dest node, then dest slot
//
// lock free hash table of in flight packets?

#endif // endif for implementation

#endif

