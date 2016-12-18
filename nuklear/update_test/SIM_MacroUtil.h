


#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __MACROUTIL_HEADERGUARD_H__
#define __MACROUTIL_HEADERGUARD_H__

#ifdef _MSC_VER
  #define _CRT_SECURE_NO_WARNINGS 1
  #define _SCL_SECURE_NO_WARNINGS 1
#endif


//#include "stdafx.h"

#include <cassert>
#include <memory>
#include <chrono>
#include <string>
#include <iomanip>
#include <iostream>
#include <functional>
#include <vector>
#include <queue>
#include <set>
#include <random>


#ifdef DO_TIME
#define TIME(expr, str) Timer __ ## str ## _timer; __ ## str ## _timer.reset(); expr; __ ## str ##_timer.tellAndReset(#str);
#else 
#undef  SIM_TIME
#define SIM_TIME(expr, str) expr;
#endif

#define ENDL ::std::cout<<::std::endl;::std::cout.flush();

#define _MRK_          ::std::cout<<"Mark@Line "<<__LINE__<<::std::endl;::std::cout.flush();
#define _MRK_THREAD_   ::std::cout<<"Line "<<__LINE__<<" Thread: "<<::std::this_thread::get_id()<<::std::endl;::std::cout.flush();
#define TELL(var)      ::std::cout<<::std::endl<<::std::setprecision(4)<<#var ## ": "<<::std::endl<<var<<::std::endl;::std::cout.flush();
#define TELL_RAW(var)  ::std::cout<<::std::endl<<::std::setprecision(4)<< var <<::std::endl;::std::cout.flush();
#define TELL_TYPE(var) ::std::cout<<#var ## ": "<< typeid(var).name() <<::std::endl<<::std::endl;::std::cout.flush();
#define TELL2(varA, varB) { TELL(varA) TELL(varB) } 
#define TELL3(varA, varB, varC) { TELL(varA) TELL(varB) TELL(varC) } 
#define TELL4(varA, varB, varC, varD) { TELL(varA) TELL(varB) TELL(varC) TELL(varD) } 
#define TELL5(varA, varB, varC, varD, varE) { TELL(varA) TELL(varB) TELL(varC) TELL(varD) TELL(varE) } 
#define TELL6(varA, varB, varC, varD, varE, varF) { TELL(varA) TELL(varB) TELL(varC) TELL(varD) TELL(varE) TELL(varF) } 
#define TELL7(varA, varB, varC, varD, varE, varF, varG) { TELL6(varA, varB, varC, varD, varE, varF) TELL(varG) } 
#define TELL8(varA, varB, varC, varD, varE, varF, varG, varH) { TELL7(varA, varB, varC, varD, varE, varF, varG) TELL(varH) } 

#define PAUSE std::cout << "Paused at line " << __LINE__ << std::endl; int VAR##__LINE__; std::cin >> VAR##__LINE__;

#ifdef _DEBUG
 #define DEBUG_PAUSE PAUSE
#else
 #define DEBUG_PAUSE ((void)0);
#endif

// Iters
#ifndef ALL
  #define ALL(collection) std::begin(collection), std::end(collection)
#endif

#define ITERATE(_collection_, _iter_var_)  for(auto _iter_var_ = begin(_collection_); _iter_var_ != end(_collection_); ++_iter_var_)


#define TO(to, var)      for(std::remove_const<decltype(to)>::type var = 0; var < to; ++var)
#define FROM(from, var) for(std::remove_const<decltype(from)>::type var = from-1; var >= decltype(from)(0); --var )
//#define OMP_TO(to, var)  for(decltype(to) var = 0; var < to; ++var)

#define FN [&]()
#define COLLAPSE(_msvc_only_collapses_macros_with_arguments)
#define SECTION(_msvc_only_collapses_macros_with_arguments, ...)

#define SIM_ALLOCA(Type, Count) (Type*)alloca( Count * sizeof(Type) );
#define DECLTYPE_ALLOCA(Var, Count) (decltype(Var)*)alloca( Count * sizeof(decltype(Var)) );

#define UNZIP_PAIR(pair, declA, declB)   auto declA = (pair).first; auto declB = (pair).second; 
#define SPLIT(pair, declA, declB)        auto declA = (pair).first; auto declB = (pair).second;
#define SPLIT_PAIR(pair, declA, declB)   auto declA = (pair).first; auto declB = (pair).second;
#define ASSIGN_PAIR(pair, varA, varB)    varA = (pair).first;  varB = (pair).second;

//#define TYPE(exp) TELL(decltype(exp))
//#define TYPE(exp) std::cout << #exp << " " << typeid(exp).name() << std::endl;

#ifdef _MSC_VER
  #ifdef _DEBUG
    //#ifdef _MSC_VER
    //#define sim_assert(expL, comp, expR) if( (exp) (comp) (expR) ) { TELL((expL)) TELL((expR)) assert( (exp) (comp) (expR) ); }
    #ifndef sim_assert 
      #define sim_assert(exp, varA, varB) if( (exp) == false ) { TELL(varA) TELL(varB) assert( (exp) ); }
    #endif
    #define func_assert(exp, FUNC) if( (exp) == false ) { FUNC assert( (exp) );  }
  #else
    //#define sim_assert(expL, comp, expR) 
    #define sim_assert(exp, varA, varB) ;
    #define func_assert(exp, FUNC) ;
  #endif
#endif

#define GLSL_MACRO(src)     "#version 150 core\n" #src
#define GLSL_330_MACRO(src) "#version 330 core\n" #src
#define LONGSTR_MACRO(src)  #src

#define PIf 3.141592653589f
#define PId 3.14159265358979323846264338327

#if defined (_MSC_VER)  // Visual studio
    #define THREAD_LOCAL __declspec( thread )
#elif defined (__GCC__) // GCC
    #define THREAD_LOCAL __thread
#endif

#endif












//void PAUSE();
//void PAUSE_EXIT(int exitStatus = 0);
//void ERROR_PAUSE_EXIT(::std::string error);

//gcc and clang 	__attribute__((deprecated)) int a;
//__attribute__((deprecated("message"))) int a;
//Visual Studio 	__declspec(deprecated) int a; 	__declspec(deprecated("message")) int a;
//Embarcadero(1)
//int a[[deprecated]]; 	int a[[deprecated("message")]];

//#define ALL(collection) begin(collection), end(collection)
//#define TO(to, var) for(auto var : range<decltype(to)>(0, to))
//#define BEGIN do{
//#define END   }while(false);
//#define COLLAPSE(_msvc_only_collapses_macros_with_arguments) for(bool collapse=true; collapse; collapse=false)
