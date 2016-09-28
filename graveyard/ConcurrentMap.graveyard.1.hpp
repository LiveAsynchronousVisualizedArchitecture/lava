//private:
//std::unique_ptr<void*> ptr;
//HANDLE     fileHndl;

//SharedMem(SharedMem const&) = delete;
//SharedMem(const char* name, ui64 size) :
//  m_sz(size)
//{
//  char path[512] = "Global\\simdb_15_";
//  strcat_s(path, sizeof(path), name);
//
//  m_fileHandle = CreateFileMapping(
//    INVALID_HANDLE_VALUE,
//    NULL,
//    PAGE_READWRITE,
//    0,
//    (DWORD)m_sz,
//    path);
//
//  if(m_fileHandle==NULL){/*error*/}
//
//  m_ptr = MapViewOfFile(m_fileHandle,   // handle to map object
//    FILE_MAP_ALL_ACCESS,   // read/write permission
//    0,
//    0,
//    m_sz);
//}
//
//ui64 size() const
//{
//  return size;
//}

//i32      get(void*   key, i32    len)
//{
//  ui32 keyhash  =  m_ch.hashBytes(key, len);
//  auto     ths  =  this;
//  return m_ch.getHashed(keyhash, 
//    [ths, key, len](ui32 blkidx){ return CompareBlock(ths,key,len,blkidx); });
//}

//ui32 idx = get( (void*)key.data(), (ui32)key.length() );
//KV    kv = read( (void*)key.data(), (ui32)key.length() );

//using COMPARE_FUNC  =  decltype( [](ui32 key){} );
//union      KV
//{
//  struct
//  {
//    uint64_t   remove  :  1;
//    uint64_t  readers  :  7;
//    uint64_t      key  : 28;
//    uint64_t      val  : 28;
//  };
//  uint64_t asInt;
//
//  // after the switch to 128 bit atomics:
//  // ui64 ubits;
//  // ui64 lbits;
//};

//
// [ths](ui32 a, ui32 b){return CompareBlocks(ths,a,b); }); 

// defKv.asInt    =  0;
//defKv.key      =  EMPTY_KEY;
//defKv.val      =  0;
//defKv.readers  =  INIT_READERS;
//
//for(ui32 i=0; i<m_sz; ++i) m_kvs[i] = defKv;
//
//m_keys.reset(new Aui32[m_sz]);
//m_vals.reset(new Aui32[m_sz]);
//
//ui32* keys = (ui32*)m_keys.get();
//ui32* vals = (ui32*)m_vals.get();
//TO(m_sz,i) {
//  keys[i] = EMPTY_KEY;
//}
//memset(vals,0,m_sz);

//void      rm(const std::string key)
//{
//  auto  len = (ui32)key.length();
//  auto  ths = this;
//  auto kbuf = (void*)key.data();
//  auto hash = m_ch.hashBytes(kbuf, len);
//  ui32  idx = m_ch.findHashed(hash,
//    [ths, kbuf, len](ui32 blkidx){ return CompareBlock(ths,kbuf,len,blkidx); });
//      
//  KV  prev = m_ch.rm(idx);
//  if(prev.key!=EMPTY_KEY) m_cs.free(prev.key);
//  if(prev.val!=EMPTY_KEY) m_cs.free(prev.val);
//}

//if( comp(probedKv.key, key) ){
//if( match(probedKv.key) ){
//  return store_kv(i, desired);
//  //return i;
//}

//if(probedKv.key == EMPTY_KEY){
//  store_kv(i, desired); 
//  return i;
//}

//if(probedKv.key != key)
//if(probedKv.key == EMPTY_KEY)
//{
  //if(probedKv.key != EMPTY_KEY) continue;                                               // The entry was either free, or contains another key.  // Usually, it contains another key. Keep probing.

//KV desired = empty_kv();
//desired.key  =  key;
//desired.val  =  val;

//if(probedKv.key != EMPTY_KEY) continue;
//if(probedKv.key == EMPTY_KEY){
//  store_kv(i, desired); 
//  return i;
//}
//if(probedKv.key != key)
//if(probedKv.key != EMPTY_KEY) continue;                                               // The entry was either free, or contains another key.  // Usually, it contains another key. Keep probing.

//kv      endRead(ui32  key)                     const
//{
//  ui32 i = intHash(key);
//  for(;; ++i)
//  {
//    i  &=  m_sz - 1;
//
//    kv probedKv = load_kv(i);
//    if(probedKv.key == key){
//      return addReaders(i, probedKv, -1);
//    }
//           
//    if(probedKv.key == EMPTY_KEY)                                 // needs to be taken out when deleting is implemented
//      return empty_kv();
//  }
//  return empty_kv();
//}

//i32*    p   =  stPtr(cur);         // p is the pointer to the first bytes of the block
//i32    nxt  =  *p;                 // nxt is the next block index
//for(; *p>0; p=stPtr(cur) )    
//{ 
//  nxt  =  *p;
//  m_cl.free(cur);
//  cur  =  nxt;
//}
//m_cl.free(cur);

//kv          put(ui32  key, ui32 val)           const
//{
//  return putHashed(intHash(key), key, val, DefaultKeyCompare);
//}
//ui32        get(ui32  key)                     const
//{
//  ui32 i = intHash(key);
//  for(;; ++i)
//  {
//    i  &=  m_sz - 1;
//
//    //ui32 probedKey =  m_keys.get()[i].load();        // atomic_load( (Aui32*)(&m_keys.get()[i]) );   //    // mint_load_32_relaxed(&m_entries[idx].key);
//
//    kv probedKv = load_kv(i);
//    if(probedKv.key==key) return probedKv.val;         // m_vals.get()[i].load();                 // atomic_load( (Aui32*)(&m_keys.get()[i]) );         // mint_load_32_relaxed(&m_entries[idx].value);
//
//    //return m_vals.get()[i].load();                   // atomic_load( (Aui32*)(&m_keys.get()[i]) );         // mint_load_32_relaxed(&m_entries[idx].value);
//
//    if(probedKv.key==EMPTY_KEY)
//      return EMPTY_KEY;
//  }
//
//  return EMPTY_KEY;
//}

//static bool  CompareBlocks(simdb* ths, i32 a, i32 b){ return ths->m_cs.compare(a,b); }
//
//bool     compare(IDX blkIdxA, IDX blkIdxB)
//{
//  // && (nxtA=nxtBlock(nxtA))>=0
//  // && (nxtB=nxtBlock(nxtB))>=0 )
//  // if(nxtA < 0) break;
//  // if(nxtB < 0) break;
//
//  size_t alen=0; size_t blen=0; IDX nxtA=blkIdxA; IDX nxtB=blkIdxB; bool blkcmp=false;
//  while( blockCompare(nxtA, nxtB, &alen, &blen) )
//  {      
//    nxtA = nxtBlock(nxtA);
//    nxtB = nxtBlock(nxtB);
//    bool lastA = nxtA<0;
//    bool lastB = nxtB<0;
//    if(lastA ^  lastB) return false;  // if one is on their last block but the other is not, return false - not actually needed? - it is needed because the blocks could be the same while one is the last and the other is not?
//    if(lastA && lastB) return  true;
//  }
//
//  return false;
//}

//simdb(ui32 blockSize, ui32 blockCount) : 
//  m_mem( malloc(blockSize*blockCount) ),
//   m_cs( (ui8*)m_mem, blockSize, blockCount),            // todo: change this to a void*
//   m_ch( blockCount )
//{}

//bool     blockCompare(i32 blkIdxA, i32 blkIdxB, size_t* out_alen=nullptr, size_t* out_blen=nullptr)
//{
//  i32 alen = blockLen(blkIdxA);
//  i32 blen = blockLen(blkIdxB);
//  if(out_alen) *out_alen = alen;
//  if(out_blen) *out_blen = blen;
//  if(alen != blen) return false;         // if their lengths aren't even the same, they can't be the same
//
//  ui8* pa  =  blockFreePtr(blkIdxA);
//  ui8* pb  =  blockFreePtr(blkIdxB);
//  return memcmp(pa, pb, alen)==0;
//}

//i32  blocks = blocksNeeded(len);
//for(i32 i=0; i<blocks; ++i)
//i32 nxt;

//void         get(i32  blkIdx, void* bytes, i32 len)
//{
//  ui8*      b = (ui8*)bytes;
//  i32  blocks = blocksNeeded(len);
//  i32     cur = blkIdx;
//  for(i32 i=0; i<blocks; ++i)
//  {
//    b   +=  readBlock(cur, b);
//    cur  =  nxtBlock(cur);
//  }
//}

//
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

//bool     lock()                    const
//{
//  m_mut.lock();
//}
//bool   unlock()                    const
//{
//  m_mut.unlock();
//}

//ui32         put(ui32  key, ui32 val) const
//{
//  using namespace std;

//  kv desired;
//  desired.key  =  key;
//  desired.val  =  val;
//  ui32      i  =  intHash(key);
//  for(;; ++i)
//  {
//    i  &=  m_sz-1;

//    //ui32 probedKey = load_key(i);
//    kv probedKv = load_kv(i);
//    if(probedKv.key != key)
//    {
//      if(probedKv.key != EMPTY_KEY) continue;                                               // The entry was either free, or contains another key.  // Usually, it contains another key. Keep probing.
//              
//      kv   expected   =  empty_kv();
//      //expected.asInt  =  0;
//      //expected.key    =  EMPTY_KEY;

//      bool   success  =  compexchange_kv(i, &expected.asInt, desired.asInt);
//      //bool   success  =  m_keys.get()[i].compare_exchange_strong(desired, key);           // The entry was free. Now let's try to take it using a CAS. 
//      if( !success && (expected.key!=key) ) continue;                                       // Another thread just stole it from underneath us.
//    }                                                                                       // Either we just added the key, or another thread did.

//    //m_vals.get()[i].store(val);
//    store_kv(i, desired);
//    return i;
//  }
//  return i;
//}

