
//#include "no_rt_util.h"

#define LIBC_HPP_IMPL
#include "libc.h"

//extern "C" void  _entry()
//{ }

//#include <cstdio>

//extern "C" int __cdecl main(int argc, char** argv)
extern "C" int __cdecl _DllMainCRTStartup()
{
  //printf("\ntiny cpp\n");
  Print("\ntiny cpp\n");
  return 0;
}

