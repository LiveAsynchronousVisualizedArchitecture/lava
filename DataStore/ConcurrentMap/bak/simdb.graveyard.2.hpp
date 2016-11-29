
//template<class MATCH_FUNC> 
//ui32     getHashed(ui32 hash, MATCH_FUNC match)            const
//{
//  ui32 i = hash;
//  for(;; ++i)
//  {
//    //i &= m_sz - 1;
//    i %= m_sz;
//    VerIdx probedKv = load_kv(i);
//    if(probedKv.idx==EMPTY_KEY) return EMPTY_KEY;
//    if( checkMatch(i, probedKv.idx, match)==MATCH_TRUE ) return probedKv.val;
//    //if( match(probedKv.key) )   return probedKv.val;
//  }
//}
//template<class MATCH_FUNC> 
//ui32    findHashed(ui32 hash, MATCH_FUNC match)            const
//{
//  ui32 i = hash;
//  for(;; ++i)
//  {
//    i &= m_sz - 1;
//    VerIdx probedKv = load_kv(i);
//    if( probedKv.idx==EMPTY_KEY )            return EMPTY_HASH_IDX;          // todo: this conflates and assumes that EMPTY_KEY is both the ConcurrentStore block index EMPTY_KEY and the ConcurrentHash EMPTY_KEY
//    if( checkMatch(i, probedKv.idx, match) ) return i;
//  }
//}

//template<class MATCH_FUNC> 
//VerIdx  readHashed(ui32 hash, MATCH_FUNC match)            const // -> Reader
//{
//  using namespace std;
//
//  ui32 i = hash;
//  for(;; ++i)
//  {
//    i &= m_sz - 1;
//    VerIdx probedKv = load_kv(i);
//    if(probedKv.idx==EMPTY_KEY) return empty_kv();
//    //if( match(probedKv.idx)==MATCH_TRUE ) return probedKv;
//    if( checkMatch(i, probedKv.version, probedKv.idx, match)==MATCH_TRUE ) return probedKv;
//
//    //if(probedKv.key==EMPTY_KEY) return empty_reader();         // empty_kv();
//    //if( match(probedKv.key)==MATCH_TRUE ){
//    //  Reader r;
//    //  r.kv        =  probedKv; // addReaders(i, probedKv, 1);
//    //  r.hash_idx  =  i;
//    //  r.doEnd     =  true;
//    //  r.ch        =  this;
//    //  return move(r); // addReaders(i, probedKv, 1);
//    //}
//
//  }
//
//  return empty_kv();
//}

//bool      rmHashed(ui32 hash, MATCH_FUNC match) const // -> Reader    
//if( decReaders(i) == false ) return probedKv; // return true;
//
//auto empty = empty_kv();
//if( compexchange_kv(i, &probedKv.asInt, empty.asInt) ) return probedKv;
//else --i;                                     // run the same loop iteration again if there was a match, but the key-value changed
//
// false;    
//
//{ return false; } //  // probedKv; // MATCH_FALSE; // empty_reader();
//Match m = match(probedKv.idx);
//
// { return false; } // return empty_kv();

//union      VerIdx         // 256 million keys (28 bits), 256 million values (28 bit),  127 readers (signed 8 bits)
//{
//  struct
//  {
//    ui64     key : 21;
//    ui64     val : 21;
//    ui64 version : 22;
//  };
//  ui64 asInt;
//};

//
//using VerIdx = ConcurrentStore::VerIdx;

//static aui64*  SpinWhileFalse(aui64* ptr)
//{
//}
//
//VerIdx             read(void*   key, i32        len) const
//{
//  using namespace std;
//  
//  //ui32 keyhash  =  m_ch.hashBytes(key, len);
//  ui32 keyhash  =  ConcurrentHash::HashBytes(key, len);
//  auto     ths  =  this;
//  return s_ch.readHashed(keyhash, 
//    [ths, key, len](ui32 blkidx, ui32 ver){ return CompareBlock(ths,blkidx,ver,key,len); });
//}

//
//}while( kv.key==empty.key || kv.readers<0 );

//
//if(!s_bls[blkIdx].vi.version == version) return MATCH_REMOVED;

//if(decReaders(blkIdx)){
//  doFree(blkIdx);
//}

//if(!incReaders(blkIdx, version)) return 0;
//i32       nxt  =  nxtBlock(blkIdx);
//size_t cpyLen  =  nxt<0? -nxt : blkFree;           // if next is negative, then it will be the length of the bytes in that block

//if(decReaders(blkIdx)){
//  doFree(blkIdx);
//  return true;
//}else return false;

//ai32* aidx = (ai32*)&(m_bls[blkIdx].readers);
//
//auto prev = aidx->fetch_add(-1);
//if(prev==0){ doFree(blkIdx); return false; }

//BlkLst cur, nxt;
//aui64* aidx = (aui64*)&(s_bls[blkIdx].asInt);
//cur.asInt   = aidx->load();
//do{
//  nxt = cur;
//  nxt.kr.readers -= 1;
//}while( !aidx->compare_exchange_strong(cur.asInt, nxt.asInt) );
//
//if(cur.kr.readers==0){ doFree(blkIdx); return false; }

//union BlkLst                            // need to do anything special to guarantee that readers is aligned so it is atomic?
//{
//  struct { KeyAndReaders kr; VerIdx vi; ui32 len; ui32 klen; };
//  //ui64 asInt;
//};

//bool      incReaders(ui32 blkIdx, ui32 version) const                   // BI is Block Index  increment the readers by one and return the previous kv from the successful swap 
//{
//  //ai32* aidx = (ai32*)&(m_bls[blkIdx].readers);
//  //i32    cur = aidx->load();
//  //i32    nxt;
//  
//  //BlkLst cur, nxt;
//  //aui64* aidx = (aui64*)&(s_bls[blkIdx].asInt);
//  //cur.asInt   = aidx->load();
//  ////nxtBl.asInt = curBl.asInt;
//  //do{
//  //  if(cur.kr.readers<0) return false;
//  //  nxt = cur;
//  //  nxt.kr.readers += 1;
//  //}while( !aidx->compare_exchange_strong(cur.asInt, nxt.asInt) );
//
//  //KeyAndReaders cur, nxt;
//  //aui32* areaders = (aui32*)&(s_bls[blkIdx].kr.asInt);    
//  //cur.asInt       = areaders->load();
//  //do{
//  //  if(cur.readers<0) return false;
//  //  nxt = cur;
//  //  nxt.readers += 1;
//  //}while( !areaders->compare_exchange_strong(cur.asInt, nxt.asInt) );
//  //
//  //return true;
//
//  KeyAndReaders cur, nxt;
//  aui64* areaders = (aui64*)&(s_bls[blkIdx].kr.asInt);    
//  cur.asInt       = areaders->load();
//  do{
//    if(cur.version!=version || cur.readers<0) return false;
//    nxt = cur;
//    nxt.readers += 1;
//  }while( !areaders->compare_exchange_strong(cur.asInt, nxt.asInt) );
//  
//  return true;
//}

//size_t cpyLen  =  nxt<0? -nxt : blkFree;           // if next is negative, then it will be the length of the bytes in that block
//
//
//cpyLen -= ofst;
//
//bool     fill = len < -1 || blkFree < len;
//size_t cpyLen = fill? blkFree : len;

//IDX          nxtBlock(i32  blkIdx)  const
//{
//  //return *(stPtr(blkIdx));
//  return s_bls[blkIdx].idx;
//}

//if( !s_ch.runMatch(hsh, matchFunc, runFunc) ) return false;
//return true;

//VerIdx        kv = s_ch.putHashed(keyhash, kidx.idx, vidx.idx,                      // this returns the previous VerIdx at the position
//  [ths, key, klen](ui32 blkidx){ return CompareBlock(ths,key,klen,blkidx); });

//i32   kidx = s_cs.alloc(klen, &blkcnt).idx;    // todo: use the VersionIdx struct // kidx is key index
//
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
//    
//s_cs.put(kidx.idx, key,);
//ui32 keyhash = m_ch.hashBytes(key, klen);
//
//if(kv.val!=EMPTY_KEY) s_cs.free(kv.val);
//if(kv.key!=EMPTY_KEY) s_cs.free(kv.key);
//
//if(kv.key != ConcurrentHash::EMPTY_KEY){ m_cs.free(kv.val); m_cs.free(kv.key); }

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
//void          put(i32  blkIdx, void* bytes, i32 len)
//{
//  ui8*       b   = (ui8*)bytes;
//  i32    blocks  =  blocksNeeded(len);
//  i32       cur  =  blkIdx;
//  for(i32 i=0; i<blocks; ++i)
//  {
//    b   +=  writeBlock(cur, b);
//    cur  =  nxtBlock(cur);
//  }
//
//  //i32  remBytes  =  0;
//  //i32    blocks  =  blocksNeeded(len, &remBytes);
//}
  //Match     compare(void*   buf, ui32 len, IDX blkIdx) const
//{
//  using namespace std;
//  
//  IDX   curidx  =  blkIdx;
//  i32      nxt  =  nxtBlock(curidx);
//  auto   blksz  =  blockFreeSize();
//  ui8*  curbuf  =  (ui8*)buf;
//  auto    klen  =  s_bls[blkIdx].klen;             if(klen!=len) return MATCH_FALSE;
//  auto  curlen  =  len;
//  //auto  remlen  =  s_bls[blkIdx].klen;
//  while(true)
//  {
//    auto p = blockFreePtr(curidx);
//    if(nxt >= 0){
//      if(curlen < blksz){ return MATCH_FALSE; }
//      Match cmp = memcmpBlk(curidx, curbuf, p, blksz);
//      if( cmp!=MATCH_TRUE ){ return cmp; }
//    }else if(-nxt != curlen){ return MATCH_FALSE; }
//    else{ return memcmpBlk(curidx, curbuf, p, curlen); }
//
//    curbuf  +=  blksz;
//    curlen  -=  blksz;
//    curidx   =  nxt;
//    nxt      =  nxtBlock(curidx);
//  }
//
//  // return MATCH_TRUE; // never reached
//}

//i64       get(const char* key, void* out_buf)
//{
//  //Reader r = read( (void*)key, (ui32)strlen(key));
//  //if(r.kv.key==EMPTY_KEY || r.kv.readers<=0){ return -1; }
//
//  KV kv = read( (void*)key, (ui32)strlen(key));
//  if(kv.key==EMPTY_KEY){ return -1; }
//
//  ui64 len = getFromBlkIdx(kv.val, out_buf);
//
//  //if(r.doRm()){ m_cs.free(r.kv.val); m_cs.free(r.kv.key); }
//
//  return len;
//}

//auto   getKeyStr(ui32 idx) -> std::string
//{    
//  using namespace std;
//
//  string s;
//  i64 retlen = 0;
//  i64 strlen = 0;
//  auto   ths = this;
//  do{
//    strlen = len(idx);
//    if(strlen<0) return string();
//    s = string(strlen, 0);
//
//    auto runFunc = [ths, &s](KV kv){
//      return IsEmpty(kv)?  0ull  :  ths->s_cs.get(kv.key, (void*)s.data() /* add length here */);
//    };
//
//    retlen = s_ch.runRead(idx, runFunc);
//  }while(retlen != strlen);
//
//  return s;
//}

// todo: obsoleted by m_ch.runRead() ?
//auto  getFromBlkIdx(i32  blkIdx, void*  out_buf) const -> ui64        // getFromBlkIdx is get from block index
//{
//  if(blkIdx==EMPTY_KEY) return 0;
//
//  return s_cs.get(blkIdx, out_buf);           // copy into the buf starting at the blkidx
//} 

//desired.val  =  val;
//if( !success && (expected.key!=key) ) continue;                                     // WRONG!? // Another thread just stole it from underneath us.
//else                                  return expected;
// && (expected.key!=key) */
// if( match(probedKv.key)==MATCH_TRUE ){

//KV       putHashed(ui32 hash, ui32 key, ui32 val, MATCH_FUNC match) const
//KV       putHashed(ui32 hash, ui32 key, MATCH_FUNC match) const

//ui32      len(ui32 idx)
//{
//  KV kv = s_ch.at(idx);
//  
//  //if(kv.key==EMPTY_KEY || kv.readers<0) return 0;
//  if( IsEmpty(kv) ) return 0;
//
//  auto    ths = this;
//  auto retLen = s_ch.runRead(idx, [ths](KV kv){
//    return IsEmpty(kv)?  0  :  ths->s_cs.len(kv.key);
//  });
//
//  return retLen;
//
//  //auto blkLen = m_cs.len(kv.key);
//  //return blkLen;
//}

//
//static bool IsEmpty(KV kv){return kv.key==EMPTY_KEY || kv.readers<0;}

//IDX  nxt;
//IDX  cur = blkIdx;
//ui32 ret = blockLen(cur);
//
//while( (nxt = nxtBlock(cur)) >= 0 ){        // todo: change this to LIST_END, initialize the BlkLst to LIST_END and use ui32 for indices
//  ret += blockLen(nxt);
//  cur  = nxt;
//}
//
//return ret;

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

//const static VersionedIdx LIST_END = { ConcurrentList::LIST_END, 0 };
//const static VersionedIdx LIST_END = { ConcurrentList::LIST_END, 0 };

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



