
/*
This file is meant to be made up of utilities that avoid using the C runtime 
*/

#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __NO_RT_UTIL_HEADERGUARD_H__
#define __NO_RT_UTIL_HEADERGUARD_H__

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

//using  ui8  =  unsigned      char;
//using ui32  =  unsigned       int;
//using ui64  =  unsigned long long;
//using  i64  =           long long;
//using  f32  =               float;
//using  f64  =              double;


#define PIf 3.141592653589f
#define PId 3.14159265358979323846264338327

// for open subdiv
#define M_PI PIf
//#define and &&
//#define not !
//#define or ||
// end open subdiv nonsense


#define           ALL(collection) collection.begin(), collection.end()
#define            TO(to, var) for(std::remove_const<decltype(to)>::type var = 0; var < to; ++var)
#define         RANGE(st, to, var)  for(std::remove_const<decltype(to)>::type var = st; var < to; ++var)
#define          FROM(from, var) for(std::remove_const<decltype(from)>::type var = from-1; var >= decltype(from)(0); --var )
#define       SECTION(_msvc_only_collapses_macros_with_arguments, ...)
#define LONGSTR_MACRO(src)  #src

//#define SIM_ALLOCA(Type, Count) (Type*)alloca( Count * sizeof(Type) );
//#define DECLTYPE_ALLOCA(Var, Count) (decltype(Var)*)alloca( Count * sizeof(decltype(Var)) );

#ifdef _MSC_VER
  #ifdef _DEBUG
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


// BEGIN printf
#define PRINTF_BUF_SZ 4096
#if defined (_MSC_VER)  // Visual studio
  //==========================================
  // LIBCTINY - Matt Pietrek 2001
  // MSDN Magazine, January 2001
  //==========================================
  #define  WIN32_LEAN_AND_MEAN
  #define  NOMINMAX
  #include <tchar.h>
  #include <windows.h>
  #include <strsafe.h>
  #include <stdio.h>
  #include <stdarg.h>

  // Force the linker to include USER32.LIB
  #pragma comment(linker, "/defaultlib:user32.lib")

  //extern "C" int __cdecl Printf(const char * format, ...)
  inline int __cdecl Printf(const char * format, ...)
  {
      char szBuf[PRINTF_BUF_SZ];
      int retValue;
      DWORD cbWritten;
      va_list argptr;
          
      va_start( argptr, format );
      //retValue = wvsprintf( szBuff, format, argptr );
      retValue = StringCchVPrintf(szBuf, PRINTF_BUF_SZ, format, argptr );
      va_end( argptr );

      auto len=strlen(szBuf);

      WriteFile(  GetStdHandle(STD_OUTPUT_HANDLE), szBuf, (DWORD)len,
                  &cbWritten, 0 );

      return retValue;
  }
#endif



#endif
