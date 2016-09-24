

// todo: lock init with mutex
// todo: implement locking resize

//Block based allocation
//-Checking if the head has been touched means either incrementing a counter every time it is written, or putting in a thread id every time it is read or written
//-Each ui32 in a vector holds the position of the next free block 
//-Alloc checks the head to read the next free position and puts it in head if head hasn't been touched. 
//-Free checks the head to read the next free position, stores the head value in the free position, then moves head to the just written new free position if head hasn't been touched

// Make block size for keys different than data
// Make frees happen from the last block to the first
// mark free cells as negative numbers so double free is caught?

// idea: use max_waste as a recipricol power of two giving the percentage of waste allowed for an allocation
// - 2 would be 25%, 3 would be 12.5% 
// - not allowed to be below 2
// idea: put atomic reader counter into each ConcurrentStore entry as a signed integer
// idea: figure out how to make ConcurrentHash a flat data structure so it can sit in shared memory

#ifndef __CONCURRENTMAP_HEADER_GUARD__
#define __CONCURRENTMAP_HEADER_GUARD__

#include <cstdint>
#include <atomic>
#include <mutex>
#include <memory>
#include <vector>

using  ui8       =   uint8_t;
using  i64       =   int64_t;
using  ui64      =   uint64_t;
using  i32       =   int32_t;
using  ui32      =   uint32_t;
using  f32       =   float;
using  f64       =   double;
using  aui64     =   std::atomic<ui64>;
using  ai32      =   std::atomic<i64>;
using  cstr      =   const char*;
using  str       =   std::string;

class ConcurrentHash
{
private:
  //using Aui64  =  std::atomic<ui64>;

public:
  static const ui8   INIT_READERS  =     0;
  static const ui8   FREE_READY    =     0;
  static const ui8   MAX_READERS   =  0xFF;
  static const ui32  EMPTY_KEY     =  0x0FFFFFFF;      // 28 bits set 

  union kv
  {
    struct
    {
      uint64_t  readers  :  8;
      uint64_t      key  : 28;
      uint64_t      val  : 28;
    };
    ui64 asInt;
  };

private:
  using i8        =  int8_t;
  using ui32      =  uint32_t;
  using ui64      =  uint64_t;
  using Aui32     =  std::atomic<ui32>;  
  using Aui64     =  std::atomic<ui64>;  
  using KVs       =  std::vector<kv>;
  using Mut       =  std::mutex;
  using UnqLock   =  std::unique_lock<Mut>;

  //const static ui32 EMPTY_KEY = 0xFFFFFFFF;

  ui32               m_sz;

  mutable KVs       m_kvs;
  //mutable Mut        m_mut;

  //ui32         load_key(ui32  i)          const
  //{
  //  using namespace std;
  //  
  //  kv keyval;
  //  keyval.asInt    =  atomic_load<ui64>( (Aui64*)&m_kvs.data()[i].asInt );              // Load the key that was there.
  //  ui32 probedKey  =  keyval.key;
  //
  //  return probedKey;
  //}

  kv           empty_kv()                  const
  {
    kv empty;
    empty.readers  =  INIT_READERS;
    empty.key      =  EMPTY_KEY;
    empty.val      =  EMPTY_KEY;
    return empty;
  }
  ui32          intHash(ui32  h)           const
  {
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
  }
  ui32     nextPowerOf2(ui32  v)           const
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
  kv            load_kv(ui32  i)           const
  {
    using namespace std;
    
    kv keyval;
    keyval.asInt   =  atomic_load<ui64>( (Aui64*)&m_kvs.data()[i].asInt );              // Load the key that was there.
    return keyval;
  }
  void         store_kv(ui32  i, kv _kv)   const
  {
    using namespace std;
    
    atomic_store<ui64>( (Aui64*)&m_kvs[i].asInt, _kv.asInt );
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
  kv         addReaders(ui32  i, kv curKv, i8 readers)         const                         // increment the readers by one and return the previous kv from the successful swap 
  {
    kv readKv = curKv;
    do
    {
      if(curKv.key     == EMPTY_KEY  ||
         curKv.readers == FREE_READY || 
         curKv.readers == MAX_READERS) 
        return curKv;                                                                // not successful if the key is empty and not successful if readers is below the starting point - the last free function or last reader should be freeing this slot - this relies on nothing incrementing readers once it has reached FREE_READY
    
      readKv           =  curKv;
      readKv.readers  +=  readers;
    } while( !compexchange_kv(i, &curKv.asInt, readKv.asInt) );

    return curKv;
  }

  //kv         incReaders(ui32  i, kv curKv) const                                     // increment the readers by one and return the previous kv from the successful swap 
  //{
  //  return addReaders(i, curKv, 1);
  //}
  //kv         decReaders(ui32  i, kv curKv) const                                     // increment the readers by one and return the previous kv from the successful swap 
  //{
  //  return addReaders(i, curKv, -1);
  //}

  //void         resize(size_t sz) const
  //{
  //  //UnqLock lock;
  //  //  m_keys.resize(sz);
  //  //  m_vals.resize(sz);
  //  //lock.unlock();
  //}

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

  bool      init(ui32   sz)
  {
    using namespace std;
    
    m_sz      =  nextPowerOf2(sz);

    kv defKv  =  empty_kv();
    // defKv.asInt    =  0;
    //defKv.key      =  EMPTY_KEY;
    //defKv.val      =  0;
    //defKv.readers  =  INIT_READERS;

    m_kvs.resize(m_sz, defKv);
    
    //for(ui32 i=0; i<m_sz; ++i) m_kvs[i] = defKv;

    //m_keys.reset(new Aui32[m_sz]);
    //m_vals.reset(new Aui32[m_sz]);
    //
    //ui32* keys = (ui32*)m_keys.get();
    //ui32* vals = (ui32*)m_vals.get();
    //TO(m_sz,i) {
    //  keys[i] = EMPTY_KEY;
    //}
    //memset(vals,0,m_sz);

    return true;
  }
  ui32       put(ui32  key, ui32 val) const
  {
    using namespace std;

    kv desired;
    desired.key  =  key;
    desired.val  =  val;
    ui32      i  =  intHash(key);
    for(;; ++i)
    {
      i  &=  m_sz-1;

      //ui32 probedKey = load_key(i);
      kv probedKv = load_kv(i);
      if(probedKv.key != key)
      {
        if(probedKv.key != EMPTY_KEY) continue;                                               // The entry was either free, or contains another key.  // Usually, it contains another key. Keep probing.
                
        kv   expected   =  empty_kv();
        //expected.asInt  =  0;
        //expected.key    =  EMPTY_KEY;

        bool   success  =  compexchange_kv(i, &expected.asInt, desired.asInt);
        //bool   success  =  m_keys.get()[i].compare_exchange_strong(desired, key);           // The entry was free. Now let's try to take it using a CAS. 
        if( !success && (expected.key!=key) ) continue;                                       // Another thread just stole it from underneath us.
      }                                                                                       // Either we just added the key, or another thread did.

      //m_vals.get()[i].store(val);
      store_kv(i, desired);
      return i;
    }
    return i;
  }
  ui32       get(ui32  key)           const
  {
    ui32 i = intHash(key);
    for(;; ++i)
    {
      i  &=  m_sz - 1;

      //ui32 probedKey =  m_keys.get()[i].load();        // atomic_load( (Aui32*)(&m_keys.get()[i]) );   //    // mint_load_32_relaxed(&m_entries[idx].key);

      kv probedKv = load_kv(i);
      if(probedKv.key==key) return probedKv.val;         // m_vals.get()[i].load();                 // atomic_load( (Aui32*)(&m_keys.get()[i]) );         // mint_load_32_relaxed(&m_entries[idx].value);

      //return m_vals.get()[i].load();                   // atomic_load( (Aui32*)(&m_keys.get()[i]) );         // mint_load_32_relaxed(&m_entries[idx].value);

      if(probedKv.key==EMPTY_KEY)
        return EMPTY_KEY;
    }

    return EMPTY_KEY;
  }
  kv        read(ui32  key)           const
  {
    ui32 i = intHash(key);
    for(;; ++i)
    {
      i  &=  m_sz - 1;

      kv probedKv = load_kv(i);
      if(probedKv.key == key) return addReaders(i, probedKv, 1);
             
      if(probedKv.key == EMPTY_KEY)                                 // needs to be taken out when deleting is implemented
        return empty_kv();
    }

    return empty_kv();
  }
  kv     endRead(ui32  key)           const
  {
    ui32 i = intHash(key);
    for(;; ++i)
    {
      i  &=  m_sz - 1;

      kv probedKv = load_kv(i);
      if(probedKv.key == key){
        return addReaders(i, probedKv, -1);
      }
             
      if(probedKv.key == EMPTY_KEY)                                 // needs to be taken out when deleting is implemented
        return empty_kv();
    }
    return empty_kv();
  }
  bool       del(ui32  key)           const
  {
    ui32 i = intHash(key);
    for(;; ++i)
    {
      i  &=  m_sz - 1;

      kv probedKv = load_kv(i);
      if(probedKv.key == key) return compexchange_kv(i, &probedKv.asInt, empty_kv().asInt);
             
      if(probedKv.key == EMPTY_KEY)                                 // needs to be taken out when deleting is implemented
        return false;
    }
    return false;
  }
  ui32      size()                    const
  {
    return m_sz;
  }

  //bool     lock()                    const
  //{
  //  m_mut.lock();
  //}
  //bool   unlock()                    const
  //{
  //  m_mut.unlock();
  //}
};

class ConcurrentList
{
public:
  union HeadUnion
  {
    struct { uint32_t cnt; uint32_t idx; };
    uint64_t asInt;
  };
  
  using    ui32  =  uint32_t;
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
 
class ConcurrentStore
{
public:

//private:
  using IDX = i32;

  ui8*                    m_addr;
  ui32               m_blockSize;
  ui32              m_blockCount;
  ai32              m_blocksUsed;
  ConcurrentList            m_cl;

  i32*            stPtr(i32 blkIdx)
  {
    return (i32*)( ((ui8*)m_addr) + blkIdx*m_blockSize );
  }
  i32          nxtBlock(i32 blkIdx)
  {
    return *(stPtr(blkIdx));
  }
  i32     blockFreeSize()
  {
    return m_blockSize - sizeof(IDX);
  }
  ui8*     blockFreePtr(i32 blkIdx)
  {
    return ((ui8*)stPtr(blkIdx)) + sizeof(IDX);
  }
  i32      blocksNeeded(i32    len, i32* out_rem=nullptr)
  {
    i32  freeSz   = blockFreeSize();
    i32  byteRem  = len % freeSz;
    i32  blocks   = len / freeSz + (byteRem? 1 : 0);    // should never be 0 if blocksize is greater than the size of the index type

    if(out_rem) *out_rem = byteRem;

    return blocks;
  }
  size_t     writeBlock(i32 blkIdx, void* bytes) // i64 len = -1)
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
  size_t      readBlock(i32 blkIdx, void* bytes)
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

  i32   alloc(i32    size, i32* out_blocks=nullptr)
  {
    i32 byteRem = 0;
    i32 blocks  = blocksNeeded(size, &byteRem);
    if(out_blocks) *out_blocks = blocks;

    i32   st = m_cl.nxt();                                     // stBlk  is starting block
    i32  cur = st;                                             // curBlk is current  block
    for(i32 i=0; i<blocks-1; ++i)
    {
      i32* p  =  stPtr(cur);
      cur     =  m_cl.nxt();
      m_blocksUsed.fetch_add(1);
      *p      =  cur;
    }
    i32* p  =  (i32*)stPtr(cur);
    *p      =  byteRem? -byteRem : -blockFreeSize();

    return st;
  }
  void   free(i32     idx)
  {
    i32   cur   =  idx;                // cur is the current block index
    i32    nxt  =  *stPtr(cur);        // nxt is the next block index
    for(; nxt>0; nxt=*stPtr(cur) ){ 
      m_cl.free(cur);
      m_blocksUsed.fetch_add(-1);
      cur  =  nxt;
    }
    m_cl.free(cur);

    //i32*    p   =  stPtr(cur);         // p is the pointer to the first bytes of the block
    //i32    nxt  =  *p;                 // nxt is the next block index
    //for(; *p>0; p=stPtr(cur) )    
    //{ 
    //  nxt  =  *p;
    //  m_cl.free(cur);
    //  cur  =  nxt;
    //}
    //m_cl.free(cur);
  }
  void    put(i32  blkIdx, void* bytes, i32 len)
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
  void    get(i32  blkIdx, void* bytes, i32 len)
  {
    ui8*      b = (ui8*)bytes;
    i32  blocks = blocksNeeded(len);
    i32     cur = blkIdx;
    for(i32 i=0; i<blocks; ++i)
    {
      b   +=  readBlock(cur, b);
      cur  =  nxtBlock(cur);
    }    
  }

};

class SharedMemory
{};

class SimDB
{
private:
  ConcurrentStore   m_cs;     // store data in blocks and get back indices
  ConcurrentHash    m_ch;     // store the indices of keys and values - contains a ConcurrentList

  // todo: deal with memory / allocate from  shared memory

public:
  SimDB(){}

  ui32  put(void* key, i32 klen, void* val, i32 vlen)
  {
    i32 kidx = m_cs.alloc(klen);                      // kidx is key index
    i32 vidx = m_cs.alloc(vlen);
    
    m_cs.put(kidx, key, klen);
    m_cs.put(vidx, val, vlen);

    return m_ch.put(kidx, vidx);
  }
  void  getKey(i32 idx, void* buf, i32 len)
  {
    m_cs.put(idx, buf, len);
  }
  void  getVal(i32 idx, void* buf, i32 len)
  {
    m_cs.put(idx, buf, len);
  }


};

#endif

