


#include <windows.h>
#include "initterm.h"


// CRT0WIN.cpp

//// Force the linker to include KERNEL32.LIB
//#pragma comment(linker, "/defaultlib:kernel32.lib")
//
//// Force 512 byte section alignment in the PE file
////#pragma comment(linker, "/OPT:NOWIN98") // sbassett
//// #pragma comment(linker, "/nodefaultlib:libc.lib")
//// #pragma comment(linker, "/nodefaultlib:libcmt.lib")
////
//// Modified version of the Visual C++ startup code.  Simplified to
//// make it easier to read.  Only supports ANSI programs.
////
//extern "C" void __cdecl WinMainCRTStartup(void)
//{
//  int mainret;
//  char *lpszCommandLine;
//  STARTUPINFO StartupInfo;
//
//  lpszCommandLine = GetCommandLine();
//
//  // Skip past program name (first token in command line).
//
//  if (*lpszCommandLine == '"')  // Check for and handle quoted program name
//  {
//    lpszCommandLine++;	// Get past the first quote
//
//                        // Now, scan, and skip over, subsequent characters until  another
//                        // double-quote or a null is encountered
//    while (*lpszCommandLine && (*lpszCommandLine != '"'))
//      lpszCommandLine++;
//
//    // If we stopped on a double-quote (usual case), skip over it.
//
//    if (*lpszCommandLine == '"')
//      lpszCommandLine++;
//  }
//  else    // First token wasn't a quote
//  {
//    while (*lpszCommandLine > ' ')
//      lpszCommandLine++;
//  }
//
//  // Skip past any white space preceeding the second token.
//
//  while (*lpszCommandLine && (*lpszCommandLine <= ' '))
//    lpszCommandLine++;
//
//  StartupInfo.dwFlags = 0;
//  GetStartupInfo(&StartupInfo);
//
//  // set up our minimal cheezy atexit table
//  _atexit_init();
//
//  // Call C++ constructors
//  _initterm(__xc_a, __xc_z);
//
//  mainret = WinMain(GetModuleHandle(NULL),
//    NULL,
//    lpszCommandLine,
//    StartupInfo.dwFlags & STARTF_USESHOWWINDOW
//    ? StartupInfo.wShowWindow : SW_SHOWDEFAULT);
//
//  _DoExit();
//
//  ExitProcess(mainret);
//}
//
//
//
//
//


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
extern "C" void __cdecl mainCRTStartup(void)
{
  //sbassett
  //_heapHndl = HeapCreate(
  //  0,    // _In_ DWORD  flOptions,        // heap is not executable, doesn't generate exceptions and is serialized (thread safe through locks)
  //  0,    // _In_ SIZE_T dwInitialSize,    // 0 is default which is one page 
  //  0 );  // _In_ SIZE_T dwMaximumSize);   // 0 is default which means the heap can grow to available memory

  int mainret, argc;

  argc = _ConvertCommandLineToArgcArgv();

  // set up our minimal cheezy atexit table
  _atexit_init();

  // Call C++ constructors
  _initterm(__xc_a, __xc_z);

  mainret = main(argc, _ppszArgv, 0);

  // sbassett 
  // HeapDestroy(_heapHndl) ?

  _DoExit();

  ExitProcess(mainret);
}


