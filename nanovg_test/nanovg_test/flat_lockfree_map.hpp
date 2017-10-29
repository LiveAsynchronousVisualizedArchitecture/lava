
// -todo: make sizeBytes() static function
// -todo: make swapIdxPair function
// -todo: copy in CncrBlkLst from simdb
// -todo: take out version from CncrLst
// -todo: put back version into ListHead
// -todo: transition CncrLst to be without any stack variables - just use the functions and pass them addresses?
// -todo: put version increment back into nxt and free functions
// -todo: convert idx function to use 64 bit ListHead with version
// -todo: put version into list free function
// -todo: make default constructor
// -todo: make constructor that takes a capacity
// -todo: copy in next power of 2 function from simdb
// -todo: make allocation function - don't need much since the only trick is the size of the flat memory, which is given by the static sizeBytes() function
// -todo: fill out BlkMeta struct - should it even be used? - won't use it for now
// -todo: make atomic load and store for list head
// -todo: fill out the rest of the constructor
// -todo: print out header
// -todo: print out list
// -todo: make a size function
// -todo: make a capacity function
// -todo: test and print size and capacity
// -todo: round capacity up to the nearest power of 2

// todo: make get()
// todo: make put()
// todo: make del()
// todo: make operator[]
// todo: make operator()

#pragma once

#if !defined(__FLAT_LOCKFREE_MAP_HEADER_GUARD_HPP__)

#include <cstdint>
#include <cstdlib>
#include <atomic>
//#include <optional>

struct flf_map
{
  using   u8    =    uint8_t;
  using  u32    =   uint32_t;
  using  u64    =   uint64_t;
  using au32    =   std::atomic<uint32_t>;
  using au64    =   std::atomic<uint64_t>;
  using Key     =   u64;
  using Value   =   u64;
  using Hash    =   u64;
  using Hasher  =   std::hash<Key>;
  using LstIdx  =   u32;

  static const u32               EMPTY  =  0x00FFFFFF;              // max value of 2^24 to set all 24 bits of the value index
  static const u32             DELETED  =  0x00FFFFFE;              // one less than the max value above
  static const u32 SPECIAL_VALUE_START  =  DELETED;                 // comparing to this is more clear than comparing to DELETED
  static const u32            LIST_END  =  0xFFFFFFFF;

  struct       Ret
  {
    union { 
      Value val;
      u8 asBytes[sizeof(Value)]; 
    };      
    bool     ok;

    operator bool() const { return ok; }
  };
  union        Idx {
    struct { u32 readers :  8;  u32 val_idx : 24; };
    u32 asInt;
  };
  union    IdxPair {
    struct { Idx first; Idx second; };
    u64 asInt;
  };
  union   ListHead
  {
    struct { u64 idx : 24; u64 ver : 40; };
    u64 asInt;

    u32    loadIdx() const
    {
      ListHead ldHd;
      ldHd.asInt = ((au64*)&asInt)->load();
      return (u32)ldHd.idx;
    }
    void     store(ListHead lh)
    {
      ((au64*)&asInt)->store(lh.asInt);
    }
  };
  struct    Header {
    // first 8 bytes - two 1 bytes characters that should equal 'lm' for lockless map
    u64     typeChar1  :  8;
    u64     typeChar2  :  8;
    u64     sizeBytes  : 48;
    
    // next 8 bytes keep track of the size of the values and number inserted - from that and the sizeBytes, capacity can be inferred
    u64          size :  32;                                    // part of the header so that it can be atomic - this could be only 24 bits since that is the max index
    u64  valSizeBytes :  32;

    // next 8 bytes is the block list head - the next index to use and the number of elements combined into one struct
    ListHead    lstHd;                                         // lstHd is list head
   };
  struct   BlkMeta {
  };

  u8*     m_mem = nullptr;  // single pointer is all that ends up on the stack

  // concurrent list functions
  void       make_list(void* addr, u32 size)                          // this constructor is for when the memory is owned an needs to be initialized
  {                                                                   // separate out initialization and let it be done explicitly in the simdb constructor?    
    u32*  lstAddr  =  (u32*)addr;
    for(u32 i=0; i<(size-1); ++i){ lstAddr[i] = i+1; }
    lstAddr[size-1] = LIST_END;
  }
  u32              nxt(au64* head, u32* lst)                                      // moves forward in the list and return the previous index
  {
    ListHead curHead, nxtHead;
    curHead.asInt = head->load();
    do{
      if(curHead.idx==LIST_END){return LIST_END;}
      nxtHead.idx = lst[curHead.idx];
      nxtHead.ver = curHead.ver + 1;
    }while( !head->compare_exchange_strong(curHead.asInt, nxtHead.asInt) );

    return curHead.idx;
  }
  u32             free(au64* head, u32* lst, u32 idx)                             // not thread safe when reading from the list, but it doesn't matter because you shouldn't be reading while freeing anyway, since the CncrHsh will already have the index taken out and the free will only be triggered after the last reader has read from it 
  {
    ListHead curHead, nxtHead;
    u32 retIdx;
    curHead.asInt = head->load();
    do{
      retIdx  = lst[idx] = curHead.idx;
      nxtHead.idx = idx;
      nxtHead.ver = curHead.ver + 1;
    }while( !head->compare_exchange_strong(curHead.asInt, nxtHead.asInt) );

    return retIdx;
  }
  u32             free(au64* head, u32* lst, u32 st, u32 en)                                            // not thread safe when reading from the list, but it doesn't matter because you shouldn't be reading while freeing anyway, since the CncrHsh will already have the index taken out and the free will only be triggered after the last reader has read from it 
  { // todo: possibly take this out, there might not be an oportunity to free a linked list of indices instead of a single index 
    ListHead curHead, nxtHead;
    u32 retIdx;
    curHead.asInt = head->load();
    do{
      retIdx = lst[en] = curHead.idx;
      nxtHead.idx = st;
      nxtHead.ver = curHead.ver + 1;
    }while( !head->compare_exchange_strong(curHead.asInt, nxtHead.asInt) );

    return retIdx;
  }
  u32              idx(au64* head) const { return ((ListHead*)head->load())->idx; }
  auto          header() const -> Header*   { return (Header*)m_mem; }
  auto        listHead() const -> ListHead* { return &(((Header*)m_mem)->lstHd); }
  u32*       listStart(u64 capacity) { return (u32*)(m_mem + offsetBytes_list(capacity)); }

  // utility functions
  Idx*         slotPtr(){ return (Idx*)(m_mem + offsetBytes_slots()); }   // slotByteOffset(idx)); }
  Idx          getSlot(Idx* idx, u64 offset)
  {
    Idx ret;
    ret.asInt  =  ((au32*)(idx + offset))->load();
    
    return ret;
  }
  bool      incReaders(void* oldIp, IdxPair*  newIp = nullptr) 
  {
    au64*   atmIncPtr  =  (au64*)(oldIp);

    Idx idxs[2];
    u64 oldVal, newVal;
    do{
      oldVal          = atmIncPtr->load(std::memory_order::memory_order_seq_cst);        // get the value of both Idx structs atomically
      *((u64*)(idxs)) = oldVal;   // default memory order for now
 
      if(idxs[0].val_idx < SPECIAL_VALUE_START &&
         idxs[1].val_idx < SPECIAL_VALUE_START ){
        idxs[0].readers  +=  1;        // increment the reader values if neithe of the indices have special values like EMPTY or DELETED
        idxs[1].readers  +=  1;
      }else{
        return false;
      }

      newVal = *((u64*)(idxs));
    }while( atmIncPtr->compare_exchange_strong(oldVal, newVal) );      // store it back if the pair of indices hasn't changed - this is not an ABA problem because we aren't relying on the values at these indices yet, we are just incrementing the readers so that 1. the data is not deleted at these indices and 2. the indices themselves can't be reused until we decrement the readers

    if(newIp) newIp->asInt = newVal;
    return true; // the readers were successfully incremented, but we need to return the indices that were swapped since we read them atomically
  }
  bool     swapIdxPair(IdxPair* ip, IdxPair* prevIp = nullptr)
  {
    using namespace std;
    
    au64*     aip  =  (au64*)ip;
    IdxPair oldIp;
    oldIp.asInt    =  aip->load();
    
    IdxPair nxtIp;
    nxtIp.first  = oldIp.second;
    nxtIp.second = oldIp.first;

    bool ok = aip->compare_exchange_strong( oldIp.asInt, nxtIp.asInt, std::memory_order_seq_cst);

    if(prevIp){ *prevIp = oldIp; }
    return ok;
  }
  Hash         hashKey(Key const& k)
  {
    return Hasher()(k);  // instances a hash function object and calls operator()
  }

  // public interface functions
  auto            get(Key const& key) -> Ret // std::optional<Value>
  {
    //HshType hh;
    //hh.hash   =  HashStr(key);
    //u32  hsh  =  hh.hash;                                                 // make sure that the hash is being squeezed into the same bit depth as the hash in HshType
    //if(out_hash){ *out_hash = hsh; }
    Hash   hsh = hashKey(key);

    //KV*   el  =  (KV*)elemStart();                                         // el is a pointer to the elements 
    Idx* slots = slotPtr();

    //u64  mod  =  map_capacity();
    //if(mod==0) return nullptr;
    u64 mod = capacity();  // todo: if(mod==0) return optional<false> or iterator/return struct equivilent

    u64    i  =  hsh % mod;
    u64   en  =  prev(i,mod);
    u64 dist  =  0;
    for(;;++i,++dist)
    {
      i %= mod;                                                                // get idx within capacity
      //HshType eh = el[i].hsh;                                                // eh is element hash

      Idx curIdx  =  getSlot(slots, i);
      if(curIdx.val_idx==DELETED){
        continue;
      }else if(curIdx.val_idx==EMPTY){
        break;
        /* return something that will evaluate to false here */
      }else if( /*check that hash then key are the same*/1==1 ){
        /* return the value here */ // get the offset of the hash key value segment, offset by the index, the offset by the sizeof Hash and Key
      }//else if(dist > wrapDist(el,i,mod) ){
        // break; /* return false here? */

      if(i==en) break;                                                 // nothing found and the end has been reached, time to break out of the loop and return a reference to a KV with its type set to NONE
    }

    Ret ret;
    ret.ok = false;
    return ret;        //std::optional<Value>(); // return a false value here since nothing was found

    //return std::optional<Value>();
    //
    //  if( eh.type == HshType::EMPTY || 
    //    (hsh == eh.hash &&                                  // if the hashes aren't the same, the keys can't be the same
    //      strncmp(el[i].key,key,sizeof(KV::Key)-1)==0) )
    //  { 
    //    return &el[i];
    //  }else if(dist > wrapDist(el,i,mod) ){
    //    //KV kv(key, hsh);
    //    KV kv(key);
    //    kv.hsh.hash = hsh;
    //    elems( elems()+1 );
    //    return &(place_rh(kv, el, i, dist, mod));
    //  }
    //
    //  if(i==en) break;                                                 // nothing found and the end has been reached, time to break out of the loop and return a reference to a KV with its type set to NONE
    //}
    //
    //return nullptr; // no empty slots and key was not found
  }
  u32     size(){ return header()->size; }
  u32 capacity()
  {
    auto hdr = header();
    u32  cap = (u32)( (hdr->sizeBytes-sizeof(Header))/sizeBytes_perCap() );
    return cap;
  }


  flf_map(){}
  flf_map(u64 capacity)
  {
    u64    cap2 = nextPowerOf2(capacity);
    u64 szBytes = sizeBytes(cap2);
    m_mem       = (u8*)malloc(szBytes);                 //nullptr;  //  allocation cast to u8* goes here

    void* list_start_addr = m_mem + offsetBytes_list(cap2);  // nullptr;  // calculate the start address of the list here - sizeof(Head) + capacity*sizeof(Idx)

    Header*    h    = header();
    h->typeChar1    = 'L';
    h->typeChar2    = 'F';
    h->sizeBytes    = szBytes;
    h->size         = 0;
    h->valSizeBytes = sizeof(Value);

    ListHead initLH;
    initLH.idx = 0;
    initLH.ver = 0;
    //listHead()->store(initLH);
    h->lstHd.store(initLH);
    make_list( list_start_addr, (u32)cap2 );
  }


  // static utility functions
  static u32      nextPowerOf2(u32  v)
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
  static u64              prev(u64  i, u64 mod)
  {
    if(mod==0) return 0;
    return i==0?  mod-1  :  i-1;
  }

  // static functions for calculating the offset of each segment of the flat memory
  // The flat memory is organized as:  | Header | 32 bit readers + idx slots | concurrent linked list (linked with indices) | hash + key + values |
  static u64  sizeBytes_perCap()
  {
    return sizeof(Idx) + sizeof(LstIdx) + sizeof(Hash)+sizeof(Key)+sizeof(Value);
  }
  static u64 offsetBytes_slots()
  {
    return sizeof(Header);
  }
  static u64   sizeBytes_slots(u64 capacity)
  {
    return capacity*sizeof(Idx);
  }
  static u64 offsetBytes_list(u64 capacity)
  {
    return offsetBytes_slots() + sizeBytes_slots(capacity);
  }
  static u64   sizeBytes_list(u64 capacity)
  {
    return capacity*sizeof(LstIdx);
  }
  static u64 offsetBytes_values(u64 capacity)
  {
    return offsetBytes_list(capacity) + sizeBytes_list(capacity);
  }
  static u64   sizeBytes_values(u64 capacity)
  {
    return capacity * sizeof(Hash)+sizeof(Key)+sizeof(Value);
  }

  static u64   sizeBytes(u64 capacity)
  {
    u64 szPerCap = sizeBytes_perCap(); //sizeof(BlkMeta) + sizeof(Idx) + sizeof(Key) + sizeof(Value) + sizeof(LstIdx);
    return sizeof(Header) + capacity*szPerCap;
  }
};

#endif










// not even correct
//u64   slotByteOffset(u64 idx){ return sizeof(Header) + idx*sizeof(IdxPair); }

//
//make_list( list_start_addr, (au64*)list_head_addr, (u32)capacity ); 

//u32          headIdx() const
//{
//}
//static const u32 NXT_VER_SPECIAL  =  0xFFFFFFFF;

//void       make_list(void* addr, u32* head, u32 size)             // this constructor is for when the memory is owned an needs to be initialized
//u32        count(au32* head) const { return ((Header*)head)->cap; }

//class     CncrLst
//{
//  // Copied from simdb 2017.10.25
//  // Internally this is an array of indices that makes a linked list
//  // Externally indices can be gotten atomically and given back atomically
//  // | This is used to get free indices one at a time, and give back in-use indices one at a time
//  // Uses the first 8 bytes that would normally store sizeBytes as the 64 bits of memory for the Head structure
//  // Aligns the head on a 64 bytes boundary with the rest of the memory on a separate 64 byte boudary. This puts them on separate cache lines which should eliminate false sharing between cores when atomicallyaccessing the Head union (which will happen quite a bit) 
//public:
//  //using     u32  =  uint32_t;
//  //using     u64  =  uint64_t;
//  //using    au64  =  std::atomic<u64>;
//  using ListVec  =  lava_vec<u32>;
//
//  //union Head
//  //{
//  //  struct { u32 ver; u32 idx; };                           // ver is version, idx is index
//  //  u64 asInt;
//  //};
//  union Head
//  {
//    u8  dblCachePad[128];
//    u32         idx;                                         // idx is index
//  };
//
//  static const u32        LIST_END = 0xFFFFFFFF;
//  static const u32 NXT_VER_SPECIAL = 0xFFFFFFFF;
//
//private:
//  //ListVec     s_lv;
//  //au64*        s_h;
//
//public:
//  //static u64   sizeBytes(u32 size) { return ListVec::sizeBytes(size) + 128; }         // an extra 128 bytes so that Head can be placed
//  static u64   sizeBytes(u32 size) { return sizeof(u32) + 128; }         // an extra 128 bytes so that Head can be placed
//  static u32  incVersion(u32    v) { return v==NXT_VER_SPECIAL?  1  :  v+1; }
//
//  CncrLst(){}
//  CncrLst(void* addr, void* headAddr, u32 size, bool owner=true)             // this constructor is for when the memory is owned an needs to be initialized
//  {                                                                          // separate out initialization and let it be done explicitly in the simdb constructor?    
//    u64    addrRem  =  (u64)headAddr % 64;
//    u64  alignAddr  =  (u64)headAddr + (64-addrRem);
//    assert( alignAddr % 64 == 0 );
//    au64*       hd  =  (au64*)alignAddr;
//    u32*   lstAddr  =  (u32*)((u64)alignAddr+64);
//    //new (&s_lv) ListVec(listAddr, size, owner);
//    
//    // give the list initial values here
//    //u32* lstAddr = 
//    //if(owner){
//    for(u32 i=0; i<(size-1); ++i){ lstAddr[i] = i+1; }
//    lstAddr[size-1] = LIST_END;
//
//    //((Head*)s_h)->idx = 0;
//    //((Head*)s_h)->ver = 0;
//    //}
//  }
//
//  u32         nxt()                                                             // moves forward in the list and return the previous index
//  {
//    Head  curHead, nxtHead;
//    curHead.asInt  =  s_h->load();
//    do{
//      if(curHead.idx==LIST_END){return LIST_END;}
//
//      nxtHead.idx  =  s_lv[curHead.idx];
//      nxtHead.ver  =  curHead.ver==NXT_VER_SPECIAL? 1  :  curHead.ver+1;
//    }while( !s_h->compare_exchange_strong(curHead.asInt, nxtHead.asInt) );
//
//    return curHead.idx;
//  }
//  u32        free(u32 idx)                                                    // not thread safe when reading from the list, but it doesn't matter because you shouldn't be reading while freeing anyway, since the CncrHsh will already have the index taken out and the free will only be triggered after the last reader has read from it 
//  {
//    Head curHead, nxtHead; u32 retIdx;
//    curHead.asInt = s_h->load();
//    do{
//      retIdx = s_lv[idx] = curHead.idx;
//      nxtHead.idx  =  idx;
//      nxtHead.ver  =  curHead.ver + 1;
//    }while( !s_h->compare_exchange_strong(curHead.asInt, nxtHead.asInt) );
//
//    return retIdx;
//  }
//  u32        free(u32 st, u32 en)                                            // not thread safe when reading from the list, but it doesn't matter because you shouldn't be reading while freeing anyway, since the CncrHsh will already have the index taken out and the free will only be triggered after the last reader has read from it 
//  {
//    Head curHead, nxtHead; u32 retIdx;
//    curHead.asInt = s_h->load();
//    do{
//      retIdx = s_lv[en] = curHead.idx;
//      nxtHead.idx  =  st;
//      nxtHead.ver  =  curHead.ver + 1;
//    }while( !s_h->compare_exchange_strong(curHead.asInt, nxtHead.asInt) );
//
//    return retIdx;
//  }
//  auto      count() const -> u32 { return ((Head*)s_h)->ver; }
//  auto        idx() const -> u32
//  {
//    Head h; 
//    h.asInt = s_h->load();
//    return h.idx;
//  }
//  auto       list() -> ListVec const* { return &s_lv; }                      // not thread safe
//  u32      lnkCnt()                                                          // not thread safe
//  {
//    u32    cnt = 0;
//    u32 curIdx = idx();
//    while( curIdx != LIST_END ){
//      curIdx = s_lv[curIdx];
//      ++cnt;
//    }
//    return cnt;
//  }
//  auto       head() -> Head* { return (Head*)s_h; }
//};

//auto      list() -> ListVec const* { return &s_lv; }                      // not thread safe
//u32     lnkCnt()                                                           // not thread safe
//{
//  u32    cnt = 0;
//  u32 curIdx = idx();
//  while( curIdx != LIST_END ){
//    curIdx = s_lv[curIdx];
//    ++cnt;
//  }
//  return cnt;
//}

//
//nxtHead.ver  =  curHead.ver + 1;

//au32* head = 
//nxtHead.ver  =  curHead.ver==NXT_VER_SPECIAL? 1  :  curHead.ver+1;

//nxtIp.asInt    =  aip->load();
//Idx      tmp = nxtIp.first;

//bool incTwoIdxReaders(u64 idx, IdxPair* newIp = nullptr)
//bool incIdxPairReaders(u64 idx, IdxPair* newIp = nullptr)
//
// u8*   firstIdxPtr  =  
// Idx*       idxPtr  =  (Idx*)(m_mem + idx*sizeof(Idx));     // it is crucial and fundamental that sizeof(Idx) needs to be 4 bytes so that two of them can be swapped even if unaligned, but this should be more correct and clear
//au64*   atmIncPtr  =  (au64*)(m_mem + idx*sizeof(Idx));       // it is crucial and fundamental that sizeof(Idx) needs to be 4 bytes so that two of them can be swapped even if unaligned, but this should be more correct and clear

//IdxPair ip;
//ip.asInt = newVal;

