
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

  static const u32   EMPTY  =  0x00FFFFFF;     // max value of 2^24 to set all 24 bits of the value index
  static const u32 DELETED  =  0x00FFFFFE;     // one less than the max value above
  static const u32 SPECIAL_VALUE_START  =  DELETED;  // comparing to this is more clear than comparing to DELETED

  struct Idx {
    u32 readers :  8;
    u32 val_idx : 24;
  };
  union IdxPair {
    struct { Idx first; Idx second; };
    u64 asInt;
  };
  struct Header {
    // first 8 bytes - two 1 bytes characters that should equal 'lm' for lockless map
    u64 typeChar1  :  8;
    u64 typeChar2  :  8;
    u64 sizeBytes  : 48;
    
    // next 8 bytes keep track of the size of the values and number inserted - from that and the sizeBytes, capacity can be inferred
    u64           size : 32;      // this could be only 24 bits since that is the max index
    u64   valSizeBytes : 32;
  };

  u8* m_mem;  // single pointer is all that ends up on the stack

  bool incTwoIdxReaders(u64 idx, IdxPair* newIp = nullptr)
  {
    // u8*   firstIdxPtr  =  
    // Idx*       idxPtr  =  (Idx*)(m_mem + idx*sizeof(Idx));     // it is crucial and fundamental that sizeof(Idx) needs to be 4 bytes so that two of them can be swapped even if unaligned, but this should be more correct and clear
    au64*   atmIncPtr  =  (au64*)(m_mem + idx*sizeof(Idx));       // it is crucial and fundamental that sizeof(Idx) needs to be 4 bytes so that two of them can be swapped even if unaligned, but this should be more correct and clear

    Idx idxs[2];
    u64 oldVal;
    u64 newVal;
    do{
      // get the value of both Idx structs atomically
      oldVal          = atmIncPtr->load(std::memory_order::memory_order_seq_cst);
      *((u64*)(idxs)) = oldVal;   // default memory order for now
 
      // increment the reader values
      if(idxs[0].val_idx < SPECIAL_VALUE_START &&
         idxs[1].val_idx < SPECIAL_VALUE_START ){
        idxs[0].readers  +=  1;
        idxs[1].readers  +=  1;
      }else{
        return false;
      }

      newVal = *((u64*)(idxs));
    }while( atmIncPtr->compare_exchange_strong(oldVal, newVal) );      // store it back if the pair of indices hasn't changed - this is not an ABA problem because we aren't relying on the values at these indices yet, we are just incrementing the readers so that 1. the data is not deleted at these indices and 2. the indices themselves can't be reused until we decrement the readers

    //IdxPair ip;
    //ip.asInt = newVal;
    if(newIp) newIp->asInt = newVal;

    return true; // the readers were successfully incremented, but we need to return the indices that were swapped since we read them atomically
  }

};

#endif
