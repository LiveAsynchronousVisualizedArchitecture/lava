
// plan: 
// - ConcurrentHash is a lava_vec and is at the start of the memory
// - ConcurrentStore takes up the rest of the memory
// | - ConcurrentStore is made up of ConcurrentList at the start of its memory
// | - block memory is everything after ConcurrentList
// - The order ends up being ConcurentHash, ConcurrentList, then block storage

// -todo: hash key data instead of the block index of the key
//       --create function to hash arbitrary bytes 
//       --create function to put into a hashmap with a pre-hashed value
// -todo: make bitwise compare function between blocks
// -todo: make a function to compare a block to an arbitrary byte buffer
// -todo: change store_kv to use compare exchange and return previous kv ?
// -todo: make remove function for ConcurrentStore? - just use free
// -todo: remove overwritten indices from the ConcurrentStore
// -todo: figure out why "wat" and "wut" match - 3 bugs: unsigned size_t in the blockcompare function, not flipping the signs of the next index when it is negative (for length), default value of true when while loop ends from the blocks comparing as false
// -todo: figure out why second insert is putting EMPTY_KEY for key and value in the hash map - comp returns true even though strings are different?
// -todo: use match function instead of block comparison for everything? better for continguous memory anyway?
// -todo: fix not finding a key too long to fit in block - wasn't updating the current block index in the loop
// -todo: return an error on running out of blocks - done with a negative blockcount if not enough blocks are available
// -todo: free blocks if not enough block are allocated
// -todo: fix infinite loop on free - last block wasn't getting next block index set when reaching LIST_END 
// -todo: deal with memory / allocate from  shared memory
// -todo: make a membuf class that encapsulates a shared memory buffer / memory mapped file on windows or linux
// -todo: make remove function
// -todo: add const to thread safe functions
// -todo: figure out why readers is so high - uninitialized KV struct in a put function
// -todo: do KV structs need versions? can an identical KV struct ever be inserted? only if the List somehow gave up identical indices, which shouldn't happen?
// -todo: test deletion from db
// -todo: check when reading if something is marked for deletion and return 0 / EMPTY_KEY if it is
// -todo: make remove function concurrent and account for the number of readers
// -todo: make remove decremented the readers but don't actually delete, so that the last reader out would delete, maybe just make a flag as mark for deletion?
// -todo: make addReaders not add any readers below zero
// -todo: figure out how to deal with deletion when there is a concurrent write - reset the readers and hash?
// -todo: make -1 an error instead of returning a length of 0? - distinguising a key with length 0 and no key could be useful 
// -todo: is only the key being deleted? - no, but vals should be removed before keys to keep the order more consistent, all other things being equal
// -todo: need to fix ConcurrentList free? it writes to the list before the compare_swap has gone through? - no, because writing to taken indices in the list doesn't matter + only one thread should be freeing a specific index at a time - if two threads were freeing the same index on top of each other, a problem bigger than atomics would be that even if both operations went through, there would be a double free
// -todo: change public get methods to use read - took out one get and changed the other to being private getFromBlkIdx
// -todo: fix shared memory aligned allocation
// -todo: make SharedMem check if the mem file already exists
// -todo: test opening the db from multiple processes - didn't work at first, likely because the copy constructor was not deleted and the destructor was run early
// -todo: test lava_vec with external memory
// -todo: convert ConcurrenList to use lava_vec
// -todo: fix hash function hashing string() different from const char* - off by one error was hashing an extra 32 bits
// -todo: fix block memory being written but get() not working (ConcurrentList not correct yet?) - hash problem, not ConcurrentList problem
// -todo: need to make ConcurrentList a flat data structure so it can be store at the start of the memory mapped file
// -todo: make ConcurrentHash flat using lava_vec
// -todo: make Store and Hash both take bool for ownership to decide whether or not to init
// -todo: test using the db from multiple processes
// -todo: make simdb read the sizes from the the database - if the Store uses the size as the head and the hashmap is sized larger than the number of blocks, how to get the number of elements? - made 12 bytes at the start of the shared memory
// -todo: store blockSize and blockCount and flags in the starting three ui64 slots
// -todo: make flag to see if the db has been initialized yet to mitigate race conditions on creating and opening the memory mapping
// -todo: make post build run a copy command for an extra .exe that can be run while the primary exe is overwritten by the compiler
// -todo: figure out why memory is different on second run - why does "wat" get inserted again? - working as intended?
// -todo: try zeroing memory of each block on free - works, just needs two memset() calls just as there are two free() calls to the list
// -todo: move block lists to a lava_vec
// -todo: figure out why memory seem correct but get is not working - just the check of readers being below or equal to zero
// -todo: make decReaders run doFree ? 
// -todo: add readers inc and dec to matching - made memcmpBlk function
// -todo: test db.rm 
// -todo: make readers on a per block level
// -todo: make free decrement block readers
// -todo: move readers to the ConcurrentStore
// -todo: make comparison function in ConcurrentHash that increments and decrements reader
// -todo: should rm on the last put erase their memory? - yes, wasn't resetting readers
// -todo: figure out why get returns -1  -  no return statement, just empty probedKv.key; 
// -todo: figure out why get after put isn't working - was switching the order of readers and index in BlkLst struct
// -todo: figure out why put after rm doesn't show up with get - readers not being reset on allocation? - forgot to take out the free-ing in put() 
// -todo: make both free and read give back a block with last one out strategy
// -todo: test again with multiple processes after redoing rm() technique - without put deleting old elements, how to get rid of overwritten elements? - needed to put back deleting in put(), but make sure rm() swaps in EMPTY_KEY
// -todo: make rmHashed return EMPTY_KEY on MATCH_REMOVED
// -todo: take readers out of hash struct 
// -todo: move hash functions to be static instead of member functions
// -todo: look up better hash function - fnv
// -todo: clean up fnv
// -todo: try making macro with ALLOCA to create a stack based lava_vec
// -todo: load blockSize and blockLength from existing shared memory
// -todo: make function to query size - simb.len(key)
// -todo: make setting the key flag be the last thing when allocating 
// -todo: have last one out remove hash index
// -todo: add removal of blocks to CH doFree ? - no, simdb does that
// -todo: make incReaders for ConcurrentHash
// -todo: have to make hash the authority, give KV struct a reader count
// -todo: put incReaders and decReaders around match function call in ConcurrentHash functions
// -todo: make setting the key flag be the first thing when freeing - actually done in decReaders
// todo: make indexed list of keys
//       -create a bitset in the ConcurrentHash? - can't use ConcurrentHash because blocks could change after query
//       -bitset has to be atomic? - can't create bitset because the key flag, reader count, and index all have to be together?
// done  -use key flag in Block Index struct, treat the entire thing as a 64 bit atomic
// done  -iterate through BlkLst vector to find the next key - done in ConcurrenHash
// done  -after a key is found, get the length of that block index
// done    -when getting the length, check if the block is a key, if not, return an error
// done    -check if the key is not empty
// done    -check if the readers is not less than 0
//       -after getting the length, read from the block index, again checking if the block index is a key and if not, returning an error 
// -todo: can cleanup be done by checking the CH entry after read and setting a flag to delete the old version? - no, because this would only shield one overlap, if more threads were swapping CH entries, they would have to wait/spinlock
// -todo: do a write up on if linear search is neccesary because a hash could land on a KV that has readers as -1 ?  - no different than any other hash map? - search stops on an empty key, but it shouldn't stop on readers<0 ? what happens if a key is inserted a few slots from where the hash lands, then the original landing place is removed? does the empty key stop the search?
// -todo: work out how doFree from ConcurrentHash connects to removing blocks - will need to be done with versioning
// -todo: do a write up on overall structure
// -todo: implement get that takes an index into ConcurrentHash - 0 length does nothing, length must be inout, if key is empty or readers is < 0, make length 0 and do nothing 
// -todo: test looking up values from the iterated keys
// -todo: make sure when looping through keys, that readers is not negative? all reads must be atomic? all BlkLst must have their original index embedded? ConcurrentHash does need readers after all?
// -todo: change pointers to shared memory to have a s_ prefix
// -todo: add version and remove readers from ConcurrentHash struct
// -todo: initialize s_version to 1 if owner==true
// -todo: make LIST_END constant
// -todo: need to have a version with each block and store it with each non head BlkIdx as well as the key value pair of ConcurrentHash - how many bits for the version? 32 bits to start? - just needs to be enough so that a so many blocks can't be gotten while a thread is stalled that the version wraps back around
// -todo: have to put version in concurrent hash so that an overwrite from a new write won't cause an ABA problem with another thread reading from it, then decrementing readers?

// todo: add length and key lenght to ConcurrentStore
// todo: change to single block index key value pairs
// todo: need more data with BlkIdx so that blocks read are known to be from the correct key value pair - do with versions
// todo: make ConcurrentStore.get take a length that it won't exceed
// todo: combine keys and data into one block run
// todo: make readers for blocks only exist on the head of the list?
// todo: make sure that a block isn't read from if readers<0
// todo: make alloc give back blocks if allocation fails
// todo: flip decReaders return value
// todo: take out size_t from ConcurrentStore
// todo: give put() return type a BLKIDX type
// todo: redo simdb functions to use runRead
// todo: change ConcurrentHash to no longer be only powers of 2
// todo: organize all C++ functions separate from C functions so that they can be #ifdef out if used as a C library
// todo: make len be a direct lookup somehow? - if ConcurrentHash is the authority, len can be done the same way as an  empty/full bitset ? 

// todo: change string to pass through to c_str() and const char* overload
// todo: take out power of 2 size restriction and use modulo
// todo: use blockSize and blockCount from the already created shared mem if not the owner
// todo: store size in the ConcurrentList? list isn't atomic so it should work well? should block lists, key sizes, and val sizes all be in their own lava_vecs ? 
// todo: redo concurrent store get to store length so that buffer can be returned
// todo: test with multiple threads in a loop
// todo: store lengths and check key lengths before trying bitwise comparison as an optimization? - would only make a difference for long keys that are larger than one block? no it would make a difference on every get?
// todo: prefetch memory for next block when looping through blocks
// todo: move to using packed key value in one block list
// todo: can block sync be done with a removed flag instead of just using readers?
// todo: change lava_vec name to flat_vec?

// todo: combine key and value storage so they are packed together in the same block list?
// todo: mark free cells as negative numbers so double free is caught? - do it only in debug mode?
// todo: make erase function that 0s out bytes? - only in debug mode?
// todo: Make block size for keys different than data?
// todo: lock init with mutex?
// todo: implement locking resize?
// todo: should the readers be integrated with the list also? 
// todo: make SharedMemory take an address and destructor, or make simdb take an address and destructor to use arbitrary memory?

//Block based allocation
//-Checking if the head has been touched means either incrementing a counter every time it is written, or putting in a thread id every time it is read or written
//-Each ui32 in a vector holds the position of the next free block 
//-Alloc checks the head to read the next free position and puts it in head if head hasn't been touched. 
//-Free checks the head to read the next free position, stores the head value in the free position, then moves head to the just written new free position if head hasn't been touched

//
// Make frees happen from the last block to the first - don't remember what this means

// idea: use max_waste as a recipricol power of two giving the percentage of waste allowed for an allocation
// - 2 would be 25%, 3 would be 12.5% 
// - not allowed to be below 2
// idea: put atomic reader counter into each ConcurrentStore entry as a signed integer
// idea: figure out how to make ConcurrentHash a flat data structure so it can sit in shared memory

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

#ifndef __CONCURRENTMAP_HEADER_GUARD__
#define __CONCURRENTMAP_HEADER_GUARD__

#include <cstdint>
#include <cstring>
#include <atomic>
#include <mutex>
#include <memory>
#include <vector>

#include <windows.h>

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
using  cstr   =   const char*;
using   str   =   std::string;

namespace {
  enum Match { MATCH_FALSE=0, MATCH_TRUE=1, MATCH_REMOVED=-1  };

  template<class T>
  class lava_noop
  {
    void operator()(){}
  };

  ui64 fnv_64a_buf(void *buf, size_t len)
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

#ifdef _WIN32
  // use _malloca ? - would need to use _freea and also know that _malloca always allocates on the heap in debug mode for some crazy reason
  #define STACK_VEC(TYPE, COUNT) lava_vec<TYPE>(_alloca(lava_vec<TYPE>::sizeBytes(COUNT)), COUNT, true);
#else
  // gcc/clang/linux ?
  #include <alloca.h>
  #define STACK_VEC(TYPE, COUNT) lava_vec<TYPE>(_alloca(lava_vec<TYPE>::sizeBytes(COUNT)), COUNT, true);  
#endif

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
    struct{ ui32    isKey :  1; i32   readers : 31; };
    ui32 asInt;
  };
  union BlkLst                            // need to do anything special to guarantee that readers is aligned so it is atomic?
  {
    struct { KeyAndReaders kr; VerIdx vi; ui32 len; ui32 klen; };
    //ui64 asInt;
  };

  using IDX         =  i32;
  using ai32        =  std::atomic<i32>;
  using BlockLists  =  lava_vec<BlkLst>;   // only the indices returned from the concurrent list are altered, and only one thread will deal with any single index at a time 
  //using BlockLists  =  lava_vec<IDX>;

  const static ui32 LIST_END = ConcurrentList::LIST_END;
  //const static VersionedIdx LIST_END = { ConcurrentList::LIST_END, 0 };
  //const static VersionedIdx LIST_END = { ConcurrentList::LIST_END, 0 };

  static BlkLst   make_BlkLst(bool isKey, i32 readers, ui32 idx, ui32 ver, ui32 len, ui32 klen)
  {
    BlkLst bl;
    bl.kr.isKey    = isKey;
    bl.kr.readers  = readers;
    bl.vi.idx      = idx;
    bl.vi.version  = ver;

    if(isKey){
      bl.len  = 0;
      bl.klen = 0;
    }else{
      bl.len  = len;
      bl.klen = klen;
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

  bool      incReaders(ui32 blkIdx)        const                   // BI is Block Index  increment the readers by one and return the previous kv from the successful swap 
  {
    //ai32* aidx = (ai32*)&(m_bls[blkIdx].readers);
    //i32    cur = aidx->load();
    //i32    nxt;
    
    //BlkLst cur, nxt;
    //aui64* aidx = (aui64*)&(s_bls[blkIdx].asInt);
    //cur.asInt   = aidx->load();
    ////nxtBl.asInt = curBl.asInt;
    //do{
    //  if(cur.kr.readers<0) return false;
    //  nxt = cur;
    //  nxt.kr.readers += 1;
    //}while( !aidx->compare_exchange_strong(cur.asInt, nxt.asInt) );

    KeyAndReaders cur, nxt;

    aui32* areaders = (aui32*)&(s_bls[blkIdx].kr.asInt);
    
    cur.asInt = areaders->load();

    do{
      if(cur.readers<0) return false;
      nxt = cur;
      nxt.readers += 1;
    }while( !areaders->compare_exchange_strong(cur.asInt, nxt.asInt) );
    
    return true;
  }
  bool      decReaders(ui32 blkIdx)        const                   // BI is Block Index  increment the readers by one and return the previous kv from the successful swap 
  {
    //ai32* aidx = (ai32*)&(m_bls[blkIdx].readers);
    //
    //auto prev = aidx->fetch_add(-1);
    //if(prev==0){ doFree(blkIdx); return false; }

    BlkLst cur, nxt;
    aui64* aidx = (aui64*)&(s_bls[blkIdx].asInt);
    cur.asInt   = aidx->load();
    do{
      nxt = cur;
      nxt.kr.readers -= 1;
    }while( !aidx->compare_exchange_strong(cur.asInt, nxt.asInt) );
    
    if(cur.kr.readers==0){ doFree(blkIdx); return false; }

    return true;
  }

private:
  mutable ConcurrentList    s_cl;       // flat data structure - pointer to memory 
  mutable BlockLists       s_bls;       // flat data structure - pointer to memory - bl is Block Lists
  void*               s_blksAddr;       // points to the block space in the shared memory
  aui64*               s_version;       // pointer to the shared version number

  // On the stack
  ui32               m_blockSize;
  ui32              m_blockCount;
  ui64                 m_szBytes;
  mutable ai32      m_blocksUsed;      // todo: this is a mistake and does no good unless it is in the shared memory

  i32             stPtr(i32  blkIdx)  const
  {
    //return (i32*)( ((i8*)m_blksAddr) + blkIdx*m_blockSize );
    //return (i32*)&(m_bls.data()[blkIdx]);
    //return m_bls[blkIdx].idx;
    //return m_bls[blkIdx].idx;
  }
  IDX          nxtBlock(i32  blkIdx)  const
  {
    //return *(stPtr(blkIdx));
    return s_bls[blkIdx].idx;
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
  i32          blockLen(i32  blkIdx)
  {
    IDX nxt = nxtBlock(blkIdx);
    if(nxt < 0) return -nxt;

    return blockFreeSize();
  }
  void           doFree(i32  blkIdx)  const        // frees a list/chain of blocks
  {
    i32   cur  =             blkIdx;          // cur is the current block index
    i32   nxt  =   s_bls[cur].idx;            //*stPtr(cur);              // nxt is the next block index
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

  size_t     writeBlock(i32  blkIdx, void* bytes)      // don't need to increment readers since write should be done before the block is exposed to any other threads
  {
    i32   blkFree  =  blockFreeSize();
    ui8*        p  =  blockFreePtr(blkIdx);
    i32       nxt  =  nxtBlock(blkIdx);
    size_t cpyLen  =  nxt<0? -nxt : blkFree;           // if next is negative, then it will be the length of the bytes in that block
    memcpy(p, bytes, cpyLen);

    return cpyLen;

    //bool     fill = len < -1 || blkFree < len;
    //size_t cpyLen = fill? blkFree : len;
  }
  size_t      readBlock(i32  blkIdx, void* bytes) const
  {
    if(!incReaders(blkIdx)) return 0;
      i32   blkFree  =  blockFreeSize();
      ui8*        p  =  blockFreePtr(blkIdx);
      i32       nxt  =  nxtBlock(blkIdx);
      size_t cpyLen  =  nxt<0? -nxt : blkFree;           // if next is negative, then it will be the length of the bytes in that block
      memcpy(bytes, p, cpyLen);
    decReaders(blkIdx);

    return cpyLen;

    //if(decReaders(blkIdx)){
    //  doFree(blkIdx);
    //}
  }

public:
  //static ui64     BlksOfst(ui32 blockSize, ui32 blockCount)

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

  //auto         alloc(i32   size, i32* out_blocks=nullptr) -> VerIdx   // todo: doesn't this need to give back the blocks if allocation fails?
  //{
  //  i32 byteRem = 0;
  //  i32 blocks  = blocksNeeded(size, &byteRem);
  //  //if(out_blocks) *out_blocks = blocks;
  //
  //  i32   st = s_cl.nxt();                                     // stBlk  is starting block
  //  if(st==LIST_END){
  //    if(out_blocks) *out_blocks = 0; 
  //    return List_End(); // LIST_END; 
  //  }
  //
  //  ui32 ver = (ui32)s_version->fetch_add(1);
  //  i32  cur = st;                                             // curBlk is current  block
  //  i32  cnt =  0;
  //  i32  nxt =  0;   // m_cl.nxt();
  //  for(i32 i=0; i<blocks-1; ++i)
  //  {
  //    //i32* p = stPtr(cur);
  //    nxt    = s_cl.nxt();
  //    if(nxt==LIST_END){ 
  //      //if(out_blocks) *out_blocks = -cnt;                     // negative count if the full amount of blocks can't be allocated
  //      break;
  //    }
  //
  //    //m_bls[cur] = { 0, nxt };                  // { readers, index }
  //    //m_bls[cur].kr.isKey   = i==0 ? 1 : 0;
  //    //m_bls[cur].kr.readers = 0;
  //    //m_bls[cur].idx        = nxt;
  //    //m_bls[cur] = make_BlkLst(i==0, 0, nxt);
  //    s_bls[cur] =  make_BlkLst(0, 0, nxt);
  //    cur        =  nxt;                        // m_cl.nxt();
  //    ++cnt;
  //    m_blocksUsed.fetch_add(1);
  //    //*p      =  cur;
  //  }
  //  //i32* p  =  (i32*)stPtr(cur);
  //  //*p      =  byteRem? -byteRem : -blockFreeSize();
  //
  //  //BlkLst bl  = { 0, byteRem? -byteRem : -blockFreeSize() };
  //  BlkLst bl  = make_BlkLst(false, 0, byteRem? -byteRem : -blockFreeSize() );
  //  s_bls[cur] = bl;
  //
  //  //m_bls[cur].readers =  0;
  //  //m_bls[cur].idx     =  byteRem? -byteRem : -blockFreeSize();
  //  if(out_blocks){ *out_blocks = nxt==LIST_END? -cnt : cnt; } 
  //  
  //  s_bls[st].kr.isKey = true;
  //
  //  //vi.idx     = st;
  //  //vi.version = s_version->fetch_add(1);
  //
  //  VerIdx vi = { st, ver };
  //
  //  return vi;
  //}

  auto         alloc(i32   size, ui32 klen, i32* out_blocks=nullptr) -> VerIdx   // todo: doesn't this need to give back the blocks if allocation fails?
  {
    i32 byteRem = 0;
    i32 blocks  = blocksNeeded(size, &byteRem);
    //if(out_blocks) *out_blocks = blocks;

    i32   st = s_cl.nxt();                                     // stBlk  is starting block
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
      if(nxt==LIST_END) break;

      if(i==0)  s_bls[cur] =  make_BlkLst(true,  0, nxt, ver, size, klen);
      else      s_bls[cur] =  make_BlkLst(false, 0, nxt, ver, 0, 0);
      cur        =  nxt;
      ++cnt;
      m_blocksUsed.fetch_add(1);
    }

    BlkLst bl  = make_BlkLst(false, 0, byteRem? -byteRem : -blockFreeSize(), ver, size, klen);
    s_bls[cur] = bl;

    if(out_blocks){ *out_blocks = nxt==LIST_END? -cnt : cnt; }     

    s_bls[st].kr.isKey = true;

    VerIdx vi = { st, ver };

    return vi;
  }
  bool         free(i32  blkIdx)        // frees a list/chain of blocks
  {
    return decReaders(blkIdx);

    //if(decReaders(blkIdx)){
    //  doFree(blkIdx);
    //  return true;
    //}else return false;
  }
  void          put(i32  blkIdx, void* bytes, i32 len)
  {
    ui8*       b   = (ui8*)bytes;
    i32    blocks  =  blocksNeeded(len);
    i32       cur  =  blkIdx;
    for(i32 i=0; i<blocks; ++i)
    {
      b   +=  writeBlock(cur, b);
      cur  =  nxtBlock(cur);
    }

    //i32  remBytes  =  0;
    //i32    blocks  =  blocksNeeded(len, &remBytes);
  }
  size_t        get(i32  blkIdx, void* bytes)            const
  {
    if(blkIdx == LIST_END){ return 0; }

    size_t    len = 0;
    size_t  rdLen = 0;
    ui8*        b = (ui8*)bytes;
    i32       cur = blkIdx;
    i32       nxt;
    while(true)
    {
      nxt    =  nxtBlock(cur);
      rdLen  =  readBlock(cur, b);                         // rdLen is read length
      b     +=  rdLen;
      len   +=  rdLen;
      if(nxt<0 || nxt==LIST_END) break;

      cur    =  nxt;
    }


    return len;                                           // only one return after the top to make sure readers can be decremented - maybe it should be wrapped in a struct with a destructor
  }
  Match   memcmpBlk(i32  blkIdx, const void* buf1, const void* buf2, size_t size) const
  {
    if(!incReaders(blkIdx)) return MATCH_REMOVED;
      auto ret = memcmp(buf1, buf2, size);
    decReaders(blkIdx);

    if(ret==0) return MATCH_TRUE;

    return MATCH_FALSE;
  }
  Match     compare(void*   buf, size_t len, IDX blkIdx) const
  {
    IDX   curidx  =  blkIdx;
    i32      nxt  =  nxtBlock(curidx);
    auto   blksz  =  blockFreeSize();
    ui8*  curbuf  =  (ui8*)buf;
    auto  curlen  =  len;
    while(true)
    {
      auto p = blockFreePtr(curidx);
      if(nxt >= 0){
        if(curlen < blksz){ return MATCH_FALSE; }
        Match cmp = memcmpBlk(curidx, curbuf, p, blksz);
        if( cmp!=MATCH_TRUE ){ return cmp; }
      }else if(-nxt != curlen){ return MATCH_FALSE; }
      else{ return memcmpBlk(curidx, curbuf, p, curlen); }

      curbuf  +=  blksz;
      curlen  -=  blksz;
      curidx   =  nxt;
      nxt      =  nxtBlock(curidx);
    }

    // return MATCH_TRUE; // never reached
  }
  ui32          len(i32  blkIdx)
  {
    IDX  nxt;
    IDX  cur = blkIdx;
    ui32 ret = blockLen(cur);

    while( (nxt = nxtBlock(cur)) >= 0 ){        // todo: change this to LIST_END, initialize the BlkLst to LIST_END and use ui32 for indices
      ret += blockLen(nxt);
      cur  = nxt;
    }

    return ret;
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
  static const  i8   RM_OWNER         =    -1;            // keep this at 0 if INIT_READERS is changed to 1, then take out remove flag
  static const ui8   LAST_READER      =     0;            // keep this at 0 if INIT_READERS is changed to 1, then take out remove flag
  static const ui8   INIT_READERS     =     0;            // eventually make this 1 again? - to catch when readers has dropped to 0
  static const ui8   FREE_READY       =     0;
  static const ui8   MAX_READERS      =  0xFF;
  //static const ui64  EMPTY_KEY        =  0x000000000FFFFFFF;   // first 28 bits set 
  //static const ui64  EMPTY_KEY        =    0x0000000000200000;   // first 21 bits set 
  static const ui64  EMPTY_KEY        =  2097151;   // first 21 bits set 
  static const ui32  EMPTY_HASH_IDX   =  0xFFFFFFFF;           // 32 bits set - hash indices are different from block indices 

  //union      KV         // 256 million keys (28 bits), 256 million values (28 bit),  127 readers (signed 8 bits)
  //{
  //  struct
  //  {
  //    ui64     key : 28;
  //    ui64     val : 28;
  //    i64  readers :  8;
  //  };
  //  ui64 asInt;
  //};

  union      KV         // 256 million keys (28 bits), 256 million values (28 bit),  127 readers (signed 8 bits)
  {
    struct
    {
      ui64     key : 21;
      ui64     val : 21;
      ui64 version : 22;
    };
    ui64 asInt;
  };

  static ui32 nextPowerOf2(ui32  v)
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
  static ui64 sizeBytes(ui32 size)
  {
    return lava_vec<KV>::sizeBytes( nextPowerOf2(size) );
  }
  static bool DefaultKeyCompare(ui32 a, ui32 b)
  {
    return a == b;
  }
  static ui32 HashBytes(void* buf, ui32 len)
  {
    ui64 hsh = fnv_64a_buf(buf, len);

    return (ui32)( (hsh>>32) ^ ((ui32)hsh));
  }
  //static bool IsEmpty(KV kv){return kv.key==EMPTY_KEY || kv.readers<0;}
  static KV    empty_kv()
  {
    KV empty;
    empty.key      =  EMPTY_KEY;
    empty.val      =  EMPTY_KEY;
    empty.version  =  0;
    //empty.readers  =  0;
    return empty;
  }
  static bool   IsEmpty(KV kv)
  {
    static KV emptykv = empty_kv();
    return emptykv.asInt == kv.asInt;
  }


private:
  using i8        =  int8_t;
  using ui32      =  uint32_t;
  using ui64      =  uint64_t;
  using Aui32     =  std::atomic<ui32>;
  using Aui64     =  std::atomic<ui64>;  
  using KVs       =  lava_vec<KV>;
  using Mut       =  std::mutex;
  using UnqLock   =  std::unique_lock<Mut>;

         ui32    m_sz;
  mutable KVs   m_kvs;

  KV            load_kv(ui32 i)            const
  {
    using namespace std;
    
    KV keyval;
    keyval.asInt   =  atomic_load<ui64>( (Aui64*)(&(m_kvs.data()[i].asInt)) );              // Load the key that was there.
    return keyval;
  }
  KV           store_kv(ui32 i, KV keyval) const
  {
    using namespace std;
    
    //atomic_store<ui64>( (Aui64*)&m_kvs[i].asInt, _kv.asInt );

    KV ret;
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
  void           doFree(ui32 i)            const
  {
    store_kv(i, empty_kv());
  }
  //KV         incReaders(ui32 i)            const
  //{
  //  KV empty = empty_kv();
  //  KV cur, nxt;
  //  aui64* aidx = (aui64*)&(m_kvs[i].asInt);
  //  cur.asInt   = aidx->load();
  //
  //  if(cur.asInt==empty.asInt) return empty;
  //
  //  do{
  //    if(cur.readers<0) return cur;                           // if readers is already below 0, this index is done and won't come back 
  //    nxt = cur;
  //    nxt.readers += 1;
  //  }while( !aidx->compare_exchange_strong(cur.asInt, nxt.asInt) );
  //  
  //  return nxt;
  //}
  //bool       decReaders(ui32 i)            const
  //{
  //  KV cur, nxt;
  //  aui64* aidx = (aui64*)&(m_kvs[i].asInt);
  //  cur.asInt   = aidx->load();
  //  do{
  //    nxt           =  cur;
  //    nxt.readers  -=    1;
  //  }while( !aidx->compare_exchange_strong(cur.asInt, nxt.asInt) );
  //  
  //  if(cur.readers==0){ doFree(i); return false; }
  //
  //  return true;
  //}

  template<class MATCH_FUNC> 
  auto       checkMatch(ui32 i, ui32 key, MATCH_FUNC match) const -> Match //  decltype(match(empty_kv()))
  {
    //incReaders(i);  // todo: have incReaders return a KV?
      Match ret = match(key);
    //decReaders(i);
    
    return ret;
  }

  template<class MATCH_FUNC, class FUNC> 
  bool       runIfMatch(ui32 i, ui32 key, MATCH_FUNC match, FUNC f) const // const -> bool
  {
    //KV kv = incReaders(i);    
      Match      m = match(key);
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
      KV defKv = empty_kv();
      for(ui64 i=0; i<m_kvs.size(); ++i)
        m_kvs[i] = defKv;
    }
  }
  ConcurrentHash(ConcurrentHash const& lval) = delete;
  ConcurrentHash(ConcurrentHash&&      rval) = delete;
  
  ConcurrentHash& operator=(ConcurrentHash const& lval) = delete;
  ConcurrentHash& operator=(ConcurrentHash&&      rval) = delete;

  KV operator[](ui32 idx) const
  {
    return m_kvs[idx];
  }

  template<class MATCH_FUNC> 
  KV       putHashed(ui32 hash, ui32 key, ui32 val, MATCH_FUNC match) const
  {
    using namespace std;
  
    KV desired   =  empty_kv();
    desired.key  =  key;
    desired.val  =  val;
    ui32      i  =  hash;
    for(;; ++i)
    {
      i  &=  m_sz-1;
      KV probedKv = load_kv(i);
      if(probedKv.key==EMPTY_KEY)
      {          
        KV   expected  =  empty_kv();
        bool  success  =  compexchange_kv(i, &expected.asInt, desired.asInt);
        //if( !success && (expected.key!=key) ) continue;                                     // WRONG!? // Another thread just stole it from underneath us.
        //else                                  return expected;
        if( success /* && (expected.key!=key) */ ) return expected;  // continue;                                       // WRONG!? // Another thread just stole it from underneath us.
        else{ --i; continue; }
      }                                                                                       // Either we just added the key, or another thread did.
      
      // if( match(probedKv.key)==MATCH_TRUE ){
      if( checkMatch(i, probedKv.key, match)==MATCH_TRUE ){
        return store_kv(i, desired);
      }
    }

    return empty_kv();  // should never be reached
  }
  template<class MATCH_FUNC> 
  ui32     getHashed(ui32 hash, MATCH_FUNC match) const
  {
    ui32 i = hash;
    for(;; ++i)
    {
      i &= m_sz - 1;
      KV probedKv = load_kv(i);
      if(probedKv.key==EMPTY_KEY) return EMPTY_KEY;
      if( checkMatch(i, probedKv.key, match)==MATCH_TRUE ) return probedKv.val;
      //if( match(probedKv.key) )   return probedKv.val;
    }
  }
  template<class MATCH_FUNC> 
  ui32    findHashed(ui32 hash, MATCH_FUNC match) const
  {
    ui32 i = hash;
    for(;; ++i)
    {
      i &= m_sz - 1;
      KV probedKv = load_kv(i);
      if( probedKv.key==EMPTY_KEY )            return EMPTY_HASH_IDX;          // todo: this conflates and assumes that EMPTY_KEY is both the ConcurrentStore block index EMPTY_KEY and the ConcurrentHash EMPTY_KEY
      if( checkMatch(i, probedKv.key, match) ) return i;
    }
  }
  template<class MATCH_FUNC> 
  KV        rmHashed(ui32 hash, MATCH_FUNC match) const // -> Reader
  //bool      rmHashed(ui32 hash, MATCH_FUNC match) const // -> Reader
  {  
    ui32 i = hash;
    for(;; ++i)
    {
      i  &=  m_sz - 1;
      KV probedKv = load_kv(i);
      if(probedKv.key==EMPTY_KEY) return empty_kv(); //{ return false; } //  // probedKv; // MATCH_FALSE; // empty_reader();
      //Match m = match(probedKv.key);
      Match m = checkMatch(i, probedKv.key, match);
      if(m==MATCH_TRUE){
        return probedKv;
        
        //if( decReaders(i) == false ) return probedKv; // return true;

        //auto empty = empty_kv();
        //if( compexchange_kv(i, &probedKv.asInt, empty.asInt) ) return probedKv;
        //else --i;                                     // run the same loop iteration again if there was a match, but the key-value changed
      }
      if(m==MATCH_REMOVED) return empty_kv(); // { return false; } // return empty_kv();
    }

    return empty_kv(); // false;    
  }
  template<class MATCH_FUNC> 
  KV      readHashed(ui32 hash, MATCH_FUNC match) const // -> Reader
  {
    using namespace std;

    ui32 i = hash;
    for(;; ++i)
    {
      i &= m_sz - 1;
      KV probedKv = load_kv(i);
      if(probedKv.key==EMPTY_KEY) return empty_kv();
      //if( match(probedKv.key)==MATCH_TRUE ) return probedKv;
      if( checkMatch(i, probedKv.key, match)==MATCH_TRUE ) return probedKv;

      //if(probedKv.key==EMPTY_KEY) return empty_reader();         // empty_kv();
      //if( match(probedKv.key)==MATCH_TRUE ){
      //  Reader r;
      //  r.kv        =  probedKv; // addReaders(i, probedKv, 1);
      //  r.hash_idx  =  i;
      //  r.doEnd     =  true;
      //  r.ch        =  this;
      //  return move(r); // addReaders(i, probedKv, 1);
      //}

    }

    return empty_kv();
  }

  template<class MATCH_FUNC, class FUNC> 
  bool      runMatch(ui32 hash, MATCH_FUNC match, FUNC f) const // -> decltype( f(KV()) )
  {
    ui32 i = hash;
    for(;; ++i)
    {
      i &= m_sz - 1;
      KV probedKv = load_kv(i);
      if( probedKv.key==EMPTY_KEY )               return false;     // todo: this conflates and assumes that EMPTY_KEY is both the ConcurrentStore block index EMPTY_KEY and the ConcurrentHash EMPTY_KEY
      if( runIfMatch(i, probedKv.key, match, f) ) return  true;
    }
  }
  
  template<class FUNC> 
  auto       runRead(ui32  idx, FUNC f)           const -> decltype( f(KV()) )    // decltype( (f(empty_kv())) )
  {
    //KV kv = incReaders(idx);
      auto ret = f( load_kv(idx) );
    //decReaders(idx);

    return ret;
  }

  bool          init(ui32   sz)
  {
    using namespace std;
    
    m_sz      =  nextPowerOf2(sz);
    m_kvs     =  lava_vec<KV>(m_sz);
    KV defKv  =  empty_kv();
    for(ui64 i=0; i<m_kvs.size(); ++i) m_kvs[i] = defKv;
    
    return true;

    //m_kvs.resize(m_sz, defKv);
  }
  KV              at(ui32  idx)                   const
  {
    //return m_kvs[idx];
    return load_kv(idx);
  }
  ui32           nxt(ui32  idx)                   const
  {
    KV empty = empty_kv();
    while(true){
      KV kv = load_kv(idx);
      if( kv.key!=empty.key /* && kv.readers>=0 */ ) break;
      idx = (idx+1) % m_sz;                                             // don't increment idx above since break comes before it here
    }
    
    return idx;
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
  void*      fileHndl;
  void*       hndlPtr;
  void*           ptr;
  ui64           size;
  bool          owner;

public:
  static void        FreeAnon(SharedMem& sm)
  {
    if(sm.hndlPtr)  UnmapViewOfFile(sm.hndlPtr);
    if(sm.fileHndl) CloseHandle(sm.fileHndl);
    sm.clear();
  }
  static SharedMem  AllocAnon(const char* name, ui64 size)
  {
    using namespace std;
    
    #ifdef _WIN32      // windows
      char path[512] = "Global\\simdb_15_";
      strcat_s(path, sizeof(path), name);

      SharedMem sm;
      sm.owner = false;
      sm.size  = size;

      sm.fileHndl = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, path);
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
        FILE_MAP_ALL_ACCESS,   // read/write permission
        0,
        0,
        0);
      if(sm.hndlPtr==nullptr){ CloseHandle(sm.fileHndl); sm.clear(); return move(sm); }
    #endif       // END windows
  
    ui64      addr = (ui64)(sm.hndlPtr);
    ui64 alignAddr = (addr + ((64-addr%64)%64));
    sm.ptr    = (void*)(alignAddr);

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
    fileHndl  =  nullptr;
    hndlPtr   =  nullptr;
    ptr       =  nullptr;
    size      =  0;
    owner     =  false;
  }
  auto data() -> void*
  {
    return ptr;
  }
};
class            simdb
{
public:
  using KV     = ConcurrentHash::KV;
  using VerIdx = ConcurrentStore::VerIdx;

private:
  SharedMem          m_mem;

  aui64*           s_flags;
  aui64*       s_blockSize;
  aui64*      s_blockCount;
  ConcurrentStore     s_cs;     // store data in blocks and get back indices
  ConcurrentHash      s_ch;     // store the indices of keys and values - contains a ConcurrentList

  // these variables are local to the stack where simdb lives, unlike the others, they are not simply a pointer into the shared memory
  mutable ui32   m_nxtChIdx;      
  mutable ui32   m_curChIdx;     
  ui64             m_blkCnt;
  ui64              m_blkSz;


  static const ui32  EMPTY_KEY = ConcurrentHash::EMPTY_KEY;          // 28 bits set 
  static const ui32   LIST_END = ConcurrentStore::LIST_END;
  static aui64*  SpinWhileFalse(aui64* ptr)
  {
  }
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
  static Match     CompareBlock(simdb const* const ths, void* buf, size_t len, i32 blkIdx)
  { 
    return ths->s_cs.compare(buf, len, blkIdx);
  }
  static bool           IsEmpty(KV kv){return ConcurrentHash::IsEmpty(kv);}           // special value for ConcurrentHash
  static bool         IsListEnd(VerIdx vi){return ConcurrentStore::IsListEnd(vi); }   // special value for ConcurrentStore

  // todo: obsoleted by m_ch.runRead() ?
  auto  getFromBlkIdx(i32  blkIdx, void*  out_buf) const -> ui64        // getFromBlkIdx is get from block index
  {
    if(blkIdx==EMPTY_KEY) return 0;

    return s_cs.get(blkIdx, out_buf);           // copy into the buf starting at the blkidx
  } 
  KV             read(void*   key, i32        len) const
  {
    using namespace std;
    
    //ui32 keyhash  =  m_ch.hashBytes(key, len);
    ui32 keyhash  =  ConcurrentHash::HashBytes(key, len);
    auto     ths  =  this;
    return s_ch.readHashed(keyhash, 
      [ths, key, len](ui32 blkidx){ return CompareBlock(ths,key,len,blkidx); });
  }

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

    if(isOwner()) s_flags->store(1);                                        // set to 1 to signal construction is done
  }

  i32       put(void*       key, ui32  klen, void* val, ui32 vlen)
  {
    i32 blkcnt = 0;
    //i32   kidx = s_cs.alloc(klen, &blkcnt).idx;    // todo: use the VersionIdx struct // kidx is key index
    
    //auto  kidx = s_cs.alloc(klen, &blkcnt);    // todo: use the VersionIdx struct // kidx is key index
    //if(kidx.idx==LIST_END) return EMPTY_KEY;
    //if(blkcnt<0){
    //  s_cs.free(kidx.idx);
    //  return EMPTY_KEY;
    //}
    //auto  vidx = s_cs.alloc(vlen, &blkcnt);    // if there aren't enough blocks, the blkCnt wil be the amount of blocks that got allocated anyway * -1.  
    //if(vidx.idx==LIST_END) return EMPTY_KEY;       // vidx is value index
    //if(blkcnt<0){
    //  s_cs.free(vidx.idx);
    //  return EMPTY_KEY;
    //}
    //s_cs.put(kidx.idx, key, klen);
    //s_cs.put(vidx.idx, val, vlen);
    
    auto  kidx = s_cs.alloc(klen+vlen, &blkcnt);    // todo: use the VersionIdx struct // kidx is key index
    if(kidx.idx==LIST_END) return EMPTY_KEY;
    if(blkcnt<0){
      s_cs.free(kidx.idx);
      return EMPTY_KEY;
    }    
    s_cs.put(kidx.idx, key, klen, val, vlen);

    //ui32 keyhash = m_ch.hashBytes(key, klen);
    ui32 keyhash = ConcurrentHash::HashBytes(key, klen);
    auto     ths = this;                                                            // this silly song and dance is because the this pointer can't be passed to a lambda
    KV        kv = s_ch.putHashed(keyhash, kidx.idx, vidx.idx,                      // this returns the previous KV at the position
      [ths, key, klen](ui32 blkidx){ return CompareBlock(ths,key,klen,blkidx); });

    //if(kv.val!=EMPTY_KEY) s_cs.free(kv.val);
    //if(kv.key!=EMPTY_KEY) s_cs.free(kv.key);

    if(kv.key!=EMPTY_KEY) s_cs.free(kv.key);

    //if(kv.key != ConcurrentHash::EMPTY_KEY){ m_cs.free(kv.val); m_cs.free(kv.key); }

    return kidx.idx;
  }
  i64       get(const char* key, void* out_buf)
  {
    //Reader r = read( (void*)key, (ui32)strlen(key));
    //if(r.kv.key==EMPTY_KEY || r.kv.readers<=0){ return -1; }

    KV kv = read( (void*)key, (ui32)strlen(key));
    if(kv.key==EMPTY_KEY){ return -1; }

    ui64 len = getFromBlkIdx(kv.val, out_buf);

    //if(r.doRm()){ m_cs.free(r.kv.val); m_cs.free(r.kv.key); }

    return len;
  }
  i64    getKey(ui32 idx, void* out_buf, ui32 klen)
  {
    if(klen<1) return 0;
    
    auto     ths = this;
    auto runFunc = [ths, out_buf](KV kv){
      return IsEmpty(kv)?  0ull  :  ths->s_cs.get(kv.key, out_buf);
    };
    auto  retLen = s_ch.runRead(idx, runFunc);

    return retLen;
  }
  i64       len(void* key, ui32 klen)
  {
    if(klen<1) return 0;

    auto       ths = this;
    void*   keyptr = key;
    auto       hsh = ConcurrentHash::HashBytes( keyptr, klen );
    
    auto   runFunc = [ths](KV kv)
    { return IsEmpty(kv)?  0ull  :  ths->s_cs.len(kv.key); };

    auto matchFunc = [ths, key, klen](ui32 blkidx){
      return CompareBlock(ths, key, klen, blkidx);
    };
    
    auto    retlen = s_ch.runMatch(hsh,  matchFunc, runFunc);
  
    return retlen;
  }
  ui32      len(ui32 idx)
  {
    KV kv = s_ch.at(idx);
    
    //if(kv.key==EMPTY_KEY || kv.readers<0) return 0;
    if( IsEmpty(kv) ) return 0;

    auto    ths = this;
    auto retLen = s_ch.runRead(idx, [ths](KV kv){
      return IsEmpty(kv)?  0  :  ths->s_cs.len(kv.key);
    });

    return retLen;

    //auto blkLen = m_cs.len(kv.key);
    //return blkLen;
  }
  ui32      nxt() const
  {
    KV   empty = s_ch.empty_kv();
    ui32 chNxt; // = empty.key;
    KV     kv;
    do{
           chNxt = s_ch.nxt(m_nxtChIdx);                 // can return the same index - it does not do the iteration after finding a non empty key
              kv = s_ch.at(chNxt);
      m_nxtChIdx = (chNxt + 1) % m_blkCnt;
    }while( IsEmpty(kv) );

    //}while( kv.key==empty.key || kv.readers<0 );

    m_curChIdx = chNxt;
    return chNxt;
  }
  ui32      cur() const
  {
    return m_curChIdx;
  }
  auto     data() const -> const void*
  {
    return s_cs.data();
  }
  ui64     size() const
  {
    //return m_mem.size;
    return s_cs.sizeBytes( (ui32)s_blockSize->load(), (ui32)s_blockCount->load());
  }
  bool  isOwner() const
  {
    return m_mem.owner;
  }
  ui64   blocks() const
  {
    return s_blockCount->load();
  }

  // separated C++ functions - these won't need to exist if compiled for a C interface
  template<class T>
  i64          get(std::vector<T> const& key, void* out_buf)
  {
    Reader r = read((void*)key.data(), (ui32)(key.size() * sizeof(T)));
    if(isEmpty(r.kv)) return -1;
    //if(r.kv.key == EMPTY_KEY || r.kv.readers <= 0){ return -1; }

    ui64 len = getFromBlkIdx(r.kv.val, out_buf);
    if(r.doRm()){ m_cs.free(r.kv.val); m_cs.free(r.kv.key); }

    return len;
  }
  auto   getKeyStr(ui32 idx) -> std::string
  {    
    using namespace std;

    string s;
    i64 retlen = 0;
    i64 strlen = 0;
    auto   ths = this;
    do{
      strlen = len(idx);
      if(strlen<0) return string();
      s = string(strlen, 0);

      auto runFunc = [ths, &s](KV kv){
        return IsEmpty(kv)?  0ull  :  ths->s_cs.get(kv.key, (void*)s.data() /* add length here */);
      };

      retlen = s_ch.runRead(idx, runFunc);
    }while(retlen != strlen);

    return s;
  }
  bool          rm(std::string const& key)
  {
    auto  len = (ui32)key.length();
    auto kbuf = (void*)key.data();
    auto hash = ConcurrentHash::HashBytes(kbuf, len);
    auto  ths = this;
    KV kv = s_ch.rmHashed(hash,
      [ths, kbuf, len](ui32 blkidx){ return CompareBlock(ths,kbuf,len,blkidx); });

    if(kv.val!=EMPTY_KEY) s_cs.free(kv.val);
    if(kv.key!=EMPTY_KEY) s_cs.free(kv.key);

    return kv.key!=EMPTY_KEY; // removed; // kv.key!=EMPTY_KEY;
  }
  // end separated C++ functions

};

#endif






//             read(void*   key, i32        len) const
//{
//  using namespace std;
//  
//  //ui32 keyhash  =  m_ch.hashBytes(key, len);
//  ui32 keyhash  =  ConcurrentHash::HashBytes(key, len);
//  auto     ths  =  this;
//  return m_ch.readHashed(keyhash, 
//    [ths, key, len](ui32 blkidx){ return CompareBlock(ths,key,len,blkidx); });
//}

//ui32   keyLen(std::string const& key)
//{
  //KV kv = read( (void*)key.data(), (ui32)key.length() );
  //
  //if(kv.key==EMPTY_KEY) return 0;
  //else                  return m_cs.len(kv.val);
//}

//auto   getStr(std::string    const& key) -> std::string
//{
//  using namespace std;
//  
//  auto      ths = this;
//  void*  keyptr = (void*)key.c_str();
//  auto   keylen = key.length();
//  i64    retlen = 0;
//  auto      hsh = ConcurrentHash::HashBytes( keyptr, (ui32)keylen );
//  do
//  {
//    string s();
//    auto   valbuf = s.data();
//    auto   vallen = s.
//    auto  runFunc = [ths, out_buf](KV kv)
//    { return IsEmpty(kv)?  0ull  :  ths->m_cs.get(kv.key, out_buf); };
//
//    auto matchFunc = [ths, keyptr, keylen](ui32 blkidx)
//    { return CompareBlock(ths,keyptr,keylen,blkidx); };
//  
//    auto    retlen = m_ch.runMatch(hsh,  matchFunc, runFunc);
//  }while(retlen != keylen);
//
//  //auto klen = keylen(key);  
//
//  return retlen;
//}

//auto   getStr(std::string    const& key) -> std::string
//{
//  using namespace std;
//  
//  auto      ths = this;
//  void*  keyptr = (void*)key.c_str();
//  auto   keylen = key.length();
//  i64    retlen = 0;
//  auto      hsh = ConcurrentHash::HashBytes( keyptr, (ui32)keylen );
//  do
//  {
//    
//    string s();
//    auto   valbuf = s.data();
//    auto   vallen = s.
//    auto  runFunc = [ths, out_buf](KV kv)
//    { return IsEmpty(kv)?  0ull  :  ths->m_cs.get(kv.key, out_buf); };
//
//    auto matchFunc = [ths, keyptr, keylen](ui32 blkidx)
//    { return CompareBlock(ths,keyptr,keylen,blkidx); };
//  
//    auto    retlen = m_ch.runMatch(hsh,  matchFunc, runFunc);
//  }while(retlen != keylen);
//
//  //auto klen = keylen(key);  
//
//  return retlen;
//}

//if(klen<1) return 0;
//
//auto     ths = this;
//auto runFunc = [ths, key](KV kv){
//  return IsEmpty(kv)?  0ull  :  ths->m_cs.len(kv.key);
//};
//
//auto  retLen = m_ch.runMatch(idx, runFunc);
//
//return retLen;
//
//auto    keylen = key.length();

//Reader r = read( (void*)key.data(), (ui32)key.length() );
//if(r.kv.key==EMPTY_KEY || r.kv.readers<=0) return -1;                // after the read, the readers should be at least 1  /*|| r.kv.remove*/
//
//if(kv.val!=EMPTY_KEY) m_cs.free(kv.val);
//if(kv.key!=EMPTY_KEY) m_cs.free(kv.key);
//
//if(r.doRm()){ m_cs.free(kv.val); m_cs.free(kv.key); }
//
//KV       kv = read( (void*)key.data(), (ui32)key.length() );
//if(kv.key==EMPTY_KEY) return -1;                                      // || r.kv.readers<=0) return -1;     // after the read, the readers should be at least 1  /*|| r.kv.remove*/
//ui64    len = getFromBlkIdx(kv.val, out_buf);

//if(kv.key==EMPTY_KEY || kv.readers<0) return 0ull;
//if(isEmpty(kv)) return 0ull;

//static const ui32  EMPTY_KEY     =  0xFFFFFFFF;          // full 32 bits set 
//static const ui64  EMPTY_KEY     =  0x00000000FFFFFFFF;    // test
//static const ui64  EMPTY_KV      =  0xFFFFFFFFFFFFFFFF;    // full 64 bits set 
//static const ui64  EMPTY_KV      =  ConcurrentHash::empty_kv();

//bool       incReaders(ui32 i)            const
//{
//  KV cur, nxt;
//  aui64* aidx = (aui64*)&(m_kvs[i].asInt);
//  cur.asInt   = aidx->load();
//  do{
//    if(cur.readers<0) return false;
//    nxt = cur;
//    nxt.readers += 1;
//  }while( !aidx->compare_exchange_strong(cur.asInt, nxt.asInt) );
//  
//  return true;
//}

//bool removed = m_ch.rmHashed(hash,
//
//if(removed) m_cs.free(kv.val);
//if(removed) m_cs.free(kv.key);


