
//#include <windows.h>
//#include "argcargv.h"
//#include "initterm.h"
//
// windows specific headers
//#define WIN32_LEAN_AND_MEAN
//#define NOMINMAX
//#include <windows.h>
//
// windows CRT declarations
//#include "initterm.h"

// Force 512 byte section alignment in the PE file
//#pragma comment(linker, "/OPT:NOWIN98")  //sbassett
//
//#pragma comment(linker, "/nodefaultlib:libc.lib")
//#pragma comment(linker, "/nodefaultlib:libcmt.lib")

//sbassett
//_heapHndl = HeapCreate(
//  0,    // _In_ DWORD  flOptions,        // heap is not executable, doesn't generate exceptions and is serialized (thread safe through locks)
//  0,    // _In_ SIZE_T dwInitialSize,    // 0 is default which is one page 
//  0 );  // _In_ SIZE_T dwMaximumSize);   // 0 is default which means the heap can grow to available memory
//
// sbassett 
// HeapDestroy(_heapHndl) ?

