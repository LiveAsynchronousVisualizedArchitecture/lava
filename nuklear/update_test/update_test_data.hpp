

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

static simdb db;



#endif
