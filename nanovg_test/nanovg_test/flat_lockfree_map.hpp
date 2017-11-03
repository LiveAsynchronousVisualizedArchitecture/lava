
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
// -todo: make decReaders
// -todo: take out optional and make Ret struct that can be cast to bool
// -todo: make wrapDist - copy from tbl
// -todo: make wrap distance check for get() / find()
// -todo: convert inc and dec to static functions
// -todo: make an incReaders and decReaders struct that decrements on destruction
// -todo: clean up comments
// -todo: make hash comparison in get()
// -todo: make key comparison in get()
// -todo: return value in get()
// -todo: fix val_idx uninitialized bug
// -todo: test get()
// -todo: make get()
// -todo: make incReader() that increments the readers on only one index
// -todo: make reader contain what was read so it can be checked for EMPTY and DELETE
// -todo: make Read fail on EMPTY or DELETE
// -todo: make put()
// -todo: write hash value and key before inserting index in put()
// -todo: insert index with readers as 1 to replace EMPTY in put() - not neccesary since the value has already been written
// -todo: make freeIdx function
// -todo: once a value has been written to the index in put() make sure to free it if no slot for it is found
// -todo: test put()
// -todo: debug why put returns false - incReaders doesn't return the old/read value when it finds EMPTY or DELETED
// -todo: re-test get() - return false, but value is correct
// -todo: figure out why get() returns false even though the value is correct - just forgot to set ret.ok to true
// -todo: make function to print map memory
// -todo: figure out why readers is set to 4 after put and get - seems to be only after get() - there are four get calls - Read destructor was calling decReaders instead of decReader
// -todo: go over get() again
// -todo: use Reader in get() for checking EMPTY and DELETED
// -todo: try putting in multiple values - figure out why third put fails - put() had a wrapDist check which wasn't neccesary
// -todo: try putting in too many values - just needed LIST_END to be 24 bits
// -todo: make a special value for readers that signifies an index is still being inserted
// -todo: make decReaders not decrement EMPTY - no incementing or decrementing of either special value
// -todo: take loop out of insertAt()
// -todo: change insert to tryInsert? - insertAt seems apt given that it will only insert at the position given or won't succeed
// -todo: add a check for the ideal distance before using insertAt
// -todo: figure out why put sets readers to 205 - nxtPair was unitialized and only set the second Idx
// -todo: figure out loop and how insertAt should handle a compare and swap failure - don't have insertAt loop - leave it to the calling function
// -todo: make a distance comparison that compares wrapped ideal distances - just use wrapDist
// -todo: put swapping into sortPair - made swpPair more general
// -todo: check for DELETED in swpPair - check will be in placeIdx
// -todo: swpPair return an ENUM for why the swap could not be completed - will be handled in placeIdx
// -todo: make placeIdx check that the Idx is in the slot being passed - will do this more through sortPair's return value
// -todo: integrate finding the current value index with the atomic compare and swap loop
// -todo: make placeIdx check that the previous Idx's readers is not live - leave this up to sortPair
// -todo: make placeIdx check that the previous Idx's ideal position is further or the same - also leave to sortPair

// todo: make a separate find function that can be used when decReaders or placeIdx don't find an index where they expect it
// todo: make sortPair return the final Idx it sorted
// todo: make a cleanDeleted() function that moves a DELETED value up until it can be set to EMPTY
// todo: make readers find both indices on decrement, even if they have moved 
// todo: change ReadPair to use an ENUM to keep track of FIRST, SECOND, BOTH, or NONE were incremented
// todo: make placeIdx find the index first, then swap it to it's destination, taking detours to place any LIVE indices in front of it
// todo: make put() find EMPTY slot and swap backwards until its key's span is found
// todo: can't find a specific DELETED slot - make function to recursivly move any DELETED slots from a span from a certain point forward - if two deleted slots are found in a row, needs to save the position of the first and try to remove the second by calling a recursive function, or using a 254 u32 stack structure residing in stack memory
// todo: make del()
// todo: make operator[]
// todo: make operator()
// todo: make take() function that deletes the index and moves the value out

#if !defined(__FLAT_LOCKFREE_MAP_HEADER_GUARD_HPP__)

#include <cstdint>
#include <cstdlib>
#include <atomic>

struct flf_map
{
  using   u8    =    uint8_t;
  using  u32    =   uint32_t;
  using  i32    =    int32_t;
  using  u64    =   uint64_t;
  using  i64    =    int64_t;
  using au32    =   std::atomic<uint32_t>;
  using au64    =   std::atomic<uint64_t>;
  using Key     =   u64;
  using Value   =   u64;
  using Hash    =   u64;
  using Hasher  =   std::hash<Key>;
  using LstIdx  =   u32;

  static const u32               EMPTY  =  0x00FFFFFF;              // max value of 2^24 to set all 24 bits of the value index
  static const u32             DELETED  =  0x00FFFFFE;              // one less than the max value above
  static const u32       SPECIAL_START  =  DELETED;                 // comparing to this is more clear than comparing to DELETED
  static const u32            LIST_END  =  0x00FFFFFF;              // needs to be 24 bits flipped because the list's indices are 24 bits
  static const  u8                LIVE  =  0xFF;                    // this is a value to set for Idx::readers to signify that an index is still being inserted

  enum  SwapResult { SWP_SUCCESS=1, SWP_DELETED, SWP_EMPTY, SWP_DIFF,  SWP_LEMPTY }; 
  struct       HKV                                                  // HKV is hash key value
  {
    Hash    hash;
    Key      key;
    Value  value;
  };
  struct       Ret
  {
    union { 
      Value value;
      u8  asBytes[sizeof(Value)]; 
    };      
    bool     ok;

    operator Value() const { return value; }
    operator  bool() const { return ok; }
  };
  union        Idx {
    struct { u32 readers :  8;  u32 val_idx : 24; };
    u32 asInt;

    bool inc(i32 increment)
    {
      if(val_idx < SPECIAL_START){
        readers += increment;        // increment the reader values if neither of the indices have special values like EMPTY or DELETED
        return true;
      }else 
        return false;
    }
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
  struct      Read
  {
    bool      ok = false;
    Idx      idx;
    Idx*    slot = nullptr;

    Read(){}
    Read(Idx* _slot) : slot(_slot) {
      ok = incReader(slot, &idx);                                          // does the new index pair need to be checked after calling this ?
    }
    ~Read(){ if(ok && slot) decReader(slot); }
    operator bool(){ return ok; }
  };
  struct  ReadPair
  {
    bool         ok = false;
    IdxPair   idxs;
    IdxPair*  slots = nullptr;

    ReadPair(){}
    ReadPair(IdxPair* _slots) : slots(_slots) {
      ok = incReaders(slots, &idxs);                                          // does the new index pair need to be checked after calling this ?
    }
    ~ReadPair(){ if(ok && slots) decReaders(slots); }
    operator bool(){ return ok; }
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
  u32*       listStart(u64 capacity){ return (u32*)(m_mem + offsetBytes_list(capacity)); }
  u32         allocIdx(u64 capacity)
  {
    ListHead* lh = listHead();
    u32     next = nxt( (au64*)lh, listStart(capacity) );
    return next;
  }
  u32          freeIdx(u32 idx, u64 capacity)
  {
    ListHead* lh = listHead();
    u32     prev = free( (au64*)lh, listStart(capacity), idx );
    return prev;
  }

  // utility functions
  HKV*        hkvStart(u64 capacity){ return (HKV*)(m_mem + offsetBytes_values(capacity)); }
  Idx*         slotPtr(){ return (Idx*)(m_mem + offsetBytes_slots()); }   // slotByteOffset(idx)); }
  Idx          getSlot(Idx* idx, u64 offset)
  {
    Idx ret;
    ret.asInt  =  ((au32*)(idx + offset))->load();
    
    return ret;
  }
  bool         swpPair(IdxPair* ip, IdxPair* prevIp) // = nullptr)
  {
    using namespace std;
        
    IdxPair nxtIp;
    nxtIp.first  = prevIp->second;
    nxtIp.second = prevIp->first;

    au64* aip = (au64*)ip;
    bool   ok = aip->compare_exchange_strong( prevIp->asInt, nxtIp.asInt); // memory_order_seq_cst);
    return ok;

    //if(prevIp){ *prevIp = oldIp; }
    //
    //IdxPair oldIp;
    //oldIp.asInt    =  aip->load();
  }
  Hash         hashKey(Key const& k)
  {
    return Hasher()(k);  // instances a hash function object and calls operator()
  }
  IdxPair     loadPair(void* pair)
  {
    IdxPair ret;
    ret.asInt = ((au64*)(pair))->load();
    return ret;
  }
  bool       halfEmpty(IdxPair p)
  {
    if(p.second.val_idx != EMPTY) return false;
    if(p.first.val_idx  >= SPECIAL_START) return false;
    
    return true;
  }
  bool        insertAt(u32 valIdx, Hash hash, u32 st, u64 capacity)
  {
    IdxPair  curPair, nxtPair;
    Idx*   slots = slotPtr();
    u32       en = (st+1) % capacity;
    auto       i = prev(st, capacity);
    IdxPair* cur = (IdxPair*)(slots + i);
    nxtPair = curPair = loadPair(cur);
    if( !halfEmpty(curPair) ){ return false; }

    nxtPair.second.readers = 1;  // todo: won't work! any return without swap will potentially decrement the readers on an EMPTY cell or some thing else this will be decremented to 0 by the ReadPair class
    nxtPair.second.val_idx = valIdx;

    // increment readers on the first
    ReadPair reader(cur);
    IdxPair  rdPair = reader.idxs;
    if( !halfEmpty(rdPair) ){ return false; }
    if( rdPair.first.readers == LIVE ){ return false; }

    // try to compare exchange both, replacing the empty with index
    bool ok = ((au64*)(cur))->compare_exchange_strong(rdPair.asInt, nxtPair.asInt);      // even though the reader count doesn't matter, it will need to be the same here  // either the new Idx reader needs to start at 1, or ReadPair needs to know which one it needs to decrement and pass that to inc and decReaders
    if(ok){ return true; }

    return false;
  }
  bool        sortPair(u32 rightSlot, u64 capacity)
  {
    IdxPair*   slotPair = (IdxPair*)(slotPtr() + rightSlot-1);    // todo: deal with first and last slot
    ReadPair   rp(slotPair);
    if(!rp) return false;

    if(rp.idxs.first.val_idx  == DELETED && 
       rp.idxs.second.val_idx == DELETED){
      sortPair(rightSlot+1, capacity);
      rp = ReadPair(slotPair);
    }else if(rp.idxs.first.readers==LIVE && rp.idxs.second.readers==LIVE){
      sortPair(rightSlot-1, capacity);
      rp = ReadPair(slotPair);
    }

    IdxPair  prev = rp.idxs;
    u32   lValIdx = prev.first.val_idx;
    u32   rValIdx = prev.second.val_idx;

    //u32     rdIdx = rp.idxs.second.val_idx;
    HKV*      hkv = hkvStart(capacity);
    Hash     lHsh = hkv[lValIdx].hash;
    Hash     rHsh = hkv[rValIdx].hash;
    if(lHsh == rHsh) return false;
    
    auto    lDist = wrapDist(lHsh,rightSlot,capacity);
    auto    rDist = wrapDist(rHsh,rightSlot,capacity);

    if( rDist > lDist+1 ){
      bool ok = swpPair(slotPair, &prev);
      if(ok) return true;
    }

    return false;

    // do the swap
    //IdxPair prev, nxt;
    //au64*  aip = (au64*)(slotPair);
    //      prev = rp.idxs;
    //nxt.first  = prev.second;
    //nxt.second = prev.first;
    //bool    ok = aip->compare_exchange_strong(prev.asInt, nxt.asInt);
  }
  void        placeIdx(u32 i, u32 valIdx, Hash hash, u64 capacity)
  {
    IdxPair* slotPair = (IdxPair*)(slotPtr() + i-1);    // todo: deal with first and last slot
    //IdxPair        ip = loadPair(slotPair);
    ReadPair rp(slotPair);
    if(rp.idxs.second.readers != LIVE){ return; }
    u32 rdIdx = rp.idxs.second.val_idx;
    if(rdIdx != valIdx){
      // find the index again if it isn't there
      // todo: will need to integrate this with an atomic compare and swap loop
    }

    // check that the first value's hash is the same - only need the hash because colliding keys might not be contiguous
    HKV*      hkv = hkvStart(capacity);
    Hash  curHash = hkv[rdIdx].hash;
    auto     dist = wrapDist(hash,i,capacity);
    //u32     ideal = curHash % capacity;
    


    // if(hash != curHash){ return false; }

    //auto dist = wrapDist(hash,i,capacity);
    if(hash != hkv[i].hash && wrapDist(curHash,i,capacity) >= dist){}
  }

  // data reading that assumes readers has already been incremented by the calling thread
  u64         wrapDist(HKV* hkv, u64 i, u64 mod)
  {
    u64 ideal = hkv[i].hash % mod;

    if(i >= ideal) return i - ideal;
    else           return i + (mod-ideal);
  }
  u64         wrapDist(Hash hash, u64 i, u64 mod)
  {
    u64 ideal = hash % mod;

    if(i >= ideal) return i - ideal;
    else           return i + (mod-ideal);
  }


  // public interface functions
  auto             get(Key const& key) -> Ret 
  {
    Hash   hsh  =  hashKey(key);
    Idx* slots  =  slotPtr();
    u64    cap  =  capacity();  // todo: if(mod==0) return optional<false> or iterator/return struct equivilent
    HKV*   hkv  =  hkvStart(cap);
    u64    mod  =  cap;
    i64      i  =  hsh % mod;
    u64     en  =  prev(i,mod);
    i64   dist  =  0;
    Ret ret;
    ret.ok = false;
    for(;;++i,++dist)
    {
      i %= mod;                                                                // get idx within capacity

      Read reader(slots+i);
      u32  valIdx = reader.idx.val_idx;
      if(valIdx==DELETED){ continue; }
      if(valIdx==EMPTY){ break; }
      
      HKV*  curHKV = hkv + valIdx;
      if(curHKV->hash==hsh && curHKV->key==key){               // check that they key is equal
         ret.value = curHKV->value;
         ret.ok    = true;
         return ret;
      }else if( (u64)dist > wrapDist(hkv,i,cap) ){                                    // dist should never be negative here, it is signed so that it can go negative and loop back around to be incremented back to 0
        break;  
      }

      if(i==en) break;                                                                // nothing found and the end has been reached, time to break out of the loop and return a reference to a KV with its type set to NONE
    }

    return ret;        // return a false value here since nothing was found
  }
  bool             put(Key const& key, Value const& value)
  {
    u64    cap  =  capacity();                                                // todo: if(mod==0) return false iterator/return struct equivilent
    u32  nxtIdx =  allocIdx(cap);
    if(nxtIdx==LIST_END){ return false; }

    bool valIns  =  false;
    Hash    hsh  =  hashKey(key);
    Idx*  slots  =  slotPtr();
    HKV*    hkv  =  hkvStart(cap);
    u64     mod  =  cap;
    i64       i  =  hsh % mod;
    u64      en  =  prev(i,mod);
    i64    dist  =  0;
    for(;;++i,++dist)
    {
      i %= mod;                                                               // get idx within capacity
      Read reader(slots+i);
      u32  valIdx = reader.idx.val_idx;
      if(valIdx==DELETED){ continue; }
      if(valIdx==EMPTY )
      { 
        // write hash key and value  
        if(!valIns){
          hkv[nxtIdx].hash  = hsh;
          hkv[nxtIdx].key   = key;
          hkv[nxtIdx].value = value;
          valIns = true;
        }

        if(dist==0){
          Idx swp, empty;
          swp.readers   = empty.readers = 0;
          swp.val_idx   = nxtIdx;
          au32*  slot   = (au32*)reader.slot;
          empty.val_idx = EMPTY;
          bool       ok = slot->compare_exchange_strong(empty.asInt, swp.asInt);
          if(ok){ return true; }                                                           // if insertion worked, return true, if not continue
          else{ continue; }
        }

        // more intricate insert if not in the ideal position - this needs to check for the end of the span by looking at two slots 
        bool ok = insertAt(nxtIdx, hsh, (u32)i, cap);
        if(ok){
          placeIdx((u32)i, nxtIdx, hsh, cap);
          return true; 
        }else{ 
          // todo: need to know why insertAt failed
          // if it was because the first is EMPTY 
          continue;
        }

      }

      if(i==en){ break; }                                                              // nothing found and the end has been reached, time to break out of the loop and return a reference to a KV with its type set to NONE
    }

    freeIdx(nxtIdx, cap);
    return false;        // return a false value here since nothing was found
  }
  u32             size(){ return header()->size; }
  u32         capacity()
  {
    auto hdr = header();
    u32  cap = (u32)( (hdr->sizeBytes-sizeof(Header))/sizeBytes_perCap() );
    return cap;
  }

  flf_map(){}
  flf_map(u64 capacity)
  {
    u64    cap2 = nextPowerOf2((u32)capacity);
    u64 szBytes = sizeBytes(cap2);
    m_mem       = (u8*)malloc(szBytes);                               //nullptr;  //  allocation cast to u8* goes here

    void* list_start_addr = m_mem + offsetBytes_list(cap2);          // nullptr;  // calculate the start address of the list here - sizeof(Head) + capacity*sizeof(Idx)

    Header*    h    = header();
    h->typeChar1    = 'L';
    h->typeChar2    = 'F';
    h->sizeBytes    = szBytes;
    h->size         = 0;
    h->valSizeBytes = sizeof(Value);

    ListHead initLH;
    initLH.idx = 0;
    initLH.ver = 0;
    h->lstHd.store(initLH);
    make_list( list_start_addr, (u32)cap2 );

    Idx initIdx;
    initIdx.readers = 0;
    initIdx.val_idx = EMPTY;
    Idx* slots = slotPtr();
    for(u32 i=0; i < cap2; ++i){ slots[i] = initIdx; }
    //listHead()->store(initLH);
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
  static bool        incReader(Idx* slot, Idx* readIdx=nullptr, i64 increment=1)
  {
    au32*   atmIncPtr  =  (au32*)(slot);
    Idx idx, newIdx;
    do{
      idx.asInt    = atmIncPtr->load(std::memory_order::memory_order_seq_cst);              // get the value of both Idx structs atomically
      newIdx.asInt = idx.asInt;

      if(idx.val_idx < SPECIAL_START){
        newIdx.readers  =  idx.readers + increment;                                        // increment the reader values if neithe of the indices have special values like EMPTY or DELETED
      }else{
        if(readIdx) readIdx->asInt = idx.asInt;
        return false; 
      }
    }while( !atmIncPtr->compare_exchange_strong(idx.asInt, newIdx.asInt) );                // store it back if the pair of indices hasn't changed - this is not an ABA problem because we aren't relying on the values at these indices yet, we are just incrementing the readers so that 1. the data is not deleted at these indices and 2. the indices themselves can't be reused until we decrement the readers

    if(readIdx) readIdx->asInt = newIdx.asInt;
    return true; // the readers were successfully incremented, but we need to return the indices that were swapped since we read them atomically
  }
  static bool        decReader(Idx* slot, Idx* readIdx=nullptr) 
  {
    return incReader(slot, readIdx, -1);
  }
  static bool       incReaders(void* oldIp, IdxPair*  newIp = nullptr, i64 increment=1) 
  {
    au64* atmIncPtr = (au64*)(oldIp);

    Idx idxs[2];
    u64 oldVal, newVal;
    do{
      oldVal          = atmIncPtr->load(std::memory_order::memory_order_seq_cst);        // get the value of both Idx structs atomically
      *((u64*)(idxs)) = oldVal;   // default memory order for now

      if( !idxs[0].inc(1) && !idxs[1].inc(1) ) return false;                             // if neither could be incremented then they are both special values, so return false

      newVal = *((u64*)(idxs));
    }while( !atmIncPtr->compare_exchange_strong(oldVal, newVal) );      // store it back if the pair of indices hasn't changed - this is not an ABA problem because we aren't relying on the values at these indices yet, we are just incrementing the readers so that 1. the data is not deleted at these indices and 2. the indices themselves can't be reused until we decrement the readers

    if(newIp) newIp->asInt = newVal;
    return true; // the readers were successfully incremented, but we need to return the indices that were swapped since we read them atomically
  }
  static bool       decReaders(void* oldIp, IdxPair*  newIp = nullptr) 
  {
    return incReaders(oldIp, newIp, -1);
  }


  // static functions for calculating the offset of each segment of the flat memory // The flat memory is organized as:  | Header | 32 bit readers + idx slots | concurrent linked list (linked with indices) | hash + key + values |
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









//Idx     empty;
//empty.readers = 0;

//Idx swp;
//swp.val_idx   = nxtIdx;
//swp.readers   = 0;
//au32*    slot = (au32*)reader.slot;
//Idx     empty;
//empty.val_idx = EMPTY;
//empty.readers = 0;
//bool       ok = slot->compare_exchange_strong(empty.asInt, swp.asInt);
//if(ok){ return true; }                                                         // if insertion worked, return true, if not continue
//else{ continue; }

//
//if( (u64)dist > wrapDist(hkv,i,cap) ){ break; }                                  // dist should never be negative here, it is signed so that it can go negative and loop back around to be incremented back to 0

//for(u32 i=st; i!=en; )
//{
//}

//static bool              inc(Idx& idx, i64 increment)
//{
//  if(idx.val_idx < SPECIAL_VALUE_START){
//    idx.readers  =  idx.readers + increment;        // increment the reader values if neithe of the indices have special values like EMPTY or DELETED
//    return true;
//  }else{ return false; }
//}

//if(idxs[0].val_idx < SPECIAL_VALUE_START &&
//  idxs[1].val_idx < SPECIAL_VALUE_START ){
//  idxs[0].readers  =  idxs[0].readers + increment;        // increment the reader values if neithe of the indices have special values like EMPTY or DELETED
//  idxs[1].readers  =  idxs[1].readers + increment;
//}else{
//  return false;
//}

//if(idxs[0].val_idx < SPECIAL_VALUE_START)
//  idxs[0].readers  =  idxs[0].readers + increment;        // increment the reader values if neithe of the indices have special values like EMPTY or DELETED

//if(idxs[1].val_idx < SPECIAL_VALUE_START )
//  idxs[1].readers  =  idxs[1].readers + increment;

//else{
//  return false;
//}

//u64 ideal = elems[idx].hsh.hash % mod;
//return wrapDist(ideal,idx,mod);

// if(curVals.second.val_idx == EMPTY &&
//
//IdxPair*  cur = (IdxPair*)(slots + i);
//au64*    acur = (au64*)(cur);
//curPair.asInt = acur->load();
//
//bool   scndOk = curPair.second.val_idx == EMPTY;
//bool   frstOk = curPair.first.val_idx < SPECIAL_VALUE_START;
//
//scndOk = curPair.second.val_idx == EMPTY;
//frstOk = curPair.first.val_idx < SPECIAL_VALUE_START;

//
//static const u32            LIST_END  =  0xFFFFFFFF;

//Idx curIdx  =  getSlot(slots, i);
//if(curIdx.val_idx==DELETED){ continue; }
//if(curIdx.val_idx==EMPTY){ break; }                                     // return something that will evaluate to false here
//
//Idx* curSlot = slots + i;
//Read reader(curSlot);
//if(!reader){ --i; --dist; continue; }    // this should never be hit because the read only fails on special values

//Read          putIdx()
//{  }

//Ret     ret;
//ret.ok = false;

//Idx curIdx  =  getSlot(slots, i);
//
//HKV* curHKV = hkv + curIdx.val_idx;
//if(curHKV->hash==hsh && curHKV->key==key){                              // check that they key is equal
//  ret.value = curHKV->value;
//  return ret;
//}else 

// || 
// (hkv[valIdx].hash==hsh && hkv[valIdx].key==key) ){
//
//HKV* curHKV = hkv + curIdx.val_idx;
//if(curHKV->hash==hsh && curHKV->key==key){                              // check that they key is equal
//  ret.value = curHKV->value;
//  return ret;
//}

//if(curIdx.val_idx==DELETED){ continue; }
//if(curIdx.val_idx==EMPTY){
//}
//
//Idx* curSlot = slots + i;
//Read reader(curSlot);
//if(!reader){ --i; --dist; continue; }                                   // todo: need to check if reader failed because of EMPTY or DELETE and make a decision, possibly integrating with the above logic

// -allocate an index (already allocated)
// -if insertion didn't work, continue, since insertion should only happen on EMPTY (so that multiple keys are supported)

//Idx idxs[2];
//*((u32*)(idxs)) = oldVal;   // default memory order for now
//newIdx = *((u32*)(idxs));

//using HKV     =   std::tuple<Hash,Key,Value>;
//struct   BlkMeta {
//};

//
// std::optional<Value>

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

//bool      ok = true;
//Idx* curSlot = slots + i;
//ok = incReaders( curSlot );                                          // does the new index pair need to be checked after calling this ?
//if(!ok) break;
//ok &= dist > wrapDist(hkv,i,cap);
//decReaders( curSlot );                       // todo: if this fails, it would need to find the current index again to make sure it decrements it
// if decReaders fails, find the slot again, maybe that should be part of decReaders, since it always needs to succeed
//
//if(!ok) break;

//
// return the value here // get the offset of the hash key value segment, offset by the index, the offset by the sizeof Hash and Key

//HshType eh = el[i].hsh;                                                // eh is element hash
//
//Idx* curSlot = slots + i;

//HshType hh;
//hh.hash   =  HashStr(key);
//u32  hsh  =  hh.hash;                                                 // make sure that the hash is being squeezed into the same bit depth as the hash in HshType
//if(out_hash){ *out_hash = hsh; }
//
//KV*   el  =  (KV*)elemStart();                                         // el is a pointer to the elements 
//
//u64  mod  =  map_capacity();
//if(mod==0) return nullptr;

//
//#include <optional>

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

