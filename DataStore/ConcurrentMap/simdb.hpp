
// -todo: make CncrLst::idx() an atomic load
// -todo: figure out why neither version of CncrStr::free() is being hit - wasn't calling del()....
// -todo: figure out 128 bit alignement of CncrHsh's VerIdx memory - padded sizeBytes() by 16 and offset the address in which the lava_vec is created with to land on the 128 bit boundary
// -todo: debug clean deletions leaving slots in a DELETED_KEY state - one side of the 128 bit alignment not being swapped - now duplicate DELETED_KEY entries are being created without being cleaned up - also deleting an adjacent EMPTY_KEY? - had to switch around bit swapping functions, lo and hi now only describe the low and high addresses
// -todo: figure out how to set the indices into a list in CncrLst so that free() can use the start and end indices to free multiple blocks
// -todo: make a CncrStr function to free a series of blocks with a begin and end, returning failure or success
// -todo: make bulk free by setting all list blocks first, then freeing the head of the list - does only the head of the list need to be freed anyway since the rest of the list is already linked together? could this reduce contention over the block atomic?
// -todo: Make frees happen from the last block to the first so that allocation might happen with contiguous blocks
// -todo: does a BlkLst need to be loaded atomically by a read operation? is it possible that a read could be out of date and use an incorrect cached version? - a thread will eventually atomically decrement the readers after reading all the blocks so it should be fine 
// -todo: try to take out checkMatch from CncrHsh - checkMatch is unused
// -todo: debug some (all?) CncrHsh indices not being deleted - only the last index? - cmpex_vi was being used with a pointer into s_vis on some functions and a pointer to a swapped vi on the stack in other functions
// -todo: redo simdb::len(blkIdx) to fit the functions as they stand now - klen and vlen were labeled out_vlen and out_version 
// -todo: take out runRead - used by direct simdb::len(blkIdx) function - also used by getKey - was previously used to wrap incReaders() and decReaders() calls around a function 
// -todo: flatten runIfMatch function to only take a function template argument but not a match function template argument - take it out all together
// -todo: figure out what to do about indices on the ends in CncrHsh - just leave a DELETED_KEY and don't turn it into an EMPTY_KEY, since it will then just be skipped over when looking for an index - make sure that cleanDeletion() and delDupe() skip the last index when they are the primary/left/lo index
// -todo: make sure that the start and end are taken care of with regards to cleaning up deletions - need to not go off the end of the array and need to figure out how to deal with spans between them
// -todo: stop using match function as a template in and just run a function in CncrHsh
//       | len()
//       | get()
//       | put()
// -todo: find any remnants of KeyVal or kv and change them to VerIdx or vi
// -todo: redo basic type definitions and put them only into class definitions
// -todo: debug larger key not being found - length not being stored in every BlkLst

// todo: test with larger keys and values that span multiple blocks
// todo: make sure readers is only used on the key block list
// todo: make sure readers deletes the block list if it is the last reader after deletion
// todo: reference count initializations so that the last process out can destroy the db
// todo: make a function to use a temp directory that can be called on linux and osx - use tmpnam/tmpfile/tmpfile from stdio.h ?
// todo: put files in /tmp/var/simdb/ ? have to work out consistent permissions and paths
// todo: re-evaluate if the high bits of the lava vec pointer still need to contain extra information - one bit is being used for ownership - it can probably be made into a flat only container and ownership can just be a matter of who runs the constructor
// todo: test with visualizer
// todo: re-evaluate strong vs weak ordering
// todo: make sure that the important atomic variables like BlockLst next are aligned? need to be aligned on cache line false sharing boundaries and not just 64 bit boundaries?
// todo: search for any embedded todo comments
// todo: clean out old commented lines
// todo: compile with maximum warnings
// todo: run existing tests

// robin hood hashing
// todo: do rm()/del() first and make deletion take care of holes in spans?
// todo: make put() insert at the first available slot then swap with one index lower if it has a IPD that is at least 2 less
//       | after swapping, set the block list's version to the new version of the index? - what if the found version is not what the old version was? leave it? 
// todo: align ConcurrentHash's lava vec on 128 bit boundary - need to make an extra 128bit value and 
// todo: -organize ConcurrentHash entry to have the index on the left side, version on the right side. 
//       Put hash in the middle and use the first two bits of the index as empty and deleted flags
//       empty : 1, deleted : 1, index : 35, hash : 35, version : 56 - total 128 bits, 34 billion entry limit 
// todo: test alignment
// todo: figure out deletion - look back at tbl with EMPTY and NONE types 
//       | even without robin hood hashing are 128 bit atomics needed to swap/bubble sort keys into the new free slot?
//       | when deleting, instead of swapping the deleted key, duplicate the next key to the previous key and so on until reaching either an empty slot, a key that is further from its optimal distance, or two of the same key in a row (indicating another thread is moving keys around)

// todo: make a 'waiting' flag or type for keys so that they can be rewritten and resized in place? - would mean that they could not be read from at any time like they can be now
// todo: make alloc look for multiple blocks then check the next block variable for its version and if the version has not changed, allocate all blocks at once?
// todo: make a resize/realloc function to change the size of a block list instead of destroying and creating all indices when updating a key? - would need a different putWeak, since the writing of the index needs  to be atomic and re-writing the currently used blocks would not work with concurrency
// todo: integrate realloc into put() function - will need to query to see if the key exists, delete it, then insert the reallocated version? - if done like this, does it disrupt for a moment a key existing, and if so, is this an insurmountable problem?

// q: do the Index numbers in VerIdx need a flag saying that they haven't been changed? - what if before a thread switches two indices, other threads delete both VerIdxs and create new VerIdxs with different ideal position distances, but the exact same Indexes? can storing the Index as an xor of the Version and the Index solve this or will there still be non unique combinations? Ths would also mean that the Index would not line up immediatly after the swap. Do the upper bits of a 64 bit version counter need to be used? If even/odd versions are used, does that alleviate the problem? Is there a way to do XOR swapping and be able to see the XOR in progress?
// q: what operations need ConcurrentHash Index values to have versions with them? 
//       | get() does not, since it can check the key value to verify the block list is what it wants and it can check the version number on the block list to make sure the blocks did not change while reading
//       | put() does, since it needs to know that the Index value it is going to change still points to the same data
//       | del() does, since it needs to know that the Index value it is going to change still points to the same data
// q: what if two version numbers are put into each block list starting entry? verA, verB, hash, and index - 128 bits
// q: if Index values are swapped atomically but their version numbers can not be known to be the same, does this require more flexibility on the part of the linear search? only stop when hitting IPD (ideal position distances) less than the IPD of the searched item more than once? 
// q: the 64 bit Index swap needs to check that both IPD are still the same after the swap?
// q: is it possible to know to go backwards when finding an element if the same index is seen with a different version on an adjacent VerIdx ?

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
   |  |  alloc() gets the index of the next block from the BlockStore.
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

 Robin Hood Hashing:
 |  The ConcurrentHash indices are organized so that when searching for a key by linear search, no encountered key has an ideal position distance lower than the key being searched for
 |  To do this concurrently, instead of looking for an empty or deleted slot in the ConcurrentHash array and simply inserting there (as with linear search), the VerIdx is put into a free slot and continuously swapped with its VerIdx to the left if the distance to its ideal position is larger.
 |  The concurrent swapping of two 64 bit structures is complicated by the fact that while 128 bit atomic compare and exchange instructions are present on most modern CPUs, they are only able to operate on 128 bit aligned memory. 
 |  While half the 128 bit pairs of two 64 bit structures could be swapped with this instruction, the other half can not.
 |  On x64 64 bit atomic compare and exchanges CAN be unaligned, meaning that if Index values are next to each other in memory, they can be compare-exchanged atomically or read atomically
 |  There are multiple insights and techniques working together that allow the concurrent swapping to happen:
 |  |  The Index of the VerIdx struct stored in the ConcurrentHash array is always unique, since it is the index into the block list.
 |  |  The Version of the VerIdx struct stored in the ConcurrentHash array is always unique if the BlockList's atomic version counter has not wrapped around after overflow
 |  |  Because both the Version AND the Index are unique, the same Version + Index combination cannot occur twice over the lifetime of the database unless the version has overflowed and wrapped around (and the same index into the BlockList ends up with the same Version)
 |  |  The 64 bit Version+Index structure (Version is 32 bits and Index is 32 bits) is flipped on even indices in the ConcurrentHash array. The memory is ordered as  |Idx Ver Ver Idx|Idx Ver Ver Idx| with '|' here representing 128 bit alignment boundaries
 |  |  The Index values, next to each other in an unaligned 64 bit configuration can be swapped atomically, though their indices can not be 
 |  del():
 |  |  Finds the matching key
 |  |  Makes sure the VerIdx matches in ConcurrentHash and atomically comapare exchanges it for a VerIdx with a DELETED_KEY index value
 |  |  Calls cleanupDeleted(i) with the current ConcurrentHash slot
 |  |  |  Checks the next slot to see if there is a VerIdx in it, and if so, check to see what its ideal position distance is
 |  |  |  If the ideal position distance is more than 0, it can be moved backwards, so duplicate it into the current slot with an atomic compare-exchange to make sure it is deleted
 |  |  |  Check the indices of the current and next slots to see if the indices are still identical and atomically compare-exchange the right index value to DELETED_KEY if they are
 |  |  |  The possibility exists that after duplication but before deletion of the duplicate, put() could swap the duplicate to the right (higher slot). Because of this, put will need to delete any indices that it finds that link to non-key block lists
 |  |  |  put() will also need to check for duplicate indices, since if they are bubble sorting a robin hood span, they could separate duplicate indices from being next to each other, but also sort them back together
 |  |  |  should get() also check for duplicates? - probably, since it would have tighter cleanup of the ConcurrentHash only at the expense of one or two extra atomic loads per read

 Other notables:
 | All of the main classes have a static sizeBytes() function that takes in the same arguments as a constructor and return the number of bytes that it will need in the shared memory

*/

#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __SIMDB_HEADER_GUARD__
#define __SIMDB_HEADER_GUARD__

#if !defined(SECTION)
  #define       SECTION(_msvc_only_collapses_macros_with_arguments, ...)
#endif

// platform specific includes - mostly for shared memory mapping and auxillary functions like open, close and the windows equivilents
#if defined(_WIN32)      // windows
  // use _malloca ? - would need to use _freea and also know that _malloca always allocates on the heap in debug mode for some crazy reason
  #define STACK_VEC(TYPE, COUNT) lava_vec<TYPE>(_alloca(lava_vec<TYPE>::sizeBytes(COUNT)), COUNT, true);
  
  #include <tchar.h>
  #define NOMINMAX
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #include <strsafe.h>

  #ifdef _MSC_VER
    #if !defined(_CRT_SECURE_NO_WARNINGS)
      #define _CRT_SECURE_NO_WARNINGS
    #endif

    #if !defined(_SCL_SECURE_NO_WARNINGS)
      #define _SCL_SECURE_NO_WARNINGS
    #endif
  #endif


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

// platform specific type definitions
#ifdef _WIN32                         // these have to be outside the anonymous namespace
  typedef void        *HANDLE;
  typedef HANDLE     *PHANDLE;
  typedef wchar_t       WCHAR;        // wc,   16-bit UNICODE character
  typedef UCHAR       BOOLEAN;        // winnt
  typedef unsigned long ULONG;
#endif

namespace {
  using    u8   =   uint8_t;
  using   u32   =   uint32_t;
  using   u64   =   uint64_t;
  using    i8   =   int8_t;
  using   i32   =   int32_t;
  using   i64   =   int64_t;
  using  au64   =   std::atomic<u64>;
  using  au32   =   std::atomic<u32>;


  enum Match { MATCH_FALSE=0, MATCH_TRUE=1, MATCH_REMOVED=-1  };

  struct _u128
  {
   volatile u64 lo; volatile u64 hi; 
    //_u128& operator=(_u128 l){ hi = l.hi; lo = l.lo; return *this; };
  };

  template<class T>
  class lava_noop
  {
    void operator()(){}
  };

  template<class T, class Deleter=std::default_delete<T>, class Allocator=std::allocator<T> >
  class lava_vec
  {
  private:
    void* p;

    void       set_size(u64  s)
    { 
      *((u64*)clearBits(p) + 1) = s;
    }
    void  set_sizeBytes(u64 sb)
    {
      *((u64*)clearBits(p)) = sb;      // first 8 bytes should be the total size of the buffer in bytes
    } 

  public:
    static void*  setDestructorBit(void* p)
    {
      //return (void*)((u64)p ^ (((u64)1l)<<63));
      return (void*)((u64)p | (1llu<<63));
    }
    static bool   getDestructorBit(void* p)
    {
      return (((u64)p)>>63)!=0;
    }
    static void*         clearBits(void* p)
    {
      return (void*)( ((u64)p) & 0x0000FFFFFFFFFFFF);
    }
    static u64          sizeBytes(u64 count)  // sizeBytes is meant to take the same arguments as a constructor and return the total number of bytes to hold the entire stucture given those arguments 
    {
      return sizeof(u64)*2 + count*sizeof(T);
    }

    lava_vec(){}
    lava_vec(u64  count)
    {
      u64 sb = lava_vec::sizeBytes(count);
      p       = Allocator().allocate(sb);
      p       = setDestructorBit(p);
      set_size(count);
      set_sizeBytes(sb);
    }
    lava_vec(void*  addr, u64 count, bool owner=true) :
      p(addr)
    {
      if(owner){
        set_sizeBytes( lava_vec::sizeBytes(count) );
        set_size(count);
      }
    }
    lava_vec(void*  addr) :
      p(addr)
    {
      //u64 sb = lava_vec::sizeBytes(count);
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

    T& operator[](u64 i)
    {
      return data()[i];
    }

    T*         data()
    {
      u64* maskptr = (u64*)clearBits(p); 
      return (T*)(maskptr+2);
    }
    u64   capacity() const
    {
      return (sizeBytes() - sizeof(u64)*2) / sizeof(T);
    }
    u64       size() const
    {
      return *((u64*)clearBits(p) + 1);   // second 8 bytes should be the number of elements
    } 
    u64  sizeBytes() const
    {
      return *((u64*)clearBits(p));   // first 8 bytes should be the total size of the buffer in bytes
    } 
    auto      addr() const -> void*
    {
      return p;
    }
  };

  inline u64 fnv_64a_buf(void const *const buf, u64 len)
  {
    // const u64 FNV_64_PRIME = 0x100000001b3;
    u64 hval = 0xcbf29ce484222325;    // FNV1_64_INIT;  // ((Fnv64_t)0xcbf29ce484222325ULL)
    u8*   bp = (u8*)buf;	           /* start of buffer */
    u8*   be = bp + len;		           /* beyond end of buffer */

    while(bp < be)                     // FNV-1a hash each octet of the buffer
    {
      hval ^= (u64)*bp++;             /* xor the bottom with the current octet */

      //hval *= FNV_64_PRIME; // does this do the same thing?  /* multiply by the 64 bit FNV magic prime mod 2^64 */
      hval += (hval << 1) + (hval << 4) + (hval << 5) +
              (hval << 7) + (hval << 8) + (hval << 40);
    }
    return hval;
  }
  
  inline bool compex128(
    volatile long long*  _Destination, 
    long long           _ExchangeHigh, 
    long long            _ExchangeLow, 
    long long*      _CompareAndResult)
  {
    //assert( ((u64)_Destination) % 16 == 0 );
    return _InterlockedCompareExchange128(
     _Destination,
     _ExchangeHigh,
     _ExchangeLow,
     _CompareAndResult) == 1;
  }

  inline void prefetch1(char const* const p)
  {
    // if msvc or intel compilers
    _mm_prefetch(p, _MM_HINT_T1);
    // else 

    //endif
    //_m_prefetch((void*)p);
  }

  #ifdef _WIN32
    using  u128 = __declspec(align(128)) /*volatile*/ _u128;
  #else
    using  u128 = volatile _u128;
  #endif

  #ifdef _WIN32
    typedef struct _UNICODE_STRING {
      USHORT Length;
      USHORT MaximumLength;
      #ifdef MIDL_PASS
          [size_is(MaximumLength / 2), length_is((Length) / 2) ] USHORT * Buffer;
      #else // MIDL_PASS
          _Field_size_bytes_part_(MaximumLength, Length) PWCH   Buffer;
      #endif // MIDL_PASS
      } UNICODE_STRING;
    typedef UNICODE_STRING *PUNICODE_STRING;

    typedef struct _OBJECT_ATTRIBUTES {
        ULONG Length;
        HANDLE RootDirectory;
        PUNICODE_STRING ObjectName;
        ULONG Attributes;
        PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
        PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
    } OBJECT_ATTRIBUTES;
    typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

    typedef long LONG;
    typedef LONG NTSTATUS;

    // the following is api poison and is a cancerous abomination, but it seems to be the only way to list the global anonymous memory maps in windows  
    #define DIRECTORY_QUERY              0x0001  
    #define STATUS_SUCCESS               ((NTSTATUS)0x00000000L)    // ntsubauth
    #define OBJ_CASE_INSENSITIVE         0x00000040L
    #define STATUS_NO_MORE_FILES         ((NTSTATUS)0x80000006L)
    #define STATUS_NO_MORE_ENTRIES       ((NTSTATUS)0x8000001AL)

    typedef struct _IO_STATUS_BLOCK {
		  union {
			  NTSTATUS Status;
			  PVOID    Pointer;
		  };
		  ULONG_PTR Information;
	  } IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;
  
    using NTOPENDIRECTORYOBJECT = NTSTATUS (WINAPI*)(
	    _Out_  PHANDLE DirectoryHandle,
	    _In_   ACCESS_MASK DesiredAccess,
	    _In_   POBJECT_ATTRIBUTES ObjectAttributes
	  );
    using NTOPENFILE = NTSTATUS (WINAPI*)(
      _Out_ PHANDLE               FileHandle,
      _In_  ACCESS_MASK        DesiredAccess,
      _In_  POBJECT_ATTRIBUTES ObjectAttributes,
      _Out_ PIO_STATUS_BLOCK   IoStatusBlock,
      _In_  ULONG                ShareAccess,
      _In_  ULONG                OpenOptions
    );
    using NTQUERYDIRECTORYOBJECT = NTSTATUS(WINAPI*)(
	    _In_       HANDLE DirectoryHandle,
	    _Out_opt_  PVOID Buffer,
	    _In_       ULONG Length,
	    _In_       BOOLEAN ReturnSingleEntry,
	    _In_       BOOLEAN RestartScan,
	    _Inout_    PULONG Context,
	    _Out_opt_  PULONG ReturnLength
	  );
    using RTLINITUNICODESTRING = VOID(*)(
      _Out_    PUNICODE_STRING DestinationString,
      _In_opt_ PCWSTR          SourceString
    );

    struct OBJECT_DIRECTORY_INFORMATION
    {
      UNICODE_STRING    name;
      UNICODE_STRING    type;
    };

    auto      GetLastErrorStdStr() -> std::string
    {
      DWORD error = GetLastError();
      if (error)
      {
        LPVOID lpMsgBuf;
        DWORD bufLen = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, NULL );
        if (bufLen)
        {
          LPCSTR lpMsgStr = (LPCSTR)lpMsgBuf;
          std::string result(lpMsgStr, lpMsgStr+bufLen);
      
          LocalFree(lpMsgBuf);

          return result;
        }
      }
      return std::string();
    }
    PVOID  GetLibraryProcAddress(PSTR LibraryName, PSTR ProcName)
    {
      return GetProcAddress(GetModuleHandleA(LibraryName), ProcName);
    }
  #endif
}

#ifdef _WIN32
  auto listDBs() -> std::vector<std::wstring>
  {
    //using namespace WINNT;

    static HMODULE _hModule = nullptr; 
    static NTOPENDIRECTORYOBJECT  NtOpenDirectoryObject  = nullptr;
    static NTOPENFILE             NtOpenFile             = nullptr;
    static NTQUERYDIRECTORYOBJECT NtQueryDirectoryObject = nullptr;
    static RTLINITUNICODESTRING   RtlInitUnicodeString   = nullptr;
    
    std::vector<std::wstring> ret;

    if(!NtOpenDirectoryObject){  
      NtOpenDirectoryObject  = (NTOPENDIRECTORYOBJECT)GetLibraryProcAddress( _T("ntdll.dll"), "NtOpenDirectoryObject");
    }
    if(!NtQueryDirectoryObject){ 
      NtQueryDirectoryObject = (NTQUERYDIRECTORYOBJECT)GetLibraryProcAddress(_T("ntdll.dll"), "NtQueryDirectoryObject");
    }
    if(!NtOpenFile){ 
      NtOpenFile = (NTOPENFILE)GetLibraryProcAddress(_T("ntdll.dll"), "NtOpenFile");
    }

    HANDLE   hDir = NULL;
    IO_STATUS_BLOCK  isb = { 0 };
    WCHAR* mempth = L"\\BaseNamedObjects";
    
    WCHAR buf[4096];
    UNICODE_STRING   pth = { 0 };
    pth.Buffer        = mempth;
    pth.Length        = (USHORT)lstrlenW(mempth) * sizeof(WCHAR);
    pth.MaximumLength = pth.Length;

    OBJECT_ATTRIBUTES oa = { 0 };
    oa.Length = sizeof( OBJECT_ATTRIBUTES );
    oa.RootDirectory      = NULL;
    oa.Attributes         = OBJ_CASE_INSENSITIVE;                               
    oa.ObjectName         = &pth;
    oa.SecurityDescriptor = NULL;                        
    oa.SecurityQualityOfService = NULL;               

    NTSTATUS status;
    status = NtOpenDirectoryObject(
      &hDir, 
      /*STANDARD_RIGHTS_READ |*/ DIRECTORY_QUERY, 
      &oa);

    if(status==STATUS_SUCCESS){ return ret; }

    BOOLEAN rescan = TRUE;
    ULONG      ctx = 0;
    ULONG   retLen = 0;
    do
    {
      status = NtQueryDirectoryObject(hDir, buf, sizeof(buf), TRUE, rescan, &ctx, &retLen);
      rescan = FALSE;
      auto info = (OBJECT_DIRECTORY_INFORMATION*)buf;

      if( lstrcmpW(info->type.Buffer, L"Section")!=0 ){ continue; }
      WCHAR wPrefix[] = L"simdb_";
      size_t pfxSz = sizeof(wPrefix);
      if( strncmp( (char*)info->name.Buffer, (char*)wPrefix, pfxSz)!=0 ){  continue; }

      std::wstring name =  std::wstring( (WCHAR*)info->name.Buffer );
      ret.push_back(name);
    }while(status!=STATUS_NO_MORE_ENTRIES);
    
    return ret;
  }
#else
  // gcc/clang/linux ?
  #include <alloca.h>
  #define STACK_VEC(TYPE, COUNT) lava_vec<TYPE>(_alloca(lava_vec<TYPE>::sizeBytes(COUNT)), COUNT, true);  
#endif

class     CncrLst
{
// Internally this is an array of indices that makes a linked list
// Externally indices can be gotten atomically and given back atomically
// | This is used to get free indices one at a time, and give back in-use indices one at a time
public:
  union Head
  {
    struct { u32 ver; u32 idx; };                      // ver is version, idx is index // todo: change cnt to version
    u64 asInt;
  };
  
  using     u32  =  uint32_t;                               // need to be i32 instead for the ConcurrentStore indices?
  using     u64  =  uint64_t;
  using ListVec  =  lava_vec<u32>;

  const static u32 LIST_END = 0xFFFFFFFF;

private:
  ListVec     m_lv;
  au64*        m_h;

public:
  static u64 sizeBytes(u32 size)
  {
    return ListVec::sizeBytes(size);
  }

  CncrLst(){}
  CncrLst(void* addr, u32 size, bool owner=true) :           // this constructor is for when the memory is owned an needs to be initialized
    m_lv(addr, size, owner)
  {                                                          // separate out initialization and let it be done explicitly in the simdb constructor?
    m_h = (au64*)addr;

    if(owner){
      for(u32 i=0; i<(size-1); ++i) m_lv[i] = i+1;
      m_lv[size-1] = LIST_END;

      ((Head*)m_h)->idx = 0;
      ((Head*)m_h)->ver = 0;
    }
                                          // uses the first 8 bytes that would normally store sizeBytes as the 64 bits of memory for the Head structure
  }

  u32        nxt()                                               // moves forward in the list and return the previous index
  {
    Head  curHead, nxtHead;
    curHead.asInt  =  m_h->load();
    do{
      if(curHead.idx==LIST_END){return LIST_END;}

      nxtHead.idx  =  m_lv[curHead.idx];
      nxtHead.ver  =  curHead.ver + 1;
    }while( !m_h->compare_exchange_strong(curHead.asInt, nxtHead.asInt) );

    return curHead.idx;
  }
  u32        free(u32 idx)                         // not thread safe when reading from the list, but it doesn't matter because you shouldn't be reading while freeing anyway, since the CncrHsh will already have the index taken out and the free will only be triggered after the last reader has read from it 
  {
    Head curHead, nxtHead; u32 retIdx;
    curHead.asInt = m_h->load();
    do{
      retIdx = m_lv[idx] = curHead.idx;
      nxtHead.idx  =  idx;
      nxtHead.ver  =  curHead.ver + 1;
    }while( !m_h->compare_exchange_strong(curHead.asInt, nxtHead.asInt) );

    return retIdx;
  }
  u32        free(u32 st, u32 en)                                // not thread safe when reading from the list, but it doesn't matter because you shouldn't be reading while freeing anyway, since the CncrHsh will already have the index taken out and the free will only be triggered after the last reader has read from it 
  {
    Head curHead, nxtHead; u32 retIdx;
    curHead.asInt = m_h->load();
    do{
      retIdx = m_lv[en] = curHead.idx;
      nxtHead.idx  =  st;
      nxtHead.ver  =  curHead.ver + 1;
    }while( !m_h->compare_exchange_strong(curHead.asInt, nxtHead.asInt) );

    return retIdx;
  }
  auto      count() const -> u32
  {
    //return ((HeadUnion*)(&m_h))->cnt;
    //return ((Head*)(&m_h))->ver;
    return ((Head*)m_h)->ver;
  }
  auto        idx() const -> u32
  {
    Head h; 
    h.asInt = m_h->load();
    return h.idx;
  }
  auto       list() -> ListVec const* 
  {
    return &m_lv;
  }            // not thread safe
  u32      lnkCnt()                     // not thread safe
  {
    u32    cnt = 0;
    //auto      l = list();
    u32 curIdx = idx();
    while( curIdx != LIST_END ){
      //curIdx = l->at(curIdx).load();
      curIdx = m_lv[curIdx];   //  l->at(curIdx).load();
      ++cnt;
    }
    return cnt;
  }
  auto       head() -> Head* { return (Head*)m_h; }
};
class     CncrStr
{
public:
  union   VerIdx
  {
    struct { u32 idx; u32 version; }; // declaring the version first and idx second puts the 
    u64 asInt;

    VerIdx(){}
    VerIdx(u32 _idx, u32 _version) : idx(_idx), version(_version) {}
  };
  union   KeyReaders
  {
    struct{ u32 isKey : 1; i32 readers : 31; };
    u32 asInt;
  };
  struct  BlkLst
  {    
    union{
      KeyReaders kr;
      struct{ u32 isKey : 1; i32 readers : 31; };
    };                                                              //  4 bytes   -   kr is key readers  // todo: make sure that kr can be incremented safely
    u32 idx, version, len, klen, hash;                             // 20 bytes
    // 24 bytes total

    BlkLst() : isKey(0), readers(0), idx(0), version(0), len(0), klen(0), hash(0)
    { 
      //kr.isKey    = 0;
      //kr.readers  = 0;
      //kr.version  = 0;
    }
    BlkLst(bool _isKey, i32 _readers, u32 _idx, u32 _version, u32 _len=0, u32 _klen=0, u32 _hash=0) : 
      isKey(_isKey),
      readers(_readers),
      idx(_idx),
      version(_version),
      hash(_hash)
    {
      //kr.isKey    = isKey;
      //kr.readers  = readers;
      //kr.version  = ver;

      //if(isKey){
        len  = _len;
        klen = _klen;
      //}else{
      //  len  = 0;
      //  klen = 0;
      //}
    } 
  };
  struct  BlkCnt { u32 end : 1; u32 cnt : 31; };                                       // this is returned from alloc() and may not be neccesary - it is the number of blocks allocated and if the end was reached

  using ai32        =  std::atomic<i32>;
  using BlockLists  =  lava_vec<BlkLst>;                                               // only the indices returned from the concurrent list are altered, and only one thread will deal with any single index at a time 

  const static u32 LIST_END = CncrLst::LIST_END;

  static VerIdx      List_End()
  { 
    VerIdx vi; 
    vi.idx     = CncrLst::LIST_END; 
    vi.version = 0; 
    return vi; 
  }
  static bool       IsListEnd(VerIdx vi)
  {
    static const VerIdx empty = List_End();
    return empty.asInt == vi.asInt;
  }

  BlkLst    incReaders(u32 blkIdx, u32 version) const                   // BI is Block Index  increment the readers by one and return the previous kv from the successful swap 
  {

    KeyReaders cur, nxt;
    BlkLst*     bl  =  &s_bls[blkIdx];
    au32* areaders  =  (au32*)&(bl->kr);                               // todo: desperatly need to check and redo this - if readers is signed, 31 bits, and part of a bit set with a flag, the flag needs to be kept while the 31 bit signed int is incremented
    cur.asInt       =  areaders->load();
    do{
      if(bl->version!=version || cur.readers<0) return BlkLst(); // make_BlkLst(0,0,0,0,0,0);
      nxt = cur;
      nxt.readers += 1;
    }while( !areaders->compare_exchange_strong(cur.asInt, nxt.asInt) );
    
    return *bl;  // after readers has been incremented this block list entry is not going away. The only thing that would change would be the readers and that doesn't matter to the calling function.
  }
  bool      decReaders(u32 blkIdx, u32 version) const                   // BI is Block Index  increment the readers by one and return the previous kv from the successful swap 
  {
    KeyReaders cur, nxt;
    BlkLst*     bl  =  &s_bls[blkIdx];
    au32* areaders  =  (au32*)&(bl->kr);
    cur.asInt       =  areaders->load();
    do{
      if(bl->version!=version) return false;
      nxt = cur;
      nxt.readers -= 1;
    }while( !areaders->compare_exchange_strong(cur.asInt, nxt.asInt) );
    
    if(cur.readers==0){ doFree(blkIdx); return false; }

    return true;
  }

private:   

  // On the thread's stack
  mutable CncrLst           s_cl;        // flat data structure - pointer to memory 
  mutable BlockLists       s_bls;        // flat data structure - pointer to memory - bl is Block Lists
  void*               s_blksAddr;        // points to the block space in the shared memory
  au64*                s_version;        // pointer to the shared version number

  u32                m_blockSize;
  u32               m_blockCount;
  u64                  m_szBytes;
  //mutable ai32      m_blocksUsed;       // todo: this is a mistake and does no good unless it is in the shared memory

  VerIdx       nxtBlock(u32  blkIdx)  const
  {
    BlkLst bl  = s_bls[blkIdx];
    VerIdx vi;
    vi.idx     = bl.idx;
    vi.version = bl.version;

    prefetch1( (char const* const)blockFreePtr(bl.idx) );

    return vi;
  }
  u32     blockFreeSize()             const { return m_blockSize; }
  u8*      blockFreePtr(u32  blkIdx)  const { return ((u8*)s_blksAddr) + blkIdx*m_blockSize; }
  u8*            blkPtr(u32  blkIdx)  const { return ((u8*)s_blksAddr) + blkIdx*m_blockSize; }
  u32      blocksNeeded(u32     len, u32* out_rem=nullptr)
  {
    u32  freeSz   = blockFreeSize();
    u32  byteRem  = len % freeSz;
    u32  blocks   = len / freeSz + (byteRem? 1 : 0);    // should never be 0 if blocksize is greater than the size of the index type

    if(out_rem) *out_rem = byteRem;

    return blocks;
  }
  u32       findListEnd(u32  blkIdx)  const                  // find the last BlkLst slot in the linked list of blocks to free 
  {
    u32 cur=blkIdx, prev=blkIdx;
    while(cur != LIST_END){
      prev = cur;
      cur  = s_bls[cur].idx;
    }

    return prev;
  }
  void           doFree(u32  blkIdx)  const        // frees a list/chain of blocks - don't need to zero out the memory of the blocks or reset any of the BlkLsts' variables since they will be re-initialized anyway
  {
    u32 listEnd  =  findListEnd(blkIdx); 
    s_cl.free(blkIdx, listEnd);
  }
  u32        writeBlock(u32  blkIdx, void const* const bytes, u32 len=0, u32 ofst=0)      // don't need to increment readers since write should be done before the block is exposed to any other threads
  {
    u32  blkFree  =  blockFreeSize();
    u8*        p  =  blockFreePtr(blkIdx);
    auto     nxt  =  nxtBlock(blkIdx);
    u32   cpyLen  =  len==0? blkFree : len;             // if next is negative, then it will be the length of the bytes in that block
    p      += ofst;
    memcpy(p, bytes, cpyLen);

    return cpyLen;
  }
  u32         readBlock(u32  blkIdx, u32 version, void *const bytes, u32 ofst=0, u32 len=0) const
  {
    BlkLst bl = incReaders(blkIdx, version);               if(bl.version==0) return 0;  // if(bl.kr.version==0) return 0;
      u32   blkFree  =  blockFreeSize();
      u8*         p  =  blockFreePtr(blkIdx);
      u32       nxt  =  bl.idx;
      u32    cpyLen  =  len==0?  blkFree-ofst  :  len;
      //cpyLen        -=  ofst;
      memcpy(bytes, p+ofst, cpyLen);
    decReaders(blkIdx, version);

    return cpyLen;
  }

public:
  /* 
    The order of the shared memory is:
    Version
    CncrLst
    BlockLists
    Blocks
  */
  static u64    BlockListsOfst(){ return sizeof(u64); }
  static u64         CListOfst(u32 blockCount){ return BlockListsOfst() + BlockLists::sizeBytes(blockCount); }      // BlockLists::sizeBytes ends up being sizeof(BlkLst)*blockCount + 2 u64 variables
  static u64          BlksOfst(u32 blockCount){ return CListOfst(blockCount) + CncrLst::sizeBytes(blockCount); }
  static u64         sizeBytes(u32 blockSize, u32 blockCount){ return BlksOfst(blockCount) + blockSize*blockCount; }

  CncrStr(){}
  CncrStr(void* addr, u32 blockSize, u32 blockCount, bool owner=true) :
    m_blockSize(blockSize),
    m_blockCount(blockCount),
    //m_blocksUsed(0),
    s_blksAddr( (u8*)addr + BlksOfst(blockCount) ),
    s_cl(       (u8*)addr + CListOfst(blockCount), blockCount, owner),
    s_bls(      (u8*)addr + BlockListsOfst(),      blockCount, owner),
    s_version(  (au64*)addr ),
    m_szBytes( *((u64*)addr) )
  {
    if(owner){
      for(u32 i=0; i<m_blockCount; ++i){ s_bls[i] = BlkLst(); }
      s_version->store(1);                            // todo: redo this, with 32 bit integers, 0 shouldn't be a special value because it could loop around start at 1, use 0 as a special value - 
    }
    assert(blockSize > sizeof(i32));
  }

  auto        alloc(u32    size, u32 klen, u32 hash, BlkCnt* out_blocks=nullptr) -> VerIdx    
  {
    u32  byteRem = 0;
    u32   blocks = blocksNeeded(size, &byteRem);
    u32       st = s_cl.nxt();
    SECTION(get the starting block index and handle errors)
    {
      if(st==LIST_END){
        if(out_blocks){ *out_blocks = {1, 0} ; } 
        return List_End(); 
      }
    }

    u32  ver = (u32)s_version->fetch_add(1);
    u32  cur = st;
    u32  nxt = 0;
    u32  cnt = 0;
    SECTION(loop for the number of blocks needed and get new block and link it to the list)
    {
      for(u32 i=0; i<blocks-1; ++i)
      {
        nxt = s_cl.nxt();
        if(nxt==LIST_END){ free(st, ver); VerIdx empty={LIST_END,0}; return empty; }

        s_bls[cur] = BlkLst(false, 0, nxt, ver, size);
        cur        = nxt;
        ++cnt;
        //m_blocksUsed.fetch_add(1);
      }
    }

    SECTION(add the last index into the list, set out_blocks and return the start index with its version)
    {      
      s_bls[cur] = BlkLst(false,0,LIST_END,ver,size,0,0);       // if there is only one block needed, cur and st could be the same

      s_bls[st].isKey = true;
      s_bls[st].hash  = hash;
      s_bls[st].len   = size;
      s_bls[st].klen  = klen;

      if(out_blocks){
        out_blocks->end = nxt==LIST_END;
        out_blocks->cnt = cnt;
      }     
      VerIdx vi(st, ver);
      return vi;
    }
  }
  bool         free(u32  blkIdx, u32 version)        // frees a list/chain of blocks
  {
    return decReaders(blkIdx, version);
  }
  void          put(u32  blkIdx, void const *const kbytes, u32 klen, void const *const vbytes, u32 vlen)  // don't need version because this will only be used after allocating and therefore will only be seen by one thread until it is inserted into the ConcurrentHash
  {
    using namespace std;
    
    u8*         b  =  (u8*)kbytes;
    bool   kjagged  =  (klen % blockFreeSize()) != 0;
    u32    kblocks  =  kjagged? blocksNeeded(klen)-1 : blocksNeeded(klen);
    u32   remklen   =  klen - (kblocks*blockFreeSize());
    
    u32   fillvlen  =  min(vlen, blockFreeSize()-remklen);
    u32   tailvlen  =  vlen-fillvlen;
    bool   vjagged  =  (tailvlen % blockFreeSize()) != 0;
    u32    vblocks  =  vjagged? blocksNeeded(tailvlen)-1 : blocksNeeded(tailvlen);
    u32    remvlen  =  max<u32>(0, tailvlen - (vblocks*blockFreeSize()) ); 

    u32       cur  =  blkIdx;
    for(u32 i=0; i<kblocks; ++i){
      b   +=  writeBlock(cur, b);
      cur  =  nxtBlock(cur).idx;
    }
    if(kjagged){
      writeBlock(cur, b, remklen);
      b    =  (u8*)vbytes;
      b   +=  writeBlock(cur, b, fillvlen, remklen);
      cur  =  nxtBlock(cur).idx;
    }
    for(u32 i=0; i<vblocks; ++i){
      b   +=  writeBlock(cur, b);
      cur  =  nxtBlock(cur).idx;
    }
    if(vjagged && remvlen>0){
      b   +=  writeBlock(cur, b, remvlen);
    }
  }
  u32           get(u32  blkIdx, u32 version, void *const bytes, u32 maxlen) const
  {
    using namespace std;

    if(blkIdx == LIST_END){ return 0; }

    BlkLst bl = incReaders(blkIdx, version);   
    
    u32 vlen = bl.len-bl.klen;
    if(bl.len==0 || vlen>maxlen ) return 0;

    auto   kdiv = div((i64)bl.klen, (i64)blockFreeSize());
    auto  kblks = kdiv.quot;
    u32    krem = (u32)kdiv.rem;
    auto vrdLen = 0;
    u32     len = 0;
    u32   rdLen = 0;
    u8*       b = (u8*)bytes;
    u8*      en = b + maxlen;
    i32     cur = blkIdx;
    VerIdx  nxt;
    for(int i=0; i<kblks; ++i){ 
      nxt = nxtBlock(cur);           if(nxt.version!=version){ goto read_failure; }
      cur = nxt.idx;
    }

    vrdLen =  min<u32>(blockFreeSize()-krem, vlen);
    rdLen  =  (u32)readBlock(cur, version, b, krem, vrdLen);
    b     +=  rdLen;
    len   +=  rdLen;
    nxt    =  nxtBlock(cur);         if(nxt.version!=version){ goto read_failure; }

    while(len<maxlen && !(nxt.idx<0) && nxt.idx!=LIST_END && nxt.version==version)
    {
      vrdLen =  min<u32>(blockFreeSize(), maxlen-len);
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
  u32        getKey(u32  blkIdx, u32 version, void *const bytes, u32 maxlen) const
  {
    if(blkIdx == LIST_END){ return 0; }

    BlkLst bl = incReaders(blkIdx, version);   
    
    if(bl.len==0 || (bl.klen)>maxlen ) return 0;

    auto   kdiv = div((i64)bl.klen, (i64)blockFreeSize());
    auto  kblks = kdiv.quot;
    u32    krem = (u32)kdiv.rem;
    u32     len = 0;
    u32   rdLen = 0;
    u8*       b = (u8*)bytes;
    u32     cur = blkIdx;
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
  Match   memcmpBlk(u32  blkIdx, u32 version, void const *const buf1, void const *const buf2, u32 len) const  // todo: eventually take out the inc and dec readers and only do them when dealing with the whole chain of blocks
  {
    if(incReaders(blkIdx, version).len==0) return MATCH_REMOVED;
      auto ret = memcmp(buf1, buf2, len);
    decReaders(blkIdx, version);

    if(ret==0) return MATCH_TRUE;

    return MATCH_FALSE;
  }
  Match     compare(u32  blkIdx, u32 version, void const *const buf, u32 len, u32 hash) const
  {
    using namespace std;
    
    u32 blklstHsh = s_bls[blkIdx].hash;
    if(s_bls[blkIdx].hash!=hash) return MATCH_FALSE;  // vast majority of calls should end here

    u32   curidx  =  blkIdx;
    VerIdx   nxt  =  nxtBlock(curidx);                            if(nxt.version!=version) return MATCH_FALSE;
    u32    blksz  =  (u32)blockFreeSize();  // todo: change this to u32
    u8*   curbuf  =  (u8*)buf;
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
  }
  u32           len(u32  blkIdx, u32 version, u32* out_vlen=nullptr) const
  {
    BlkLst bl = s_bls[blkIdx];
    //if(version==bl.kr.version && bl.len>0){
    if(version==bl.version && bl.len>0){
      if(out_vlen) *out_vlen = bl.len - bl.klen;
      return bl.len;
    }else 
      return 0;
  }
  auto         list()      const -> CncrLst const&
  {
    return s_cl;
  }
  auto         data()      const -> const void*
  {
    return (void*)s_blksAddr;
  }
  u32    blockCount()      const
  {
    return m_blockCount; // m_cl.sizeBytes();
  }
  auto       blkLst(u32 i) const -> BlkLst
  {
    return s_bls[i];
  }

  friend class CncrHsh;
};
class     CncrHsh
{
public:
  using  VerIdx   = CncrStr::VerIdx;
  using  BlkLst   = CncrStr::BlkLst;

  struct VerIpd { u32 version, ipd; };                       // ipd is Ideal Position Distance

  static const i8   RM_OWNER         =     -1;               // keep this at 0 if INIT_READERS is changed to 1, then take out remove flag
  static const u8   LAST_READER      =      0;               // keep this at 0 if INIT_READERS is changed to 1, then take out remove flag
  static const u8   INIT_READERS     =      0;               // eventually make this 1 again? - to catch when readers has dropped to 0
  static const u8   FREE_READY       =      0;
  static const u8   MAX_READERS      =   0xFF;
  static const u32  KEY_MAX          =   0xFFFFFFFF; 
  static const u32  EMPTY_KEY        =   KEY_MAX;            // first 21 bits set 
  static const u32  DELETED_KEY      =   KEY_MAX - 1;        // 0xFFFFFFFE;       // 1 less than the EMPTY_KEY
  static const u32  EMPTY_HASH_IDX   =   0xFFFFFFFF;         // 32 bits set - hash indices are different from block indices 
  static const u32  LIST_END         =   CncrStr::LIST_END;

  static u64           sizeBytes(u32 size)                   // the size in bytes that this structure will take up in the shared memory
  {
    return lava_vec<VerIdx>::sizeBytes(size) + 16;           // extra 16 bytes for 128 bit alignment padding 
  }
  static u32        nextPowerOf2(u32  v)
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
  static u32           HashBytes(const void *const buf, u32 len)
  {
    u64 hsh = fnv_64a_buf(buf, len);

    return (u32)( (hsh>>32) ^ ((u32)hsh));
  }
  static VerIdx         empty_vi(){ return VerIdx(EMPTY_KEY,0); }
  static VerIdx       deleted_vi()
  {
    VerIdx empty;
    empty.idx      =  DELETED_KEY;
    empty.version  =  0;
    return empty;
  }
  static i64              vi_i64(VerIdx vi)
  {
    u64 iVi = vi.asInt;
    return *((i64*)(&iVi));                                              // interpret the u64 bits directly as a signed 64 bit integer instead    
  }
  static i64              vi_i64(u64  i){ return *((i64*)&i); }          // interpret the u64 bits directly as a signed 64 bit integer instead    
  static bool            IsEmpty(VerIdx vi)
  {
    static VerIdx emptyvi = empty_vi();
    return emptyvi.asInt == vi.asInt;
  }
  static u32                lo32(u64 n){ return (n>>32); }
  static u32                hi32(u64 n){ return (n<<32)>>32; }
  static u64               swp32(u64 n){ 
   return (((u64)hi32(n))<<32)  |  ((u64)lo32(n)); }
  static u64             inclo32(u64 n, u32 i){ return ((u64)hi32(n)+i)<<32 | lo32(n); }
  static u64             incHi32(u64 n, u32 i){ return ((u64)hi32(n))<<32 | (lo32(n)+i); }
  static u64          shftToHi64(u32 n){ return ((u64)n)<<32; }
  static u64              make64(u32 lo, u32 hi){ return (((u64)lo)<<32) | ((u64)hi); }

private:
  using Au32     =  std::atomic<u32>;
  using Au64     =  std::atomic<u64>;
  using VerIdxs  =  lava_vec<VerIdx>;
  using Mut      =  std::mutex;
  using UnqLock  =  std::unique_lock<Mut>;

          u32       m_sz;
  mutable VerIdxs   s_vis;           // s_vis is key value(s) - needs to be changed to versioned indices, m_vis
          CncrStr*  m_csp;           // csp is concurrent store pointer

  u32            nxtIdx(u32 i)                 const { return (i+1)%m_sz; }
  u32           prevIdx(u32 i)                 const { return std::min(i-1, m_sz-1); } // clamp to m_sz-1 for the case that hash==0, which will result in an unsigned integer wrap
  VerIdx        load_vi(u32 i)                 const
  {
    using namespace std;
    
    au64* avi = (au64*)(s_vis.data()+i);                            // avi is atomic versioned index
    u64   cur = swp32(avi->load());                                 // need because of endianess? // atomic_load<u64>( (au64*)(m_vis.data()+i) );              // Load the key that was there.

    if(i%2==1) return VerIdx(hi32(cur), lo32(cur));
    else       return VerIdx(lo32(cur), hi32(cur));
  }
  VerIdx       store_vi(u32 i, u64 vi)         const
  {
    using namespace std;
        
    bool odd = i%2 == 1;
    VerIdx strVi;
    if(odd) strVi = VerIdx(lo32(vi), hi32(vi));            // the odd numbers need to be swapped so that their indices are on the outer border of 128 bit alignment - the indices need to be on the border of the 128 bit boundary so they can be swapped with an unaligned 64 bit atomic operation
    else    strVi = VerIdx(hi32(vi), lo32(vi));

    u64 prev = atomic_exchange<u64>( (au64*)(s_vis.data()+i), *((u64*)(&strVi)) );

    if(odd) return VerIdx(lo32(prev), hi32(prev));
    else    return VerIdx(hi32(prev), lo32(prev));
  }
  bool         cmpex_vi(u32 i, VerIdx expected, VerIdx desired) const
  {
    using namespace std;

    //u64     exp = *((u64*)expected);     // i%2? swp32(*((u64*)expected)) : *((u64*)expected);
    u64     exp = i%2? swp32(expected.asInt) : expected.asInt;
    u64    desi = i%2? swp32(desired.asInt) : desired.asInt;  //desired.asInt;                                          // desi is desired int
    au64*  addr = (au64*)(s_vis.data()+i);
    auto before = addr->load();
    bool     ok = addr->compare_exchange_strong( exp, desi );
    auto  after = addr->load();
    
    //return atomic_compare_exchange_strong(addr, (u64*)expected, desi);                      // The entry was free. Now let's try to take it using a CAS. 
    //addr->store(desi);
    //return true;

    return ok;
  }
  void           doFree(u32 i)                 const
  {
    store_vi(i, empty_vi().asInt);
  }
  VerIpd            ipd(u32 i, VerIdx vi)      const  // ipd is Ideal Position Distance - it is the distance a CncrHsh index value is from the position that it gets hashed to 
  {
    BlkLst bl = m_csp->blkLst(vi.idx);
    u32    ip = bl.hash % m_sz;     // ip is Ideal Position
    //u32   ipd = vi.idx>ip?  vi.idx-ip  :  m_csp->blockCount() - ip + vi.idx;  // todo: change vi.idx to u32 so that there aren't sign mismatch warnings
    u32   ipd = i>ip?  i-ip  :  m_csp->blockCount() - ip + i;  // todo: change vi.idx to u32 so that there aren't sign mismatch warnings

    //return {bl.kr.version, ipd};
    return {bl.version, ipd};
  }
  bool          delDupe(u32 i)                 const                                 // delete duplicate indices - l is left index, r is right index - will do something different depending on if the two slots are within 128 bit alignment or not
  {
    if(i%2==0)
    {                                                                               // both indices are within a 128 bit boundary, so the 128 bit atomic can (and must) be used
      i64 rgtDel, lftDel;  _u128 viDbl;
      _u128* viDblAddr = (_u128*)&s_vis[i];                                         // find 128 bit offset address
      viDbl            = *viDblAddr;                                                // todo: should this use a 128 bit atomic load? if it isn't the same, the atomic compare exchange will load it atomically
      do{
        u32 l = hi32(viDbl.lo);
        u32 r = lo32(viDbl.hi);                                                     //  this unintuitivness might be because of endianness, need to look into it
        if(l==DELETED_KEY && r==EMPTY_KEY){
          //u64 rgtSwp = viDbl.lo;
          //rgtDel     = *((i64*)&rgtSwp);
          rgtDel = vi_i64( swp32(empty_vi().asInt) );
          lftDel = vi_i64( empty_vi() );
        }else if(l!=r || l>=DELETED_KEY){                                           // check if both the indices are the same and if they are, that they aren't both deleted or both empty 
          return false;                     
        }else{
          u64 lftSwp = swp32(viDbl.hi);
          lftDel     = *((i64*)&lftSwp);                                            // if both the indices are the same, make a new right side VerIdx with the idx set to DELETED_KEY
          rgtDel     = vi_i64( deleted_vi() );                                      // interpret the u64 bits directly as a signed 64 bit integer instead
        }
      }while( !compex128( (i64*)viDblAddr, rgtDel, lftDel, (i64*)&viDbl) );         // then compare and swap for a version with the new right side VerIdx // todo: does this need to be in a loop that only breaks when the two indices are not the same?
    }else
    {
      au64* idxDblAddr; u64 idxDbl, desired;
      u32* leftAddr = ((u32*)(&s_vis[i]))+1;                                        // if the two VerIdxs are not in a 128 bit boundary, then use a 64 bit compare and swap to set the right side index to DELETED_KEY
      idxDblAddr    = (au64*)leftAddr;                                              // increment the address by 4 bytes so that it lines up with the start of the two indices, then cast it to an atomic 64 bit unsigned integer for the compare and switch
      idxDbl        = idxDblAddr->load();
      do{
        u32  l = lo32(idxDbl);
        u32  r = hi32(idxDbl);
        if(l==DELETED_KEY && r==EMPTY_KEY){                                         // change the deleted key to empty if it is to the left of an empty slot and therefore at the end of a span
          desired = make64(EMPTY_KEY, EMPTY_KEY);
        }else if(l!=r || l>=DELETED_KEY){
          return false; 
        }else{                                                                      // if the indices are the same then do the compare and swap
          desired = make64(l, DELETED_KEY);                                         // make the new 64 bit integer with the right index set to DELETED_KEY
        }
      }while( !idxDblAddr->compare_exchange_strong(idxDbl, desired) );              // looping here would essentially mean that the indices change but are still identical to each other
    }

    return true;
  }
  bool    cleanDeletion(u32 i, u8 depth=0)     const
  {
    VerIdx curVi, nxtVi; VerIpd nxtVp; u32 nxtI;

    clean_loop: while(i!=m_sz-1 && (nxtVi=load_vi(nxtI=nxtIdx(i))).idx <= DELETED_KEY )           // dupe_nxt stands for duplicate next, since we are duplicating the next VerIdx into the current (deleted) VerIdx slot
    {
      curVi = load_vi(i);
      if(curVi.idx == EMPTY_KEY){ return false; }

      nxtVp = ipd(nxtI, nxtVi);
      if(nxtVp.version!=nxtVi.version){ continue; }                                  // the versions don't match, so start over on the same index and skip the compare exchange 
      else if(nxtVp.ipd==0){ return true; }                                          // should this be converted to an empty slot since it is the end of a span? // next slot's versions match and its VerIdx is in its ideal position, so we are done 
      else if( cmpex_vi(i, curVi, nxtVi) ){ 
        delDupe(i);
        i = nxtIdx(i);
      }

    }

    if(i!=m_sz-1){
      if(nxtVi.idx == DELETED_KEY){                                                    // if the next index is deleted, try to clean the next index, then come back and try to delete this one again
        if(depth<1){ cleanDeletion(nxtIdx(i), depth+1); }                              // could this recurse to the depth of the number of blocks?
        goto clean_loop;
      }else if(nxtVi.idx==EMPTY_KEY){
        delDupe(i);    
      }
    }

    return true;
  }

  template<class FUNC> 
  bool       runIfMatch(VerIdx vi, const void* const buf, u32 len, u32 hash, FUNC f) const
  {
    //VerIdx kv = incReaders(i);    
      Match      m = m_csp->compare(vi.idx, vi.version, buf, len, hash);
      bool matched = false;                                                   // not inside a scope
      if(m==MATCH_TRUE){ matched=true; f(vi); }          
    //decReaders(i);
    
    return matched;
  }

public:
  CncrHsh(){}
  CncrHsh(void* addr, u32 size, CncrStr* cs, bool owner=true) :
    m_sz(nextPowerOf2(size)),
    //s_vis(addr, m_sz),
    m_csp(cs)
  {
    u64     paddr  =  (u64)addr;                // paddr is padded address
    u8        rem  =  16 - paddr%16;
    u8       ofst  =  16 - rem;
    void* algnMem  =  (void*)(paddr+ofst);      assert( ((u64)algnMem) % 16 == 0 ); 

    new (&s_vis) VerIdxs(algnMem, m_sz);        // initialize the lava_vec of VerIdx structs with the 128 bit aligned address
    
    if(owner){
      init(size, cs);
    }
  }
  CncrHsh(CncrHsh const& lval) = delete;
  CncrHsh(CncrHsh&&      rval) = delete;
  CncrHsh& operator=(CncrHsh const& lval) = delete;
  CncrHsh& operator=(CncrHsh&&      rval) = delete;

  VerIdx  operator[](u32 idx) const
  {
    return s_vis[idx];
  }

  VerIdx   putHashed(u32 hash, VerIdx lstVi, const void *const key, u32 klen) const
  {
    using namespace std;
    static const VerIdx empty   = empty_vi();

    VerIdx desired = lstVi;
    u32 i=hash%m_sz, en=prevIdx(i);
    for(;; i=nxtIdx(i) )
    {
      VerIdx vi = load_vi(i);
      if(vi.idx>=DELETED_KEY){                                                              // it is either deleted or empty
        bool success = cmpex_vi(i, vi, desired);
        if(success){return vi;}
        else{ i=prevIdx(i); continue; }                                                     // retry the same loop again if a good slot was found but it was changed by another thread between the load and the compare-exchange
      }                                                                                     // Either we just added the key, or another thread did.

      if(m_csp->compare(vi.idx,vi.version,key,klen,hash) != MATCH_TRUE){continue;}

      bool success = cmpex_vi(i, vi, desired);
      if(success){ return vi; }
      else{ i=prevIdx(i); continue; }

      if(i==en) break;
    }

    return empty;  // should never be reached
  }

  template<class FUNC> 
  bool      runMatch(const void *const key, u32 klen, u32 hash, FUNC f)       const 
  {
    using namespace std;
    
    u32  i = hash % m_sz;
    u32 en = prevIdx(i);
    for(;; i=nxtIdx(i) )
    {
      VerIdx vi = load_vi(i);
      if(vi.idx == EMPTY_KEY){ return false;  }                                             // only EMPTY_KEY is the short circuit, since DELETED_KEY means you are still within a span and need to keep searching
      else if(vi.idx == DELETED_KEY){ continue; }
      else if( runIfMatch(vi, key, klen, hash, f) ){ return true; }
      else if(i==en){ return false; }
    }
  }
  
  VerIdx   delHashed(const void *const key, u32 klen, u32 hash)               const
  {  
    using namespace std;
    static const VerIdx   empty = empty_vi();
    static const VerIdx deleted = deleted_vi();

    u32  i = hash % m_sz;
    u32 en = prevIdx(i); 
    for(;; i=nxtIdx(i) )
    {
      VerIdx vi = load_vi(i);

      if(vi.idx==EMPTY_KEY){   return empty;   }
      if(vi.idx==DELETED_KEY){ return deleted; }

      Match m = m_csp->compare(vi.idx, vi.version, key, klen, hash);
      if(m==MATCH_TRUE){
        bool success = cmpex_vi(i, vi, deleted);
        if(success){
          cleanDeletion(i);
          return vi;
        }else{ i=prevIdx(i); continue; }

        return vi;
      }

      if(m==MATCH_REMOVED || i==en){ return empty; }
    }

    return empty; 
  }

  bool          init(u32    sz, CncrStr* cs)
  {
    using namespace std;
    static const u64 iempty    =  empty_vi().asInt;
    static const u64 swpempty  =  swp32(iempty);

    u32 hi = hi32(iempty);
    u32 lo = lo32(iempty);
    
    m_csp   =  cs;
    m_sz    =  sz;
    //new (&m_vis) lava_vec<VerIdx>(m_sz);                   // placement new because the copy constructor and assignment operator are deleted.  msvc doesn't care, but clang does
    
    //u64 ver0, ver1;
    //ver0         =  empty_vi().asInt;
    //ver1         =  swp32(empty_vi().asInt);
    //ver1.version  =  1;
    
    //for(u32 i=0; i<sz; i+=2) store_vi(i, iempty);            // evens 
    //for(u32 i=1; i<sz; i+=2) store_vi(i, iempty);            // odds

    //for(u32 i=0; i<sz; i+=2) *((u64*)(&m_vis[i])) = iempty;          // evens 
    //for(u32 i=1; i<sz; i+=2) *((u64*)(&m_vis[i])) = swpempty;        // odds

    for(u32 i=0; i<sz; i+=2) s_vis[i] = VerIdx(EMPTY_KEY,0);         // evens 
    for(u32 i=1; i<sz; i+=2) s_vis[i] = VerIdx(0,EMPTY_KEY);         // odds
    
    return true;
  }
  VerIdx          at(u32   idx)                const
  {
    return load_vi(idx);
  }
  u32            nxt(u32 stIdx)                const
  {
    auto idx = stIdx;
    VerIdx empty = empty_vi();
    do{
      VerIdx vi = load_vi(idx);
      if(vi.idx != empty.idx) break;
      idx = (idx+1) % m_sz;                                             // don't increment idx above since break comes before it here

      if(idx==stIdx) return empty.idx;
    }while(true);

    return  idx;

    /* && kv.readers>=0 */
  }
  u32           size()                         const
  {
    return m_sz;
  }
  auto          data()                         const -> void* { return s_vis.data(); }
  u64      sizeBytes()                         const
  {
    return s_vis.sizeBytes();
  }
  i64        swapNxt(u32   idx)                const
  {
    i64 retries = -1;
    if(idx%2==0)
    {                                                                 // if idx is even just use 128 bit atomic straight    
      u128*  idxAddr  =  (u128*)( ((u64*)s_vis.addr())+idx );
      u128   dblvi;                                                   // dblvi is double Version Index - it is used to point to two VerIdx structs at the same time 
      u128   swpvi;                                                   // swpvi is swapped version index - the two indices swapped - this is the desired value 
      do{           
        ++retries;                                                    // this will need to swap the side of VerIdx too
        dblvi = *idxAddr;                   
        swpvi = { swp32(idxAddr->lo), swp32(idxAddr->hi) };           // not needed? can use the values directly?
        //swpvi.hi = incLo32(swpvi.hi, 2);                              // swpvi.hi is the left VerIdx, actually ordered as IdxVer, so the lo 32 bits of that are the version number
        //swpvi.lo = incHi32(swpvi.lo, 2);                              // swpvi.lo is the right VerIdx, ordered as VerIdx since the versions are in the middle of the 128 bit alignments and the indices are on the outside
      }while( compex128( (i64*)(idxAddr), swpvi.hi, swpvi.lo, (i64*)(&dblvi) )==1 );
    }
    else                                                              // must be on an odd number, and so will need to use a 64 bit atomic to swap the indices in the middle
    {
      au64* idxAddr = (au64*)( ((u32*)s_vis.addr())+(idx*2+1) );      // offsets using u32, so idx needs to be double, then one more to point to the two indices that are on either side of the 128 bit atomic boundary    
      u64       cur = idxAddr->load();
      u64      swpd;
      do{
        ++retries;
        swpd = swp32(cur);
      }while( !idxAddr->compare_exchange_strong(cur, swpd) );           // compare and swap the hi and lo 32 bits
    }

    return retries;
  }
  i64            len(const void *const key, u32 klen, u32* out_vlen=nullptr, u32* out_version=nullptr) const
  {
    if(klen<1) return 0;

    u32 hash=HashBytes(key,klen), len=0, ver=0, i=hash%m_sz, en=prevIdx(i);
    for(;; i=nxtIdx(i) )
    {
      VerIdx vi = load_vi(i);
      if(vi.idx == EMPTY_KEY){ return 0ull;  }                                      // only EMPTY_KEY is the short circuit, since DELETED_KEY means you are still within a span and need to keep searching
      else if(vi.idx == DELETED_KEY){ continue; }
      
      Match      m = m_csp->compare(vi.idx, vi.version, key, klen, hash);
      if(m==MATCH_TRUE){ return m_csp->len(vi.idx, vi.version, out_vlen); }
      if(i==en){ return 0ull; }
    }

    return len;                                                                     // shouldn't be hit
  }
  bool           get(const void *const key, u32 klen, void *const out_val, u32 vlen) const
  {
    if(klen<1){ return 0; }

    u32 hash=HashBytes(key,klen); 
    CncrStr*  csp = m_csp;
    auto  runFunc = [csp, out_val, vlen](VerIdx vi){
      return csp->get(vi.idx, vi.version, out_val, vlen);
    };

    return runMatch(key, klen, hash, runFunc);
  }
  bool           put(const void *const key, u32 klen, const void *const val, u32 vlen, u32* out_startBlock=nullptr) 
  {
    assert(klen>0);

    u32     hash = CncrHsh::HashBytes(key, klen);
    VerIdx lstVi = m_csp->alloc(klen+vlen, klen, hash);                                 // lstVi is block list versioned index
    if(out_startBlock){ *out_startBlock = lstVi.idx; }
    if(lstVi.idx==LIST_END){ return false; }

    m_csp->put(lstVi.idx, key, klen, val, vlen);

    VerIdx vi = putHashed(hash, lstVi, key, klen);
    if(vi.idx<DELETED_KEY){ m_csp->free(vi.idx, vi.version); }                          // putHashed returns the entry that was there before, which is the entry that was replaced. If it wasn't empty, we free it here. 

    return true;
  }
  bool           del(const void *const key, u32 klen)
  {
    CncrStr*  csp = m_csp;
    auto     hash = CncrHsh::HashBytes(key, klen);
    VerIdx     vi = delHashed(key, klen, hash);
    bool   doFree = vi.idx<DELETED_KEY;
    if(doFree){ m_csp->free(vi.idx, vi.version); }

    return doFree;
  }
  VerIdx        load(u32 i)                    const
  {    
    au64* avi = (au64*)(s_vis.data()+i);                            // avi is atomic versioned index
    u64   cur = swp32(avi->load());                                 // need because of endianess? // atomic_load<u64>( (au64*)(m_vis.data()+i) );              // Load the key that was there.

    if(i%2==1) return VerIdx(hi32(cur), lo32(cur));
    else       return VerIdx(lo32(cur), hi32(cur));
  }
};
struct  SharedMem       // in a halfway state right now - will need to use arbitrary memory and have other OS implementations for shared memory eventually
{
  static const int alignment = 0;
  
  #ifdef _WIN32
    void*      fileHndl;
  #elif defined(__APPLE__) || defined(__MACH__) || defined(__unix__) || defined(__FreeBSD__) // || defined(__linux__) ?    // osx, linux and freebsd
    int        fileHndl;
  #endif

  void*       hndlPtr;
  void*           ptr;
  u64           size;
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
  static SharedMem  AllocAnon(const char* name, u64 size)
  {
    using namespace std;
    
    SharedMem sm;
    sm.owner = false;
    sm.size  = alignment==0? size  :  alignment-(size%alignment);

    #ifdef _WIN32      // windows
      //char path[512] = "Global\\simdb_";
      char path[512] = "simdb_";
    //#elif defined(__APPLE__) || defined(__FreeBSD__) // || defined(__linux__) ?    // osx, linux and freebsd
    #elif defined(__APPLE__) || defined(__MACH__) || defined(__unix__) || defined(__FreeBSD__) || defined(__linux__)  // osx, linux and freebsd
      char path[512] = "/tmp/simdb_15_";
      //char path[512] = "simdb_15_";
    #endif

    // todo: need to check that name and path are less than sizeof(path) here
    u64 len = strlen(path) + strlen(name);
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
  
    u64      addr = (u64)(sm.hndlPtr);
    u64 alignAddr = addr;
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
class       simdb
{
public:
  using    u8   =  uint8_t;
  using   str   =  std::string;
  using BlkCnt  =  CncrStr::BlkCnt;
  using VerIdx  =  CncrHsh::VerIdx;

private:
  au64*      s_flags;
  au64*      s_blockSize;
  au64*      s_blockCount;
  CncrStr    s_cs;               // store data in blocks and get back indices
  CncrHsh    s_ch;               // store the indices of keys and values - contains a ConcurrentList

  // these variables are local to the stack where simdb lives, unlike the others, they are not simply a pointer into the shared memory
  SharedMem           m_mem;
  mutable u32    m_nxtChIdx;
  mutable u32    m_curChIdx;
  u64              m_blkCnt;
  u64               m_blkSz;

public:
  static const u32    EMPTY_KEY = CncrHsh::EMPTY_KEY;          // 28 bits set 
  static const u32  DELETED_KEY = CncrHsh::DELETED_KEY;        // 28 bits set 
  static const u32   FAILED_PUT = CncrHsh::EMPTY_KEY;          // 28 bits set 
  static const u32     LIST_END = CncrStr::LIST_END;
  static u64        OffsetBytes()
  {
    return sizeof(au64)*3;
  }
  static u64            MemSize(u64 blockSize, u64 blockCount)
  {
    auto  hashbytes = CncrHsh::sizeBytes((u32)blockCount);
    auto storebytes = CncrStr::sizeBytes((u32)blockSize, (u32)blockCount);
    return  hashbytes + storebytes + OffsetBytes();
  }
  static Match     CompareBlock(simdb const *const ths, i32 blkIdx, u32 version, void const *const buf, u32 len, u32 hash)
  { 
    return ths->s_cs.compare(blkIdx, version, buf, len, hash);
  }
  static bool           IsEmpty(VerIdx vi){return CncrHsh::IsEmpty(vi);}         // special value for CncrHsh
  static bool         IsListEnd(VerIdx vi){return CncrStr::IsListEnd(vi);}       // special value for CncrStr

public:
  simdb(){}
  simdb(const char* name, u32 blockSize, u32 blockCount) : 
    m_curChIdx(0)
  {
    new (&m_mem) SharedMem( SharedMem::AllocAnon(name, MemSize(blockSize,blockCount)) );

    s_blockCount =  ((au64*)m_mem.data())+2;
    s_blockSize  =  ((au64*)m_mem.data())+1;
    s_flags      =   (au64*)m_mem.data();

    if(isOwner()){
      s_blockCount->store(blockCount);
      s_blockSize->store(blockSize);
    }else{                                                      // need to spin until ready
      while(s_flags->load()==false){}
      m_mem.size = MemSize(s_blockSize->load(), s_blockCount->load());
    }

    //auto chSz = s_ch.sizeBytes();
    auto cncrHashSize = CncrHsh::sizeBytes(blockCount);
    new (&s_cs) CncrStr( ((u8*)m_mem.data())+cncrHashSize+OffsetBytes(), 
                                 (u32)s_blockSize->load(), 
                                 (u32)s_blockCount->load(), 
                                 m_mem.owner);                  // todo: change this to a void* - ? why change the owner flag to a void pointer?

    new (&s_ch) CncrHsh( ((u8*)m_mem.data())+OffsetBytes(), 
                                (u32)s_blockCount->load(),
                                &s_cs,                          // the address of the CncrStr
                                m_mem.owner);

    m_blkCnt = s_blockCount->load();
    m_blkSz  = s_blockSize->load();

    // todo: initialized flag and reference count
    if(isOwner()) s_flags->store(1);                             // set to 1 to signal construction is done
  }

  i64          len(const void *const key, u32 klen, u32* out_vlen=nullptr, u32* out_version=nullptr) const
  {
    return s_ch.len(key, klen, out_vlen, out_version);
  }
  bool         get(const void *const key, u32 klen, void *const   out_val, u32 vlen) const
  {
    return s_ch.get(key, klen, out_val, vlen);
  }
  bool         put(const void *const key, u32 klen, const void *const val, u32 vlen, u32* out_startBlock=nullptr)
  {
    return s_ch.put(key, klen, val, vlen, out_startBlock);
  }
  bool         del(const void *const key, u32 klen)
  {
    return s_ch.del(key, klen);
  }

  bool         put(char const* const key, const void *const val, u32 vlen, u32* out_startBlock=nullptr)
  {
    assert(strlen(key)>0);
    return put(key, (u32)strlen(key), val, vlen, out_startBlock);
  }
  bool         get(char const* const key, void *const val, u32 vlen) const // todo: should val not be a const pointer?
  {
    return get(key, (u32)strlen(key), val, vlen);
  }
  i64          len(u32 idx, u32 version, u32* out_klen=nullptr, u32* out_vlen=nullptr) const
  { 
    u32 total_len = s_cs.len(idx, version, out_vlen); 
    if(total_len>0){
      *out_klen = total_len - *out_vlen;
      return total_len;
    }
    return 0;
  }
  void       flush() const
  {
    FlushViewOfFile(m_mem.hndlPtr, m_mem.size);
  }
  VerIdx       nxt() const                                   // this version index represents a hash index, not an block storage index
  {
    auto        st = m_nxtChIdx;
    VerIdx   empty = s_ch.empty_vi();
    u32    chNxt; // = empty.key;
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
  bool      getKey(u32 idx, u32 version, void *const out_buf, u32 klen) const
  {
    if(klen<1) return false;

    VerIdx vi = s_ch.load(idx);  
    if(vi.idx <= CncrHsh::DELETED_KEY || vi.version!=version){ return false; }
    u32 l = s_cs.getKey(vi.idx, vi.version, out_buf, klen);                         // l is length
    if(l<1){return false;}

    return true;
  }
  u32          cur() const { return m_curChIdx; }
  auto        data() const -> const void* const { return s_cs.data(); }
  u64         size() const { return CncrStr::sizeBytes( (u32)s_blockSize->load(), (u32)s_blockCount->load()); }
  bool     isOwner() const { return m_mem.owner; }
  u64       blocks() const { return s_blockCount->load(); }
  auto         mem() const -> void* { return m_mem.hndlPtr; }
  u64      memsize() const { return m_mem.size; }
  auto    hashData() const -> void const* const { return s_ch.data(); }

  // separated C++ functions - these won't need to exist if compiled for a C interface
  struct VerStr { 
    u32 v; str s; 
    bool  operator<(VerStr const& vs) const { if(s==vs.s) return v<vs.v; else return s<vs.s; }  
    bool  operator<(str const&    rs) const { return s<rs;    }
    bool operator==(VerStr const& vs) const { return s==vs.s && v==vs.v; } 
  };   

  i64          put(str    const& key, str const& value)
  {
    return put(key.data(), (u32)key.length(), value.data(), (u32)value.length());
  }
  bool         get(str    const& key, str*   out_value) const
  {
    u32    vlen = 0;
    len(key.data(), (u32)key.length(), &vlen);
    new (out_value) std::string(vlen,'\0');
    bool     ok = get(key.data(), (u32)key.length(), (void*)out_value->data(), vlen);

    return ok;
  }
  auto         get(str    const& key)                   const -> std::string
  {
    str ret;
    if(this->get(key, &ret)) return ret;
    else return str("");
  }
  VerStr    nxtKey(u64* searched=nullptr)               const
  {
    u32 klen, vlen;
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
    
    //ok         = this->len(nxt.idx, nxt.version, 
    //                       &klen, &vlen);               
    //if(!ok)
    //  return {nxt.version, ""};

    i64 total_len = this->len(nxt.idx, nxt.version, &klen, &vlen);               
    if(total_len==0){ return {nxt.version, ""}; }
    
    str key(klen,'\0');
    ok         = this->getKey(nxt.idx, nxt.version, 
                              (void*)key.data(), klen); 
                              
    if(!ok || strlen(key.c_str())!=key.length() )
      return {nxt.version, ""};

    return { nxt.version, key };                    // copy elision 
  }
  auto  getKeyStrs() const -> std::vector<VerStr>
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

    //u32       i = 0;
    //str nxt = nxtKey();
    u64 srchCnt = 0;
    auto     nxt = nxtKey();                             
    while( srchCnt<m_blkCnt && keys.find(nxt)==keys.end() )
    {
      if(nxt.s.length()>0) keys.insert(nxt);
    
      u64 searched = 0;
      nxt = nxtKey(&searched);
      srchCnt += searched;
      //++i;
    }

    //if(out_versions) new (out_versions) vec<u32>()
    return vector<VerStr>(keys.begin(), keys.end());
  }
  bool         del(str const& key)
  {
    return this->del( (void const* const)key.data(), (u32)key.length() );
  }

  template<class T>
  i64          put(str    const& key, std::vector<T> const& val)
  {    
    return put(key.data(), (u32)key.length(), val.data(), (u32)(val.size()*sizeof(T)) );
  }
  // end separated C++ functions

};



#endif










//
//template<class T, class A=std::allocator<T> > using vec = std::vector<T, A>;  // will need C++ ifdefs eventually

//using    i8   =   int8_t;
//using    u8   =   uint8_t;
//using   i64   =   int64_t;
//using   u64   =   uint64_t;
//using   i32   =   int32_t;
//using   u32   =   uint32_t;
//using   f32   =   float;
//using   f64   =   double;
//using  au64   =   std::atomic<u64>;
//using  au32   =   std::atomic<u32>;
//using  ai32   =   std::atomic<i64>;
//using   ai8   =   std::atomic<i8>;
//using  cstr   =   const char*;
//using   str   =   std::string;             // will need C++ ifdefs eventually or just need to be taken out
//using    u8   =   uint8_t;

//static bool  DefaultKeyCompare(u32 a, u32 b)
//{
//  return a == b;
//}

// goto clean_loop;
//else if( cmpex_vi(i, &curVi, nxtVi) ){ 
//i = nxtIdx(i);                                                               // this could mean the current slot has a deleted key and isn't cleanup by this function

//bool       runIfMatch(u32 i, u32 version, u32 key, MATCH_FUNC match, FUNC f) const
//
//Match      m = match(key, version);
//VerIdx    vi = load_vi(i);
//
//template<class MATCH_FUNC, class FUNC> 
//bool      runMatch(u32 hash, MATCH_FUNC match, FUNC f)        const 
//else if( runIfMatch(i, vi.version, vi.idx, match, f) ){ return true; }

//auto hash = CncrHsh::HashBytes(key, klen);
////auto  ths = this;
//auto matchFunc = [ths, key, klen, hash](u32 blkidx, u32 ver){
//  return CompareBlock(ths,blkidx,ver,key,klen,hash);
//};
//
//u32       hash = HashBytes(key, klen);
//auto matchFunc = [csp, key, klen, hash](u32 blkidx, u32 ver){
//  return csp->compare(blkidx,ver,key,klen,hash);
//};
//    
//return runMatch(hash, matchFunc, runFunc);
//u32  out_klen = 0;
// out_klen=0;

//static const VerIdx deleted = deleted_kv();
//
//VerIdx desired   =  empty;
//desired.idx      =  lstVi.idx;
//desired.version  =  lstVi.version;
//u32           i  =  hash % m_sz;
//u32          en  =  prevIdx(i);

//template<class FUNC> 
//auto       runRead(u32  idx, u32 version, FUNC f)             const -> decltype( f(VerIdx()) )
//{  
//  auto  vi = load_vi(idx);        if(vi.version!=version) return false;
//  return f(vi);
//}

//bool    success  =  cmpex_vi(i, s_vis.data()+i, desired);      
//
//if( checkMatch(vi.version, vi.idx, match)!=MATCH_TRUE ){ continue; }
//
//bool success = cmpex_vi(i, s_vis.data()+i, desired);

//if(klen<1) return false;
//
//auto     ths = this;
//auto runFunc = [ths, klen, out_buf](VerIdx kv){
//  if(IsEmpty(kv)) return false;
//  auto getlen = ths->s_cs.getKey(kv.idx, kv.version, out_buf, klen);
//  if(getlen<1) return false;
//  
//  return true;
//};
//return s_ch.runRead(idx, version, runFunc);

//VerIdx ret;
//if(i%2==1) ret.asInt = swp32(cur);
//else       ret.asInt = cur;

//auto  ths = this;
//bool   ok = s_ch.runRead(idx, version, 
//[ths, out_vlen, out_version](VerIdx kv)
//{
//  u32 vlen = 0;
//  auto tlen = ths->s_cs.len(kv.idx, kv.version, out_vlen);
//  if(tlen>0){
//    *out_vlen = tlen - *out_vlen;
//    return true;
//  }
//  return false;
//});  
//return ok;

//
//bool         cmpex_vi(u32 i, VerIdx* volatile expected, VerIdx desired) const

//template<class MATCH_FUNC> 
//auto       checkMatch(u32 version, u32 key, MATCH_FUNC match) const -> Match //  decltype(match(empty_kv()))
//{
//  //incReaders(i);  // todo: have incReaders return a VerIdx?
//    Match ret = match(key, version);
//  //decReaders(i);
//  
//  return ret;
//}
//
//template<class MATCH_FUNC, class FUNC> 
//bool       runIfMatch(u32 i, u32 version, u32 key, MATCH_FUNC match, FUNC f) const // const -> bool
//{
//  //VerIdx kv = incReaders(i);    
//    //Match      m = match(key, version);
//    Match      m = m_csp->compare(blkIdx, version, keybuf, klen);
//    bool matched = false;                                       // not inside a scope
//    if(m==MATCH_TRUE){ matched=true; f(load_vi(i)); }          
//  //decReaders(i);
//  
//  return matched;
//}

// decltype( (f(empty_kv())) )
//
//VerIdx kv = incReaders(idx);
//auto ret = f(vi);
//decReaders(idx);

//u32       hash = HashBytes(key, klen);    
//CncrStr*   csp = m_csp;
//auto matchFunc = [csp, key, klen, hash](u32 blkidx, u32 ver){
//  return csp->compare(blkidx,ver,key,klen,hash);
//};
//
////m_csp->compare(blkIdx, version, buf, len, hash);
//
//u32 len=0; u32 ver=0;
//auto runFunc = [csp, &len, &ver, out_vlen](VerIdx kv){
//  len = IsEmpty(kv)?  0ull  :  csp->len(kv.idx, kv.version, out_vlen);
//  ver = kv.version;
//};
//    
//u32  i = hash % m_sz;
//u32 en = prevIdx(i);
//
//if( runIfMatch(i, vi.version, vi.idx, match, f) ){ return true; }
//else if(i==en){ return false; }
//
//if(out_version){ *out_version = ver; }
//if( !runMatch(hash, matchFunc, runFunc) ) return 0;
//if(out_version){ *out_version = ver; }

//if(curVi.idx >= DELETED_KEY){ return false; }
//
////nxtVp = ipd(nxtIdx(i), nxtVi);
//nxtVp = ipd(nxtI, nxtVi);
//if(nxtVp.version!=nxtVi.version){ continue; /*goto clean_loop;*/ }             // the versions don't match, so start over on the same index and skip the compare exchange 
//else if(nxtVp.ipd==0){ return true; }                                          // should this be converted to an empty slot since it is the end of a span? // next slot's versions match and its VerIdx is in its ideal position, so we are done 
//else if( cmpex_vi(i, &curVi, nxtVi) ){ 
//  delDupe(i);
//  i = nxtIdx(i);
//}
//
//if(nxtVi.idx==EMPTY_KEY){ 
//  if(delDupe(i)){ return true; }
//  
//  i = nxtIdx(i); continue;
//}

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


