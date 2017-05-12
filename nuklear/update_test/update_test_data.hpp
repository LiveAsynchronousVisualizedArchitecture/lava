

#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __UPDATE_TEST_DATA_STRUCTURES_HEADERGUARD_HPP__
#define __UPDATE_TEST_DATA_STRUCTURES_HEADERGUARD_HPP__

//#ifdef _MSC_VER
//  #define _CRT_SECURE_NO_WARNINGS 1
//  #define _SCL_SECURE_NO_WARNINGS 1
//#endif
#ifdef _MSC_VER
  #if !defined(_CRT_SECURE_NO_WARNINGS)
    #define _CRT_SECURE_NO_WARNINGS
  #endif

  #if !defined(_SCL_SECURE_NO_WARNINGS)
    #define _SCL_SECURE_NO_WARNINGS
  #endif
#endif

#include "../../DataStore/ConcurrentMap/simdb.hpp"
#include "../IndexedVerts.h"
#include "ChanImg.hpp"

//using  ui8  =  unsigned      char;
//using ui32  =  unsigned       int;
//using ui64  =  unsigned long long;
//using  i64  =           long long;

//using    u8   =   uint8_t;
//using   u32   =   uint32_t;
//using   u64   =   uint64_t;
//using    i8   =   int8_t;
using   i32   =   int32_t;
//using   i64   =   int64_t;
//using  au64   =   std::atomic<u64>;
//using  au32   =   std::atomic<u32>;
using  f32  =               float;
using  f64  =              double;

template<class T, class A=std::allocator<T> > using vec = std::vector<T, A>;

static simdb db;



#endif
