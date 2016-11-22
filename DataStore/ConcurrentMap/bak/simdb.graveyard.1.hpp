
//
//auto hash = m_ch.hashBytes(kbuf, len);

//    empty.remove   =  0;
    //empty.readers  =  INIT_READERS;
    //empty.key      =  0;
    //empty.val      =  0;

//union BlkIdx
//{
//  struct
//  {
//    signed   long long readers  : 12;     // 2^11 is 2048, so 2048 maximum threads can read this one block index
//    unsigned long long     idx  : 52;     // 4 quadrillion indices should be enough for anybody
//  };
//  ui64 asInt;
//};
  
//struct BlkLst
//{
//  i32   readers;
//  ui32      idx;
//};

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

//i32*            stPtr(i32  blkIdx)  const
//
//BlkLst      decReaders(ui32 blkIdx)         const                   // BI is Block Index  increment the readers by one and return the previous kv from the successful swap 
//{
//  std::atomic_fetch_add( (aui32*)&(m_bls[blkIdx].readers), -1);
//}
//
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

//
//using Reader = ConcurrentHash::Reader;

//static const ui32  EMPTY_KEY     =  0x0FFFFFFF;      // 28 bits set 
//using KVs       =  std::vector<KV>;
//using Match     =  ConcurrentStore::Match; 

//static ui32 IntHash(ui32  h)
//{
//  h ^= h >> 16;
//  h *= 0x85ebca6b;
//  h ^= h >> 13;
//  h *= 0xc2b2ae35;
//  h ^= h >> 16;
//  return h;
//}
//static ui32 HashBytes(void* buf, ui32 len)
//{
//  ui32  rethash  =  0;
//  ui32* cur      =  (ui32*)buf;
//  ui32  loops    =  len/sizeof(ui32);
//  ui32* end      =  cur + loops;
//  for(; cur!=end; ++cur){ rethash ^= IntHash(*cur); }
//
//  ui32  rem      =  len - loops;
//  ui32  lst      =  0;
//  ui8*  end8     =  (ui8*)end;
//  for(ui8 i=0; i<rem; ++i){ lst ^= *end8 << (rem-1-i); }
//  
//  rethash ^= IntHash(lst);
//
//  return rethash;
//}

//bool        del(ui32  key)                      const
//{
//  ui32 i = IntHash(key);
//  for(;; ++i)
//  {
//    i  &=  m_sz - 1;
//
//    KV probedKv = load_kv(i);
//    if(probedKv.key == key) return compexchange_kv(i, &probedKv.asInt, empty_kv().asInt);
//           
//    if(probedKv.key == EMPTY_KEY)                                 // needs to be taken out when deleting is implemented
//      return false;
//  }
//  return false;
//}

//#define HAVE_64BIT_LONG_LONG
//using Fnv64_t = ui64;
//
//#define FNV1_64_INIT ((Fnv64_t)0xcbf29ce484222325ULL)
//#define FNV_64_PRIME ((Fnv64_t)0x100000001b3ULL)
//#if defined(HAVE_64BIT_LONG_LONG)
//#else /* HAVE_64BIT_LONG_LONG */
//  #define FNV_64_PRIME_LOW ((unsigned long)0x1b3)	/* lower bits of FNV prime */
//  #define FNV_64_PRIME_SHIFT (8)		/* top FNV prime shift above 2^32 */
//#endif /* HAVE_64BIT_LONG_LONG */
//
//Fnv64_t  fnv_64a_buf(void *buf, size_t len)    // Fnv64_t hval)
//Fnv64_t hval = FNV1_64_INIT;
//unsigned char *bp = (unsigned char *)buf;	 /* start of buffer */
//unsigned char *be = bp + len;		           /* beyond end of buffer */
//hval ^= (Fnv64_t)*bp++;   /* xor the bottom with the current octet */
//#if defined(HAVE_64BIT_LONG_LONG)
//
  //#if defined(NO_FNV_GCC_OPTIMIZATION)
  //#else /* NO_FNV_GCC_OPTIMIZATION */
  //#endif /* NO_FNV_GCC_OPTIMIZATION */

//#else /* HAVE_64BIT_LONG_LONG */
//
//unsigned long val[4];			/* hash value in base 2^16 */
//unsigned long tmp[4];			/* tmp 64 bit value */
//
// /*
//  * Convert Fnv64_t hval into a base 2^16 array
//  */
//val[0] = hval.w32[0];
//val[1] = (val[0] >> 16);
//val[0] &= 0xffff;
//val[2] = hval.w32[1];
//val[3] = (val[2] >> 16);
//val[2] &= 0xffff;
//
// /*
//  * FNV-1a hash each octet of the buffer
//  */
//while (bp < be) {
//
// /* xor the bottom with the current octet */
//val[0] ^= (unsigned long)*bp++;
//
// /*
// /* multiply by the 64 bit FNV magic prime mod 2^64
// /*
// /* Using 0x100000001b3 we have the following digits base 2^16:
// /*
// /*	0x0	0x100	0x0	0x1b3
// /*
// /* which is the same as:
// /*
// /*	0x0	1<<FNV_64_PRIME_SHIFT	0x0	FNV_64_PRIME_LOW
// */
// /* multiply by the lowest order digit base 2^16 */
//tmp[0] = val[0] * FNV_64_PRIME_LOW;
//tmp[1] = val[1] * FNV_64_PRIME_LOW;
//tmp[2] = val[2] * FNV_64_PRIME_LOW;
//tmp[3] = val[3] * FNV_64_PRIME_LOW;
// /* multiply by the other non-zero digit */
//tmp[2] += val[0] << FNV_64_PRIME_SHIFT;	/* tmp[2] += val[0] * 0x100 */
//tmp[3] += val[1] << FNV_64_PRIME_SHIFT;	/* tmp[3] += val[1] * 0x100 */
// /* propagate carries */
//tmp[1] += (tmp[0] >> 16);
//val[0] = tmp[0] & 0xffff;
//tmp[2] += (tmp[1] >> 16);
//val[1] = tmp[1] & 0xffff;
//val[3] = tmp[3] + (tmp[2] >> 16);
//val[2] = tmp[2] & 0xffff;
// /*
//* Doing a val[3] &= 0xffff; is not really needed since it simply
//* removes multiples of 2^64.  We can discard these excess bits
//* outside of the loop when we convert to Fnv64_t.
//*/
//}
//
// /*
//  * Convert base 2^16 array back into an Fnv64_t
//  */
//hval.w32[1] = ((val[3]<<16) | val[2]);
//hval.w32[0] = ((val[1]<<16) | val[0]);
//
//#endif /* HAVE_64BIT_LONG_LONG */

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
//auto     empty_reader()                   const -> Reader        
//{
//  using namespace std;
//  
//  Reader r;
//  r.kv       = empty_kv();
//  r.ch       = nullptr;                    // this;
//  r.doEnd    = false;
//  r.hash_idx = EMPTY_KEY;                  // EMPTY_KEY is used for actual keys and values, but this is an index into the KV vector
//
//  return move(r);
//}
//KV         addReaders(ui32  i, KV* curKv, i8 readers)        const                         // increment the readers by one and return the previous kv from the successful swap 
//{
//// todo: does curKv need to be a pointer?
//  KV readKv = *curKv;     // todo: does this need to be in the loop? - no it is assigned inside
//  do
//  {
//    if(curKv->key==EMPTY_KEY || (readers>0 && readKv.readers<0) )//  ||
//      return *curKv;                                                                // not successful if the key is empty and not successful if readers is below the starting point - the last free function or last reader should be freeing this slot - this relies on nothing incrementing readers once it has reached FREE_READY
//       //curKv.readers == FREE_READY) // || 
//       //curKv.readers == MAX_READERS) 
//  
//    readKv           =  *curKv;
//    readKv.readers  +=   readers;
//  } while( !compexchange_kv(i, (ui64*)curKv, readKv.asInt) );
//  //} while( !compexchange_kv(i, &curKv.asInt, readKv.asInt) );
//
//  return readKv; // curKv;
//}

//union      KV
//{
//  struct
//  {
//    signed   long long readers  :  8;
//    unsigned long long     key  : 28;
//    unsigned long long     val  : 28;
//  };
//  uint64_t asInt;
//
//  // after the switch to 128 bit atomics:
//  // ui64 ubits;
//  // ui64 lbits;
//};
//struct Reader
//{
//  bool                     doEnd;       //  8 bits?
//  ui32                  hash_idx;       // 32 bits
//  KV                          kv;       // 64 bits
//  ConcurrentHash const*       ch;       // 64 bits
//
//  Reader(){}
//  //Reader(Reader const& lval) = delete;
//  Reader(Reader&) = delete;
//  Reader(Reader&& rval)
//  {
//    doEnd     =  rval.doEnd;
//    hash_idx  =  rval.hash_idx;
//    kv        =  rval.kv;
//    ch        =  rval.ch;
//
//    rval.doEnd    = false;
//    rval.hash_idx = EMPTY_KEY;
//    rval.ch       = nullptr;
//  }
//
//  bool doRm() // doRm is do remove 
//  {
//    //if(doEnd && ch){
//    if(ch){
//      //KV kv = ch->endRead(hash_idx);
//      if(kv.readers == RM_OWNER){ // && kv.remove){
//        //ch->endRead(hash_idx);
//        doEnd = false;
//        return true;
//      }
//    }
//
//    return false;
//  }
//  ~Reader()
//  {
//    //if(doEnd && ch) ch->endRead(hash_idx);              // checks the ConcurrentHash pointer for nullptr, not sure if this is neccesary but it probably doesn't hurt much
//
//    //if(ch) ch->endRead(hash_idx);              // checks the ConcurrentHash pointer for nullptr, not sure if this is neccesary but it probably doesn't hurt much
//  }
//};

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
  //KV           rm(ui32  idx)                      const    // doesn't keep track of readers
  //{
  //  return store_kv(idx, empty_kv());
  //}

//ui32  idx = m_ch.rmHashed(hash,
//Reader  r = m_ch.rmHashed(hash,
//  [ths, kbuf, len](ui32 blkidx){ return CompareBlock(ths,kbuf,len,blkidx); });
    
//if(r.doRm()){                                            // need to check doRm
//  if(r.kv.val!=EMPTY_KEY) m_cs.free(r.kv.val);
//  if(r.kv.key!=EMPTY_KEY) m_cs.free(r.kv.key);           // need to remove the block index lists if true
//}
        
//KV  prev = m_ch.rm(idx);
//if(prev.key!=EMPTY_KEY) m_cs.free(prev.key);
//if(prev.val!=EMPTY_KEY) m_cs.free(prev.val);

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
//  }
// return empty_reader(); // false; // empty_kv();  // should never be reached

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


