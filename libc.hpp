
//==========================================
// LIBCTINY - Matt Pietrek 2001
// MSDN Magazine, January 2001
//==========================================

#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __LIBC_HPP_HEADERGUARD_H__
#define __LIBC_HPP_HEADERGUARD_H__

//#include <exception>

  //[[noreturn]] void terminate(){}

//namespace std
//{
  //[[noreturn]] void terminate() noexcept;
  //[[noreturn]] void terminate() ;
//}


#if defined(_WIN32)

#pragma comment(linker, "/defaultlib:user32.lib")
#pragma comment(linker, "/defaultlib:kernel32.lib")
#pragma comment(lib,    "../../shared/sys_msvcrt.lib")

#ifndef UNICODE
  //#define UNICODE
#endif

#define _NO_CRT_STDIO_INLINE
#define  WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

extern "C" void __cdecl __std_terminate(){}
extern "C" void __cdecl __CxxFrameHandler3(){}
extern "C" void __cdecl __chkstk(){}
extern "C" __declspec(noreturn) void __cdecl _invalid_parameter_noinfo_noreturn(void){}
namespace std
{
  [[noreturn]] void __cdecl _Xbad_alloc(void){}
  [[noreturn]] void __cdecl _Xlength_error(char const *){}
}

//int __cdecl printf(const wchar_t * format, ...)
//{
//  wchar_t szBuff[4096];
//  int retValue;
//  DWORD cbWritten;
//  va_list argptr;
//
//  va_start( argptr, format );
//  retValue = wvsprintf( szBuff, format, argptr );
//  va_end( argptr );
//
//  WriteFile(  GetStdHandle(STD_OUTPUT_HANDLE), szBuff, retValue,
//    &cbWritten, 0 );
//
//  return retValue;
//}

//// windows specific headers
//#define WIN32_LEAN_AND_MEAN
//#define NOMINMAX
//#include <windows.h>

extern char * _ppszArgv[];
int __cdecl _ConvertCommandLineToArgcArgv(void);

typedef void(__cdecl *_PVFV)(void);
extern _PVFV __xc_a[], __xc_z[];    /* C++ initializers */

void __cdecl _initterm(_PVFV * pfbegin, _PVFV * pfend);
void __cdecl _atexit_init(void);
void __cdecl _DoExit(void);

//#pragma comment(linker, "/defaultlib:kernel32.lib")

//#if defined(need_to_make_subsystem_windows_switch)
  //extern "C" int __cdecl main(int, char **, char **);
  extern "C" int __cdecl main(int, char**);

  extern "C" void __cdecl mainCRTStartup(void)
  {
    int mainret, argc;

    argc = _ConvertCommandLineToArgcArgv();

    _atexit_init();                              // set up our minimal cheezy atexit table

    _initterm(__xc_a, __xc_z);                   // Call C++ constructors
    //mainret = main(argc, _ppszArgv, 0);
    mainret = main(argc, _ppszArgv);

    _DoExit();
    ExitProcess(mainret);
  }
//#endif

#include <cstdint>
#include <cstring>

#define _MAX_CMD_LINE_ARGS  128
char* _ppszArgv[_MAX_CMD_LINE_ARGS + 1];

//extern "C" void __cdecl WinMainCRTStartup(void)
//{
//  int mainret;
//  char *lpszCommandLine;
//  STARTUPINFO StartupInfo;
//
//  lpszCommandLine = GetCommandLine();
//
//  // Skip past program name (first token in command line).
//  if (*lpszCommandLine == '"')  // Check for and handle quoted program name
//  {
//    lpszCommandLine++;	// Get past the first quote
//
//                        // Now, scan, and skip over, subsequent characters until  another
//                        // double-quote or a null is encountered
//    while (*lpszCommandLine && (*lpszCommandLine != '"'))
//      lpszCommandLine++;
//
//    if (*lpszCommandLine == '"')     // If we stopped on a double-quote (usual case), skip over it.
//      lpszCommandLine++;
//  }
//  else {    // First token wasn't a quote
//    while (*lpszCommandLine > ' ')
//      lpszCommandLine++;
//  }
//
//  // Skip past any white space preceeding the second token.
//  while (*lpszCommandLine && (*lpszCommandLine <= ' '))
//    lpszCommandLine++;
//
//  StartupInfo.dwFlags = 0;
//  GetStartupInfo(&StartupInfo);
//
//  _atexit_init();    // set up our minimal cheezy atexit table
//  _initterm(__xc_a, __xc_z);   // Call C++ constructors
//  mainret = WinMain(GetModuleHandle(NULL),
//    NULL,
//    lpszCommandLine,
//    StartupInfo.dwFlags & STARTF_USESHOWWINDOW
//    ? StartupInfo.wShowWindow : SW_SHOWDEFAULT);
//
//  _DoExit();
//  ExitProcess(mainret);
//}

//extern "C" int __cdecl printf(const char * format, ...);
//extern "C" int __cdecl printf(const char * format, ...)
//{
//  char szBuff[1024];
//  int retValue;
//  DWORD cbWritten;
//  va_list argptr;
//
//  va_start(argptr, format);
//  retValue = wvsprintf(szBuff, format, argptr);
//  va_end(argptr);
//
//  WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), szBuff, retValue,
//    &cbWritten, 0);
//
//  return retValue;
//}

// Force the linker to include USER32.LIB
//#pragma comment(linker, "/defaultlib:user32.lib")

//extern "C" int __cdecl Printf(const char * format, ...)

//#include <strsafe.h>
////#include <stdio.h>
////#include <stdarg.h>
//#define PRINTF_BUF_SZ 4096
//inline int __cdecl Printf(const char * format, ...)
//{
//    char szBuf[PRINTF_BUF_SZ];
//    int retValue;
//    DWORD cbWritten;
//    va_list argptr;
//        
//    va_start( argptr, format );
//    //retValue = wvsprintf( szBuff, format, argptr );
//    retValue = StringCchVPrintf(szBuf, PRINTF_BUF_SZ, format, argptr );
//    va_end( argptr );
//
//    auto len=strlen(szBuf);
//
//    OutputDebugString(szBuf);
//    //WriteFile(  GetStdHandle(STD_OUTPUT_HANDLE), szBuf, (DWORD)len, &cbWritten, 0 );
//
//    return retValue;
//}

void Print(const char* s)
{
  AllocConsole();
  auto hndl = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD cbWritten;
  WriteFile(hndl, s, strlen(s), &cbWritten, 0);
  //OutputDebugString(s);
}
      

// command line arguments to fill argc and argv
int __cdecl _ConvertCommandLineToArgcArgv(void)
{
  int cbCmdLine;
  int argc;
  PSTR pszSysCmdLine, pszCmdLine;

  // Set to no argv elements, in case we have to bail out
  _ppszArgv[0] = 0;

  // First get a pointer to the system's version of the command line, and figure out how long it is.
  pszSysCmdLine = GetCommandLine();
  cbCmdLine = lstrlen(pszSysCmdLine);

  // Allocate memory to store a copy of the command line.  We'll modify
  // this copy, rather than the original command line.  Yes, this memory
  // currently doesn't explicitly get freed, but it goes away when the
  // process terminates.
  pszCmdLine = (PSTR)HeapAlloc(GetProcessHeap(), 0, cbCmdLine + 1);
  if (!pszCmdLine)
    return 0;

  // Copy the system version of the command line into our copy
  lstrcpy(pszCmdLine, pszSysCmdLine);

  if ('"' == *pszCmdLine)   // If command line starts with a quote ("),
  {                           // it's a quoted filename.  Skip to next quote.
    pszCmdLine++;

    _ppszArgv[0] = pszCmdLine;  // argv[0] == executable name

    while (*pszCmdLine && (*pszCmdLine != '"'))
      pszCmdLine++;

    if (*pszCmdLine)      // Did we see a non-NULL ending?
      *pszCmdLine++ = 0;  // Null terminate and advance to next char
    else
      return 0;           // Oops!  We didn't see the end quote
  }
  else    // A regular (non-quoted) filename
  {
    _ppszArgv[0] = pszCmdLine;  // argv[0] == executable name

    while (*pszCmdLine && (' ' != *pszCmdLine) && ('\t' != *pszCmdLine))
      pszCmdLine++;

    if (*pszCmdLine)
      *pszCmdLine++ = 0;  // Null terminate and advance to next char
  }

  // Done processing argv[0] (i.e., the executable name).  Now do th
  // actual arguments

  argc = 1;

  while (1)
  {
    // Skip over any whitespace
    while (*pszCmdLine && (' ' == *pszCmdLine) || ('\t' == *pszCmdLine))
      pszCmdLine++;

    if (0 == *pszCmdLine) // End of command line???
      return argc;

    if ('"' == *pszCmdLine)   // Argument starting with a quote???
    {
      pszCmdLine++;   // Advance past quote character

      _ppszArgv[argc++] = pszCmdLine;
      _ppszArgv[argc] = 0;

      // Scan to end quote, or NULL terminator
      while (*pszCmdLine && (*pszCmdLine != '"'))
        pszCmdLine++;

      if (0 == *pszCmdLine)
        return argc;

      if (*pszCmdLine)
        *pszCmdLine++ = 0;  // Null terminate and advance to next char
    }
    else                        // Non-quoted argument
    {
      _ppszArgv[argc++] = pszCmdLine;
      _ppszArgv[argc] = 0;

      // Skip till whitespace or NULL terminator
      while (*pszCmdLine && (' ' != *pszCmdLine) && ('\t' != *pszCmdLine))
        pszCmdLine++;

      if (0 == *pszCmdLine)
        return argc;

      if (*pszCmdLine)
        *pszCmdLine++ = 0;  // Null terminate and advance to next char
    }

    if (argc >= (_MAX_CMD_LINE_ARGS))
      return argc;
  }
}

extern "C" void* __cdecl   malloc(size_t bytes)
{
  //return (void*)HeapAlloc(_heapHndl, 0, bytes);
  return HeapAlloc(GetProcessHeap(), 0, bytes);
}
extern "C" void* __cdecl   calloc(size_t nitems, size_t size)
{
  //return HeapAlloc( _heapHndl, HEAP_ZERO_MEMORY, nitems * size );
  return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nitems * size);
}
extern "C" void* __cdecl  realloc(void * p, size_t size)
{
  if (p)
    return HeapReAlloc(GetProcessHeap(), 0, p, size);
  else    // 'p' is 0, and HeapReAlloc doesn't act like realloc() here
    return HeapAlloc(GetProcessHeap(), 0, size);
}
extern "C" void  __cdecl     free(_Pre_maybenull_ _Post_invalid_ void*    mem)
{
  //HeapFree(_heapHndl, 0, mem);
  HeapFree(GetProcessHeap(), 0, mem);
}
void* __cdecl operator        new(size_t s)
{
  return HeapAlloc(GetProcessHeap(), 0, s);
}
void  __cdecl operator     delete(void*  p)
{
  HeapFree(GetProcessHeap(), 0, p);
}

extern "C" double   _hypot(double x, double y) { return 0; }
extern "C" void     _fltused() {}  // symbol needs to be defined for some reason
//#endif  // end hypot function from musl (needed by std::vector for some reason)

// begin various windows entry points
//void entry(int argc, char** argv);
//int  main(int argc, char** argv) { entry(argc, argv); }
//int WINAPI WinMain (
//    _In_ HINSTANCE hInstance,
//    _In_opt_ HINSTANCE hPrevInstance,
//    _In_ LPSTR lpCmdLine,
//    _In_ int nShowCmd
//    )
//{
//  auto argc = _ConvertCommandLineToArgcArgv();
//  entry(argc, _ppszArgv);
//}
//void DllMain() {}
// end various windows entry points

extern "C" int    __cdecl   isspace(int c){ return c == ' ' || (unsigned)c - '\t' < 5; }
extern "C" int    __cdecl   isdigit(int c){ return (unsigned)c - '0' < 10; }
extern "C" long   __cdecl      atol(const char * pstr)
{
  int  cCurr;    // The current character.
  long lTotal;   // The running total.
  int  iIsNeg;   // Holds the '-' sign.

  while(isspace(*pstr)){ ++pstr; }        // Slide past any whitespace.

  // Get the current character.
  cCurr = *pstr++;
  // Save the negative sign.
  iIsNeg = cCurr;
  if (('-' == cCurr) || ('+' == cCurr))
  {
    // We have a sign, so skip it.
    cCurr = *pstr++;
  }

  // Initialize the total.
  lTotal = 0;

  // While we have digits, addem up.
  while (isdigit(cCurr))
  {
    // Add this digit to the total.
    lTotal = 10 * lTotal + (cCurr - '0');
    // Do the next character.
    cCurr = *pstr++;
  }

  // If we have a negative sign, convert the value.
  if ('-' == iIsNeg)
  {
    return (-lTotal);
  }
  else
  {
    return (lTotal);
  }
}
extern "C" int    __cdecl      atoi(const char * pstr)
{
  return ((int)atol(pstr));
}

//extern "C" char*  __cdecl   _strupr(char *s)
//{
//  CharUpperBuff(s, lstrlen(s));
//  return s;
//}
//extern "C" char*  __cdecl   _strlwr(char *s)
//{
//  CharLowerBuff(s, lstrlen(s));
//  return s;
//}
//extern "C" int    __cdecl  _strcmpi(const char *s1, const char *s2)
//{
//  return lstrcmpi(s1, s2);
//}
//extern "C" int    __cdecl  _stricmp(const char *s1, const char *s2)
//{
//  return lstrcmpi(s1, s2);
//}
//#define strupr  _strupr
//#define strlwr  _strlwr

#endif  // end win32
#endif  // end header guard

#if defined(LIBC_HPP_IMPL)
  #pragma data_seg(".CRT$XCA")
  _PVFV __xc_a[] = { NULL };

  #pragma data_seg(".CRT$XCZ")
  _PVFV __xc_z[] = { NULL };

  #pragma data_seg()  /* reset */

  #pragma comment(linker, "/merge:.CRT=.data")

  typedef void(__cdecl *_PVFV)(void);

  void __cdecl _initterm(
    _PVFV * pfbegin,
    _PVFV * pfend
  )
  {
    /*
    * walk the table of function pointers from the bottom up, until
    * the end is encountered.  Do not skip the first entry.  The initial
    * value of pfbegin points to the first valid entry.  Do not try to
    * execute what pfend points to.  Only entries before pfend are valid.
    */
    while (pfbegin < pfend)
    {
      // if current table entry is non-NULL, call thru it.
      if (*pfbegin != NULL)
        (**pfbegin)();
      ++pfbegin;
    }
  }

  static _PVFV * pf_atexitlist = 0;
  static unsigned max_atexitlist_entries = 0;
  static unsigned cur_atexitlist_entries = 0;

  void __cdecl _atexit_init(void)
  {
    max_atexitlist_entries = 32;
    pf_atexitlist = (_PVFV *)calloc(max_atexitlist_entries,
      sizeof(_PVFV*));
  }

  int __cdecl atexit(_PVFV func)
  {
    if (cur_atexitlist_entries < max_atexitlist_entries)
    {
      pf_atexitlist[cur_atexitlist_entries++] = func;
      return 0;
    }

    return -1;
  }

  void __cdecl _DoExit(void)
  {
    if (cur_atexitlist_entries)
    {
      _initterm(pf_atexitlist,
        // Use ptr math to find the end of the array
        pf_atexitlist + cur_atexitlist_entries);
    }
  }

  //namespace std
  //{
  //  //_STD terminate();
  //  [[noreturn]] _STD void terminate() noexcept {}
  //}

  extern "C" void __cdecl __std_terminate() {}
  extern "C" void __cdecl __CxxFrameHandler3() {}

  void __cdecl terminate() {}

  //namespace std
  //{
    //[[noreturn]] void ::std::terminate() noexcept {}
    //void __cdecl _std_terminate() noexcept {}
   // void __cdecl terminate() {}
  //}
#endif








//#pragma comment(linker, "/defaultlib:user32.lib")
//#pragma comment(linker, "/defaultlib:kernel32.lib")

//
// Modified version of the Visual C++ startup code.  Simplified to
// make it easier to read.  Only supports ANSI programs.
//

//void* memcpy(void* dest, const void* src, size_t n)
//{
//  unsigned char* d = (unsigned char*)dest;
//  const unsigned char* s = (const unsigned char*)src;
//
//  for (; n; n--) *d++ = *s++;
//  return dest;
//}
//#pragma warning(disable:4146)  // hides msvc 'negative op on unsigned' warning
//void* memset(void *dest, int c, size_t n)
//{
//  unsigned char* s = (unsigned char*)dest;
//  size_t k;
//
//  /* Fill head and tail with minimal branching. Each
//  * conditional ensures that all the subsequently used
//  * offsets are well-defined and in the dest region. */
//  if (!n) return dest;
//  s[0] = s[n - 1] = c;
//  if (n <= 2) return dest;
//  s[1] = s[n - 2] = c;
//  s[2] = s[n - 3] = c;
//  if (n <= 6) return dest;
//  s[3] = s[n - 4] = c;
//  if (n <= 8) return dest;
//
//  /* Advance pointer to align it at a 4-byte boundary,
//  * and truncate n to a multiple of 4. The previous code
//  * already took care of any head/tail that get cut off
//  * by the alignment. */
//  k = -(uintptr_t)s & 3;
//
//  //k = (-((uintptr_t)s)) & 3;
//  s += k;
//  n -= k;
//  n &= -4;
//
//  /* Pure C fallback with no aliasing violations. */
//  for (; n; n--, s++) *s = c;
//
//  return dest;
//}
//#pragma warning(default:4146)  
