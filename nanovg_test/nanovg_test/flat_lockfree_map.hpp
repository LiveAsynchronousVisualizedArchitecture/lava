
// -todo: make sizeBytes() static function

// todo: make swapIdxPair function
// todo: make allocation function
// todo: fill out BlkMeta struct - hash and what else since the size is constant

#pragma once

#if !defined(__FLAT_LOCKFREE_MAP_HEADER_GUARD_HPP__)

#include <cstdint>
#include <atomic>

struct flf_map
{
  using   u8   =    uint8_t;
  using  u32   =   uint32_t;
  using  u64   =   uint64_t;
  using au64   =   std::atomic<uint64_t>;
  using Key    =   u64;
  using Value  =   u64;
  using Hash   =   std::hash<Key>;

  static const u32   EMPTY  =  0x00FFFFFF;              // max value of 2^24 to set all 24 bits of the value index
  static const u32 DELETED  =  0x00FFFFFE;              // one less than the max value above
  static const u32 SPECIAL_VALUE_START  =  DELETED;     // comparing to this is more clear than comparing to DELETED

  struct       Idx {
    u32 readers :  8;
    u32 val_idx : 24;
  };
  union    IdxPair {
    struct { Idx first; Idx second; };
    u64 asInt;
  };
  struct    Header {
    // first 8 bytes - two 1 bytes characters that should equal 'lm' for lockless map
    u64 typeChar1  :  8;
    u64 typeChar2  :  8;
    u64 sizeBytes  : 48;
    
    // next 8 bytes keep track of the size of the values and number inserted - from that and the sizeBytes, capacity can be inferred
    u64           size : 32;      // this could be only 24 bits since that is the max index
    u64   valSizeBytes : 32;
  };
  struct   BlkMeta {
  };

  u8*     m_mem = nullptr;  // single pointer is all that ends up on the stack

  u64   slotByteOffset(u64 idx){ return sizeof(Header) + idx*sizeof(IdxPair); }
  Idx*         slotPtr(u64 idx){ return (Idx*)(m_mem + slotByteOffset(idx)); }
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

    //nxtIp.asInt    =  aip->load();
    //Idx      tmp = nxtIp.first;
    
    IdxPair nxtIp;
    nxtIp.first  = oldIp.second;
    nxtIp.second = oldIp.first;

    bool ok = aip->compare_exchange_strong( oldIp.asInt, nxtIp.asInt, std::memory_order_seq_cst);

    if(prevIp){ *prevIp = oldIp; }
    return ok;
  }
  u64          hashKey(Key const& k)
  {
    return Hash()(k);  // instances a hash function object and calls operator()
  }


  static u64 sizeBytes(u64 capacity)
  {
    u64 szPerCap = sizeof(BlkMeta) + sizeof(Idx) + sizeof(Key) + sizeof(Value);
    return sizeof(Header) + capacity*szPerCap;
  }
};

#endif








//bool incTwoIdxReaders(u64 idx, IdxPair* newIp = nullptr)
//bool incIdxPairReaders(u64 idx, IdxPair* newIp = nullptr)
//
// u8*   firstIdxPtr  =  
// Idx*       idxPtr  =  (Idx*)(m_mem + idx*sizeof(Idx));     // it is crucial and fundamental that sizeof(Idx) needs to be 4 bytes so that two of them can be swapped even if unaligned, but this should be more correct and clear
//au64*   atmIncPtr  =  (au64*)(m_mem + idx*sizeof(Idx));       // it is crucial and fundamental that sizeof(Idx) needs to be 4 bytes so that two of them can be swapped even if unaligned, but this should be more correct and clear

//IdxPair ip;
//ip.asInt = newVal;

