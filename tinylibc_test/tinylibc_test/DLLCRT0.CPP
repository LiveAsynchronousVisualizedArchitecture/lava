//==========================================
// LIBCTINY - Matt Pietrek 2001
// MSDN Magazine, January 2001
// FILE: DLLCRT0.CPP
//==========================================
#include <windows.h>
#include "initterm.h"

// Force the linker to include KERNEL32.LIB
#pragma comment(linker, "/defaultlib:kernel32.lib")

// Force 512 byte section alignment in the PE file
//#pragma comment(linker, "/OPT:NOWIN98")  //sbassett

// #pragma comment(linker, "/nodefaultlib:libc.lib")
// #pragma comment(linker, "/nodefaultlib:libcmt.lib")

// User routine DllMain is called on all notifications

extern BOOL WINAPI DllMain(
        HANDLE  hDllHandle,
        DWORD   dwReason,
        LPVOID  lpreserved
        ) ;

//
// Modified version of the Visual C++ startup code.  Simplified to
// make it easier to read.  Only supports ANSI programs.
//
extern "C"
BOOL WINAPI _DllMainCRTStartup(
        HANDLE  hDllHandle,
        DWORD   dwReason,
        LPVOID  lpreserved
        )
{
    if ( dwReason == DLL_PROCESS_ATTACH )
    {
        // set up our minimal cheezy atexit table
        _atexit_init();

        // Call C++ constructors
        _initterm( __xc_a, __xc_z );
    }

    BOOL retcode = DllMain(hDllHandle, dwReason, lpreserved);

    if ( dwReason == DLL_PROCESS_DETACH )
    {
        _DoExit();
    }

    return retcode ;
}
