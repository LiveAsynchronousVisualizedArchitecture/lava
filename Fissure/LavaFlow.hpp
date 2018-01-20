

#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __LAVAFLOW_HEADERGUARD_HPP__
#define __LAVAFLOW_HEADERGUARD_HPP__

#include <cstdint>
#include <bitset>
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>
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
  #define NOMINMAX
  #include <Windows.h>
#endif

#define LAVA_ARG_COUNT 512

#if defined(_MSC_VER)
  //namespace fs = std::tr2::sys;                                                             // todo: different compiler versions would need different filesystem paths
  namespace fs = std::experimental::filesystem;
#endif

//#define _SCL_SECURE_NO_WARNINGS
struct LavaQ
{
// single producer, multi-consumer queue
// Design: Two buffers with a single 64 bit atomic that contains which buffer to use as well as the start and end of the queue
// Two function pointers contain the allocation and deallocation functions to use so that the data structure can cross and shared library boundary

//public:
  using         T = int;
  using      aPtr = std::atomic<T*>;
  using       u64 = uint64_t;
  using     abool = std::atomic<bool>;
  using       au8 = std::atomic<u8>;
  using      au64 = std::atomic<u64>;
  using AllocFunc = void*(*)(u64);
  using  FreeFunc = void(*)(void*);

  //union StEnBuf
  //{
  //  struct { u64 st : 31; u64 en : 31; u64 useA : 1; };
  //  u64 asInt;
  //};
  union StEnBuf
  {
    struct { u64 st : 63; u64 useA : 1; };
    u64 asInt;
  };

//private:
  const u32 INITIAL_CAPACITY = 2;

  AllocFunc   m_alloc = nullptr;
  FreeFunc     m_free = nullptr;
  T*           m_memA = nullptr;
  T*           m_memB = nullptr;
  //aPtr*        m_memA = nullptr;
  //aPtr*        m_memB = nullptr;
  au8          m_capA = 0;                             // capacity will always be a power of two
  au8          m_capB = 0;                             // capacity will always be a power of two
  au64          m_end = 0;
  StEnBuf       m_buf;

  #ifndef _NDEBUG
    std::thread::id  writeThrd;
  #endif

  void init()
  {
    m_buf.useA = true;
    m_buf.st   = 0;
    //m_buf.en   = 0;
    //m_end = 0;

    #ifndef _NDEBUG
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
    if(m_memA)
      free(m_memA);
    if(m_memB)
      free(m_memB);
  }


  StEnBuf       loadBuf() const
  {
    StEnBuf ret;
    ret.asInt = ((au64*)&m_buf.asInt)->load();
    return ret;
  }
  StEnBuf switchBuffers() const
  {
    // it doesn't matter if the start and end have changed when copying the memory from one buffer to the other, since the reading threads will only increment the start
    StEnBuf prev, buf;
    au64* abuf = (au64*)&m_buf.asInt;
    do{
      prev = buf = loadBuf();
      buf.useA = !buf.useA;
    }while( !abuf->compare_exchange_strong(prev.asInt, buf.asInt) );

    return prev;
  }
  //StEnBuf        incEnd() const
  u64            incEnd()
  {
    assert(std::this_thread::get_id() == writeThrd);

    return m_end.fetch_add(1);

    //StEnBuf prev, buf;
    //au64* abuf = (au64*)&m_buf.asInt;
    //do{
    //  prev = buf = loadBuf();
    //  buf.en += 1;
    //}while( !abuf->compare_exchange_strong(prev.asInt, buf.asInt) );
    //
    //return prev;
  }
  //StEnBuf      incStart() const
  //{
  //  StEnBuf prev, buf;
  //  au64* abuf = (au64*)&m_buf.asInt;
  //  do{
  //    prev = buf = loadBuf();
  //    buf.st += 1;
  //  }while( !abuf->compare_exchange_strong(prev.asInt, buf.asInt) );
  //
  //  return prev;
  //}
  u64              capA() const
  {
    // use functions for getting capacity, since it will always be a power of two, and thus will never be over 64, let alone the 255 that a u8 can store
    assert(m_capA < 64);
    return (1 << m_capA.load()) >> 1;
  }
  u64              capB() const
  {
    // use functions for getting capacity, since it will always be a power of two, and thus will never be over 64, let alone the 255 that a u8 can store
    assert(m_capB < 64);
    return (1 << m_capB.load()) >> 1;
  }
  void           expand()
  {
    // 0. assume that the opposite buffer pointer is nullptr
    // 1. make another allocation 
    // 2. copy the data
    // 3. switch the buffers
    // 4. deallocate the previous allocation

    using namespace std;
    
    auto buf = loadBuf();
    auto  en = m_end.load(); 
    if( m_memA==nullptr && m_memB==nullptr ){
      // initial allocation
      m_capA = INITIAL_CAPACITY;
      m_memA = (T*)m_alloc( capA() * sizeof(T) );
    }else if( buf.useA ){
      auto   prevCap = m_capA.load();
      m_capB.store(prevCap + 1);                             // since capacity is stored as a power of 2 instead of an absolute number, only add 1 instead of bitshifting 1 over  
      auto prvRealCap = capA();                              // previous real capacity - this should equal 2 to the power of the m_capA variable
      auto nxtRealCap = capB();                              // next real capacity - this should equal 2 to the power of the m_capB variable
      void* nxtAlloc = m_alloc( nxtRealCap * sizeof(T) );    // the capacity functions will get the absolute value from the power of 2 that is stored in the 1 byte capacity variables

      //if(m_memB) free(m_memB);
      //m_memB         = (T*)nxtAlloc;                         // 1
      T* prevMem = m_memB;
      m_memB     = (T*)nxtAlloc;                         // 1
      if(prevMem) free(prevMem);

      //copy(m_memA, m_memA + prvRealCap, m_memB);             // 2  - only need to copy the number of T equal to the capacity of memA
      
      for(u64 i=buf.st; i<en; ++i){                          // other threads can increment buf.st, but that should only mean some of the copied elements have already been popped and not cause an actual problem
        auto prevIdx = i % prvRealCap;
        auto  nxtIdx = i % nxtRealCap;
        m_memB[nxtIdx] = m_memA[prevIdx];                    // only this thread can change anything, so no other thread should interfere with the actual data
      }

      switchBuffers();                                   // this has to make sure that the start and end stay the same, but the current thread is the only one that can change the buffer bit or the end. 
      //m_free(m_memA);                                  // 4
      //m_memA = nullptr;                                // 4
    }else{
      auto    prevCap = m_capB.load();
      m_capA.store(prevCap + 1);                             // since capacity is stored as a power of 2 instead of an absolute number, only add 1 instead of bitshifting 1 over  
      auto prvRealCap = capB();                              // previous real capacity - this should equal 2 to the power of the m_capA variable
      auto nxtRealCap = capA();                              // next real capacity - this should equal 2 to the power of the m_capB variable
      void*  nxtAlloc = m_alloc( nxtRealCap * sizeof(T) );   // the capacity functions will get the absolute value from the power of 2 that is stored in the 1 byte capacity variables

      T* prevMem = m_memA;
      m_memA     = (T*)nxtAlloc;                             // 1
      if(prevMem) free(prevMem);

      for(u64 i=buf.st; i<en; ++i){                          // other threads can increment buf.st, but that should only mean some of the copied elements have already been popped and not cause an actual problem
        auto prevIdx = i % prvRealCap;
        auto  nxtIdx = i % nxtRealCap;
        m_memA[nxtIdx] = m_memB[prevIdx];                    // only this thread can change anything, so no other thread should interfere with the actual data
      }

      switchBuffers();                                   // this has to make sure that the start and end stay the same, but the current thread is the only one that can change the buffer bit or the end. 
    }

    //}else{
    //  auto   prevCap = m_capB.load();
    //  m_capA.store(prevCap + 1);                         // since capacity is stored as a power of 2 instead of an absolute number, only add 1 instead of bitshifting 1 over  
    //  void* nxtAlloc = m_alloc( capA() * sizeof(T) );    // the capacity functions will get the absolute value from the power of 2 that is stored in the 1 byte capacity variables
    //  
    //  if(m_memA) free(m_memA);
    //  m_memA         = (T*)nxtAlloc;                     // 1

    //  copy(m_memB, m_memB + capB(), m_memA);             // 2  - only need to copy the number of T equal to the capacity of memA

    //  switchBuffers();                                   // this has to make sure that the start and end stay the same, but the current thread is the only one that can change the buffer bit
    //  //m_free(m_memB);                                    // 4
    //  //m_memB = nullptr;                                  // 4
    //}
  }
  u64              size() const
  { 
    auto buf = loadBuf();
    return m_end.load() - buf.st;
  }
  StEnBuf          push(T const& val)
  {
    assert(std::this_thread::get_id() == writeThrd);

    // will need to increment both the current index and size at the same time
    // if the capacity is not high enough, use a secondary buffer to enlarge it
    // this should work well, since only the owning thread can make it larger, while any thread can pop() items from the queue
    u64 cap=0;                          // this will append a value to the end, while pop, will grab it from the start/current index
    auto buf = loadBuf();
    if( buf.useA ){
      cap = capA();
    }else{
      cap = capB();
    }

    if( size() >= cap ){
      expand();
      buf = loadBuf();
    }

    if( buf.useA ){
      auto idx = m_end.load() % capA();
      m_memA[idx] = val;
    }else{ 
      auto idx = m_end.load() % capB();
      m_memB[idx] = val;
    }

    incEnd();

    return buf;
  }
  bool              pop(T& ret) const
  {
    //ret = 85;
    //return true;

    // this will need to copy the element indexed by m_cur, 
    // then make sure the index (which will only increase) hasn't changed while incrementing the start
    StEnBuf prev, buf;
    au64* abuf = (au64*)&m_buf.asInt;
      prev = buf = loadBuf();
      assert(buf.st <= m_end);
      if(buf.st == m_end) return false;
      //if(buf.st == buf.en) return false;

      u64 idx = 0;
      u64 cap = 0;
      if( buf.useA ){
        cap = capA();
        idx = buf.st % cap;
        ret = m_memA[idx];
      }else{ 
        cap = capB();
        idx = buf.st % cap;
        ret = m_memB[idx];
      }

      assert(ret >= 0);

      buf.st += 1;
    bool ok = abuf->compare_exchange_strong(prev.asInt, buf.asInt);

    return ok;
  }
  T                 atA(u64 i)
  {
    return m_memA[i];
  }
  T                 atB(u64 i)
  {
    return m_memB[i];
  }
};

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
static inline u64 popcount64(u64 x)
{
  u64 m1 = 0x5555555555555555ll;
  u64 m2 = 0x3333333333333333ll;
  u64 m4 = 0x0F0F0F0F0F0F0F0Fll;
  u64 h01 = 0x0101010101010101ll;

  x -= (x >> 1) & m1;
  x = (x & m2) + ((x >> 2) & m2);
  x = (x + (x >> 4)) & m4;

  return (x * h01) >> 56;
}
// GCC built in intrinsic - int  __builtin_popcount(unsigned int)
// End libpopcnt

// static data segment data
#if defined(_WIN32)
  static const std::string  liveExt(".live.dll");
#endif
static __declspec(thread)       void*   lava_thread_heap = nullptr;       // thread local handle for thread local heap allocations
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
inline void*   LavaHeapInit(size_t initialSz = 0)
{
  if(!lava_thread_heap) {
    lava_thread_heap = HeapCreate(HEAP_NO_SERIALIZE, initialSz, 0);
  }
  return lava_thread_heap;
}
inline void*   LavaHeapAlloc(size_t sz)
{
  void* thread_heap = LavaHeapInit(sz);

  void* ret = nullptr;
  if(thread_heap)
    ret = HeapAlloc(thread_heap, HEAP_NO_SERIALIZE, sz);

  return ret;
}
inline int     LavaHeapFree(void* memptr)
{
  void* thread_heap = LavaHeapInit();

  void* ret = nullptr;
  if(thread_heap && memptr) {
    auto ret = HeapFree(thread_heap, HEAP_NO_SERIALIZE, memptr);
    return ret;
  }else{ return 0; }
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
  //if(n==0) return;
  //
  //ThreadFree(p);

  LavaHeapFree(p);
  //p = nullptr;
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
//using lava_threadQ       =  std::queue<LavaPacket, std::vector<LavaPacket,ThreadAllocator<LavaPacket>> >;
//using lava_threadQ       =  std::queue<LavaOut, std::vector<LavaOut,ThreadAllocator<LavaOut>> >;
//using lava_threadQ       =  std::queue<LavaOut, std::deque<LavaOut,ThreadAllocator<LavaOut>> >;
using lava_threadQ       =  std::queue<LavaOut, std::deque<LavaOut> >;

//extern "C" using            FlowFunc  =  uint64_t (*)(LavaParams*, LavaVal*, LavaOut*);   // data flow node function
//extern "C" using            FlowFunc  =  uint64_t (*)(LavaParams*, LavaFrame*, LavaOut*);   // node function taking a LavaFrame in - todo: need to consider output, might need a LavaOutFrame or something similiar 
extern "C" using           LavaAlloc  =  void* (*)(uint64_t);                                    // custom allocation function passed in to each node call
extern "C" using  GetLavaFlowNodes_t  =  LavaNode*(*)();                                         // the signature of the function that is searched for in every shared library - this returns a LavaFlowNode* that is treated as a sort of null terminated list of the actual nodes contained in the shared library 
extern "C" using            FlowFunc  =  uint64_t (*)(LavaParams*, LavaFrame*, lava_threadQ*);   // node function taking a LavaFrame in - todo: need to consider output, might need a LavaOutFrame or something similiar 

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
  u8        count() const  // make x an atomic load
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
  enum { NONE=0, END, DATA_ERROR, STORE, MEMORY, SEQUENCE, ENUMERATION };                // todo: does this need store sequence and memory sequence?
  u8 asInt;
};
union          LavaId                                                // this Id serves as both a nodeId and LavaFlowSlot index, since a LavaFlowSlot index will alway coordinate with only one node 
{    
  static  const u64 NODE_NONE = 0xFFFFFFFFFFFF;                  // 48 bits all set
  static  const u64 SLOT_NONE = 0xFFFF;                          // 16 bits all set

  struct { 
    u64  nid : 48;                                       // id is the node id number - This is a unique number for each node that doesn't change. It can refer back to a node since it doesn't represent an ordering or an index into an array 
    u64 sidx : 16;                                       // idx is the index of the LavaFlowSlot - for a node this is 0
  };
  u64 asInt;

  LavaId() : nid(NODE_NONE), sidx(SLOT_NONE) {}
  LavaId(u64 _id, u64 _idx=SLOT_NONE) : nid(_id), sidx(_idx) {}

  bool   operator==(LavaId  r) const { return nid==r.nid && sidx==r.sidx; }
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
  u32            inputs;
  u32           outputs;
  u64             frame;
  LavaId             id;
  LavaAlloc   mem_alloc;
  //LavaPut           put;
};
struct        LavaVal
{
  u64      type :  3;          // ArgType
  u64     value : 61;
};
struct        LavaOut
{
  // todo: change to LavaVal
  u64      type :  3;          // ArgType
  u64     value : 61;          // This will hold the address in memory - could also hold the starting block index in the database 

  union {
    struct { u64 frame; u32 slot; u32 listIdx; };
    u8 bytes[16];
  }key;
};
struct        LavaMsg
{
  u64        id;
  LavaVal    val;
};
struct     LavaPacket
{
  u64    ref_count;                               // todo: is this used? 
  u64        frame : 55;
  u64       framed :  1;
  u64     priority :  8;
  u64    dest_node : 48;
  u64    dest_slot : 16;
  u64     src_node : 48;
  u64     src_slot : 16;
  u64   rangeStart;
  u64     rangeEnd;
  u64     sz_bytes;                               // the size in bytes can be used to further sort the packets so that the largets are processed first, possibly resulting in less memory usage over time
  LavaMsg      msg;

  bool operator<(LavaPacket const& r) const
  {    
    if(frame    != r.frame)    return frame    > r.frame;               // want lowest frame numbers to be done first 
    if(priority != r.priority) return priority < r.priority;            // want highest priority to be done first
    if(sz_bytes != r.sz_bytes) return sz_bytes < r.sz_bytes;            // want largest sizes to be done first
    if(framed   != r.framed)   return framed   < r.framed;              // want framed packets to be done first so that their dependencies can be resolved

    return msg.id > r.msg.id;
  }
};
struct      LavaFrame
{
  enum FRAME { ERR_FRAME = 0xFFFFFFFFFFFFFFFE, NO_FRAME = 0xFFFFFFFFFFFFFFFF };
  static const u64 PACKET_SLOTS = 16;
  //using Slots = std::atomic<std::bitset<PACKET_SLOTS>>;
  //using Slots = std::bitset<PACKET_SLOTS>;
  using Slots = AtomicBitset;

  u64                dest = LavaId::NODE_NONE;             // The destination node this frame will be run with
  u64               frame = 0;                             // The numer of this frame - lowest frame needs to be run first
  u64           src_frame = 0;                             // Does this need to come from the message node?
  u64          dest_frame = 0;                             // should this come from the node instance?
  Slots          slotMask;// = 0;                             // The bit mask respresenting which slots already have packets in them

  u16               slots = 0;                           // The total number of slots needed for this frame to be complete 
  LavaPacket      packets[PACKET_SLOTS];

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
  enum Type { NONE=0, FLOW, MSG, NODE_ERROR=0xFFFFFFFFFFFFFFFF };                              // this should be filled in with other node types like scatter, gather, transform, generate, sink, blocking sink, blocking/pinned/owned msg - should a sink node always be pinned to it's own thread

  FlowFunc              func;
  uint64_t         node_type;
  const char*           name;
  const char**      in_names;
  const char**     out_names;
  const char**      in_types;
  const char**     out_types;
  uint64_t           version;
};
struct       LavaInst
{
  using au32  =  std::atomic<uint32_t>;
  using au64  =  std::atomic<uint64_t>;

  enum State { NORMAL=0, RUN_ERROR, LOAD_ERROR, COMPILE_ERROR };

  LavaId         id;
  LavaNode*    node;
  u32        inputs;
  u32       outputs;
  union{ State state = NORMAL; u32 stateU32; };

  mutable u64   frame = 0;  // todo: make this atomic 
  mutable u64    time = 0;  // todo: make this atomic / make functions to add time and get the current time

  u64   fetchIncFrame() const
  {
    return std::atomic_fetch_add( (au64*)(&frame), 1);
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
  enum State { NORMAL=0, HIGHLIGHTED, SELECTED, SLOT_ERROR };

  LavaId id; bool in=false; State state=NORMAL;

  LavaFlowSlot(){}
  LavaFlowSlot(LavaId Id, bool In=false) : id(Id), in(In), state(NORMAL) {}

  //u64 nid; bool in=false; State state=NORMAL;
  //LavaFlowSlot(u64 nId, bool In=false) : nid(nId), in(In), state(NORMAL) {}
};
struct        LavaMem
{
  using au64 = std::atomic<uint64_t>;

  void* ptr = nullptr;

  uint64_t    refCount()const{ return ((uint64_t*)ptr)[0]; }
  uint64_t   sizeBytes()const{ return ((uint64_t*)ptr)[1]; }
  uint64_t&   refCount()     { return ((uint64_t*)ptr)[0]; }
  uint64_t&  sizeBytes()     { return ((uint64_t*)ptr)[1]; }
  void*           data()     { return ((uint64_t*)ptr)+2;  }

  uint64_t      incRef()     { return ((au64*)ptr)->fetch_add( 1); }
  uint64_t      decRef()     { return ((au64*)ptr)->fetch_add(-1); }

  static LavaMem fromDataAddr(uint64_t addr)                                       // create a LavaMem struct from the address of the start of the data (after the reference count and sizeBytes at the start of the allocation)
  {
    LavaMem lm;
    lm.ptr =  (void*)(addr - sizeof(uint64_t)*2);
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

  //union { LavaId A; LavaId dest; LavaNode* ndptr; }; 
  //union { LavaId B; LavaId  src; };
};
// end data types


extern "C" __declspec(dllexport) LavaNode* GetLavaFlowNodes();   // prototype of function to return static plugin loading struct
// end function declarations

using lava_memvec          =  std::vector<LavaMem, ThreadAllocator<LavaMem> >;

// Lava Helper Functions

// todo: once tbl is switched to not be an array, this might not need to be a template
// todo: allocation template parameters might mean that a template is still neccesary
// todo: A table type that has empty allocation parameters could mean an unowned type
//       | the unowned type could have a constructor that takes any tbl and makes it unowned, treating it effectivly as a reference
template<class T> LavaOut  LavaTblToOut(LavaParams const* lp, tbl<T> const& t)
{
  void* outmem  =  lp->mem_alloc(t.sizeBytes());
  memcpy(outmem, t.memStart(), t.sizeBytes());

  LavaOut o;
  o.value = (u64)outmem;
  o.type  = LavaArgType::MEMORY;

  return o;
}
// End Lava Helper Functions

class       LavaGraph                  // LavaGraph should specifically be about the connections between nodes
{
public:
  enum       Cmd { ADD_NODE=0, ADD_SLOT, TGL_CNCT, DEL_NODE, DEL_CNCT };
  union      Arg { LavaId id; u64 val; LavaNode* ndptr; };
  struct Command { Cmd cmd; Arg A; Arg B; };

  struct NodeInstance { uint64_t id; LavaNode* nd; };                  // a struct used for returning an instance of a node - the Nodes map of ids and LavaFlowNode pointers  

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
  using MsgIds        =  std::unordered_set<LavaId, LavaId>;           // LavaId has an operator() to hash itself
  using MsgCache      =  std::vector<LavaId>;                          // LavaId has an operator() to hash itself
  using NormalizeMap  =  std::unordered_map<uint64_t, uint64_t>;
  using CmdQ          =  std::queue<LavaCommand>;
  using RetStk        =  std::stack<LavaCommand::Arg>;
  using ArgVec        =  std::vector<LavaCommand::Arg>;
  //using CmdQ          =  std::priority_queue<LavaCommand>;

private:
  mutable abool         m_useA;

  uint64_t             m_nxtId;               // nxtId is next id - a counter for every node created that only increases, giving each node a unique id
  CmdQ                  m_cmdq;
  RetStk                 m_stk;

  //NodeInsts          m_nodesA;
  //Slots              m_slotsA;
  //CnctMap            m_cnctsA;
  //SrcMap         m_destCnctsA;
  //MsgIds          m_msgNodesA;

  NodeInsts           m_nodesA;
  Slots               m_slotsA;
  CnctMap             m_cnctsA;
  SrcMap          m_destCnctsA;
  MsgIds           m_msgNodesA;
  MsgCache         m_msgCacheA;

  NodeInsts           m_nodesB;
  Slots               m_slotsB;
  CnctMap             m_cnctsB;
  SrcMap          m_destCnctsB;
  MsgIds           m_msgNodesB;
  MsgCache         m_msgCacheB;

public:
  NodeInsts&            curNodes(){ return m_useA.load()?  m_nodesA     : m_nodesB;     }
  Slots&                curSlots(){ return m_useA.load()?  m_slotsA     : m_slotsB;     }
  CnctMap&              curCncts(){ if(m_useA.load()) return m_cnctsA; else return m_cnctsB; }
  SrcMap&           curDestCncts(){ return m_useA.load()?  m_destCnctsA : m_destCnctsB; }
  MsgIds&            curMsgNodes(){ return m_useA.load()?  m_msgNodesA  : m_msgNodesB;  }
  MsgCache&          curMsgCache(){ return m_useA.load()?  m_msgCacheA  : m_msgCacheB;  }
  NodeInsts const&      curNodes()const{ return m_useA.load()?  m_nodesA     : m_nodesB;     }
  Slots     const&      curSlots()const{ return m_useA.load()?  m_slotsA     : m_slotsB;     }
  CnctMap   const&      curCncts()const{ return m_useA.load()?  m_cnctsA     : m_cnctsB;     }
  SrcMap    const&  curDestCncts()const{ return m_useA.load()?  m_destCnctsA : m_destCnctsB; }
  MsgIds    const&   curMsgNodes()const{ return m_useA.load()?  m_msgNodesA  : m_msgNodesB;  }

  NodeInsts&            oppNodes(){ return !m_useA.load()?  m_nodesA     : m_nodesB;     }
  Slots&                oppSlots(){ return !m_useA.load()?  m_slotsA     : m_slotsB;     }
  CnctMap&              oppCncts(){ if(!m_useA.load()) return m_cnctsA; else return m_cnctsB; }
  SrcMap&           oppDestCncts(){ return !m_useA.load()?  m_destCnctsA : m_destCnctsB; }
  MsgIds&            oppMsgNodes(){ return !m_useA.load()?  m_msgNodesA  : m_msgNodesB;  }
  MsgCache&          oppMsgCache(){ return !m_useA.load()?  m_msgCacheA  : m_msgCacheB;  }
  NodeInsts const&      oppNodes()const{ return !m_useA.load()?  m_nodesA     : m_nodesB;     }
  Slots     const&      oppSlots()const{ return !m_useA.load()?  m_slotsA     : m_slotsB;     }
  CnctMap   const&      oppCncts()const{ return !m_useA.load()?  m_cnctsA     : m_cnctsB;     }
  SrcMap    const&  oppDestCncts()const{ return !m_useA.load()?  m_destCnctsA : m_destCnctsB; }
  MsgIds    const&   oppMsgNodes()const{ return !m_useA.load()?  m_msgNodesA  : m_msgNodesB;  }


  void            init()
  { 
    m_useA.store(true);
    m_nxtId = 1;
  }
  void              mv(LavaGraph&& rval)
  {
    using namespace std;

    m_nodesA     = move(rval.m_nodesA); 
    m_slotsA     = move(rval.m_slotsA); 
    m_cnctsA     = move(rval.m_cnctsA); 
    m_destCnctsA = move(rval.m_destCnctsA);

    m_nodesB     = move(rval.m_nodesB); 
    m_slotsB     = move(rval.m_slotsB); 
    m_cnctsB     = move(rval.m_cnctsB); 
    m_destCnctsB = move(rval.m_destCnctsB);

    //m_running   = rval.m_running;
    //m_ids       = move(rval.m_ids);
  }
  void              cp(LavaGraph const& lval)
  {
    using namespace std;

    m_nodesA     = lval.m_nodesA;
    m_slotsA     = lval.m_slotsA; 
    m_cnctsA     = lval.m_cnctsA; 
    m_destCnctsA = lval.m_destCnctsA;

    m_nodesB     = lval.m_nodesB;
    m_slotsB     = lval.m_slotsB; 
    m_cnctsB     = lval.m_cnctsB; 
    m_destCnctsB = lval.m_destCnctsB;
  }
  u64              nxt(){ return m_nxtId++; }

  auto   nodeDestSlots(vec_nptrs const& nds) -> vec_ids
  {
    using namespace std;

    vec_ids sidxs;                                            // sidxs is slot indexes
    for(auto ni : nds){                                       // ni is Node Instance np is node pointer and nds is nodes
      auto si = lower_bound(ALL(curSlots()), LavaId(ni.id.nid), [](auto a,auto b){ return a.first < b; } );          // si is slot iterator
      if(si != end(curSlots())  &&  si->first.nid == ni.id.nid){
        LavaFlowSlot& s = si->second;
        if(s.in) sidxs.push_back(si->first);
      }
    }
    return sidxs;                                        // RVO
  }
  auto       nodeSlots(vec_nptrs const& nds) -> vec_ids
  {
    using namespace std;

    vec_ids sidxs;                                            // sidxs is LavaFlowSlot indexes
    for(auto ni : nds){                                       // np is node pointer and nds is nodes
      auto si = lower_bound(ALL(curSlots()), LavaId(ni.id.nid), [](auto a,auto b){ return a.first < b; } );          // si is slot iterator
      if(si != end(curSlots())  &&  si->first.nid == ni.id.nid){
        LavaFlowSlot& s = si->second;
        sidxs.push_back(si->first);        
      }
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
  
  u64          nxtSlot(u64 nid) // non-const, part of writing, and needs to use the opposite buffer
  {
    auto si = nodeSlots(nid);                   // si is slot iterator
    i64 cur = -1;
    while(si != end(oppSlots())   && 
      si->first.nid  == nid && 
      si->first.sidx <= (u64)(cur+1) ){
      //cur = si->first.idx;
      ++cur; ++si; 
    }
    //return cur;

    return cur + 1;
  }
  u32       countSlots(const char** typeList)
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
    NormalizeMap nids;
    nids.reserve(curNodes().size());
    u64 cur = 1;
    for(auto& kv : curNodes()){
      nids[kv.first] = cur++;
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

    // slots
    Slots nxtSlots;
    for(auto& kv : curSlots()){
      LavaId nxtId  = kv.first;
      nxtId.nid     = nids[nxtId.nid];
      LavaFlowSlot nxtS = kv.second;
      auto nxtSltId = nids[nxtS.id.nid];
      nxtS.id.nid   = nxtSltId;
      nxtS.id.sidx  = nxtS.id.sidx;
      nxtSlots.insert({nxtId, nxtS});
    }
    curSlots() = move(nxtSlots);

    // node ids 
    NodeInsts nxtNds;
    for(auto& kv : curNodes()){
      u64     nxtId = nids[kv.first];
      LavaInst inst = curNodes()[kv.first];
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
    curSlots().clear();
    curCncts().clear();
    curDestCncts().clear();
    curMsgNodes().clear();

    init();
    //m_ids.clear();
  }
  //u64                 put(LavaCommand::Command cmd, LavaId A, LavaId B=LavaId())
  u64                  put(LavaCommand::Command cmd, LavaCommand::Arg A, LavaCommand::Arg B = LavaCommand::Arg() )
  {
    m_cmdq.push({cmd, A, B});
    return m_cmdq.size();
  }
  auto                exec() -> ArgVec
  {
    using namespace std;
    
    ArgVec returned;
    auto sz = m_cmdq.size();
    if(sz > 0)
    {
      if(m_useA.load()){
        m_nodesB     = m_nodesA;
        m_slotsB     = m_slotsA; 
        m_cnctsB     = m_cnctsA; 
        m_destCnctsB = m_destCnctsA;
        m_msgNodesB  = m_msgNodesA;
        m_msgCacheB  = m_msgCacheA;
      }else{
        m_nodesA     = m_nodesB;
        m_slotsA     = m_slotsB; 
        m_cnctsA     = m_cnctsB; 
        m_destCnctsA = m_destCnctsB;
        m_msgNodesA  = m_msgNodesB;
        m_msgCacheA  = m_msgCacheB;
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
      auto&    cur = oppMsgNodes();
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
    //u64 id = nxt();
    if(ln->node_type == LavaNode::MSG)
      oppMsgNodes().insert(nid);

    LavaInst li = makeInst(nid, ln);
    return oppNodes().insert({nid, li}).first->first;                             // returns a pair that contains the key-value pair
  }
  uint64_t    addNode(LavaNode* ln, bool newId=true)
  {
    u64 id = nxt();
    if(ln->node_type == LavaNode::MSG)
      oppMsgNodes().insert(id);

    LavaInst li = makeInst(id, ln);
    return oppNodes().insert({id, li}).first->first;                             // returns a pair that contains the key-value pair
  }
  bool        delNode(uint64_t nid)
  {
    // erase any connects that go with the slots here
    auto     si = this->nodeSlots(nid);                       // si is slot iterator
    auto siCnct = si; 
    for(; siCnct!=end(oppSlots())  &&  siCnct->first.nid==nid; ++siCnct ){
      if(siCnct->second.in) this->delDestCnct(nid);
      else                  this->delSrcCncts(nid);
    }

    auto slcnt = oppSlots().erase(nid);                         // slcnt is slot count
    auto   cnt = oppNodes().erase(nid);
    oppMsgNodes().erase(nid);

    return (cnt+slcnt) > 0;                                  // return true if 1 or more elements were erased, return false if no elements were erasedm
  }
  LavaId      addSlot(LavaFlowSlot  s, u64 sidx=0)
  {
    LavaId id(s.id.nid, sidx);
    id.sidx = sidx? sidx  :  nxtSlot(s.id.nid);
    s.id    = id;
    oppSlots().insert({id, s});

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

    //LavaInst ret;
    //ret.id   = nIter->first;
    //ret.node = nIter->second;
    //return ret;

    return nIter->second;
  }
  auto           node(u64 id) const -> LavaInst
  {
    auto nIter = curNodes().find(id);                                          // nIter is node iterator
    if(nIter == end(curNodes())) return errorInst();

    return nIter->second;
  }
  auto          nodes() const -> vec_insts
  {
    vec_insts nds;
    nds.reserve(curNodes().size());
    for(auto& on : curNodes()){                                  // on is order and node - order is on.first    LavaInst is on.second
      nds.push_back(on.second);

      //NodeInstance inst;
      //inst.id = on.first;
      //inst.nd = on.second;
      //nds.push_back(inst);
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
  u64             nsz() const { return curNodes().size(); }
  auto       msgNodes() const -> MsgIds const&
  {
    return curMsgNodes();
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
  auto           slot(LavaId   id) -> LavaFlowSlot*
  {
    auto si = curSlots().find(id);
    if(si == curSlots().end()) 
      return nullptr;

    return &si->second;
  }
  auto      nodeSlots(u64     nid) const -> decltype(curSlots().cbegin())
  {
    return lower_bound(ALL(curSlots()), nid, [](auto a,auto b){ return a.first.nid < b; } );
  }
  auto      nodeSlots(u64     nid) -> decltype(oppSlots().begin())
  {
    return lower_bound(ALL(oppSlots()), nid, [](auto a,auto b){ return a.first.nid < b; } );
  }
  auto          slots() -> Slots& { return curSlots(); }
  auto          slots() const -> Slots const& { return curSlots();  }
  auto       getSlots() -> Slots& { return curSlots(); }
  auto        srcSlot(LavaId destId) -> LavaFlowSlot* 
  {
    auto ci = curCncts().find(destId);
    if(ci != curCncts().end()){
      LavaFlowSlot* s = slot(ci->second);
      return s;
    }else 
      return nullptr;
  }
  auto      destSlots(LavaId  srcId) -> decltype(curDestCncts().find(srcId))
  {
    return curDestCncts().find(srcId);
  }
  auto        slotEnd() -> decltype(curSlots().end()) { return curSlots().end(); }
  u64             ssz() const { return curSlots().size(); }

  // connections
  u32     delSrcCncts(LavaId  src)
  {
    u32  cnt = 0;
    auto  di = curDestCncts().find(src);
    for(; di!=curDestCncts().end() && di->first==src; ++cnt, ++di){   // ++di,
      curCncts().erase(di->second);
    }
    curDestCncts().erase(src);

    return cnt;

    //di = m_destCncts.find(src);
  }
  auto      destCncts(LavaId  src) -> decltype(curDestCncts().begin()) // C++14 -> decltype(m_slots.find(Id(nid)))
  {
    return lower_bound(ALL(curDestCncts()), LavaId(src), [](auto a,auto b){ return a.first < b; } );
  }
  u64   destCnctCount(LavaId  src)
  {
    return curDestCncts().count(src);
  }
  bool    delDestCnct(LavaId dest)
  {
    auto iter = curCncts().find(dest);
    if(iter==curCncts().end()) return false;

    auto src = iter->second;
    curCncts().erase(dest);

    auto srcIter = curDestCncts().find(src);
    for(; srcIter!=end(curDestCncts()) && srcIter->first==src && srcIter->second==dest; ){
      auto cpy = srcIter++;
      curDestCncts().erase(cpy);
    }

    return true;
  }
  auto    destCnctEnd() -> decltype(curDestCncts().end()) { return curDestCncts().end(); }
  auto        cnctEnd() -> decltype(curCncts().end())     { return curCncts().end(); }
  auto      cnctBegin() -> decltype(curCncts().begin())   { return curCncts().begin(); }
  auto          cncts() -> CnctMap& { return curCncts(); }
  auto          cncts() const -> CnctMap const& { return curCncts(); }
  u64          cnctsz() const { return curCncts().size(); }
  u32         delCnct(LavaId id)
  {
    u32 cnt = 0;
    LavaFlowSlot* s = this->slot(id);
    if(!s) return 0;

    if(s->in){
      auto iter = curCncts().find(id);
      return delCnct(iter->second, iter->first);
    }else{
      auto iter = curDestCncts().find(id);
      auto  idx = iter->first;
      while(iter != curDestCncts().end() && iter->first == idx){
        curCncts().erase(iter->second);
        iter = curDestCncts().erase(iter);
        ++cnt;
      }
    }

    return cnt;
  }
  auto    srcCnctsMap() -> SrcMap&
  {
    return curDestCncts();
  }
};
struct       LavaFlow
{
public:
  using au64            =  std::atomic<uint64_t>;
  using PacketQueue     =  std::priority_queue<LavaPacket>;
  using FrameQueue      =  std::vector<LavaFrame>;
  using MsgNodeVec      =  std::vector<uint64_t>;
  using Mutex           =  std::mutex;
  using PktCalbk        =  void (*)(LavaPacket pkt);
  //using PacketCallback  =  void (*)(LavaPacket pkt);

  enum FlowErr { NONE=0, RUN_ERR=0xFFFFFFFFFFFFFFFF };

  lava_pathHndlMap           libs;     // libs is libraries - this maps the live path of the shared libary with the OS specific handle that the OS loading function returns
  lava_nidMap                nids;     // nids is node ids  - this maps the name of the node to all of the graph node ids that use it
  lava_flowNodes             flow;
  lava_flowPtrs              ptrs;     // ptrs are the LavaFlowNode pointers - each one needs a unique id so they can be referenced elsewhere 
  lava_hndlNodeMap         ndptrs;     // ndptrs is node pointers - a map from each handle to the one (zero?) or more LavaFlowNode pointers the shared lib contains
  lava_nameNodeMap      nameToPtr;     // maps node names to their pointers 

  mutable bool          m_running = false;            // todo: make this atomic
  mutable u64          m_curMsgId = 0;                // todo: make this atomic
  mutable u64             m_frame = 0;                // todo: make this atomic
  mutable LavaId          m_curId = LavaNode::NONE;   // todo: make this atomic - won't be used as a single variable anyway
  mutable u64       m_threadCount = 0;                // todo: make this atomic
  mutable u32             version = 0;                // todo: make this atomic
  mutable Mutex            m_qLck;
  mutable Mutex       m_frameQLck;
  mutable PktCalbk packetCallback;
  mutable PacketQueue           q;
  mutable FrameQueue       frameQ;
  mutable au64         m_nxtMsgNd = 0;
  //mutable PacketCallback  packetCallback;            // todo: make this an atomic version of the function pointer

  MsgNodeVec          m_msgNodesA;
  LavaGraph                 graph;


  void  udpateMsgIdxCache() // todo: not thread safe - this takes the current hash map of message nodes 
  {
    auto& cur = graph.curMsgNodes();
    m_msgNodesA.clear();
    m_msgNodesA.reserve( cur.size() );

    for(auto const& lid : cur){
      m_msgNodesA.push_back(lid.nid);
    }
    //new (&m_msgNodesA) MsgNodeVec( ALL(graph.curMsgNodes()) );
  }
  u64      fetchIncNxtMsg() const                    // fetchIncNxtMsg is fetch increment next message (node index)
  {
    //auto sz = m_msgNodesA.size();
    //return sz? m_nxtMsgNd.fetch_add(1) % sz  :   LavaId::NODE_NONE;
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

    //u64   idx = cur.size()>0? fetchIncNxtMsg()%cur.size();
    //u64    id = LavaId::NODE_NONE;
    //if(idx != LavaId::NODE_NONE){ 
    //  id = m_msgNodesA[idx];
    //}
    //
    //return LavaId::NODE_NONE;
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
  //void          pauseLoop(){}
};

#if defined(__LAVAFLOW_IMPL__)

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
  printf("\n addr: %llu  data addr: %llu  ref count: %llu   size bytes: %llu \n", 
    (u64)(lm.ptr), (u64)(lm.data()), (u64)lm.refCount(), (u64)lm.sizeBytes() );
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
auto        GetRefreshPaths() -> lava_paths
{
  using namespace std;
  using namespace  fs;

  static const regex lavaRegex("lava_.*");
  static const regex extRegex(".*\\.live\\.dll");

  auto libPath = path( GetSharedLibPath() );
  libPath.remove_filename();
  path    root = libPath;

  vector<str> paths;
  auto    dirIter = directory_iterator(root);
  for(auto& d : dirIter){
    auto   p = d.path();
    if(!p.has_filename()){ continue; }

    auto ext = p.extension().generic_string();                     // ext is extension
    if(ext!=".dll"){ continue; }

    str fstr = p.filename().generic_string();                      // fstr is file string
    if(  regex_match(fstr, extRegex) ){ continue; }
    if( !regex_match(fstr,lavaRegex) ){ continue; }

    auto livepth = p;
    livepth.replace_extension( liveExt );

    bool refresh = true;
    if( exists(livepth) ){
      auto liveWrite = last_write_time(livepth).time_since_epoch().count();         // liveWrite is live write time - the live shared library file's last write time 
      auto origWrite = last_write_time(p).time_since_epoch().count();               // origWrite is orginal write time - the original shared library file's last write time
      if( liveWrite > origWrite ) refresh = false;
    }

    if(refresh) paths.push_back( p.generic_string() );
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

void*               LavaAlloc(uint64_t sizeBytes)
{
  //uint64_t* mem = (uint64_t*)malloc(sizeBytes + sizeof(uint64_t)*2);
  uint64_t* mem = (uint64_t*)LavaHeapAlloc(sizeBytes + sizeof(uint64_t)*2);
  mem[0]  =  0;              // reference count
  mem[1]  =  sizeBytes;      // number of bytes of main allocation

  return (void*)(mem + 2);  // sizeof(uint64_t)*2);
}
void                 LavaFree(uint64_t addr)
{
  void* p = (void*)(addr - sizeof(uint64_t)*2);
  //free(p);
  LavaHeapFree(p);
}

//uint64_t      exceptWrapper(FlowFunc f, LavaFlow& lf, LavaParams* lp, LavaVal* inArgs, LavaOut* outArgs)
//uint64_t                runFunc(LavaFlow&   lf, lava_memvec& ownedMem, uint64_t nid, LavaParams* lp, LavaVal* inArgs,  LavaOut* outArgs) noexcept   // runs the function in the node given by the node id, puts its output into packets and ultimatly puts those packets into the packet queue

LavaInst::State exceptWrapper(FlowFunc f, LavaFlow& lf, LavaParams* lp, LavaFrame* inFrame, lava_threadQ* outArgs) // LavaOut* outArgs)
{
  LavaInst::State        ret = LavaInst::NORMAL;  // LavaFlow::NONE;
  uint64_t         winExcept = 0;
  __try{
    //f(lp, inArgs, outArgs);
    f(lp, inFrame, outArgs);
  }__except( (winExcept=GetExceptionCode()) || EXCEPTION_EXECUTE_HANDLER ){
    ret =  LavaInst::RUN_ERROR; // LavaFlow::RUN_ERR;
    //lf.graph.setState(lp->id.nid, LavaInst::RUN_ERROR);

    printf("\n windows exception code: %llu \n", winExcept);
  }

  return ret;
}
LavaInst::State       runFunc(LavaFlow&   lf, lava_memvec& ownedMem, uint64_t nid, LavaParams* lp, LavaFrame* inFrame,  lava_threadQ* outArgs) noexcept // LavaOut* outArgs) noexcept   // runs the function in the node given by the node id, puts its output into packets and ultimatly puts those packets into the packet queue
{
  using namespace std;
  using namespace std::chrono;

  LavaInst&  li = lf.graph[nid];
  FlowFunc func = li.node->func;                      //lf.graph[nid].node->func;
  if(func)
  {
    LavaParams lp;
    SECTION(create arguments and call function)
    {
      lp.inputs      =     1;
      lp.outputs     =   512;
      lp.frame       =   lf.m_frame;
      lp.id          =   LavaId(nid);
      lp.mem_alloc   =   LavaAlloc;             //lp.mem_alloc   =   malloc;  // LavaHeapAlloc;

      auto  stTime = high_resolution_clock::now();
        LavaInst::State ret = exceptWrapper(func, lf, &lp, inFrame, outArgs);
        //if(ret != LavaFlow::NONE){ return ret; }
        if(ret != LavaInst::NORMAL){ return ret; }
      auto endTime = high_resolution_clock::now();
      duration<u64,nano> diff = (endTime - stTime);
      li.addTime( diff.count() );   //li.time += diff.count();
    }
    SECTION(create packets and put them into packet queue)
    {
      //TO(lp.outputs,i)
      while(outArgs->size() > 0)
      {
        LavaOut outArg = outArgs->front();
        outArgs->pop();
        //if(outArgs[i].type == LavaArgType::NONE){ continue; }

        // create new value for the new packet
        LavaVal val;
        val.type  = outArg.type;
        val.value = outArg.value;
        auto sidx = outArg.key.slot;
        //val.type  = outArgs[i].type;
        //val.value = outArgs[i].value;
        //auto sidx = outArgs[i].key.slot;

        //LavaMem mem = LavaMem::fromDataAddr(outArgs[i].value);
        //PrintLavaMem(mem);
        LavaMem  mem = LavaMem::fromDataAddr(outArg.value);
        auto szBytes = mem.sizeBytes();

        // create new packet 
        LavaPacket basePkt;
        basePkt.frame       =   lf.m_frame;            // increment the frame on every major loop through both data and message nodes - how to know when a full cycle has passed? maybe purely by message nodes - only increment frame if data is created through a message node cycle
        basePkt.framed      =   false;                 // would this go on the socket?
        basePkt.src_node    =   nid;
        basePkt.src_slot    =   sidx;
        basePkt.msg.id      =   lf.nxtMsgId();         // todo: rethink this, since nxtMsgId might not be neccesary
        basePkt.msg.val     =   val;

        // route the packet using the graph - the packet may be copied multiple times and go to multiple destination slots
        LavaId   src  =  { nid, sidx };
        auto      di  =  lf.graph.destCncts(src);                            // di is destination iterator
        auto   diCnt  =  di;                                                 // diCnt is destination iterator counter - used to count the number of destination slots this packet will be copied to so that the reference count can be set correctly
        auto    diEn  =  lf.graph.destCnctEnd();

        for(; di!=diEn && di->first==src; ++di)
        {                                                                    // loop through the 1 or more destination slots connected to this source
          LavaId   pktId = di->second;
          LavaPacket pkt = basePkt;                                          // pkt is packet
          pkt.dest_node  = pktId.nid;
          pkt.dest_slot  = pktId.sidx;

          mem.incRef();

          lf.putPacket(pkt);

          //lf.m_qLck.lock();              // mutex lock
          //  lf.q.push(pkt);              // todo: use a mutex here initially
          //lf.m_qLck.unlock();            // mutex unlock
        }

        ownedMem.push_back(mem);

        lf.packetCallback(basePkt);

        //PutMem(LavaId(basePkt.src_node, basePkt.src_slot) , mem);
        //auto keys = vizdb.getKeyStrs();
        //printdb(vizdb);
      }
    } // SECTION(create packets and put them into packet queue)

    return LavaInst::NORMAL;
  }

  return LavaInst::LOAD_ERROR;
}

}

void               LavaInit()
{
  //new (&db)     simdb("lava_db", 128, 2<<4);
  //new (&vizdb)  simdb("viz_db",  128, 2<<4);
}
bool        RefreshFlowLibs(LavaFlow& inout_flow)
{
  bool     newlibs  =  false;
  auto       paths  =  GetRefreshPaths();
  if(paths.size() > 0){
    newlibs = true;
    ++inout_flow.version;
  }

  auto   livePaths  =  GetLivePaths(paths);

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

  // extract the flow node lists from the handles
  auto flowNdLists = GetFlowNodeLists(loadedHndls);

  // extract the flow nodes from the lists and put them into the multi-map
  TO(livePaths.size(),i)
  {
    LavaNode* ndList = flowNdLists[i];
    if(ndList){
      auto const& p = livePaths[i]; 
      inout_flow.flow.erase(p);                              // delete the current node list for the livePath
      for(; ndList->func!=nullptr; ++ndList){                // insert each of the LavaFlowNodes in the ndList into the multi-map
        inout_flow.nameToPtr.erase(ndList->name);
        inout_flow.nameToPtr.insert( {ndList->name, ndList} );
        inout_flow.flow.insert( {p, ndList} );
      }
    }else{
      // set the node's state to an error
    }
  }

  return newlibs;
}
void               LavaLoop(LavaFlow& lf) noexcept
{
  using namespace std;
  const LavaOut defOut = { LavaArgType::NONE, 0, 0, 0, 0 };

  LavaHeapInit();

  lava_memvec ownedMem;

  lf.incThreadCount();

  LavaVal      inArgs[LAVA_ARG_COUNT];              // these will end up on the per-thread stack when the thread enters this function, which is what we want - thread specific memory for the function call
  lava_threadQ   outQ;                              // queue of the output arguments
  LavaFrame   inFrame;
  LavaFrame    runFrm;
  LavaOut     outArgs[LAVA_ARG_COUNT];              // if the arguments are going to 
  memset(inArgs, 0, sizeof(inArgs) );
  TO(LAVA_ARG_COUNT,i){ outArgs[i] = defOut; }      // memset(outArgs, 0, sizeof(outArgs) );

  while(lf.m_running)
  {    
    SECTION(make a frame from a packet or run a message node)
    {
      LavaPacket    pckt;
      LavaParams      lp;
      u64         nodeId;
      bool doFlow = lf.nxtPacket(&pckt);
      if(doFlow) 
        SECTION(if there is a packet available, fit it into a existing frame or create a new frame)
        {
          u16 sIdx  =  pckt.dest_slot;
          lf.m_frameQLck.lock();                                          // lock mutex        
            SECTION(loop through the outstanding frames to find the match for this packet)
            {
              TO(lf.frameQ.size(), i)                                     // loop through the current frames looking for one with the same frame number and destination slot id - if no frame is found to put the packet into, make a new one - keep track of the lowest full frame while looping and use that if no full frame is found?
              {
                auto& frm = lf.frameQ[i];
                if(frm.dest != pckt.dest_node){ continue; }               // todo: unify dest_node and dest_id etc. as one LavaId LavaPacket
                if(frm.frame != pckt.frame){ continue; }

                //bool slotTaken = frm.getSlot(sIdx);
                bool slotTaken = frm.slotMask[sIdx];
                if(!slotTaken){
                  frm.packets[sIdx] = pckt;
                }

                if( frm.allFilled() ){                                  // If all the slots are filled, copy the frame out and erase it
                  runFrm = frm;
                  lf.frameQ.erase(lf.frameQ.begin()+i);                 // can't use an index, have to use an iterator for some reason
                  break;                                                // break out of the loop since a frame was found
                }
              }
            }
            SECTION(if a frame was not found for this packet, create one, then put it in the array if the frame has more than one slot)
            {
              LavaInst& ndInst   =  lf.graph.node(pckt.dest_node);
              LavaFrame    frm;
              frm.slots  =  ndInst.inputs;                               // find the number of input slots for the dest node
              frm.dest   =  pckt.dest_node;
              frm.frame  =  pckt.frame;
              //runFrm.frame        =  ndInst.fetchIncFrame();
              frm.putSlot(sIdx, pckt);
              
              if(frm.slots > 1)
                lf.frameQ.push_back(frm);                                // New frame that starts with the current packet put into it 
              else
                runFrm = frm;
            }
          lf.m_frameQLck.unlock();                                       // unlock mutex

          nodeId = runFrm.dest;
        }
      else   
        SECTION(try to run a single message node if there was no packet found)           // todo: find a single message node and run that, remembering the place
        {
          nodeId  =  lf.nxtMsgId();
          // todo: need to work out here if the message node is available - locking and lock free message nodes would come in to play

        } // SECTION(loop through message nodes)

      SECTION(run the node with the id and frame)
      {
        LavaInst::State ret = (nodeId!=LavaId::NODE_NONE)? runFunc(lf, ownedMem, nodeId, &lp, &runFrm, &outQ)  :  LavaInst::LOAD_ERROR;  // LavaFlow::RUN_ERR; 
        switch(ret)
        {
          case LavaInst::RUN_ERROR:{
            lf.graph.setState(nodeId, LavaInst::RUN_ERROR);
            //lf.putPacket(pckt);               // if there was an error, put the packet back into the queue
          }break;
          case LavaInst::LOAD_ERROR:{
            lf.graph.setState(nodeId, LavaInst::RUN_ERROR);  // todo: should deal with this at load time and not here of course
          }break;
          case LavaInst::NORMAL:
          default:{ // if everything worked, decrement the references of all the packets in the frame
            if(doFlow){
              //TO(LavaFrame::PACKET_SLOTS,i) if(runFrm.getSlot(i)){ 
              TO(LavaFrame::PACKET_SLOTS,i) if(runFrm.slotMask[i]){ 
                LavaMem mem = LavaMem::fromDataAddr(runFrm.packets[i].msg.val.value);
                mem.decRef();
              }
            }
          }break;
        }
      }
    }
    SECTION(partition owned allocations and free those with their reference count at 0)
    {
      for(auto const& lm : ownedMem){
        PrintLavaMem(lm);
        //printf("\n reference count: %llu \n", (u64)lm.refCount() );
      }

      auto zeroRef  = partition(ALL(ownedMem), [](auto a){return a.refCount() > 0;} );                           // partition the memory with zero references to the end / right of the vector so they can be deleted by just cutting down the size of the vector
      auto freeIter = zeroRef;
      for(; freeIter != end(ownedMem); ++freeIter){                                                              // loop through the memory with zero references and free them
        LavaFree( (uint64_t)freeIter->data() );
      }

      ownedMem.erase(zeroRef, end(ownedMem));                                                                    // erase the now freed memory
    }
  } // while(m_running)

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







//return m_sz; // todo: make this subtract m_cur from m_end and take care of looping
//
//return buf.en - buf.st;
//
//if(buf.en >= buf.st)
//  return buf.en - buf.st;
//else{
//  u64 cap = m_capB;
//  if(buf.useA)
//     cap = m_capA;
//  auto st = buf.st % cap;
//  auto en = buf.en % cap;
//}

// this needs to check the capacity against the size, and that means checking the wrapping distance between the start and the end
//if( buf.en+1 >= cap ){ 
//
// don't need to write within the increment loop since there is only one writing thread
//if( buf.useA ){
//  auto idx = (buf.en) % capA();
//  m_memA[idx] = val;
//}else{ 
//  auto idx = (buf.en) % capB();
//  m_memB[idx] = val;
//}

//struct PushResult { bool usedA; u64 endIdx; };
//
//u64           m_sz = 0;
//
//void         storeBuf(StEnBuf buf)
//{
//  ((au64*)&m_buf.asInt)->store(buf.asInt);
//}

//auto buf = loadBuf();
//
//if( buf.useA ){
//  ret = m_memA[buf.st];
//}else{ 
//  ret = m_memB[buf.st];
//}

//auto nxtCap    = m_capB.load() << 1;
//m_capA.store(nxtCap);
//void* nxtAlloc = m_alloc( nxtCap * sizeof(T) );    // 1
//
//m_memA = (T*)nxtAlloc;                             // 1
//copy(m_memA, m_memA + m_sz, m_memB);               // 2
//m_capA.store( m_capB.load() );
//
//switchBuffers();
//m_free(m_memB);                                    // 4
//m_memB = nullptr;                                  // 4

//u8          m_cap = 0;                         // capacity will always be a power of two
//au64        m_curA = 0;
//au64        m_curB = 0;
//au64        m_endA = 0;
//au64        m_endB = 0;
//abool       m_useA = true;

//if( m_useA.load() ){
//  auto cur = m_curA.fetch_add(1);
//  auto  en = m_endA.load();
//  if(cur == en) return false;
//  ret = m_memA[cur];
//}else{ 
//  auto cur = m_curB.fetch_add(1);
//  auto  en = m_endB.load();
//  if(cur == en) return false;
//  ret = m_memB[cur];
//}

//useA = m_useA.load();           // expand should have switched this
//
//en  = m_endA.load();
//cap = capA();
//
//m_endA.fetch_add(1);       // because this is the only thread writing, it can simply update the enrent index after copying the value
//
//en  = m_endB.load();
//cap = capB();
//
//m_endB.fetch_add(1);
//
//PushResult ret;
//ret.endIdx = en;
//ret.usedA  = useA; 

//
//if( m_useA.load() ){

//auto buf = loadBuf();
//buf.useA = false;

//m_endB.store( m_endA.load() );  // todo: these need to be unified with the buffer switching boolean
//m_curB.store( m_curA.load() );
//
//m_useA.store(false);                               // 3  -  now that the opposite buffer is good to go, switch the single boolean that controls which buffer to use

//m_endA.store( m_endB.load() );  // todo: these need to be unified with the buffer switching boolean
//m_curA.store( m_curB.load() );
//
//m_useA.store(false);                               // 3  -  now that the opposite buffer is good to go, switch the single boolean that controls which buffer to use

//m_memA = (T*)nxtAlloc;                             // 1
//copy(m_memA, m_memA + m_sz, m_memB);               // 2
//m_useA.store(true);                                // 3 - -  now that the opposite buffer is good to go, switch the single boolean that controls which buffer to use
//m_free(m_memB);                                    // 4
//m_memB = nullptr;                                  // 4

// todo: need to check that cur does not exceed size - is size needed or just capacity?
//if(m_cur.load() == m_sz || m_sz == m_cap){
//if(m_cur.load() == m_sz || m_sz == m_cap){
//  expand();
//}
//
//u64   en = m_en.load();      //fetch_add(1);

//T curVal = 0;
//
//auto cur = m_curA.fetch_add(1) % m_capA.load();
//
//auto cur = m_curB.fetch_add(1) % m_capB.load();
//ret = m_memB[cur];
