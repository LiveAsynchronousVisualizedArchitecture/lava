
// -todo: test 128 bit atomics
// -todo: find memory corruption bug on looping insert of random numbers - just put key where value should be in C++ string put
// -todo: figure out why all keys are being printed - getKeys() returns a single empty string
// -todo: figure out why getKeys returns one empty string - for short blocks, the last block is a key/starting block because it is also the first block. The make_BlkLst function made non keys have length and key length automatically set to 0.
// -todo: figure out why duplicate keys aren't overwriting each other - putHashed needs to match without using version, then compare and swap using version, checking the match each time
// -todo: test with multiple threads in a loop
// -todo: take out infinite loop possibility in rm
// -todo: take out any inf loops in runMatch
// -todo: change getKeyStrs() to get the number looped through by nxtKey() so it isn't O(n^2)
// -todo: put in osx stuff here - likely mmmap with shared memory

// todo: convert to 64 bit integers
// todo: put files in /tmp/var/simdb/ ? have to work out consistent permissions and paths 
// todo: make windows version have permissions for just read and write
// todo: make a macro to have separate windows and unix paths?
// todo: make realloc that changes the size of a block list
// todo: check if memset to 0 is still anywhere in the release build - line 1827 still has a memset to 0 on free
// todo: make put give back FAILED_PUT on error
// todo: make ConcurrentStore get() stop before exceeding maxlen?
// todo: make put return VerIdx ?
// todo: make a DELETED value for hash entries so that when something is removed, it doesn't block a linear search
// todo: make readers for blocks only exist on the head of the list?
// todo: prefetch memory for next block when looping through blocks - does this require a system call for shared memory and does it lock? it should just be the prefetch instruction or an unoptimized away load? use intrinsic?
// todo: look at making a memory access to the next block that can't be optimized away
// todo: Make frees happen from the last block to the first so that allocation might happen with contiguous blocks
// todo: make bulk free by setting all list blocks first, then freeing the head of the list - does only the head of the list need to be freed anyway since the rest of the list is already linked together? could this reduce contention over the block atomic?
// todo: make 128 bit atomic function
// todo: organize ConcurrentHash entry to have the index on the left side, version on the right side. 
//       Put hash in the middle and use the first two bits of the index as empty and deleted flags
//       empty : 1, deleted : 1, index : 35, hash : 35, version : 56 - total 128 bits, 34 billion entry limit 
// todo: make resize function so block lists can be resized rather than freed, only to be reallocated
// todo: test alignment

// q: can cleanup be done by a ring buffer of block lists? would the ring buffer only need to be as long as the number of threads if each thread helps clear out the ring buffer after every free()? Probably not, because delayed deallocation would be useful only when there is a reader/ref count, which would mean many more reads than writes could stall the ability to free? But each thread can really only hold one reference at a time so maybe it would work? 
// q: if using a ring buffer, indices might be freed in between non-freed indices, in which case the pointer to beginning and end would not be able to shift, and therefore would need space for more indices than just the number of threads
// q: if using a ring buffer for frees, could a thread freeing the index then bubble sort backwards and move the begin pointer forwards until the begin and end pointers have only non freed indices between them 
// q: what is the benefit of a ring buffer? does it prevent the need for a spinlock anywhere?
// q: does the main program even need a key value store? can't it just use the block list indices directly?
// q: should maximum probe length be used here?
// q: should use a separate 'tombstone' value for a deleted value?
// q: should store hash to be able to know probe length and as a short circuit for false matches?
// q: can deleted entries be returned to empty entries on removal or some other operation somehow? if robinhood hashing means keeping the probe distance to only partial overlapping spans, does that open up the possibility?
//    - on removal, if the previous entry is EMPTY or if the previous entry has a probe span of 0, set the current entry to EMPTY
//      can actually loop backwards the (length of the span) - 1 (to the natural position + 1) to see if there are any overlapping spans?
//      on removal, loop backwards to robin hood hash any overlapping span
//      after robin hood hashing from the natural position set any DELETED keys after to EMPTY since they are after a span of 0 or after EMPTY 
//    - if the current entry is EMPTY, loop forward setting adjacent DELETED entries to EMPTY
// q: does robinhood hashing mean keeping a stack of moved values until the end of a robery chain is complete, then copying from the end of the chain to beggining? 
// q: could ISPC be compiled to C++ intrinsics to integrate into a one header file structure? - should be useful for hashing and comparison?
// q: can make a bitmap for filled hash indices if the bit is set after the KV is set, and the bit is unset before unsetting the KV
// q: can intrinsics be used for 128bit atomics?
// q: make ConcurrentHash be a 48 bit index and a 48 bit version
//    - pack them together and use unaligned atomic reads and writes?
//    - be able to modify an index,version,index triplet with a 128 bit atomic or a version,index,version triplet?  
// q: can some sort of stack or list or current outstanding version reads be used to make sure version numbers don't wrap around, therefore needing less bits for the version number?
// q: can a high bits version number and low bits version number be used to detect vesion number wrap arounds?  One 64 bit number while just the lower bits are stored?
// q: interleave reference counting / readers count on both hash and store?
//    - when reading from a ConcurrentHash value, increment the readers by 1, increment the block idx by 1, then decrement the ConcurrentHash
//    - does this destroy the ability to do robinhood hashing without spinlocking?
// q: on removal, follow DELETED entries forward until hitting a key entry, and if the removed entry was inside its probe span, bring the found entry up by copying, then remove the found entry, then loop forward, trying to set DELETED entries to EMPTY if they are after EMPTY or after the end of a probe span
// q: can part of the version be put on one side of a struct and part on the other side? maybe even interleave bits?
// q: just have a flag bit on each side? this would allow signaling that only one thread at a time is doing the removal cleanup and allow cleanup concurrently with insertion? 
//    1. flip bits in between two entries
//    ////2. consider the opposite bits to be 'changed' bits and flip them only if an insert sees either of the bits flipped
//    ///////3. if the removal process detects a 'changed' bit, ... 
//    //////4. is it needed if versions are involved, and even so would it work?
//    2. what is the problem? - need to be able to tell if previous entry is the same when changing the next entry 
//    3. keep index first and version second so that it is possible to loop forward and look at the previous version and the current index - if a high bit is used as a flag for empty, then the low bits of the version and the high bits of the index could be change atomically, meaning 32 bit precision for versions only in one brief moment 
//       !- if only the upper bit is needed, can the 64 bit atomic operation happen with 56 bits for the version and only 8 bits for the index?
//    4. then check the whole version and do some sort of error handling and recovery if the version is not the same - although 56 bits should be a year's worth of cpu cycles, so having 72 quadrillion allocations between a few instructions on another thread seems unlikely 
//    5. in theory a mutex could be used and the entire range of versions could be remapped and the master version atomic could be set to the new max+1
// q: is it possible to defragment and if so it is possible to do on removal?

/*
 SimDB

 What it does:
 |  SimDB is a key value store that uses arbitrary byte data as both the key and the value. 
 |  It additionally uses shared memory, which allows processes to communicate with each other quickly.  
 |  It is lock free and scales well with multiple threads writing, reading, and removing concurrently.  

 How it works:
 |-simdb:
   | This contains the interface as well as ConcurrentHash, ConcurentStore, and SharedMem classes.
   |-SharedMem:
   |  |  Interface to OS specific shared memory functions.
   |-ConcurrentHash:
   |  |  Hash map that uses atomics along with 64 bit values to make sure that reading and writing is lock free.
   |-ConcurrentStore:
   |  |  Keeps track of block lists.  This exposes an alloc() function and a free() function. 
   |  |  alloc() gets the number of blocks from the BlockStore.
   |  |  The BlkLst lava_vec is used to make linked lists of block indices. 
   |  |  It is an array of one integer per block with the integer at a given index representing the index of the next block.  
   |  |  This keeps the total length and the key length / value offset since it is already reference counted
   |  |-BlockStore:
   |  |-BlockList (lava_vec):

 Terms:
 |-Block List: 
 |  A sequence of block indices.  The entry in ConcurrentHash gives the position in the block list array where the list starts.  
 |  The value at each index in the array contains the index of the next block.  
 |  The list end is know when a special value of LIST_END is found as the value in the array.
 |-Block List Version:
 |  This is a version number given to each block list on allocation (not each block). 
 |  It is used to link a ConcurrentHash value to the block list. 
 |  If the versions are the same, it is known that the block list at the index read from ConcurrentHash has not changed.
 |  This change could happen if:
 |  |  1. Thread ONE reads the entry in ConcurrentHash but has not accessed the block list index in the entry yet. Pretend that thread one stalls and nothing more happens until further down.
 |  |  2. Thread TWO has already allocated a block list and swaps its new entry for the old entry which is still carried by thread one. 
 |  |  3. Thread TWO now must free the block list given by the old entry, which it does, because no thread is reading it since thread one is still stalled.
 |  |  4. Thread TWO allocates another block list, which ends up using the blocks it just deallocated.
 |  |  5. Thread ONE wakes up and reads from the block index it found in the ConcurrentHash entry, which is no longer the same and may not even be the head of the list.
 |  |  If the index is used purely for matching the binary key, this wouldn't be a problem. 
 |  |  When the index is used to find a binary value however, this is a problem, since the length of a different value could be the same, and there would be no data to be able to tell that they are different.

 How it achieves lock free concurrency:
 |  ConcurrentHash is treated as the authority of what is stored in the database. 
 |  It has an array of 64 bit integers. Each 64 bit integer is dealt with atomically.
 |  Its individual bits are used as a bitfied struct containing an index into ConcurrentStore's block list as well as the version number of that list.
 |-Finding a matching index: 
 |  |  1. Use the hash of the key bytes to jump to an index.
 |  |  2. Load the integer atomically from that index and cast to the bitfield struct.
 |  |  3. Use the index from that struct to read the bytes from the list of blocks in BlkLst. 
 |  |  4. Use version from that struct to verify that each block is part of the list given by the ConcurrenHash entry.
 |  |  5. If there is a match, keep reading the list of blocks to fill the output buffer with the value section of the block list.

*/

#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __SIMDB_HEADER_GUARD__
#define __SIMDB_HEADER_GUARD__

#include <cstdint>
#include <cstring>
#include <atomic>
#include <mutex>
#include <memory>
#include <vector>
#include <string>
#include <unordered_set>
#include <set>
#include <algorithm>
#include <cassert>

// platform specifics - mostly for shared memory mapping and auxillary functions like open, close and the windows equivilents
#ifdef _MSC_VER
  #if !defined(_CRT_SECURE_NO_WARNINGS)
    #define _CRT_SECURE_NO_WARNINGS
  #endif

  #if !defined(_SCL_SECURE_NO_WARNINGS)
    #define _SCL_SECURE_NO_WARNINGS
  #endif
#endif

#if defined(_WIN32)      // windows

 //#if defined(_CRT_SECURE_NO_WARNINGS)
 //  #undef _CRT_SECURE_NO_WARNINGS
 //  #define _CRT_SECURE_NO_WARNINGS
 //#endif
 //#undef _CRT_SECURE_NO_WARNINGS
 //#define _CRT_SECURE_NO_WARNINGS 1       // msvc mandatory error nonsense when using some standard C functions like fopen 

 //#if !defined(_CRT_SECURE_NO_WARNINGS)
 //  #define _CRT_SECURE_NO_WARNINGS        // msvc mandatory error nonsense when using some standard C functions like fopen 
 //#endif

 #define NOMINMAX
 #define WIN32_LEAN_AND_MEAN
 #include <windows.h>
#elif defined(__APPLE__) || defined(__MACH__) || defined(__unix__) || defined(__FreeBSD__) || defined(__linux__)  // osx, linux and freebsd
  // for mmap and munmap
  // PROT_READ and PROT_WRITE  to allow reading and writing but not executing of the mapped memory pages
  // MAP_ANONYMOUS | MAP_SHARED for the anonymous shared memory we want
  // mmap is system call 2 on osx, freebsd, and linux
  // the apple docs for mmap say "BSD System Calls" so I guess they haven't changed them around
  #include <sys/mman.h>
  #include <sys/fcntl.h>
  #include <sys/errno.h>
  #include <sys/unistd.h>
  #include <sys/file.h>         // for flock
  #include <unistd.h>
#endif

#ifdef _WIN32
  // use _malloca ? - would need to use _freea and also know that _malloca always allocates on the heap in debug mode for some crazy reason
  #define STACK_VEC(TYPE, COUNT) lava_vec<TYPE>(_alloca(lava_vec<TYPE>::sizeBytes(COUNT)), COUNT, true);
#else
  // gcc/clang/linux ?
  #include <alloca.h>
  #define STACK_VEC(TYPE, COUNT) lava_vec<TYPE>(_alloca(lava_vec<TYPE>::sizeBytes(COUNT)), COUNT, true);  
#endif

using    i8   =   int8_t;
using   ui8   =   uint8_t;
using   i64   =   int64_t;
using  ui64   =   uint64_t;
using   i32   =   int32_t;
using  ui32   =   uint32_t;
using   f32   =   float;
using   f64   =   double;
using aui32   =   std::atomic<ui32>;
using aui64   =   std::atomic<ui64>;
using  ai32   =   std::atomic<i64>;
using   ai8   =   std::atomic<i8>;
using  cstr   =   const char*;
using   str   =   std::string;

template<class T, class A=std::allocator<T> > using vec = std::vector<T, A>;

namespace {
  enum Match { MATCH_FALSE=0, MATCH_TRUE=1, MATCH_REMOVED=-1  };

  template<class T>
  class lava_noop
  {
    void operator()(){}
  };

  ui64 fnv_64a_buf(void const *const buf, ui64 len)
  {
    // const ui64 FNV_64_PRIME = 0x100000001b3;
    ui64 hval = 0xcbf29ce484222325;    // FNV1_64_INIT;  // ((Fnv64_t)0xcbf29ce484222325ULL)
    ui8*   bp = (ui8*)buf;	           /* start of buffer */
    ui8*   be = bp + len;		           /* beyond end of buffer */

    while(bp < be)                     // FNV-1a hash each octet of the buffer
    {
      hval ^= (ui64)*bp++;             /* xor the bottom with the current octet */

      //hval *= FNV_64_PRIME; // does this do the same thing?  /* multiply by the 64 bit FNV magic prime mod 2^64 */
      hval += (hval << 1) + (hval << 4) + (hval << 5) +
              (hval << 7) + (hval << 8) + (hval << 40);
    }
    return hval;
  }

template<class T, class Deleter=std::default_delete<T>, class Allocator=std::allocator<T> >
class lava_vec
{
private:
  void* p;

  void       set_size(ui64  s)
  { 
    *((ui64*)clearBits(p) + 1) = s;
  }
  void  set_sizeBytes(ui64 sb)
  {
    *((ui64*)clearBits(p)) = sb;      // first 8 bytes should be the total size of the buffer in bytes
  } 

public:
  static void*  setDestructorBit(void* p)
  {
    //return (void*)((ui64)p ^ (((ui64)1l)<<63));
    return (void*)((ui64)p | (1llu<<63));
  }
  static bool   getDestructorBit(void* p)
  {
    return (((ui64)p)>>63)!=0;
  }
  static void*         clearBits(void* p)
  {
    return (void*)( ((ui64)p) & 0x0000FFFFFFFFFFFF);
  }
  static ui64          sizeBytes(ui64 count)  // sizeBytes is meant to take the same arguments as a constructor and return the total number of bytes to hold the entire stucture given those arguments 
  {
    return sizeof(ui64)*2 + count*sizeof(T);
  }

  lava_vec(){}
  lava_vec(ui64  count)
  {
    ui64 sb = lava_vec::sizeBytes(count);
    p       = Allocator().allocate(sb);
    p       = setDestructorBit(p);
    set_size(count);
    set_sizeBytes(sb);
  }
  lava_vec(void*  addr, ui64 count, bool owner=true) :
    p(addr)
  {
    //ui64 sb = lava_vec::sizeBytes(count);
    //p       = addr;
    if(owner){
      set_sizeBytes( lava_vec::sizeBytes(count) );
      set_size(count);
    }
  }
  lava_vec(void*  addr) :
    p(addr)
  {
    //ui64 sb = lava_vec::sizeBytes(count);
    //set_size(count);
    //set_sizeBytes(sb);
    //
    //p = addr;
  }
  lava_vec(lava_vec const&) = delete;
  void operator=(lava_vec const&) = delete;
  lava_vec(lava_vec&& rval)
  {
    p      = rval.p;
    rval.p = nullptr;
  }
  ~lava_vec()
  {
    if(p && getDestructorBit(p)){
      Deleter().operator()((T*)clearBits(p));  //free(p);
      p = nullptr;
    }
  }

  T& operator[](ui64 i)
  {
    return data()[i];
  }

  T*         data()
  {
    //ui64 pnum = 
    ui64* maskptr = (ui64*)clearBits(p); // (ui64*)( ((ui64)p) & 0x0000FFFFFFFFFFFF);
    return (T*)(maskptr+2);
    //return (void*)((ui64*)p+2);
  }
  ui64   capacity() const
  {
    return (sizeBytes() - sizeof(ui64)*2) / sizeof(T);
  }
  ui64       size() const
  {
    return *((ui64*)clearBits(p) + 1);   // second 8 bytes should be the number of elements
  } 
  ui64  sizeBytes() const
  {
    return *((ui64*)clearBits(p));   // first 8 bytes should be the total size of the buffer in bytes
  } 
  auto       addr() const -> void*
  {
    return p;
  }
};
}

class   ConcurrentList
{
public:
  union Head
  {
    struct { ui32 ver; ui32 idx; };                      // ver is version, idx is index // todo: change cnt to version
    ui64 asInt;
  };
  
  using    ui32  =  uint32_t;                            // need to be i32 instead for the ConcurrentStore indices?
  using    ui64  =  uint64_t;
  //using ListVec  =  std::vector< std::atomic<ui32> >;  // does this need to be atomic? all the contention should be over the head
  using ListVec  =  lava_vec<ui32>;
  //using HeadInt  =  ui64;
  //using    Head  =  std::atomic<ui64>;

  const static ui32 LIST_END = 0xFFFFFFFF;

private:
  ListVec     m_lv;
  aui64*       m_h;
  //ui64   m_szBytes;

public:
  static ui64 sizeBytes(ui32 size)
  {
    return ListVec::sizeBytes(size);
  }

  ConcurrentList(){}
  //ConcurrentList(ui32 size) : 
  //  m_lv(size)
  //{
  //  for(uint32_t i=0; i<(size-1); ++i) m_lv[i]=i+1;
  //  m_lv[size-1] = LIST_END;
  //
  //  m_h = 0;
  //}
  ConcurrentList(void* addr, ui32 size, bool owner=true) :           // this constructor is for when the memory is owned an needs to be initialized
    m_lv(addr, size, owner)
  {
    m_h = (aui64*)addr;

    if(owner){
      for(uint32_t i=0; i<(size-1); ++i) m_lv[i]=i+1;
      m_lv[size-1] = LIST_END;

      ((Head*)m_h)->idx = 0;
      ((Head*)m_h)->ver = 0;
    }
                                          // uses the first 8 bytes that would normally store sizeBytes as the 64 bits of memory for the Head structure
  }
  //ConcurrentList(void* addr) :           // this constructor is for memory that is not owned and so does not need to be initialized, just used
  //  m_lv(addr)
  //{
  //  m_h = (aui64*)addr;
  //  //m_h->asInt = 0;
  //}

  auto        nxt() -> uint32_t    // moves forward in the list and return the previous index
  {
    //HeadUnion  curHead;
    //HeadUnion  nxtHead;
    Head curHead;
    Head nxtHead;

    curHead.asInt  =  m_h->load(); // std::atomic_load(m_h);  // m_h.load();
    do 
    {
      if(curHead.idx==LIST_END) return LIST_END;

      nxtHead.idx  =  m_lv[curHead.idx];
      nxtHead.ver  =  curHead.ver + 1;
    } while( !m_h->compare_exchange_strong(curHead.asInt, nxtHead.asInt) );

    //return nxtHead.idx;
    return curHead.idx;
  }
  auto       free(ui32 idx) -> uint32_t   // not thread safe yet when reading from the list, but it doesn't matter because you shouldn't be reading while freeing anyway?
  {
    Head curHead;
    Head nxtHead;
    uint32_t    retIdx;

    curHead.asInt = m_h->load();
    do 
    {
      retIdx = m_lv[idx] = curHead.idx;
      nxtHead.idx  =  idx;
      nxtHead.ver  =  curHead.ver + 1;
    } while( !m_h->compare_exchange_strong(curHead.asInt, nxtHead.asInt) );

    return retIdx;
  }
  auto      count() const -> ui32
  {
    //return ((HeadUnion*)(&m_h))->cnt;
    //return ((Head*)(&m_h))->ver;
    return ((Head*)m_h)->ver;
  }
  auto        idx() -> ui32
  {
    //return ((Head*)(&m_h))->idx;
    return ((Head*)m_h)->idx;
  }            // not thread safe
  auto       list() -> ListVec const* 
  {
    return &m_lv;
  }            // not thread safe
  ui32     lnkCnt()                     // not thread safe
  {
    ui32    cnt = 0;
    //auto      l = list();
    ui32 curIdx = idx();
    while( curIdx != LIST_END ){
      //curIdx = l->at(curIdx).load();
      curIdx = m_lv[curIdx];   //  l->at(curIdx).load();
      ++cnt;
    }
    return cnt;
  }
  //ui64  sizeBytes() const
  //{
  //  return *((ui64*)addr);
  //}
};
class  ConcurrentStore
{
public:
  union VerIdx
  {
    struct {
      i32      idx;
      ui32 version;
    };
    ui64 asInt;
  };
  union KeyAndReaders
  {
    struct{ ui32    isKey :  1; i32   readers : 31; ui32 version; };
    ui64 asInt;

    //ui32 asInt;
  };
  union BlkLst                            // need to do anything special to guarantee that readers is aligned so it is atomic?
  {
    //struct { KeyAndReaders kr; ui32 version; i32 idx; ui32 len, klen; };
    struct { KeyAndReaders kr; i32 idx; ui32 len, klen; };
  };

  using IDX         =  i32;
  using ai32        =  std::atomic<i32>;
  using BlockLists  =  lava_vec<BlkLst>;   // only the indices returned from the concurrent list are altered, and only one thread will deal with any single index at a time 
  //using BlockLists  =  lava_vec<IDX>;

  const static ui32 LIST_END = ConcurrentList::LIST_END;

  static BlkLst   make_BlkLst(bool isKey, i32 readers, ui32 idx, ui32 ver, ui32 len, ui32 klen)
  {
    BlkLst bl;
    bl.kr.isKey    = isKey;
    bl.kr.readers  = readers;
    bl.idx      = idx;
    bl.kr.version  = ver;

    if(isKey){
      bl.len  = len;
      bl.klen = klen;
    }else{
      bl.len  = 0;
      bl.klen = 0;
    }

    return bl;
  }
  static VerIdx      List_End()
  { 
    VerIdx vi; 
    vi.idx     = ConcurrentList::LIST_END; 
    vi.version = 0; 
    return vi; 
  }
  static bool       IsListEnd(VerIdx vi)
  {
    static const VerIdx empty = List_End();
    return empty.asInt == vi.asInt;
  }

  BlkLst    incReaders(ui32 blkIdx, ui32 version) const                   // BI is Block Index  increment the readers by one and return the previous kv from the successful swap 
  {

    KeyAndReaders cur, nxt;
    BlkLst* bl      = &s_bls[blkIdx];
    aui64* areaders = (aui64*)&(bl->kr.asInt);    
    cur.asInt       = areaders->load();
    do{
      if(cur.version!=version || cur.readers<0) return make_BlkLst(0,0,0,0,0,0);
      nxt = cur;
      nxt.readers += 1;
    }while( !areaders->compare_exchange_strong(cur.asInt, nxt.asInt) );
    
    return *bl;  // after readers has been incremented this block list entry is not going away. The only thing that would change would be the readers and that doesn't matter to the calling function.
  }
  bool      decReaders(ui32 blkIdx, ui32 version) const                   // BI is Block Index  increment the readers by one and return the previous kv from the successful swap 
  {
    KeyAndReaders cur, nxt;
    aui64* areaders = (aui64*)&(s_bls[blkIdx].kr.asInt);    
    cur.asInt = areaders->load();
    do{
      if(cur.version!=version) return false;
      nxt = cur;
      nxt.readers -= 1;
    }while( !areaders->compare_exchange_strong(cur.asInt, nxt.asInt) );
    
    if(cur.readers==0){ doFree(blkIdx); return false; }

    return true;
  }

private:
  mutable ConcurrentList    s_cl;       // flat data structure - pointer to memory 
  mutable BlockLists       s_bls;       // flat data structure - pointer to memory - bl is Block Lists
  void*               s_blksAddr;       // points to the block space in the shared memory
  aui64*               s_version;       // pointer to the shared version number

  // On the thread's stack
  ui32               m_blockSize;
  ui32              m_blockCount;
  ui64                 m_szBytes;
  mutable ai32      m_blocksUsed;      // todo: this is a mistake and does no good unless it is in the shared memory

  //i32             stPtr(i32  blkIdx)  const
  //{
    //return (i32*)( ((i8*)m_blksAddr) + blkIdx*m_blockSize );
    //return (i32*)&(m_bls.data()[blkIdx]);
    //return m_bls[blkIdx].idx;
    //return m_bls[blkIdx].idx;
  //}
  VerIdx       nxtBlock(i32  blkIdx)  const
  {
    BlkLst bl = s_bls[blkIdx];
    VerIdx vi;
    vi.idx     = bl.idx;
    vi.version = bl.kr.version;
    return vi;      // bl.idx, bl.kr.version };  // s_bls[blkIdx].kr.;
  }
  i32     blockFreeSize()             const
  {
    //return m_blockSize - sizeof(IDX);
    return m_blockSize;
  }
  ui8*     blockFreePtr(i32  blkIdx)  const
  {
    //return ((ui8*)stPtr(blkIdx)) + sizeof(IDX);
    return ((ui8*)s_blksAddr) + blkIdx*m_blockSize;
  }
  ui8*           blkPtr(i32  blkIdx)  const
  {
    //return ((ui8*)stPtr(blkIdx)) + sizeof(IDX);
    return ((ui8*)s_blksAddr) + blkIdx*m_blockSize;
  }
  i32      blocksNeeded(i32     len, i32* out_rem=nullptr)
  {
    i32  freeSz   = blockFreeSize();
    i32  byteRem  = len % freeSz;
    i32  blocks   = len / freeSz + (byteRem? 1 : 0);    // should never be 0 if blocksize is greater than the size of the index type

    if(out_rem) *out_rem = byteRem;

    return blocks;
  }
  i32          blockLen(i32  blkIdx)     // todo: change this to be either i64 or a 'BlkLen' struct that has the flag if nxt isn't valid?
  {
    VerIdx nxt = nxtBlock(blkIdx);
    if(nxt.idx < 0) return -(nxt.idx);

    return blockFreeSize();
  }
  void           doFree(i32  blkIdx)  const        // frees a list/chain of blocks
  {
    i32   cur  =             blkIdx;          // cur is the current block index
    i32   nxt  =   s_bls[cur].idx;         //*stPtr(cur);              // nxt is the next block index
    for(; nxt>0; nxt=s_bls[cur].idx)
    { 
      memset(blkPtr(cur), 0, m_blockSize);    // zero out memory on free, 
      //m_bls[cur].readers = 0;               // reset the reader count
      
      s_bls[cur].kr.isKey   = false;          // make sure key is false;
      s_bls[cur].kr.readers = 0;              // reset the reader count
      
      s_cl.free(cur);
      m_blocksUsed.fetch_add(-1);
      cur  =  nxt;
    }
    memset(blkPtr(cur), 0, m_blockSize);       // 0 out memory on free, 
    //m_bls[cur].readers = 0;                  // reset the reader count
    s_bls[cur].kr.readers = 0;                 // reset the reader count
    s_cl.free(cur);
  }
  ui32       writeBlock(i32  blkIdx, void const* const bytes, ui32 len=0, ui32 ofst=0)      // don't need to increment readers since write should be done before the block is exposed to any other threads
  {
    i32   blkFree  =  blockFreeSize();
    ui8*        p  =  blockFreePtr(blkIdx);
    auto      nxt  =  nxtBlock(blkIdx);
    ui32   cpyLen  =  len==0? blkFree : len;             // if next is negative, then it will be the length of the bytes in that block
    p      += ofst;
    memcpy(p, bytes, cpyLen);

    return cpyLen;
  }
  ui32        readBlock(i32  blkIdx, ui32 version, void *const bytes, ui32 ofst=0, ui32 len=0) const
  {
    BlkLst bl = incReaders(blkIdx, version);      if(bl.kr.version==0) return 0;
      i32   blkFree  =  blockFreeSize();
      ui8*        p  =  blockFreePtr(blkIdx);
      i32       nxt  =  bl.idx;
      ui32   cpyLen  =  len==0?  blkFree-ofst  :  len;
      //cpyLen        -=  ofst;
      memcpy(bytes, p+ofst, cpyLen);
    decReaders(blkIdx, version);

    return cpyLen;
    //ui32   cpyLen  =  nxt<0? -nxt : blkFree;             // if next is negative, then it will be the length of the bytes in that block
  }

public:
  /* 
    The order of the shared memory is:
    Version
    ConcurrentList
    BlockLists
    Blocks
  */
  static ui64    BlockListsOfst()
  {
    //return sizeof(*s_version);
    return sizeof(ui64);
  }
  static ui64         CListOfst(ui32 blockCount)
  {
    return BlockListsOfst() + BlockLists::sizeBytes(blockCount);
  }
  static ui64          BlksOfst(ui32 blockCount)
  {
    //return sizeof(*s_version) + ConcurrentList::sizeBytes(blockCount) + BlockLists::sizeBytes(blockCount);
    return CListOfst(blockCount) + ConcurrentList::sizeBytes(blockCount);
  }
  static ui64         sizeBytes(ui32 blockSize, ui32 blockCount)
  {
    //return ConcurrentList::sizeBytes(blockCount) + BlockLists::sizeBytes(blockCount) + blockSize*blockCount;
    return BlksOfst(blockCount) + blockSize*blockCount;
  }

  ConcurrentStore(){}
  ConcurrentStore(void* addr, ui32 blockSize, ui32 blockCount, bool owner=true) :
    m_blockSize(blockSize),
    m_blockCount(blockCount),
    m_blocksUsed(0),
    //m_cl(m_blockCount)
    s_blksAddr( (i8*)addr + BlksOfst(blockCount) ),
    s_cl(  (i8*)addr + CListOfst(blockCount), blockCount, owner),
    s_bls( (i8*)addr + BlockListsOfst(),      blockCount, owner),
    s_version(  (aui64*)addr ),
    m_szBytes( *((ui64*)addr) )
  {
    //if(owner) *((ui64*)addr) = blockCount;

    if(owner){
      s_version->store(1);                            // start at 1, use 0 as a special value
      //BlkLst def; 
      //def.idx     = 0; 
      //def.readers = 0; 
      //for(ui64 i=0; i<m_bls.size(); ++i) m_bls[i]=def;
    }
    assert(blockSize > sizeof(IDX));
  }

  auto        alloc(i32    size, ui32 klen, i32* out_blocks=nullptr) -> VerIdx   // todo: doesn't this need to give back the blocks if allocation fails?
  {
    i32 byteRem  =  0;
    i32  blocks  =  blocksNeeded(size, &byteRem);
    //if(out_blocks) *out_blocks = blocks;

    ui32   st = s_cl.nxt();                                     // stBlk  is starting block
    if(st==LIST_END){
      if(out_blocks) *out_blocks = 0; 
      return List_End(); // LIST_END; 
    }

    ui32 ver  =  (ui32)s_version->fetch_add(1);
    i32  cur  =  st;
    i32  cnt  =   0;
    i32  nxt  =   0;
    for(i32 i=0; i<blocks-1; ++i)
    {
      nxt    = s_cl.nxt();
      if(nxt==LIST_END){ free(st, ver); VerIdx empty={LIST_END,0}; return empty; }

      if(i==0)  s_bls[cur] =  make_BlkLst(true,  0, nxt, ver, size, klen);
      else      s_bls[cur] =  make_BlkLst(false, 0, nxt, ver, 0, 0);
      cur        =  nxt;
      ++cnt;
      m_blocksUsed.fetch_add(1);
    }

    BlkLst bl  = make_BlkLst(cur==st, 0, byteRem? -byteRem : -blockFreeSize(), ver, size, klen);
    s_bls[cur] = bl;

    if(out_blocks){ *out_blocks = nxt==LIST_END? -cnt : cnt; }     

    s_bls[st].kr.isKey = true;

    VerIdx vi = { st, ver };
    return vi;

    //return s_bls[st].vi;
  }
  bool         free(ui32 blkIdx, ui32 version)        // frees a list/chain of blocks
  {
    return decReaders(blkIdx, version);
  }
  void          put(i32  blkIdx, void const *const kbytes, i32 klen, void const *const vbytes, i32 vlen)  // don't need version because this will only be used after allocating and therefore will only be seen by one thread until it is inserted into the ConcurrentHash
  {
    using namespace std;
    
    ui8*         b  =  (ui8*)kbytes;
    bool   kjagged  =  (klen % blockFreeSize()) != 0;
    i32    kblocks  =  kjagged? blocksNeeded(klen)-1 : blocksNeeded(klen);
    i32   remklen   =  klen - (kblocks*blockFreeSize());
    
    i32   fillvlen  =  min(vlen, blockFreeSize()-remklen);
    i32   tailvlen  =  vlen-fillvlen;
    bool   vjagged  =  (tailvlen % blockFreeSize()) != 0;
    i32    vblocks  =  vjagged? blocksNeeded(tailvlen)-1 : blocksNeeded(tailvlen);
    i32    remvlen  =  max<i32>(0, tailvlen - (vblocks*blockFreeSize()) ); 

    i32       cur  =  blkIdx;
    for(i32 i=0; i<kblocks; ++i){
      b   +=  writeBlock(cur, b);
      cur  =  nxtBlock(cur).idx;
    }
    if(kjagged){
      writeBlock(cur, b, remklen);
      b    =  (ui8*)vbytes;
      b   +=  writeBlock(cur, b, fillvlen, remklen);
      cur  =  nxtBlock(cur).idx;
    }
    for(i32 i=0; i<vblocks; ++i){
      b   +=  writeBlock(cur, b);
      cur  =  nxtBlock(cur).idx;
    }
    if(vjagged && remvlen>0){
      b   +=  writeBlock(cur, b, remvlen);
    }
  }
  ui32          get(i32  blkIdx, ui32 version, void *const bytes, ui32 maxlen) const
  {
    using namespace std;

    if(blkIdx == LIST_END){ return 0; }

    BlkLst bl = incReaders(blkIdx, version);   
    
    auto vlen = bl.len-bl.klen;
    if(bl.len==0 || vlen>maxlen ) return 0;

    auto   kdiv = div(bl.klen, blockFreeSize());
    auto  kblks = kdiv.quot;
    auto   krem = kdiv.rem;
    auto vrdLen = 0;
    ui32    len = 0;
    ui32  rdLen = 0;
    i8*       b = (i8*)bytes;
    i8*      en = b + maxlen;
    i32     cur = blkIdx;
    VerIdx  nxt;
    for(int i=0; i<kblks; ++i){ 
      nxt = nxtBlock(cur);           if(nxt.version!=version){ goto read_failure; }
      cur = nxt.idx;
    }

    vrdLen =  min<ui32>(blockFreeSize()-krem, vlen);
    rdLen  =  readBlock(cur, version, b, krem, vrdLen);
    b     +=  rdLen;
    len   +=  rdLen;
    nxt    =  nxtBlock(cur);         if(nxt.version!=version){ goto read_failure; }

    while(len<maxlen && !(nxt.idx<0) && nxt.idx!=LIST_END && nxt.version==version)
    {
      vrdLen =  min<ui32>(blockFreeSize(), maxlen-len);
      cur    =  nxt.idx;
      rdLen  =  readBlock(cur, version, b, 0, vrdLen);  if(rdLen==0) break;        // rdLen is read length
      b     +=  rdLen;
      len   +=  rdLen;
      nxt    =  nxtBlock(cur);
    }

  read_failure:
    decReaders(blkIdx, version);

    return len;                                           // only one return after the top to make sure readers can be decremented - maybe it should be wrapped in a struct with a destructor
  }
  ui32       getKey(ui32  blkIdx, ui32 version, void *const bytes, ui32 maxlen) const
  {
    if(blkIdx == LIST_END){ return 0; }

    BlkLst bl = incReaders(blkIdx, version);   
    
    if(bl.len==0 || (bl.klen)>maxlen ) return 0;

    auto   kdiv = div(bl.klen, blockFreeSize());
    auto  kblks = kdiv.quot;
    auto   krem = kdiv.rem;
    ui32    len = 0;
    ui32  rdLen = 0;
    i8*       b = (i8*)bytes;
    ui32    cur = blkIdx;
    VerIdx  nxt = { blkIdx, version };

    if(krem>0) --kblks;
    int i=0;
    while( i<kblks && !(nxt.idx<0) && nxt.idx!=LIST_END && nxt.version==version)
    {
      cur    =  nxt.idx;
      rdLen  =  readBlock(cur, version, b);          if(rdLen==0) goto read_failure;        // rdLen is read length
      b     +=  rdLen;
      len   +=  rdLen;
      nxt    =  nxtBlock(cur);
    }
    rdLen  =  readBlock(cur, version, b, 0, krem);
    b     +=  rdLen;
    len   +=  rdLen;

  read_failure:
    decReaders(blkIdx, version);

    return len;                                           // only one return after the top to make sure readers can be decremented - maybe it should be wrapped in a struct with a destructor
    
    //for(int i=0; i<kblks; ++i){ 
    //  nxt    =  nxtBlock(cur);                 if(nxt.version!=version){ goto read_failure; }
    //  rdLen  =  readBlock(cur, version, b);
    //  b     +=  rdLen;
    //  len   +=  rdLen;
    //  cur    =  nxt.idx;
    //}
    //
    //rdLen  =  readBlock(cur, version, b, krem);
    //b     +=  rdLen;
    //len   +=  rdLen;
    //nxt    =  nxtBlock(cur);                  if(nxt.version!=version){ goto read_failure; }
    //
    ////while(true)
    //while( !(nxt.idx<0) && nxt.idx!=LIST_END && nxt.version==version)
    //{
    //  cur    =  nxt.idx;
    //  rdLen  =  readBlock(cur, version, b);  if(rdLen==0) break;        // rdLen is read length
    //  b     +=  rdLen;
    //  len   +=  rdLen;
    //  nxt    =  nxtBlock(cur);
    //  //if(nxt<0 || nxt==LIST_END) break;
    //}
  }
  Match   memcmpBlk(i32  blkIdx, ui32 version, void const *const buf1, void const *const buf2, ui32 len) const  // todo: eventually take out the inc and dec readers and only do them when dealing with the whole chain of blocks
  {
    if(incReaders(blkIdx, version).len==0) return MATCH_REMOVED;
      auto ret = memcmp(buf1, buf2, len);
    decReaders(blkIdx, version);

    if(ret==0) return MATCH_TRUE;

    return MATCH_FALSE;
  }
  Match     compare(IDX  blkIdx, ui32 version, void const *const buf, ui32 len) const
  {
    using namespace std;
    
    IDX   curidx  =  blkIdx;
    //i32      nxt  =  nxtBlock(curidx);
    auto     nxt  =  nxtBlock(curidx);                            if(nxt.version!=version) return MATCH_FALSE;
    ui32   blksz  =  (ui32)blockFreeSize();  // todo: change this to ui32
    ui8*  curbuf  =  (ui8*)buf;
    auto    klen  =  s_bls[blkIdx].klen;                          if(klen!=len) return MATCH_FALSE;
    auto  curlen  =  len;
    while(true)
    {
      auto p = blockFreePtr(curidx);
      if(blksz > curlen){
        return memcmpBlk(curidx, version, curbuf, p, curlen);
      }else{
        Match cmp = memcmpBlk(curidx, version, curbuf, p, blksz); if(cmp!=MATCH_TRUE){ return cmp; }
      }

      curbuf  +=  blksz;
      curlen  -=  blksz;
      curidx   =  nxt.idx;
      nxt      =  nxtBlock(curidx);                               if(nxt.version!=version) return MATCH_FALSE;
    }

    //if(curlen < blksz){ return MATCH_FALSE; }
    //auto cmplen = min(curlen, (ui32)blksz);
    //Match   cmp = memcmpBlk(curidx, curbuf, p, cmplen);
    //}else if(-nxt != curlen){ return MATCH_FALSE; }
    // return MATCH_TRUE; // never reached
  }
  ui32          len(i32  blkIdx, ui32 version, ui32* out_vlen) const
  {
    BlkLst bl = s_bls[blkIdx];
    if(version==bl.kr.version && bl.len>0){
      *out_vlen = bl.len - bl.klen;
      return bl.len;
    }else 
      return 0;
  }
  auto         list() const -> ConcurrentList const&
  {
    return s_cl;
  }
  auto         data() const -> const void*
  {
    return (void*)s_blksAddr;
  }
  ui64   blockCount() const
  {
    return 0; // m_cl.sizeBytes();
  }
};
class   ConcurrentHash
{
private:

public:
  using VerIdx = ConcurrentStore::VerIdx;

  static const  i8   RM_OWNER         =    -1;            // keep this at 0 if INIT_READERS is changed to 1, then take out remove flag
  static const ui8   LAST_READER      =     0;            // keep this at 0 if INIT_READERS is changed to 1, then take out remove flag
  static const ui8   INIT_READERS     =     0;            // eventually make this 1 again? - to catch when readers has dropped to 0
  static const ui8   FREE_READY       =     0;
  static const ui8   MAX_READERS      =  0xFF;
  static const ui64  EMPTY_KEY        =  2097151;         // first 21 bits set 
  static const ui32  EMPTY_HASH_IDX   =  0xFFFFFFFF;           // 32 bits set - hash indices are different from block indices 
  //static const ui64  EMPTY_KEY        =  0x000000000FFFFFFF;   // first 28 bits set 
  //static const ui64  EMPTY_KEY        =    0x0000000000200000;   // first 21 bits set 

  static ui32       nextPowerOf2(ui32  v)
  {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;

    return v;
  }
  static ui64          sizeBytes(ui32 size)
  {
    //return lava_vec<VerIdx>::sizeBytes( nextPowerOf2(size) );
    return lava_vec<VerIdx>::sizeBytes(size);
  }
  static bool  DefaultKeyCompare(ui32 a, ui32 b)
  {
    return a == b;
  }
  static ui32          HashBytes(const void *const buf, ui32 len)
  {
    ui64 hsh = fnv_64a_buf(buf, len);

    return (ui32)( (hsh>>32) ^ ((ui32)hsh));
  }
  static VerIdx         empty_kv()
  {
    VerIdx empty;
    empty.idx      =  EMPTY_KEY;
    //empty.val      =  EMPTY_KEY;
    empty.version  =  0;
    //empty.readers  =  0;
    return empty;
  }
  static bool            IsEmpty(VerIdx kv)
  {
    static VerIdx emptykv = empty_kv();
    return emptykv.asInt == kv.asInt;
  }

private:
  using i8        =  int8_t;
  using ui32      =  uint32_t;
  using ui64      =  uint64_t;
  using Aui32     =  std::atomic<ui32>;
  using Aui64     =  std::atomic<ui64>;  
  using VerIdxs   =  lava_vec<VerIdx>;
  using Mut       =  std::mutex;
  using UnqLock   =  std::unique_lock<Mut>;

          ui32      m_sz;
  mutable VerIdxs   m_kvs;

  VerIdx        load_kv(ui32 i)                const
  {
    using namespace std;
    
    VerIdx keyval;
    keyval.asInt   =  atomic_load<ui64>( (Aui64*)(&(m_kvs.data()[i].asInt)) );              // Load the key that was there.
    return keyval;
  }
  VerIdx       store_kv(ui32 i, VerIdx keyval) const
  {
    using namespace std;
    
    //atomic_store<ui64>( (Aui64*)&m_kvs[i].asInt, _kv.asInt );

    VerIdx ret;
    ret.asInt = atomic_exchange<ui64>( (Aui64*)(&(m_kvs[i].asInt)), keyval.asInt);
    return ret;
  }
  bool  compexchange_kv(ui32 i, ui64* expected, ui64 desired) const
  {
    using namespace std;
    
    //kv    keyval;
    //keyval.asInt    =  atomic_load<ui64>( (Aui64*)&m_kvs.data()[i].asInt );
    //keyval.key      =  key;
    //keyval.val      =  val;
    //return success;

    return atomic_compare_exchange_strong( (Aui64*)&(m_kvs.data()[i].asInt), expected, desired);                      // The entry was free. Now let's try to take it using a CAS. 
  }
  void           doFree(ui32 i)                const
  {
    store_kv(i, empty_kv());
  }

  template<class MATCH_FUNC> 
  auto       checkMatch(ui32 version, ui32 key, MATCH_FUNC match) const -> Match //  decltype(match(empty_kv()))
  {
    //incReaders(i);  // todo: have incReaders return a VerIdx?
      Match ret = match(key, version);
    //decReaders(i);
    
    return ret;
  }

  template<class MATCH_FUNC, class FUNC> 
  bool       runIfMatch(ui32 i, ui32 version, ui32 key, MATCH_FUNC match, FUNC f) const // const -> bool
  {
    //VerIdx kv = incReaders(i);    
      Match      m = match(key, version);
      bool matched = false;                                       // not inside a scope
      if(m==MATCH_TRUE){ matched=true; f(load_kv(i)); }          
    //decReaders(i);
    
    return matched;
  }

public:
  ConcurrentHash(){}
  ConcurrentHash(ui32 size)
  {
    init(size);
  }
  ConcurrentHash(void* addr, ui32 size, bool owner=true) :
    m_sz(nextPowerOf2(size)),
    m_kvs(addr, m_sz)
  {
    if(owner){
      VerIdx defKv = empty_kv();
      for(ui64 i=0; i<m_kvs.size(); ++i)
        m_kvs[i] = defKv;
    }
  }
  ConcurrentHash(ConcurrentHash const& lval) = delete;
  ConcurrentHash(ConcurrentHash&&      rval) = delete;
  
  ConcurrentHash& operator=(ConcurrentHash const& lval) = delete;
  ConcurrentHash& operator=(ConcurrentHash&&      rval) = delete;

  VerIdx operator[](ui32 idx) const
  {
    return m_kvs[idx];
  }

  template<class MATCH_FUNC> 
  VerIdx   putHashed(ui32 hash, VerIdx vi, MATCH_FUNC match) const
  {
    using namespace std;
    static const VerIdx empty = empty_kv();

    VerIdx desired   =  empty;
    desired.idx      =  vi.idx;
    desired.version  =  vi.version;
    ui32          i  =  hash;
    ui32         en  =  min(hash%m_sz - 1, m_sz-1); // clamp to m_sz-1 for the case that hash==0, which will result in an unsigned integer wrap?   // % m_sz;   //>0? hash-1  :  m_sz
    for(;; ++i)
    {
      i %= m_sz;
      VerIdx probedKv = load_kv(i);
      if(probedKv.idx==EMPTY_KEY)
      {          
        VerIdx expected  =  empty;
        bool    success  =  compexchange_kv(i, &expected.asInt, desired.asInt);
        if(success) return expected;  // continue;                                       // WRONG!? // Another thread just stole it from underneath us.
        else{ i==0? (m_sz-1)  : (i-1); continue; }  // retry the same loop again
      }                                                                                    // Either we just added the key, or another thread did.
      
      if( checkMatch(probedKv.version, probedKv.idx, match)!=MATCH_TRUE ) continue;
      bool success = compexchange_kv(i, &probedKv.asInt, desired.asInt);
      if(success) return probedKv;
      else{ i==0? (m_sz-1)  : (i-1); continue; }

      if(i==en) break;
    }

    return empty;  // should never be reached
  }
  template<class MATCH_FUNC> 
  VerIdx    rmHashed(ui32 hash, MATCH_FUNC match)            const
  {  
    using namespace std;
    static const VerIdx empty = empty_kv();

    ui32  i = hash;
    ui32 en = min(hash%m_sz - 1, m_sz-1); // clamp to m_sz-1 for the case that hash==0, which will result in an unsigned integer wrap? 
    for(;; ++i)
    {
      i %= m_sz;
      VerIdx probedKv = load_kv(i);

      if(probedKv.idx==EMPTY_KEY) return empty;

      Match m = checkMatch(probedKv.version, probedKv.idx, match);
      if(m==MATCH_TRUE){
        bool success = compexchange_kv(i, &probedKv.asInt, empty.asInt);
        if(success) return probedKv;
        else{ i==0? (m_sz-1)  : (i-1); continue; }  // retry the same loop again

        return probedKv;   
      }
      if(m==MATCH_REMOVED || i==en){ return empty; }

      //if(i==en) break;
    }

    return empty; 
  }
  template<class MATCH_FUNC, class FUNC> 
  bool      runMatch(ui32 hash, MATCH_FUNC match, FUNC f)    const // -> decltype( f(VerIdx()) )
  {
    using namespace std;
    
    ui32  i = hash;
    ui32 en = min(hash%m_sz - 1, m_sz-1); // clamp to m_sz-1 for the case that hash==0, which will result in an unsigned integer wrap?   // % m_sz;   //>0? hash-1  :  m_sz
    for(;; ++i)
    {
      i %= m_sz;
      VerIdx probedKv = load_kv(i);
      if( probedKv.idx==EMPTY_KEY ) return false;     // todo: this conflates and assumes that EMPTY_KEY is both the ConcurrentStore block index EMPTY_KEY and the ConcurrentHash EMPTY_KEY?
      if( runIfMatch(i, probedKv.version, probedKv.idx, match, f) ) return  true;
      if(i==en) return false;
    }
  }
  template<class FUNC> 
  auto       runRead(ui32 idx, ui32 version, FUNC f)         const -> decltype( f(VerIdx()) )    // decltype( (f(empty_kv())) )
  {
    //VerIdx kv = incReaders(idx);
    //auto ret = f(vi);
    //decReaders(idx);

    auto  vi = load_kv(idx);        if(vi.version!=version) return false;
    return f(vi);
  }

  bool          init(ui32   sz)
  {
    using namespace std;
    
    //m_sz      =  nextPowerOf2(sz);
    m_sz      =  sz;
    //m_kvs     =  lava_vec<VerIdx>(m_sz);
    new (&m_kvs) lava_vec<VerIdx>(m_sz); // placement new because the copy constructor and assignment operator are deleted.  msvc doesn't care, but clang does
    VerIdx defKv  =  empty_kv();
    for(ui64 i=0; i<m_kvs.size(); ++i) m_kvs[i] = defKv;
    
    return true;
  }
  VerIdx          at(ui32  idx)                   const
  {
    //return m_kvs[idx];
    return load_kv(idx);
  }
  ui32           nxt(ui32  stIdx)                 const
  {
    auto idx = stIdx;
    VerIdx empty = empty_kv();
    do{
      VerIdx kv = load_kv(idx);
      if(kv.idx != empty.idx) break;
      idx = (idx+1) % m_sz;                                             // don't increment idx above since break comes before it here

      if(idx==stIdx) return empty.idx;
    }while(true);

    return  idx;

    /* && kv.readers>=0 */
  }
  ui32          size()                            const
  {
    return m_sz;
  }
  ui64     sizeBytes()                            const
  {
    return m_kvs.sizeBytes();
  }
};
struct       SharedMem       // in a halfway state right now - will need to use arbitrary memory and have other OS implementations for shared memory eventually
{
  static const int alignment = 0;
  
  #ifdef _WIN32
    void*      fileHndl;
  #elif defined(__APPLE__) || defined(__MACH__) || defined(__unix__) || defined(__FreeBSD__) // || defined(__linux__) ?    // osx, linux and freebsd
    int        fileHndl;
  #endif

  void*       hndlPtr;
  void*           ptr;
  ui64           size;
  bool          owner;

  static  bool file_exists(char const* filename)
  {
    if(FILE* file = fopen(filename, "r")){
      fclose(file);
      return true;
    }
    return false;
  }

public:
  static void        FreeAnon(SharedMem& sm)
  {
    #ifdef _WIN32
      if(sm.hndlPtr){
        UnmapViewOfFile(sm.hndlPtr);
      }
      if(sm.fileHndl){
        CloseHandle(sm.fileHndl);
      }
    #elif defined(__APPLE__) || defined(__MACH__) || defined(__unix__) || defined(__FreeBSD__) // || defined(__linux__) ?    // osx, linux and freebsd
      if(sm.hndlPtr){
        munmap(sm.hndlPtr, sm.size);  // todo: size here needs to be the total size, and errors need to be checked
      }
      if(sm.fileHndl){
        close(sm.fileHndl); // todo: get close to work
        // todo: deal with errors here as well?
      }
    #endif

    sm.clear();
  }
  static SharedMem  AllocAnon(const char* name, ui64 size)
  {
    using namespace std;
    
    SharedMem sm;
    sm.owner = false;
    sm.size  = alignment==0? size  :  alignment-(size%alignment);

    #ifdef _WIN32      // windows
      char path[512] = "Global\\simdb_15_";
    //#elif defined(__APPLE__) || defined(__FreeBSD__) // || defined(__linux__) ?    // osx, linux and freebsd
    #elif defined(__APPLE__) || defined(__MACH__) || defined(__unix__) || defined(__FreeBSD__) || defined(__linux__)  // osx, linux and freebsd
      char path[512] = "/tmp/simdb_15_";
      //char path[512] = "simdb_15_";
    #endif

    // todo: need to check that name and path are less than sizeof(path) here
    size_t len = strlen(path) + strlen(name);
    if(len > sizeof(path)-1){ /* todo: handle errors here */ }
    else{ strcat(path, name); }

    #ifdef _WIN32      // windows
      sm.fileHndl = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, path);
      if(sm.fileHndl==NULL)
      {
        sm.fileHndl = CreateFileMapping(
          INVALID_HANDLE_VALUE,
          NULL,
          PAGE_READWRITE,
          0,
          (DWORD)size,
          path);
        if(sm.fileHndl!=NULL) sm.owner=true;
      }
    
      if(sm.fileHndl==NULL){return move(sm);}

      sm.hndlPtr = MapViewOfFile(sm.fileHndl,   // handle to map object
        FILE_MAP_READ | FILE_MAP_WRITE, // FILE_MAP_ALL_ACCESS,   // read/write permission
        0,
        0,
        0);
      if(sm.hndlPtr==nullptr){ CloseHandle(sm.fileHndl); sm.clear(); return move(sm); }
      // END windows
    //#elif defined(__APPLE__) || defined(__FreeBSD__) // || defined(__linux__) ?    // osx, linux and freebsd
    #elif defined(__APPLE__) || defined(__MACH__) || defined(__unix__) || defined(__FreeBSD__) || defined(__linux__)  // osx, linux and freebsd
      //sm.fileHndl = open(path, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH ); // O_CREAT | O_SHLOCK ); // | O_NONBLOCK );
      sm.owner   = true; // todo: have to figure out how to detect which process is the owner

      FILE* fp = fopen(path,"rw");
      if(fp){
        fclose(fp);
        sm.fileHndl = open(path, O_RDWR);
        sm.owner = false;
      }else{
        sm.fileHndl = open(path, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR |S_IRGRP|S_IWGRP | S_IROTH|S_IWOTH ); // O_CREAT | O_SHLOCK ); // | O_NONBLOCK );
        if(sm.fileHndl == -1){
          printf("open failed, file handle was -1 \nFile name: %s \nError number: %d \n\n", path, errno); 
          fflush(stdout);
        }
        else{
          //flock(sm.fileHndl, LOCK_EX);   // exclusive lock  // LOCK_NB
        }
      }

      if(sm.owner){  // todo: still need more concrete race protection
        //fstore_t store = {F_ALLOCATECONTIG, F_PEOFPOSMODE, 0, (off_t)size};

        //struct flock lck = {0,0,0};
        fcntl(sm.fileHndl, F_GETLK, &flock);

        flock(sm.fileHndl, LOCK_EX);   // exclusive lock  // LOCK_NB
        fcntl(sm.fileHndl, F_PREALLOCATE); //  todo: try F_ALLOCATECONTIG at some point
        ftruncate(sm.fileHndl, size);   // todo: don't truncate if not the owner, and don't pre-allocate either ?
        flock(sm.fileHndl, LOCK_EX);
        // get the error number and handle the error
      }
      //sm.hndlPtr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, sm.fileHndl, 0);

      //auto zeromem = malloc(size);
      //memset(zeromem, 0, size);
      //write(sm.fileHndl, zeromem, size);
      //free(zeromem);
      sm.hndlPtr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED , sm.fileHndl, 0); // MAP_PREFAULT_READ  | MAP_NOSYNC
      close(sm.fileHndl);
      sm.fileHndl = 0;
      // memset(sm.hndlPtr, 0, size);
 
      if(sm.hndlPtr==MAP_FAILED){
        printf("mmap failed\nError number: %d \n\n", errno);
        fflush(stdout);
        /*todo: handle this error*/ 
      }
    #endif       
  
    ui64      addr = (ui64)(sm.hndlPtr);
    ui64 alignAddr = addr;
    if(alignment!=0) alignAddr = addr + ((alignment-addr%alignment)%alignment);       // why was the second modulo needed?
    sm.ptr         = (void*)(alignAddr);

    return move(sm);
  }

  SharedMem(){}
  SharedMem(SharedMem&)       = delete;
  SharedMem(SharedMem&& rval)
  {
    fileHndl       =  rval.fileHndl;
    hndlPtr        =  rval.hndlPtr;
    ptr            =  rval.ptr;
    size           =  rval.size;
    owner          =  rval.owner;

    rval.clear();
  }
  ~SharedMem()
  {
    SharedMem::FreeAnon(*this);
  }
  void clear()
  {
    fileHndl  =  (decltype(fileHndl))0;
    hndlPtr   =  nullptr;
    ptr       =  nullptr;
    size      =  0;
    owner     =  false;
  }
  auto  data() -> void*
  {
    return ptr;
  }
};
class            simdb
{
public:
  using VerIdx = ConcurrentHash::VerIdx;

private:
  SharedMem           m_mem;

  aui64*            s_flags;
  aui64*        s_blockSize;
  aui64*       s_blockCount;
  ConcurrentStore      s_cs;     // store data in blocks and get back indices
  ConcurrentHash       s_ch;     // store the indices of keys and values - contains a ConcurrentList

  // these variables are local to the stack where simdb lives, unlike the others, they are not simply a pointer into the shared memory
  mutable ui32   m_nxtChIdx;      
  mutable ui32   m_curChIdx;     
  ui64             m_blkCnt;
  ui64              m_blkSz;

public:
  static const ui32  EMPTY_KEY = ConcurrentHash::EMPTY_KEY;          // 28 bits set 
  static const ui32 FAILED_PUT = ConcurrentHash::EMPTY_KEY;          // 28 bits set 
  static const ui32   LIST_END = ConcurrentStore::LIST_END;
  static ui64       OffsetBytes()
  {
    return sizeof(aui64)*3;
  }
  static ui64           MemSize(ui64 blockSize, ui64 blockCount)
  {
    auto  hashbytes = ConcurrentHash::sizeBytes((ui32)blockCount);
    auto storebytes = ConcurrentStore::sizeBytes((ui32)blockSize, (ui32)blockCount);
    return  hashbytes + storebytes + OffsetBytes();
  }
  static Match     CompareBlock(simdb const *const ths, i32 blkIdx, ui32 version, void const *const buf, ui32 len)
  { 
    return ths->s_cs.compare(blkIdx, version, buf, len);
  }
  static bool           IsEmpty(VerIdx kv){return ConcurrentHash::IsEmpty(kv);}         // special value for ConcurrentHash
  static bool         IsListEnd(VerIdx vi){return ConcurrentStore::IsListEnd(vi); }     // special value for ConcurrentStore

public:
  simdb(){}
  simdb(const char* name, ui32 blockSize, ui32 blockCount) : 
    m_curChIdx(0)
    //m_mem( SharedMem::AllocAnon(name, MemSize(blockSize,blockCount)) ),
    //m_ch( ((i8*)m_mem.data())+OffsetBytes(), blockCount, m_mem.owner),
    //m_cs( ((i8*)m_mem.data())+m_ch.sizeBytes(blockCount)+OffsetBytes(), blockSize, blockCount, m_mem.owner),                 // todo: change this to a void*
    //m_blockCount( ((aui64*)m_mem.data())+2 ),
    //m_blockSize(  ((aui64*)m_mem.data())+1 ),
    //m_flags(       (aui64*)m_mem.data() )
  {
    new (&m_mem) SharedMem( SharedMem::AllocAnon(name, MemSize(blockSize,blockCount)) );

    s_blockCount =  ((aui64*)m_mem.data())+2;
    s_blockSize  =  ((aui64*)m_mem.data())+1;
    s_flags      =   (aui64*)m_mem.data();

    if(isOwner()){
      s_blockCount->store(blockCount);
      s_blockSize->store(blockSize);
    }
    else{                                                       // need to spin until ready
      while(s_flags->load()==false){}
      m_mem.size = MemSize(s_blockSize->load(), s_blockCount->load());
      //new (&m_ch) ConcurrentHash( ((i8*)m_mem.data())+OffsetBytes(), m_blockCount->load(), m_mem.owner);
      //new (&m_cs) ConcurrentStore( ((i8*)m_mem.data())+m_ch.sizeBytes(m_blockCount->load())+OffsetBytes(), m_blockSize->load(), m_blockCount->load(), m_mem.owner);                 // todo: change this to a void*
    }

    //new (&m_mem) SharedMem( SharedMem::AllocAnon(name, MemSize(blockSize,blockCount)) );
    //new (&m_ch) ConcurrentHash( ( ((i8*)m_mem.data())+OffsetBytes(), blockCount, m_mem.owner) );
    //new (&m_cs) ConcurrentStore(  ((i8*)m_mem.data())+m_ch.sizeBytes(blockCount)+OffsetBytes(), blockSize, blockCount, m_mem.owner);                 // todo: change this to a void*
    new (&s_ch) ConcurrentHash( ((i8*)m_mem.data())+OffsetBytes(), 
                                       (ui32)s_blockCount->load(), 
                                                      m_mem.owner);
    auto chSz = s_ch.sizeBytes();
    new (&s_cs) ConcurrentStore( ((i8*)m_mem.data())+chSz+OffsetBytes(), 
                                              (ui32)s_blockSize->load(), 
                                             (ui32)s_blockCount->load(), 
                                                           m_mem.owner);                 // todo: change this to a void*

    m_blkCnt = s_blockCount->load();
    m_blkSz  = s_blockSize->load();

    // todo: initialized flag and reference count
    if(isOwner()) s_flags->store(1);                                        // set to 1 to signal construction is done
  }

  i32          put(const void *const key, ui32 klen, const void *const val, ui32 vlen)
  {
    assert(klen>0);

    i32 blkcnt = 0;
    
    auto vi = s_cs.alloc(klen+vlen, klen, &blkcnt);    // todo: use the VersionIdx struct // kidx is key index
    if(vi.idx==LIST_END) return EMPTY_KEY;

    s_cs.put(vi.idx, key, klen, val, vlen);

    ui32 keyhash = ConcurrentHash::HashBytes(key, klen);
    auto     ths = this;                                                        // this silly song and dance is because the this pointer can't be passed to a lambda
    VerIdx    kv = s_ch.putHashed(keyhash, vi,                                  // this returns the previous VerIdx at the position
      [ths, key, klen](ui32 blkidx, ui32 ver){
        return CompareBlock(ths,blkidx,ver,key,klen);
      });

    if(kv.idx!=EMPTY_KEY) s_cs.free(kv.idx, kv.version);                       // putHashed returns the entry that was there before, which is the entry that was replaced. If it wasn't empty, we free it here. 

    return vi.idx;
  }
  i32          put(char const* const key, const void *const val, ui32 vlen)
  {
    assert(strlen(key)>0);
    return put(key, (ui32)strlen(key), val, vlen);
  }
  bool         get(const void *const key, ui32 klen, void *const   out_val, ui32 vlen) const
  {
    if(klen<1) return 0;

    auto       ths = this;
    auto       hsh = ConcurrentHash::HashBytes(key, klen);

    auto matchFunc = [ths, key, klen](ui32 blkidx, ui32 ver){
      return CompareBlock(ths,blkidx,ver,key,klen);
    };
    
    ui32  out_klen = 0;
    auto   runFunc = [ths, &out_klen, out_val, vlen](VerIdx kv){
      return ths->s_cs.get(kv.idx, kv.version, out_val, vlen);
    };

    return s_ch.runMatch(hsh, matchFunc, runFunc);
  }
  bool         get(char const* const key, void *const val, ui32 vlen) const
  {
    return get(key, (ui32)strlen(key), val, vlen);
  }
  bool          rm(const void *const key, ui32 klen)
  {
    auto hash = ConcurrentHash::HashBytes(key, klen);
    auto  ths = this;
    VerIdx kv = s_ch.rmHashed(hash,
      [ths, key, klen](ui32 blkidx, ui32 ver){ return CompareBlock(ths,blkidx,ver,key,klen); });

    if(kv.idx!=EMPTY_KEY) s_cs.free(kv.idx, kv.version);

    return kv.idx!=EMPTY_KEY;
  }
  i64          len(const void *const key, ui32 klen, ui32* out_vlen=nullptr, ui32* out_version=nullptr) const
  {
    if(klen<1) return 0;

    auto       ths = this;
    auto       hsh = ConcurrentHash::HashBytes(key, klen);

    auto matchFunc = [ths, key, klen](ui32 blkidx, ui32 ver){
      return CompareBlock(ths,blkidx,ver,key,klen);
    };
    
    ui32 len=0;
    ui32 ver=0;
    auto runFunc = [ths, &len, &ver, out_vlen](VerIdx kv){
      len = IsEmpty(kv)?  0ull  :  ths->s_cs.len(kv.idx, kv.version, out_vlen);
      ver = kv.version;
    };

    if( !s_ch.runMatch(hsh,  matchFunc, runFunc) ) return 0;

    if(out_version) *out_version = ver;
    return len;
  }
  bool         len(ui32 idx, ui32 version, ui32* out_klen=nullptr, ui32* out_vlen=nullptr) const
  {
    auto  ths = this;
    bool   ok = s_ch.runRead(idx, version, 
    [ths, out_klen, out_vlen](VerIdx kv)
    {
      ui32 vlen = 0;
      auto tlen = ths->s_cs.len(kv.idx, kv.version, out_vlen);
      if(tlen>0){
        *out_klen = tlen - *out_vlen;
        return true;
      }
      return false;
    });

    return ok;
  }
  void       flush() const
  {
    FlushViewOfFile(m_mem.hndlPtr, m_mem.size);
  }
  VerIdx       nxt() const                                   // this version index represents a hash index, not an block storage index
  {
    auto        st = m_nxtChIdx;
    VerIdx   empty = s_ch.empty_kv();
    ui32    chNxt; // = empty.key;
    VerIdx     vi;
    do{
           chNxt = s_ch.nxt(m_nxtChIdx);      if(chNxt==empty.idx) return empty;             // can return the same index - it does not do the iteration after finding a non empty key
              vi = s_ch.at(chNxt);
      m_nxtChIdx = (chNxt + 1) % m_blkCnt;
    }while( IsEmpty(vi) );                               // m_nxtChIdx!=st && 

    m_curChIdx = chNxt;
    VerIdx ret = {chNxt, vi.version};
    
    return ret;
  }
  bool      getKey(ui32 idx, ui32 version, void *const out_buf, ui32 klen)   const
  {
    if(klen<1) return false;
    
    auto     ths = this;
    auto runFunc = [ths, klen, out_buf](VerIdx kv){
      if(IsEmpty(kv)) return false;
      auto getlen = ths->s_cs.getKey(kv.idx, kv.version, out_buf, klen);
      if(getlen<1) return false;
      
      return true;
    };
    return s_ch.runRead(idx, version, runFunc);
  }
  ui32         cur() const
  {
    return m_curChIdx;
  }
  auto        data() const -> const void* const
  {
    return s_cs.data();
  }
  ui64        size() const
  {
    //return m_mem.size;
    //return s_cs.sizeBytes( (ui32)s_blockSize->load(), (ui32)s_blockCount->load());
    return ConcurrentStore::sizeBytes( (ui32)s_blockSize->load(), (ui32)s_blockCount->load());
  }
  bool     isOwner() const
  {
    return m_mem.owner;
  }
  ui64      blocks() const
  {
    return s_blockCount->load();
  }
  auto         mem() const -> void*
  {
    return m_mem.hndlPtr;
  }
  ui64     memsize() const
  {
    return m_mem.size;
  }

  // separated C++ functions - these won't need to exist if compiled for a C interface
  struct VerStr { 
    ui32 v; str s; 
    bool  operator<(VerStr const& vs) const { if(s==vs.s) return v<vs.v; else return s<vs.s; }  
    bool  operator<(str const&    rs) const { return s<rs;    }
    bool operator==(VerStr const& vs) const { return s==vs.s && v==vs.v; } 
  };   

  i64          put(str    const& key, str const& value)
  {
    return put(key.data(), (ui32)key.length(), value.data(), (ui32)value.length());
  }
  bool         get(str    const& key, str* out_value) const
  {
    ui32   vlen = 0;
    auto  kvLen = len(key.data(), (ui32)key.length(), &vlen);
    new (out_value) std::string(vlen,'\0');
    bool     ok = get(key.data(), (ui32)key.length(), (void*)out_value->data(), vlen);

    return ok;
  }
  auto         get(str    const& key)                 const -> std::string
  {
    str ret;
    if(this->get(key, &ret)) return ret;
    else return str("");
  }
  VerStr    nxtKey(ui64* searched=nullptr)            const
  {
    ui32 klen, vlen;
    bool    ok = false;
    i64   prev = (i64)m_nxtChIdx;
    VerIdx nxt = this->nxt();

    if(searched){
      i64 sidx  = (i64)nxt.idx;       // sidx is signed index
      i64 sprev = (i64)prev;          // sprev is signed previous
      *searched = (sidx-sprev)>0?  sidx-sprev  :  (m_blkCnt-sprev-1) + sidx+1;
    }
    if(nxt.idx==EMPTY_KEY) 
      return {nxt.version, ""};
    
    ok         = this->len(nxt.idx, nxt.version, 
                           &klen, &vlen);               
    if(!ok)
      return {nxt.version, ""};
    
    str key(klen,'\0');
    ok         = this->getKey(nxt.idx, nxt.version, 
                              (void*)key.data(), klen); 
                              
    if(!ok || strlen(key.c_str())!=key.length() )
      return {nxt.version, ""};

    //if(out_version) *out_version = nxt.version;
    //if(ok) 


    return { nxt.version, key };                    // copy elision 
    //else   return { 0, "" };
  }
  auto  getKeyStrs() const -> vec<VerStr>            // vec<ui32>* out_versions=nullptr
  {
    using namespace std;
    
    //unordered_set<str> keys;
    
    //template<> struct hash<VerStr>{
    //  size_t operator()(VerStr const& vs){ 
    //    return hash<str>()(vs.str);
    //  }  
    //};
    //unordered_set<VerStr> keys;

    set<VerStr> keys;

    //ui32       i = 0;
    //str nxt = nxtKey();
    ui64 srchCnt = 0;
    auto     nxt = nxtKey();                             
    while( srchCnt<m_blkCnt && keys.find(nxt)==keys.end() )
    {
      if(nxt.s.length()>0) keys.insert(nxt);
    
      ui64 searched = 0;
      nxt = nxtKey(&searched);
      srchCnt += searched;
      //++i;
    }

    //if(out_versions) new (out_versions) vec<ui32>()
    return vec<VerStr>(keys.begin(), keys.end());
  }
  bool          rm(str const& key)
  {
    return this->rm( (void const* const)key.data(), (ui32)key.length() );
  }

  template<class T>
  i64          put(str    const& key, vec<T> const& val)
  {    
    return put(key.data(), (ui32)key.length(), val.data(), (ui32)(val.size()*sizeof(T)) );
  }


  /*
  template<class T>
  i64          get(vec<T> const& key, void*  out_buf) const     // todo: needs to be redone
  {
    Reader r = read((void*)key.data(), (ui32)(key.size() * sizeof(T)));
    if(isEmpty(r.kv)) return -1;
    //if(r.kv.key == EMPTY_KEY || r.kv.readers <= 0){ return -1; }

    ui64 len = getFromBlkIdx(r.kv.val, out_buf);
    if(r.doRm()){ m_cs.free(r.kv.val); m_cs.free(r.kv.idx); }

    return len;
  }
  */
  // end separated C++ functions

};

#endif






//str       nxtKey(ui32* out_version=nullptr)         const
//{
//  ui32 klen, vlen;
//  bool    ok = false;
//  VerIdx nxt = this->nxt();                           if(nxt.idx==EMPTY_KEY) return "";
//  ok         = this->len(nxt.idx, nxt.version, 
//                         &klen, &vlen);               if(!ok) return "";
//  str key(klen,'\0');
//  ok         = this->getKey(nxt.idx, nxt.version, 
//                            (void*)key.data(), klen); if(!ok) return "";
//
//  if(out_version) *out_version = nxt.version;
//  return key;                    // copy elision 
//}

//template< template<class> class V, class T>
//using VAL_TYPE = V::value_type;
//vec<int>::value_type

//if(blkcnt<0){
//  s_cs.free(vi.idx, vi.version);
//  return EMPTY_KEY;
//}    

//template<class MATCH_FUNC> 
//VerIdx       checkMatch(ui32 i, ui32 key, MATCH_FUNC match) const -> Match //  decltype(match(empty_kv()))
//{
//  Match ret = match(key);
//  
//  return ret;
//}

// todo: WRONG? check needs to be run on each spin if the entry is different
//if( checkMatch(i, probedKv.version, probedKv.idx, match)==MATCH_TRUE ){
  //return store_kv(i, desired);
//}

//
//i  &=  m_sz - 1;


/*
open system call reference from http://www.unix.com/man-page/FreeBSD/2/open/

           O_RDONLY	   open for reading only
	   O_WRONLY	   open for writing only
	   O_RDWR	   open for reading and writing
	   O_EXEC	   open for execute only
	   O_NONBLOCK	   do not block on open
	   O_APPEND	   append on each write
	   O_CREAT	   create file if it does not exist
	   O_TRUNC	   truncate size to 0
	   O_EXCL	   error if create and file exists
	   O_SHLOCK	   atomically obtain a shared lock
	   O_EXLOCK	   atomically obtain an exclusive lock
	   O_DIRECT	   eliminate or reduce cache effects
	   O_FSYNC	   synchronous writes
	   O_SYNC	   synchronous writes
	   O_NOFOLLOW	   do not follow symlinks
	   O_NOCTTY	   don't assign controlling terminal
	   O_TTY_INIT	   restore default terminal attributes
	   O_DIRECTORY	   error if file is not a directory
	   O_CLOEXEC	   set FD_CLOEXEC upon open
*/


