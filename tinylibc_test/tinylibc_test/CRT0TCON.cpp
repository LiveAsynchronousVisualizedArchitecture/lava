//==========================================
// LIBCTINY - Matt Pietrek 2001
// MSDN Magazine, January 2001
// FILE: CRT0TCON.CPP
//==========================================
#include <windows.h>
#include "argcargv.h"
#include "initterm.h"

// Force the linker to include KERNEL32.LIB
#pragma comment(linker, "/defaultlib:kernel32.lib")

// Force 512 byte section alignment in the PE file
//#pragma comment(linker, "/OPT:NOWIN98")  //sbassett

//#pragma comment(linker, "/nodefaultlib:libc.lib")
//#pragma comment(linker, "/nodefaultlib:libcmt.lib")

extern "C" int __cdecl main(int, char **, char **);    // In user's code

//sbassett
//extern "C"
 //HANDLE _heapHndl;

//
// Modified version of the Visual C++ startup code.  Simplified to
// make it easier to read.  Only supports ANSI programs.
//
extern "C" void __cdecl mainCRTStartup( void )
{
  //sbassett
  //_heapHndl = HeapCreate(
  //  0,    // _In_ DWORD  flOptions,        // heap is not executable, doesn't generate exceptions and is serialized (thread safe through locks)
  //  0,    // _In_ SIZE_T dwInitialSize,    // 0 is default which is one page 
  //  0 );  // _In_ SIZE_T dwMaximumSize);   // 0 is default which means the heap can grow to available memory


    int mainret, argc;

    argc = _ConvertCommandLineToArgcArgv( );

    // set up our minimal cheezy atexit table
    _atexit_init();

    // Call C++ constructors
    _initterm( __xc_a, __xc_z );

    mainret = main( argc, _ppszArgv, 0 );

    // sbassett 
    // HeapDestroy(_heapHndl) ?

    _DoExit();

    ExitProcess(mainret);
}
