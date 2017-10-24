
#pragma once

#if !defined(__FLAT_LOCKFREE_MAP_HEADER_GUARD_HPP__)

#include <cstdint>

struct flf_map
{
  using  u8   =    uint8_t;
  using u32   =   uint32_t;
  using u64   =   uint64_t;

  struct Idx {
    u32 readers :  8;
    u32 val_idx : 24;
  };

  struct Header {
    u64 : 8
  };

 u8* m_mem;

  void incTwoIdxReaders(u64 idx)
  {
    m_mem
  }

};

#endif
