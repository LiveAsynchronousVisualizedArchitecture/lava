
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

// todo: store size in the ConcurrentList? list isn't atomic so it should work well? should block lists, key sizes, and val sizes all be in their own lava_vecs ? 
// todo: redo concurrent store get to store length so that buffer can be returned
// todo: test with multiple threads in a loop
// todo: store lengths and check key lengths before trying bitwise comparison as an optimization? - would only make a difference for long keys that are larger than one block? no it would make a difference on every get?
// todo: prefetch memory for next block when looping through blocks
// todo: move to using packed key value in one block list
// todo: can block sync be done with a removed flag instead of just using readers?


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

namespace {
  enum Match { MATCH_FALSE=0, MATCH_TRUE=1, MATCH_REMOVED=-1  };
}

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

class   ConcurrentList
{
public:
  union Head
  {
    struct { ui32 ver; ui32 idx; };   // ver is version, idx is index // todo: change cnt to version
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
  //union BlkIdx
  //{
  //  struct
  //  {
  //    signed   long long readers  : 12;     // 2^11 is 2048, so 2048 maximum threads can read this one block index
  //    unsigned long long     idx  : 52;     // 4 quadrillion indices should be enough for anybody
  //  };
  //  ui64 asInt;
  //};
  
  struct BlkLst
  {
    i32   readers;
    ui32      idx;
  };

  using IDX         =  i32;
  using ai32        =  std::atomic<i32>;
  using BlockLists  =  lava_vec<BlkLst>;
  //using BlockLists  =  lava_vec<IDX>;

  const static ui32 LIST_END = ConcurrentList::LIST_END;

  bool      incReaders(ui32 blkIdx)        const                   // BI is Block Index  increment the readers by one and return the previous kv from the successful swap 
  {
    ai32* aidx = (ai32*)&(m_bls[blkIdx].readers);
    i32    cur = aidx->load();
    i32    nxt;
    do{
      if(cur<0) return false;
      nxt = cur + 1;
    }while( !aidx->compare_exchange_strong(cur, nxt) );
    
    return true;
  }
  bool      decReaders(ui32 blkIdx)        const                   // BI is Block Index  increment the readers by one and return the previous kv from the successful swap 
  {
    ai32* aidx = (ai32*)&(m_bls[blkIdx].readers);
    auto prev = aidx->fetch_add(-1);
    if(prev==0){ doFree(blkIdx); return false; }
    
    return true;
  }

  //BlkLst      decReaders(ui32 blkIdx)         const                   // BI is Block Index  increment the readers by one and return the previous kv from the successful swap 
  //{
  //  std::atomic_fetch_add( (aui32*)&(m_bls[blkIdx].readers), -1);
  //}
    
  //  BlkIdx   rdBI, prevBI;                                                          // rdBI   is read block index 
  //  //BlkIdx prevBI = *curBI;                                                       // prevBI is previous block index 
  //  do{
  //    prevBI.asInt = rdBI.asInt = atomic_load( (aui32*)curBI );
  //    if(readers>0 && rdBI.readers<0) return *curBI;                                // not successful if the key is empty and not successful if readers is below the starting point - the last free function or last reader should be freeing this slot - this relies on nothing incrementing readers once it has reached FREE_READY
  //  
  //    rdBI           =  *curBI;
  //    rdBI.readers  +=   readers;
  //  //} while( !compexchange_kv(i, (ui64*)curBI, rdBI.asInt) );
  //  } while( ((aui32*)curBI)->compare_exchange_strong(prevBI.asInt, rdBI.asInt) );
  //  //atomic_compare_exchange_strong( (aui64*)&m_kvs.data()[i].asInt, expected, desired);  
  //
  //  return rdBI;
  //}


private:
  mutable ConcurrentList    m_cl;       // flat data structure - pointer to memory 
  mutable BlockLists       m_bls;       // flat data structure - pointer to memory - bl is Block Lists

  void*               m_blksAddr;
  ui32               m_blockSize;
  ui32              m_blockCount;
  mutable ai32      m_blocksUsed;
  ui64                 m_szBytes;

  //i32*            stPtr(i32  blkIdx)  const
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
    return m_bls[blkIdx].idx;
  }
  i32     blockFreeSize()             const
  {
    //return m_blockSize - sizeof(IDX);
    return m_blockSize;
  }
  ui8*     blockFreePtr(i32  blkIdx)  const
  {
    //return ((ui8*)stPtr(blkIdx)) + sizeof(IDX);
    return ((ui8*)m_blksAddr) + blkIdx*m_blockSize;
  }
  ui8*           blkPtr(i32  blkIdx)  const
  {
    //return ((ui8*)stPtr(blkIdx)) + sizeof(IDX);
    return ((ui8*)m_blksAddr) + blkIdx*m_blockSize;
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
    i32   nxt  =   m_bls[cur].idx;         //*stPtr(cur);              // nxt is the next block index
    for(; nxt>0; nxt=m_bls[cur].idx){ 
      memset(blkPtr(cur), 0, m_blockSize);    // zero out memory on free, 
      m_bls[cur].readers = 0;              // reset the reader count
      m_cl.free(cur);
      m_blocksUsed.fetch_add(-1);
      cur  =  nxt;
    }
    memset(blkPtr(cur), 0, m_blockSize);       // 0 out memory on free, 
    m_bls[cur].readers = 0;              // reset the reader count
    m_cl.free(cur);
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
  static ui64     BlksOfst(ui32 blockSize, ui32 blockCount)
  {
    return ConcurrentList::sizeBytes(blockCount) + BlockLists::sizeBytes(blockCount);
  }
  static ui64    CListOfst(ui32 blockCount)
  {
    return BlockLists::sizeBytes(blockCount);
  }
  static ui64    sizeBytes(ui32 blockSize, ui32 blockCount)
  {
    return ConcurrentList::sizeBytes(blockCount) + BlockLists::sizeBytes(blockCount) + blockSize*blockCount;
  }

  ConcurrentStore(){}
  ConcurrentStore(void* addr, ui32 blockSize, ui32 blockCount, bool owner=true) :
    m_blksAddr( (i8*)addr + BlksOfst(blockSize,blockCount) ),
    m_blockSize(blockSize),
    m_blockCount(blockCount),
    m_blocksUsed(0),
    //m_cl(m_blockCount)
    m_cl( (i8*)addr + CListOfst(blockCount), blockCount, owner),
    m_bls( addr, blockCount, owner),
    m_szBytes( *((ui64*)addr) )
  {
    //if(owner) *((ui64*)addr) = blockCount;

    if(owner){
      //BlkLst def; 
      //def.idx     = 0; 
      //def.readers = 0; 
      //for(ui64 i=0; i<m_bls.size(); ++i) m_bls[i]=def;
    }
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
      //i32* p = stPtr(cur);
      nxt    = m_cl.nxt();
      if(nxt==LIST_END){ 
        //if(out_blocks) *out_blocks = -cnt;                     // negative count if the full amount of blocks can't be allocated
        break;
      }

      m_bls[cur] = { 0, nxt };   // { readers, index }
      cur        =  nxt;         // m_cl.nxt();
      ++cnt;
      m_blocksUsed.fetch_add(1);
      //*p      =  cur;
    }
    //i32* p  =  (i32*)stPtr(cur);
    //*p      =  byteRem? -byteRem : -blockFreeSize();

    BlkLst bl  = { 0, byteRem? -byteRem : -blockFreeSize() };
    m_bls[cur] = bl;
    //m_bls[cur].readers =  0;
    //m_bls[cur].idx     =  byteRem? -byteRem : -blockFreeSize();
    if(out_blocks){ *out_blocks = nxt==LIST_END? -cnt : cnt; } 

    return st;
  }
  bool        free(i32  blkIdx)        // frees a list/chain of blocks
  {
    return decReaders(blkIdx);

    //if(decReaders(blkIdx)){
    //  doFree(blkIdx);
    //  return true;
    //}else return false;
  }
  //void        free(i32     idx)        // frees a list/chain of blocks
  //{
  //  i32   cur  =  idx;                // cur is the current block index
  //  i32   nxt  =  *stPtr(cur);        // nxt is the next block index
  //  for(; nxt>0; nxt=*stPtr(cur) ){ 
  //    memset(stPtr(cur), 0, m_blockSize);       // 0 out memory on free, 
  //    m_cl.free(cur);
  //    m_blocksUsed.fetch_add(-1);
  //    cur  =  nxt;
  //  }
  //  memset(stPtr(cur), 0, m_blockSize);       // 0 out memory on free, 
  //  m_cl.free(cur);
  //}
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
  size_t        get(i32  blkIdx, void* bytes)          const
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
  Match     compare(void* buf, size_t len, IDX blkIdx) const
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
  auto        list() const -> ConcurrentList const&
  {
    return m_cl;
  }
  auto        data() const -> const void*
  {
    return (void*)m_blksAddr;
  }
  ui64  blockCount() const
  {
    return 0; // m_cl.sizeBytes();
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
    //Reader(Reader const& lval) = delete;
    Reader(Reader&) = delete;
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

      //if(ch) ch->endRead(hash_idx);              // checks the ConcurrentHash pointer for nullptr, not sure if this is neccesary but it probably doesn't hurt much
    }
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

  //const static ui32 EMPTY_KEY = 0xFFFFFFFF;

private:
  using i8        =  int8_t;
  using ui32      =  uint32_t;
  using ui64      =  uint64_t;
  using Aui32     =  std::atomic<ui32>;
  using Aui64     =  std::atomic<ui64>;  
  //using KVs       =  std::vector<KV>;
  using KVs       =  lava_vec<KV>;
  using Mut       =  std::mutex;
  using UnqLock   =  std::unique_lock<Mut>;
  //using Match     =  ConcurrentStore::Match; 

         ui32   m_sz;
  mutable KVs   m_kvs;

  //template<class MATCH_FUNC>
  //Match readAndMatch(ui32 kvIdx, KV* inout_curKv, MATCH_FUNC match) const
  //{
  //  Match ret = MATCH_FALSE;
  //  *inout_curKv = addReaders(kvIdx, inout_curKv,  1);
  //  if(match(key)) ret=MATCH_TRUE;
  //  *inout_curKv = addReaders(kvIdx, inout_curKv, -1);
  //
  //  return ret;
  //}

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
    using namespace std;
    
    Reader r;
    r.kv       = empty_kv();
    r.ch       = nullptr;                    // this;
    r.doEnd    = false;
    r.hash_idx = EMPTY_KEY;                  // EMPTY_KEY is used for actual keys and values, but this is an index into the KV vector

    return move(r);
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

    return atomic_compare_exchange_strong( (Aui64*)&(m_kvs.data()[i].asInt), expected, desired);                      // The entry was free. Now let's try to take it using a CAS. 
  }
  KV         addReaders(ui32  i, KV* curKv, i8 readers)        const                         // increment the readers by one and return the previous kv from the successful swap 
  {
  // todo: does curKv need to be a pointer?
    KV readKv = *curKv;     // todo: does this need to be in the loop? - no it is assigned inside
    do
    {
      if(curKv->key==EMPTY_KEY || (readers>0 && readKv.readers<0) )//  ||
        return *curKv;                                                                // not successful if the key is empty and not successful if readers is below the starting point - the last free function or last reader should be freeing this slot - this relies on nothing incrementing readers once it has reached FREE_READY
         //curKv.readers == FREE_READY) // || 
         //curKv.readers == MAX_READERS) 
    
      readKv           =  *curKv;
      readKv.readers  +=   readers;
    } while( !compexchange_kv(i, (ui64*)curKv, readKv.asInt) );
    //} while( !compexchange_kv(i, &curKv.asInt, readKv.asInt) );

    return readKv; // curKv;
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
        //if( !success && (expected.key!=key) ) continue;                                       // WRONG!? // Another thread just stole it from underneath us.
        //else                                  return expected;
        if( success /* && (expected.key!=key) */ ) return expected;  // continue;                                       // WRONG!? // Another thread just stole it from underneath us.
        else{ --i; continue; }
      }                                                                                       // Either we just added the key, or another thread did.
      
      if( match(probedKv.key)==MATCH_TRUE ){
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
  KV      rmHashed(ui32 hash, MATCH_FUNC match) const // -> Reader
  {
    //using namespace std;
  
    ui32 i = hash;
    for(;; ++i)
    {
      i  &=  m_sz-1;
      KV probedKv = load_kv(i);
      if(probedKv.key==EMPTY_KEY) return empty_kv();  // probedKv; // MATCH_FALSE; // empty_reader();
      Match m = match(probedKv.key);
      if(m==MATCH_TRUE){
        auto empty = empty_kv();
        if( compexchange_kv(i, &probedKv.asInt, empty.asInt) ) return probedKv;
        else --i;                                     // run the same loop iteration again if there was a match, but the key-value changed
      }

//      {          
//        //KV    expected  =  probedKv;
//        //KV     desired  =  expected;
////        desired.remove  =  1;
//        //bool   success  =  compexchange_kv(i, &expected.asInt, desired.asInt);
//
//        //probedKv.key;
//        //probedKv.val;
//
//        KV kv = probedKv; //addReaders(i, probedKv, -1);
//        //if(probedKv.readers==RM_OWNER){
//          Reader r    =  empty_reader();       // todo: don't need the initialization - comment it out once working
//          r.ch        =  this;
//          r.hash_idx  =  i;
//          r.kv        =  kv;
//          //r.kv        =  desired;
//          r.doEnd     =  true;
//          return move(r);
//        //}else continue;
//      }                                                                                       // Either we just added the key, or another thread did.      
    }

    return empty_kv();
    
    // return empty_reader(); // false; // empty_kv();  // should never be reached
  }
  template<class MATCH_FUNC> 
  KV    readHashed(ui32 hash, MATCH_FUNC match) const // -> Reader
  {
    using namespace std;

    ui32 i = hash;
    for(;; ++i)
    {
      i &= m_sz - 1;
      KV probedKv = load_kv(i);
      if(probedKv.key==EMPTY_KEY) return empty_kv();
      if( match(probedKv.key)==MATCH_TRUE ) return probedKv;

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

  bool       init(ui32   sz)
  {
    using namespace std;
    
    m_sz      =  nextPowerOf2(sz);
    m_kvs     =  lava_vec<KV>(m_sz);
    KV defKv  =  empty_kv();
    for(ui64 i=0; i<m_kvs.size(); ++i) m_kvs[i] = defKv;
    
    return true;

    //m_kvs.resize(m_sz, defKv);
  }
  //KV         read(ui32  key)                      const
  //{
  //  ui32 i = intHash(key);
  //  for(;; ++i)
  //  {
  //    i  &=  m_sz - 1;
  //
  //    KV probedKv = load_kv(i);
  //    //if(probedKv.key == key) return addReaders(i, probedKv, 1);
  //           
  //    if(probedKv.key == EMPTY_KEY)                                 // needs to be taken out when deleting is implemented
  //      return empty_kv();
  //  }
  //
  //  return empty_kv();
  //}
  //KV      endRead(ui32  idx)                      const
  //{
  //  KV probedKv = load_kv(idx);
  //  if(probedKv.key == EMPTY_KEY) return empty_kv();
  //
  //  return addReaders(idx, probedKv, -1);
  //}
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
  //KV           rm(ui32  idx)                      const    // doesn't keep track of readers
  //{
  //  return store_kv(idx, empty_kv());
  //}
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
    ui32* end      =  cur + loops;
    for(; cur!=end; ++cur){ rethash ^= intHash(*cur); }

    ui32  rem      =  len - loops;
    ui32  lst      =  0;
    ui8*  end8     =  (ui8*)end;
    for(ui8 i=0; i<rem; ++i){ lst ^= *end8 << (rem-1-i); }
    
    rethash ^= intHash(lst);

    return rethash;
  }  // todo: make static
  ui64  sizeBytes()                               const
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
        size);
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
  //using Match = ConcurrentStore::Match; 
  using KV    = ConcurrentHash::KV;

private:
  using Reader = ConcurrentHash::Reader;

  //void*            m_mem;
  SharedMem          m_mem;
  aui64*           m_flags;
  aui64*       m_blockSize;
  aui64*      m_blockCount;
  ConcurrentStore     m_cs;     // store data in blocks and get back indices
  ConcurrentHash      m_ch;     // store the indices of keys and values - contains a ConcurrentList

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
    return ths->m_cs.compare(buf, len, blkIdx);
  }

  auto  getFromBlkIdx(i32  blkIdx, void*  out_buf) const -> ui64        // getFromBlkIdx is get from block index
  {
    if(blkIdx==EMPTY_KEY) return 0;

    return m_cs.get(blkIdx, out_buf);           // copy into the buf starting at the blkidx
  } 
  KV             read(void*   key, i32        len) const // -> Reader
  {
    using namespace std;
    
    ui32 keyhash  =  m_ch.hashBytes(key, len);
    auto     ths  =  this;
    return m_ch.readHashed(keyhash, 
      [ths, key, len](ui32 blkidx){ return CompareBlock(ths,key,len,blkidx); });
  }

public:
  simdb(){}
  simdb(const char* name, ui32 blockSize, ui32 blockCount) : 
    m_mem( SharedMem::AllocAnon(name, MemSize(blockSize,blockCount)) ),
    m_ch( ((i8*)m_mem.data())+OffsetBytes(), blockCount, m_mem.owner),
    m_cs( ((i8*)m_mem.data())+m_ch.sizeBytes(blockCount)+OffsetBytes(), blockSize, blockCount, m_mem.owner),                 // todo: change this to a void*
    m_blockCount( ((aui64*)m_mem.data())+2 ),
    m_blockSize(  ((aui64*)m_mem.data())+1 ),
    m_flags(       (aui64*)m_mem.data() )
  {
    if(isOwner()){
      m_blockCount->store(blockCount);
      m_blockSize->store(blockSize);
      m_flags->store(1);                                        // set to 1 to signal construction is done
    }
    else{                                                       // need to spin until ready
      // todo: need to read the blockCount and blockSize values - not use the constructor
      while(m_flags->load()==false){}
      new (&m_ch) ConcurrentHash( ((i8*)m_mem.data())+OffsetBytes(), blockCount, m_mem.owner);
      new (&m_cs) ConcurrentStore( ((i8*)m_mem.data())+m_ch.sizeBytes(blockCount)+OffsetBytes(), blockSize, blockCount, m_mem.owner);                 // todo: change this to a void*
    }
  }

  i32       put(void*   key, ui32  klen, void* val, ui32 vlen)
  {
    i32 blkcnt = 0;
    i32   kidx = m_cs.alloc(klen, &blkcnt);    // kidx is key index
    if(kidx==LIST_END) return EMPTY_KEY;
    if(blkcnt<0){
      m_cs.free(kidx);
      return EMPTY_KEY;
    }
    i32   vidx = m_cs.alloc(vlen, &blkcnt);    // if there aren't enough blocks, the blkCnt wil be the amount of blocks that got allocated anyway * -1.  
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

    if(kv.val!=EMPTY_KEY) m_cs.free(kv.val);
    if(kv.key!=EMPTY_KEY) m_cs.free(kv.key);

    //if(kv.key != ConcurrentHash::EMPTY_KEY){ m_cs.free(kv.val); m_cs.free(kv.key); }

    return kidx;
  }
  i64       get(const std::string key, void* out_buf)
  {
    //Reader r = read( (void*)key.data(), (ui32)key.length() );
    //if(r.kv.key==EMPTY_KEY || r.kv.readers<=0) return -1;            // after the read, the readers should be at least 1  /*|| r.kv.remove*/
    KV kv = read( (void*)key.data(), (ui32)key.length() );
    if(kv.key==EMPTY_KEY) return -1; // || r.kv.readers<=0) return -1;            // after the read, the readers should be at least 1  /*|| r.kv.remove*/

    ui64 len = getFromBlkIdx(kv.val, out_buf);

    //if(kv.val!=EMPTY_KEY) m_cs.free(kv.val);
    //if(kv.key!=EMPTY_KEY) m_cs.free(kv.key);

    //if(r.doRm()){ m_cs.free(kv.val); m_cs.free(kv.key); }
  
    return len;
  }
  i64       get(const char *key, void* out_buf)
  {
    //Reader r = read( (void*)key, (ui32)strlen(key));
    //if(r.kv.key==EMPTY_KEY || r.kv.readers<=0){ return -1; }

    KV kv = read( (void*)key, (ui32)strlen(key));
    if(kv.key==EMPTY_KEY){ return -1; }

    ui64 len = getFromBlkIdx(kv.val, out_buf);

    //if(r.doRm()){ m_cs.free(r.kv.val); m_cs.free(r.kv.key); }

    return len;
  }
  void       rm(const std::string key)
  {
    auto  len = (ui32)key.length();
    auto  ths = this;
    auto kbuf = (void*)key.data();
    auto hash = m_ch.hashBytes(kbuf, len);
    //ui32  idx = m_ch.rmHashed(hash,
    //Reader  r = m_ch.rmHashed(hash,
    //  [ths, kbuf, len](ui32 blkidx){ return CompareBlock(ths,kbuf,len,blkidx); });
    KV kv = m_ch.rmHashed(hash,
      [ths, kbuf, len](ui32 blkidx){ return CompareBlock(ths,kbuf,len,blkidx); });

    if(kv.val!=EMPTY_KEY) m_cs.free(kv.val);
    if(kv.key!=EMPTY_KEY) m_cs.free(kv.key);
    
    //if(r.doRm()){                                            // need to check doRm
    //  if(r.kv.val!=EMPTY_KEY) m_cs.free(r.kv.val);
    //  if(r.kv.key!=EMPTY_KEY) m_cs.free(r.kv.key);           // need to remove the block index lists if true
    //}
        
    //KV  prev = m_ch.rm(idx);
    //if(prev.key!=EMPTY_KEY) m_cs.free(prev.key);
    //if(prev.val!=EMPTY_KEY) m_cs.free(prev.val);
  }
  auto     data() const -> const void*
  {
    return m_cs.data();
  }
  ui64     size() const
  {
    return m_mem.size;
  }
  bool  isOwner() const
  {
    return m_mem.owner;
  }

};

#endif








//union BlkIdx
//{
//  struct
//  {
//    // todo: does this need to have a version number with it?
//    signed   int  readers  :  8;     // 2^7 is 128, so 128 maximum threads can read this one block index
//    unsigned int      idx  : 24;     // 16 million indices
//  };
//  ui32 asInt;
//};

//m_cs( ((ui8*)m_mem.data())+ConcurrentHash::sizeBytes(blockCount), blockSize, blockCount)                 // todo: change this to a void*
//m_cs( (ui8*)m_mem.data(), blockSize, blockCount),               // todo: change this to a void*

//((ui64*)((i8*)p+data_offset))[i];
//T* ofst = (T*)((ui64*)p+2);
//return ofst[i];

//auto lv = *(m_cl.list());
//for(ui64 i=0; i<lv.size(); ++i) std::cout << lv[i] << " ";
////std::cout << "\n" << ((ConcurrentList::Head*)&lv[-4])->idx << "\n" << std::endl;
//std::cout << "\n" << ((ConcurrentList::Head*)lv.addr())->idx << "\n" << std::endl;

