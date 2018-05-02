
/*
This file is meant to be made up of utilities that avoid using the C runtime 
*/

#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __NO_RT_UTIL_HEADERGUARD_H__
#define __NO_RT_UTIL_HEADERGUARD_H__

#include <type_traits>
#include <cstdint>
#include <limits>

//#ifdef _MSC_VER
//  #define _CRT_SECURE_NO_WARNINGS
//  #define _SCL_SECURE_NO_WARNINGS
//#endif
#ifdef _MSC_VER
  #if !defined(_CRT_SECURE_NO_WARNINGS)
    #define _CRT_SECURE_NO_WARNINGS
  #endif

  #if !defined(_SCL_SECURE_NO_WARNINGS)
    #define _SCL_SECURE_NO_WARNINGS
  #endif
#endif


#ifdef _MSC_VER  // taken from Cereal 
#  define NO_RT_DLL_EXPORT extern "C" __declspec(dllexport)
#  define NO_RT_USED
#else // clang or gcc
#   define NO_RT_DLL_EXPORT
#   define NO_RT_USED __attribute__ ((__used__))
#endif

using    u8  =   uint8_t;
using   u16  =  uint16_t;
using   u32  =  uint32_t;
using   ui8  =   uint8_t;
using   u64  =  uint64_t;
using    i8  =    int8_t;
using   i16  =   int16_t;
using   i32  =   int32_t;
using   i64  =   int64_t;
using   f32  =     float;
using   f64  =    double;
using  ui16  =  uint16_t;
using  ui32  =  uint32_t;
using  ui64  =  uint64_t;

#define PIf 3.141592653589f
#define PId 3.14159265358979323846264338327

#define SQR(x) ((x)*(x))


#ifndef TO
  #define            TO(to, var) for(uint64_t var = 0, lim = (uint64_t)to; var < lim; ++var)
#endif

#define           ALL(collection) collection.begin(), collection.end()
#define         RANGE(st, to, var)  for(std::remove_const<decltype(to)>::type var = st; var < to; ++var)

#ifndef FROM
  #define       FROM(from, var) for(int64_t var = ((i64)from)-1; var >= int64_t(0); --var)
#endif

#define       SECTION(_msvc_only_collapses_macros_with_arguments, ...)
#define LONGSTR_MACRO(src)  #src

//#define SIM_ALLOCA(Type, Count) (Type*)alloca( Count * sizeof(Type) );
//#define DECLTYPE_ALLOCA(Var, Count) (decltype(Var)*)alloca( Count * sizeof(decltype(Var)) );

#ifdef _MSC_VER
  //#ifdef _DEBUG
  #ifndef NDEBUG
    //#ifdef _MSC_VER
    //#define sim_assert(expL, comp, expR) if( (exp) (comp) (expR) ) { TELL((expL)) TELL((expR)) assert( (exp) (comp) (expR) ); }
    #define sim_assert(exp, varA, varB) if( (exp) == false ) { assert( (exp) ); }
    #define func_assert(exp, FUNC) if( (exp) == false ) { FUNC assert( (exp) );  }
  #else
    //#define sim_assert(expL, comp, expR) 
    #define sim_assert(exp, varA, varB) ;
    #define func_assert(exp, FUNC) ;
  #endif
#endif

#if defined (_MSC_VER)  // Visual studio
    #define THREAD_LOCAL __declspec( thread )
#elif defined (__GCC__) // GCC
    #define THREAD_LOCAL __thread
#endif

const float   INFf       = std::numeric_limits<float>::infinity();
const float   SIG_NANf   = std::numeric_limits<float>::signaling_NaN();

#ifdef NO_RT_UTIL_IMPL

void* __cdecl operator       new(size_t s)
{
  return malloc(s);
}
void __cdecl operator     delete(void* p)
{
  free(p);
}

extern "C" double   _hypot(double x, double y){ return 0; }
extern "C" void     _fltused(){}  // symbol needs to be defined for some reason

#endif

#endif








//using   i8  =              int8_t;
//using  i32  =             int32_t;
//using  i64  =           long long;

//
//#define            TO(to, var) for(std::remove_const<decltype( (ui64)to )>::type var = 0; var < to; ++var)

// for open subdiv
//#define M_PI PIf
//#define and &&
//#define not !
//#define or ||
// end open subdiv nonsense

//#ifndef FROM
//  #define       FROM(from, var) for(std::remove_const<decltype(from)>::type var = from-1; var >= decltype(from)(0); --var )
//#endif

// BEGIN printf
//#define PRINTF_BUF_SZ 4096
//#if defined (_MSC_VER)  // Visual studio
//  //==========================================
//  // LIBCTINY - Matt Pietrek 2001
//  // MSDN Magazine, January 2001
//  //==========================================
//  #define  WIN32_LEAN_AND_MEAN
//  #define  NOMINMAX
//  #include <windows.h>
//  #include <strsafe.h>
//  #include <stdio.h>
//  #include <stdarg.h>
//
//  // Force the linker to include USER32.LIB
//  #pragma comment(linker, "/defaultlib:user32.lib")
//
//  //extern "C" int __cdecl Printf(const char * format, ...)
//  inline int __cdecl Printf(const char * format, ...)
//  {
//      char szBuf[PRINTF_BUF_SZ];
//      int retValue;
//      DWORD cbWritten;
//      va_list argptr;
//          
//      va_start( argptr, format );
//      //retValue = wvsprintf( szBuff, format, argptr );
//      retValue = StringCchVPrintf(szBuf, PRINTF_BUF_SZ, format, argptr );
//      va_end( argptr );
//
//      auto len=strlen(szBuf);
//
//      WriteFile(  GetStdHandle(STD_OUTPUT_HANDLE), szBuf, (DWORD)len,
//                  &cbWritten, 0 );
//
//      return retValue;
//  }
//#endif
