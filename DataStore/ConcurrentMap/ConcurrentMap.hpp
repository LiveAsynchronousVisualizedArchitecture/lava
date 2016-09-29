
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

// todo: need to make ConcurrentList a flat data structure so it can be store at the start of the memory mapped file
// todo: store size in the ConcurrentList?
// todo: test using the db from multiple processes
// todo: test with multiple threads in a loop
// todo: redo concurrent store get to store length so that buffer can be returned
// todo: store lengths and check key lengths before trying bitwise comparison as an optimization? - would only make a difference for long keys that are larger than one block? no it would make a difference on every get?
// todo: prefetch memory for next block when looping through blocks

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
using aui64   =   std::atomic<ui64>;
using  ai32   =   std::atomic<i64>;
using  cstr   =   const char*;
using   str   =   std::string;

template<class T>
class lava_noop
{
  void operator()(){}
};

template<class T, class Deleter=std::default_delete<T>, class Allocator=std::allocator<T> >
class lava_vec
{
private:
  //using T = i32;

  void* p;

  void       set_size(ui64 s)
  { 
    *((ui64*)p + 1) = s;
  }
  void  set_sizeBytes(ui64 sb)
  {
    *((ui64*)p) = sb;      // first 8 bytes should be the total size of the buffer in bytes
  } 

public:
  //static const ui64 data_offset = sizeof(ui64) * 2;

  static ui64 sizeBytes(ui64 count)
  {
    return  sizeof(ui64)*2 + count*sizeof(T);
  }

  lava_vec(){}
  lava_vec(ui64  count)
  {
    ui64 sb = lava_vec::sizeBytes(count);
    p       = Allocator().allocate(sb); // malloc(sb);
    p       = (void*)( (ui64)p ^ ( ((ui64)1)<<63 ) );
    set_size(count);
    set_sizeBytes(sb);
  }
  lava_vec(void*  addr, ui64 count)
  {
    ui64 sb = lava_vec::sizeBytes(count);
    p       = addr;
    set_size(count);
    set_sizeBytes(sb);
  }
  lava_vec(lava_vec&& rval)
  {
    p      = rval.p;
    rval.p = nullptr;
  }
  ~lava_vec()
  {
    if(p){
      Deleter().operator()((T*)p);  //free(p);
      p = nullptr;
    }
  }

  T& operator[](ui64 i)
  {
    //((ui64*)((i8*)p+data_offset))[i];
    T* ofst = (T*)((ui64*)p+2);
    return ofst[i];
  }

  auto       data() -> void*
  {
    return (void*)((ui64*)p+2);
  }
  ui64   capacity() const
  {
    return (sizeBytes() - sizeof(ui64)*2) / sizeof(T);
  }
  ui64       size() const
  {
    return *((ui64*)p + 1);   // second 8 bytes should be the number of elements
  } 
  ui64  sizeBytes() const
  {
    return *((ui64*)p);   // first 8 bytes should be the total size of the buffer in bytes
  } 
};

class   ConcurrentHash
{
private:

public:
  static const  i8   RM_OWNER      =    -1;            // keep this at 0 if INIT_READERS is changed to 1, then take out remove flag
  static const ui8   LAST_READER   =     0;            // keep this at 0 if INIT_READERS is changed to 1, then take out remove flag
  static const ui8   INIT_READERS  =     0;            // eventually make this 1 again? - to catch when readers has dropped to 0
  static const ui8   FREE_READY    =     0;
  static const ui8   MAX_READERS   =  0xFF;
  static const ui32  EMPTY_KEY     =  0x0FFFFFFF;      // 28 bits set 

  union      KV
  {
    struct
    {
      signed   long long readers  :  8;
      unsigned long long     key  : 28;
      unsigned long long     val  : 28;
    };
    uint64_t asInt;

    // after the switch to 128 bit atomics:
    // ui64 ubits;
    // ui64 lbits;
  };
  struct Reader
  {
    bool                     doEnd;       //  8 bits?
    ui32                  hash_idx;       // 32 bits
    KV                          kv;       // 64 bits
    ConcurrentHash const*       ch;       // 64 bits

    Reader(){}
    Reader(Reader const& lval) = delete;
    Reader(Reader&& rval)
    {
      doEnd     =  rval.doEnd;
      hash_idx  =  rval.hash_idx;
      kv        =  rval.kv;
      ch        =  rval.ch;

      rval.doEnd    = false;
      rval.hash_idx = EMPTY_KEY;
      rval.ch       = nullptr;
    }

    bool doRm() // doRm is do remove 
    {
      //if(doEnd && ch){
      if(ch){
        //KV kv = ch->endRead(hash_idx);
        if(kv.readers == RM_OWNER){ // && kv.remove){
          //ch->endRead(hash_idx);
          doEnd = false;
          return true;
        }
      }

      return false;
    }
    ~Reader()
    {
      //if(doEnd && ch) ch->endRead(hash_idx);              // checks the ConcurrentHash pointer for nullptr, not sure if this is neccesary but it probably doesn't hurt much
      if(ch) ch->endRead(hash_idx);              // checks the ConcurrentHash pointer for nullptr, not sure if this is neccesary but it probably doesn't hurt much
    }
  };
  
  static bool DefaultKeyCompare(ui32 a, ui32 b)
  {
    return a == b;
  }

  //const static ui32 EMPTY_KEY = 0xFFFFFFFF;

private:
  using i8        =  int8_t;
  using ui32      =  uint32_t;
  using ui64      =  uint64_t;
  using Aui32     =  std::atomic<ui32>;
  using Aui64     =  std::atomic<ui64>;  
  using KVs       =  std::vector<KV>;
  using Mut       =  std::mutex;
  using UnqLock   =  std::unique_lock<Mut>;

         ui32   m_sz;
  mutable KVs   m_kvs;

  KV           empty_kv()                   const
  {
    KV empty;
//    empty.remove   =  0;
    empty.readers  =  INIT_READERS;
    empty.key      =  0;
    empty.val      =  0;
    empty.key      =  EMPTY_KEY;
    empty.val      =  EMPTY_KEY;
    return empty;
  }
  auto     empty_reader()                   const -> Reader        
  {
    Reader r;
    r.kv       = empty_kv();
    r.ch       = nullptr;                    // this;
    r.doEnd    = false;
    r.hash_idx = EMPTY_KEY;                  // EMPTY_KEY is used for actual keys and values, but this is an index into the KV vector

    return r;
  }
  ui32          intHash(ui32  h)            const
  {
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
  }
  ui32     nextPowerOf2(ui32  v)            const
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
  KV            load_kv(ui32  i)            const
  {
    using namespace std;
    
    KV keyval;
    keyval.asInt   =  atomic_load<ui64>( (Aui64*)(&(m_kvs.data()[i].asInt)) );              // Load the key that was there.
    return keyval;
  }
  KV           store_kv(ui32  i, KV keyval) const
  {
    using namespace std;
    
    //atomic_store<ui64>( (Aui64*)&m_kvs[i].asInt, _kv.asInt );

    KV ret;
    ret.asInt = atomic_exchange<ui64>( (Aui64*)(&(m_kvs[i].asInt)), keyval.asInt);
    return ret;
  }
  bool  compexchange_kv(ui32  i, ui64* expected, ui64 desired) const
  {
    using namespace std;
    
    //kv    keyval;
    //keyval.asInt    =  atomic_load<ui64>( (Aui64*)&m_kvs.data()[i].asInt );
    //keyval.key      =  key;
    //keyval.val      =  val;
    //return success;

    return atomic_compare_exchange_strong( (Aui64*)&m_kvs.data()[i].asInt, expected, desired);                      // The entry was free. Now let's try to take it using a CAS. 
  }
  KV         addReaders(ui32  i, KV curKv, i8 readers)         const                         // increment the readers by one and return the previous kv from the successful swap 
  {
    KV readKv = curKv;
    do
    {
      if(curKv.key==EMPTY_KEY || readKv.readers<0)//  ||
        return curKv;                                                                // not successful if the key is empty and not successful if readers is below the starting point - the last free function or last reader should be freeing this slot - this relies on nothing incrementing readers once it has reached FREE_READY
         //curKv.readers == FREE_READY) // || 
         //curKv.readers == MAX_READERS) 
    
      readKv           =  curKv;
      readKv.readers  +=  readers;
    } while( !compexchange_kv(i, &curKv.asInt, readKv.asInt) );

    return readKv; // curKv;
  }

public:
  ConcurrentHash(){}
  ConcurrentHash(ui32 sz)
  {
    init(sz);
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
  
    KV desired = empty_kv();
    desired.key  =  key;
    desired.val  =  val;
    ui32      i  =  hash;
    for(;; ++i)
    {
      i  &=  m_sz-1;
      KV probedKv = load_kv(i);
      if(probedKv.key == EMPTY_KEY)
      {          
        KV   expected   =  empty_kv();
        bool   success  =  compexchange_kv(i, &expected.asInt, desired.asInt);
        if( !success && (expected.key!=key) ) continue;                                       // WRONG!? // Another thread just stole it from underneath us.
        else                                  return expected;
      }                                                                                       // Either we just added the key, or another thread did.
      
      if( match(probedKv.key) ){
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
      if( match(probedKv.key) )   return probedKv.val;
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
      if(probedKv.key==EMPTY_KEY) return EMPTY_KEY;
      if( match(probedKv.key) )   return i; // probedKv.val;
    }
  }
  template<class MATCH_FUNC> 
  auto      rmHashed(ui32 hash, MATCH_FUNC match) const -> Reader
  {
    using namespace std;
  
    ui32      i  =  hash;
    for(;; ++i)
    {
      i  &=  m_sz-1;
      KV probedKv = load_kv(i);
      if(probedKv.key==EMPTY_KEY) return empty_reader();
      if(match(probedKv.key))
      {          
        //KV    expected  =  probedKv;
        //KV     desired  =  expected;
//        desired.remove  =  1;
        //bool   success  =  compexchange_kv(i, &expected.asInt, desired.asInt);
        KV kv = addReaders(i, probedKv, -1);
        //if(probedKv.readers==RM_OWNER){
          Reader r    =  empty_reader();       // todo: don't need the initialization - comment it out once working
          r.ch        =  this;
          r.hash_idx  =  i;
          r.kv        =  kv;
          //r.kv        =  desired;
          r.doEnd     =  true;
          return move(r);
        //}else continue;
      }                                                                                       // Either we just added the key, or another thread did.      
    }

    return empty_reader(); // false; // empty_kv();  // should never be reached
  }
  template<class MATCH_FUNC> 
  auto    readHashed(ui32 hash, MATCH_FUNC match) const -> Reader
  {
    using namespace std;

    ui32 i = hash;
    for(;; ++i)
    {
      i &= m_sz - 1;
      KV probedKv = load_kv(i);
      if(probedKv.key==EMPTY_KEY) return empty_reader();         // empty_kv();
      if( match(probedKv.key) ){
        Reader r;
        r.kv        =  addReaders(i, probedKv, 1);
        r.hash_idx  =  i;
        r.doEnd     =  true;
        r.ch        =  this;
        return move(r); // addReaders(i, probedKv, 1);
      }
    }
  }

  bool       init(ui32   sz)
  {
    using namespace std;
    
    m_sz      =  nextPowerOf2(sz);
    KV defKv  =  empty_kv();
    m_kvs.resize(m_sz, defKv);
    
    return true;
  }
  KV         read(ui32  key)                      const
  {
    ui32 i = intHash(key);
    for(;; ++i)
    {
      i  &=  m_sz - 1;

      KV probedKv = load_kv(i);
      if(probedKv.key == key) return addReaders(i, probedKv, 1);
             
      if(probedKv.key == EMPTY_KEY)                                 // needs to be taken out when deleting is implemented
        return empty_kv();
    }

    return empty_kv();
  }
  KV      endRead(ui32  idx)                      const
  {
    KV probedKv = load_kv(idx);
    if(probedKv.key == EMPTY_KEY) return empty_kv();

    return addReaders(idx, probedKv, -1);
  }
  bool        del(ui32  key)                      const
  {
    ui32 i = intHash(key);
    for(;; ++i)
    {
      i  &=  m_sz - 1;

      KV probedKv = load_kv(i);
      if(probedKv.key == key) return compexchange_kv(i, &probedKv.asInt, empty_kv().asInt);
             
      if(probedKv.key == EMPTY_KEY)                                 // needs to be taken out when deleting is implemented
        return false;
    }
    return false;
  }
  KV           rm(ui32  idx)                      const    // doesn't keep track of readers
  {
    return store_kv(idx, empty_kv());
  }
  KV           at(ui32  idx)                      const
  {
    return m_kvs[idx];
  }
  ui32       size()                               const
  {
    return m_sz;
  }
  ui32  hashBytes(void* buf, ui32 len)            const
  {
    ui32  rethash  =  0;
    ui32* cur      =  (ui32*)buf;
    ui32  loops    =  len/sizeof(ui32);
    ui32* end      =  cur + loops + 1;
    for(; cur!=end; ++cur){ rethash ^= intHash(*cur); }

    ui32  rem      =  len - loops;
    ui32  lst      =  0;
    ui8*  end8     =  (ui8*)end;
    for(ui8 i=0; i<rem; ++i){ lst ^= *end8 << (rem-1-i); }
    
    rethash ^= intHash(lst);

    return rethash;
  }
};
class   ConcurrentList
{
public:
  union HeadUnion
  {
    struct { uint32_t cnt; uint32_t idx; };
    uint64_t asInt;
  };
  
  using    ui32  =  uint32_t;  // need to be i32 instead for the ConcurrentStore indices?
  using    ui64  =  uint64_t;
  using ListVec  =  std::vector< std::atomic<ui32> >;  // does this need to be atomic? all the contention should be over the head
  using HeadInt  =  ui64;
  using    Head  =  std::atomic<ui64>;

  const static ui32 LIST_END = 0xFFFFFFFF;

private:
  ListVec  m_lv;
  Head      m_h;

public:
  ConcurrentList(){}
  ConcurrentList(ui32 size) : 
    m_lv(size)
  {
    for(uint32_t i=0; i<(size-1); ++i) m_lv[i]=i+1;
    m_lv[size-1] = LIST_END;

    m_h = 0;
  }

  auto     nxt() -> uint32_t    // moves forward in the list and return the previous index
  {
    HeadUnion  curHead;
    HeadUnion  nxtHead;

    curHead.asInt  =  m_h.load();
    do 
    {
      if(curHead.idx==LIST_END) return LIST_END;

      nxtHead.idx  =  m_lv[curHead.idx];
      nxtHead.cnt  =  curHead.cnt + 1;
    } while( !m_h.compare_exchange_strong(curHead.asInt, nxtHead.asInt) );

    //return nxtHead.idx;
    return curHead.idx;
  }
  auto    free(ui32 idx) -> uint32_t   // not thread safe yet when reading from the list, but it doesn't matter because you shouldn't be reading while freeing anyway?
  {
    HeadUnion  curHead;
    HeadUnion  nxtHead;
    uint32_t    retIdx;

    curHead.asInt = m_h.load();
    do 
    {
      retIdx = m_lv[idx] = curHead.idx;
      nxtHead.idx  =  idx;
      nxtHead.cnt  =  curHead.cnt + 1;
    } while( !m_h.compare_exchange_strong(curHead.asInt, nxtHead.asInt) );

    return retIdx;
  }
  auto   count() const -> uint32_t
  {
    return ((HeadUnion*)(&m_h))->cnt;
  }
  auto     idx() -> uint32_t
  {
    return ((HeadUnion*)(&m_h))->idx;
  }            // not thread safe
  auto    list() -> ListVec const* 
  {
    return &m_lv;
  }            // not thread safe
  ui32  lnkCnt()                     // not thread safe
  {
    ui32    cnt = 0;
    auto      l = list();
    ui32 curIdx = idx();
    while( curIdx != LIST_END ){
      curIdx = l->at(curIdx).load();
      ++cnt;
    }
    
    return cnt;
  }
};
class  ConcurrentStore
{
public:
  using IDX  =  i32;

  const static ui32 LIST_END = ConcurrentList::LIST_END;

private:
  ui8*                    m_addr;
  ui32               m_blockSize;
  ui32              m_blockCount;
  ai32              m_blocksUsed;
  ConcurrentList            m_cl;

  i32*            stPtr(i32  blkIdx)  const
  {
    return (i32*)( ((ui8*)m_addr) + blkIdx*m_blockSize );
  }
  i32          nxtBlock(i32  blkIdx)  const
  {
    return *(stPtr(blkIdx));
  }
  i32     blockFreeSize()             const
  {
    return m_blockSize - sizeof(IDX);
  }
  ui8*     blockFreePtr(i32  blkIdx)  const
  {
    return ((ui8*)stPtr(blkIdx)) + sizeof(IDX);
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
  size_t     writeBlock(i32  blkIdx, void* bytes) // i64 len = -1)
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
    i32   blkFree  =  blockFreeSize();
    ui8*        p  =  blockFreePtr(blkIdx);
    i32       nxt  =  nxtBlock(blkIdx);
    size_t cpyLen  =  nxt<0? -nxt : blkFree;           // if next is negative, then it will be the length of the bytes in that block
    memcpy(bytes, p, cpyLen);

    return cpyLen;
  }

public:
  ConcurrentStore(){}
  ConcurrentStore(ui8* addr, ui32 blockSize, ui32 blockCount) :
    m_addr(addr),
    m_blockSize(blockSize),
    m_blockCount(blockCount),
    m_blocksUsed(0),
    m_cl(m_blockCount)
  {
    assert(blockSize > sizeof(IDX));
  }

  i32        alloc(i32    size, i32* out_blocks=nullptr)
  {
    i32 byteRem = 0;
    i32 blocks  = blocksNeeded(size, &byteRem);
    //if(out_blocks) *out_blocks = blocks;

    i32   st = m_cl.nxt();                                     // stBlk  is starting block
    if(st==LIST_END){
      if(out_blocks) *out_blocks = 0; 
      return LIST_END; 
    }
    i32  cur = st;                                             // curBlk is current  block
    i32  cnt = 0;
    i32  nxt = 0;   // m_cl.nxt();
    for(i32 i=0; i<blocks-1; ++i)
    {
      i32* p = stPtr(cur);
      nxt    = m_cl.nxt();
      if(nxt==LIST_END){ 
        //if(out_blocks) *out_blocks = -cnt;                     // negative count if the full amount of blocks can't be allocated
        break;
      }

      cur     =  nxt;        // m_cl.nxt();
      ++cnt;
      m_blocksUsed.fetch_add(1);
      *p      =  cur;
    }
    i32* p  =  (i32*)stPtr(cur);
    *p      =  byteRem? -byteRem : -blockFreeSize();
    if(out_blocks){ *out_blocks = nxt==LIST_END? -cnt : cnt; } 

    return st;
  }
  void        free(i32     idx)        // frees a list/chain of blocks
  {
    i32   cur   =  idx;                // cur is the current block index
    i32    nxt  =  *stPtr(cur);        // nxt is the next block index
    for(; nxt>0; nxt=*stPtr(cur) ){ 
      m_cl.free(cur);
      m_blocksUsed.fetch_add(-1);
      cur  =  nxt;
    }
    m_cl.free(cur);
  }
  void         put(i32  blkIdx, void* bytes, i32 len)
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
  size_t       get(i32  blkIdx, void* bytes) const
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
  bool     compare(void* buf, size_t len, IDX blkIdx) const
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
        if(curlen < blksz){ return false; }
        else if( memcmp(curbuf, p, blksz)!=0 ){ return false; }
      }else if(-nxt != curlen){ return false; }
      else{ return memcmp(curbuf, p, curlen)==0; }

      curbuf  +=  blksz;
      curlen  -=  blksz;
      curidx   =  nxt;
      nxt      =  nxtBlock(curidx);
    }

    return true;     
  }
  auto        list() const -> ConcurrentList const&
  {
    return m_cl;
  }
  auto        data() const -> const void*
  {
    return (void*)m_addr;
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
        size);
      if(sm.hndlPtr==nullptr){ CloseHandle(sm.fileHndl); sm.clear(); return move(sm); }
    #endif       // END windows
  
    ui64      addr = (ui64)(sm.hndlPtr);
    ui64 alignAddr = (addr + ((64-addr%64)%64));
    sm.ptr    = (void*)(alignAddr);

    return move(sm);
  }

  SharedMem(){}
  SharedMem(SharedMem const&) = delete;
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
  using KV = ConcurrentHash::KV;

private:
  using Reader = ConcurrentHash::Reader;

  //void*            m_mem;
  SharedMem        m_mem;
  ConcurrentStore   m_cs;     // store data in blocks and get back indices
  ConcurrentHash    m_ch;     // store the indices of keys and values - contains a ConcurrentList

  static const ui32  EMPTY_KEY = ConcurrentHash::EMPTY_KEY;          // 28 bits set 
  static const ui32   LIST_END = ConcurrentStore::LIST_END;
  static bool   CompareBlock(simdb const* const ths, void* buf, size_t len, i32 blkIdx)
  { 
    return ths->m_cs.compare(buf, len, blkIdx);
  }

  auto  getFromBlkIdx(i32  blkIdx, void*  out_buf) const -> ui64        // getFromBlkIdx is get from block index
  {
    if(blkIdx==EMPTY_KEY) return 0;

    return m_cs.get(blkIdx, out_buf);           // copy into the buf starting at the blkidx
  } 
  auto           read(void*   key, i32    len) const -> Reader
  {
    ui32 keyhash  =  m_ch.hashBytes(key, len);
    auto     ths  =  this;
    return m_ch.readHashed(keyhash, 
      [ths, key, len](ui32 blkidx){ return CompareBlock(ths,key,len,blkidx); });
  }

public:
  simdb(){}
  simdb(const char* name, ui32 blockSize, ui32 blockCount) : 
    m_mem( SharedMem::AllocAnon(name, blockSize*blockCount) ),
    m_cs( (ui8*)m_mem.data(), blockSize, blockCount),               // todo: change this to a void*
    m_ch( blockCount )
  {}

  i32      put(void*   key, ui32  klen, void* val, ui32 vlen)
  {
    i32 blkcnt = 0;
    i32   kidx = m_cs.alloc(klen, &blkcnt);    // kidx is key index
    if(kidx==LIST_END) return EMPTY_KEY;
    if(blkcnt<0){
      m_cs.free(kidx);
      return EMPTY_KEY;
    }
    i32   vidx = m_cs.alloc(vlen, &blkcnt);
    if(vidx==LIST_END) return EMPTY_KEY;       // vidx is value index
    if(blkcnt<0){
      m_cs.free(vidx);
      return EMPTY_KEY;
    }

    m_cs.put(kidx, key, klen);
    m_cs.put(vidx, val, vlen);

    ui32 keyhash = m_ch.hashBytes(key, klen);
    auto     ths = this;                                          // this silly song and dance is because the this pointer can't be passed to a lambda
    KV        kv = m_ch.putHashed(keyhash, kidx, vidx, 
      [ths, key, klen](ui32 blkidx){ return CompareBlock(ths,key,klen,blkidx); });

    if(kv.key != ConcurrentHash::EMPTY_KEY){ m_cs.free(kv.val); m_cs.free(kv.key); }

    return kidx;
  }
  i64      get(const std::string key, void* out_buf)
  {
    Reader r = read( (void*)key.data(), (ui32)key.length() );
    if(r.kv.key==EMPTY_KEY || r.kv.readers<=0) return -1;            // after the read, the readers should be at least 1  /*|| r.kv.remove*/

    ui64 len = getFromBlkIdx(r.kv.val, out_buf);
    if(r.doRm()){ m_cs.free(r.kv.val); m_cs.free(r.kv.key); }
  
    return len;
  }
  void      rm(const std::string key)
  {
    auto  len = (ui32)key.length();
    auto  ths = this;
    auto kbuf = (void*)key.data();
    auto hash = m_ch.hashBytes(kbuf, len);
    //ui32  idx = m_ch.rmHashed(hash,
    Reader  r = m_ch.rmHashed(hash,                          // need to get a reader back
      [ths, kbuf, len](ui32 blkidx){ return CompareBlock(ths,kbuf,len,blkidx); });
    
    if(r.doRm()){                                            // need to check doRm
      if(r.kv.val!=EMPTY_KEY) m_cs.free(r.kv.val);
      if(r.kv.key!=EMPTY_KEY) m_cs.free(r.kv.key);           // need to remove the block index lists if true
    }
        
    //KV  prev = m_ch.rm(idx);
    //if(prev.key!=EMPTY_KEY) m_cs.free(prev.key);
    //if(prev.val!=EMPTY_KEY) m_cs.free(prev.val);
  }
  auto    data() const -> const void*
  {
    return m_cs.data();
  }
  ui64    size() const
  {
    return m_mem.size;
  }
  bool isOwner() const
  {
    return m_mem.owner;
  }

};

#endif













