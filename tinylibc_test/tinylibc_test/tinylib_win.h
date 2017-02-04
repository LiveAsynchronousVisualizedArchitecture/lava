

#ifndef __TINYLIB_WIN_HEADERGUARD_H__
#define __TINYLIB_WIN_HEADERGUARD_H__

#if defined(_WIN32)

#include <cstdint>
#include <cstring>

// all linker pragmas
#pragma comment(linker, "/defaultlib:user32.lib")
#pragma comment(linker, "/defaultlib:kernel32.lib")


// all data segement declarations
#define _MAX_CMD_LINE_ARGS  128
char* _ppszArgv[_MAX_CMD_LINE_ARGS+1];

// windows specific headers
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

// windows CRT declarations
#include "initterm.h"
extern "C" void __cdecl WinMainCRTStartup(void)
{
  int mainret;
  char *lpszCommandLine;
  STARTUPINFO StartupInfo;

  lpszCommandLine = GetCommandLine();

  // Skip past program name (first token in command line).
  if( *lpszCommandLine == '"' )  // Check for and handle quoted program name
  {
    lpszCommandLine++;	// Get past the first quote

    // Now, scan, and skip over, subsequent characters until  another
    // double-quote or a null is encountered
    while( *lpszCommandLine && (*lpszCommandLine != '"') )
      lpszCommandLine++;

    if( *lpszCommandLine == '"' )     // If we stopped on a double-quote (usual case), skip over it.
      lpszCommandLine++;
  }else{    // First token wasn't a quote
    while( *lpszCommandLine > ' ' )
        lpszCommandLine++;
  }

  // Skip past any white space preceeding the second token.
  while( *lpszCommandLine && (*lpszCommandLine <= ' ') )
      lpszCommandLine++;

  StartupInfo.dwFlags = 0;
  GetStartupInfo( &StartupInfo );

  _atexit_init();    // set up our minimal cheezy atexit table
  _initterm( __xc_a, __xc_z );   // Call C++ constructors
  mainret = WinMain( GetModuleHandle(NULL),
                      NULL,
                      lpszCommandLine,
                      StartupInfo.dwFlags & STARTF_USESHOWWINDOW
                          ? StartupInfo.wShowWindow : SW_SHOWDEFAULT );

  _DoExit();
  ExitProcess(mainret);
}





extern "C" int __cdecl printf(const char * format, ...);
extern "C" int __cdecl printf(const char * format, ...)
{
    char szBuff[1024];
    int retValue;
    DWORD cbWritten;
    va_list argptr;
          
    va_start( argptr, format );
    retValue = wvsprintf( szBuff, format, argptr );
    va_end( argptr );

    WriteFile(  GetStdHandle(STD_OUTPUT_HANDLE), szBuff, retValue,
                &cbWritten, 0 );

    return retValue;
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
    cbCmdLine = lstrlen( pszSysCmdLine );

    // Allocate memory to store a copy of the command line.  We'll modify
    // this copy, rather than the original command line.  Yes, this memory
    // currently doesn't explicitly get freed, but it goes away when the
    // process terminates.
    pszCmdLine = (PSTR)HeapAlloc( GetProcessHeap(), 0, cbCmdLine+1 );
    if ( !pszCmdLine )
        return 0;

    // Copy the system version of the command line into our copy
    lstrcpy( pszCmdLine, pszSysCmdLine );

    if ( '"' == *pszCmdLine )   // If command line starts with a quote ("),
    {                           // it's a quoted filename.  Skip to next quote.
        pszCmdLine++;
    
        _ppszArgv[0] = pszCmdLine;  // argv[0] == executable name
    
        while ( *pszCmdLine && (*pszCmdLine != '"') )
            pszCmdLine++;

        if ( *pszCmdLine )      // Did we see a non-NULL ending?
            *pszCmdLine++ = 0;  // Null terminate and advance to next char
        else
            return 0;           // Oops!  We didn't see the end quote
    }
    else    // A regular (non-quoted) filename
    {
        _ppszArgv[0] = pszCmdLine;  // argv[0] == executable name

        while ( *pszCmdLine && (' ' != *pszCmdLine) && ('\t' != *pszCmdLine) )
            pszCmdLine++;

        if ( *pszCmdLine )
            *pszCmdLine++ = 0;  // Null terminate and advance to next char
    }

    // Done processing argv[0] (i.e., the executable name).  Now do th
    // actual arguments

    argc = 1;

    while ( 1 )
    {
        // Skip over any whitespace
        while ( *pszCmdLine && (' ' == *pszCmdLine) || ('\t' == *pszCmdLine) )
            pszCmdLine++;

        if ( 0 == *pszCmdLine ) // End of command line???
            return argc;

        if ( '"' == *pszCmdLine )   // Argument starting with a quote???
        {
            pszCmdLine++;   // Advance past quote character

            _ppszArgv[ argc++ ] = pszCmdLine;
            _ppszArgv[ argc ] = 0;

            // Scan to end quote, or NULL terminator
            while ( *pszCmdLine && (*pszCmdLine != '"') )
                pszCmdLine++;
                
            if ( 0 == *pszCmdLine )
                return argc;
            
            if ( *pszCmdLine )
                *pszCmdLine++ = 0;  // Null terminate and advance to next char
        }
        else                        // Non-quoted argument
        {
            _ppszArgv[ argc++ ] = pszCmdLine;
            _ppszArgv[ argc ] = 0;

            // Skip till whitespace or NULL terminator
            while ( *pszCmdLine && (' '!=*pszCmdLine) && ('\t'!=*pszCmdLine) )
                pszCmdLine++;
            
            if ( 0 == *pszCmdLine )
                return argc;
            
            if ( *pszCmdLine )
                *pszCmdLine++ = 0;  // Null terminate and advance to next char
        }

        if ( argc >= (_MAX_CMD_LINE_ARGS) )
            return argc;
    }
}

// Begin malloc, realloc, calloc, free, new and delete
extern "C" void* __cdecl  malloc(size_t bytes)
{
  //return (void*)HeapAlloc(_heapHndl, 0, bytes);
  return HeapAlloc( GetProcessHeap(), 0, bytes);
}
extern "C" void* __cdecl  calloc(size_t nitems, size_t size)
{
  //return HeapAlloc( _heapHndl, HEAP_ZERO_MEMORY, nitems * size );
  return HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, nitems * size);
}
extern "C" void* __cdecl realloc(void * p, size_t size)
{
  if(p)
    return HeapReAlloc( GetProcessHeap(), 0, p, size );
  else    // 'p' is 0, and HeapReAlloc doesn't act like realloc() here
    return HeapAlloc( GetProcessHeap(), 0, size );
}
extern "C" void  __cdecl    free(_Pre_maybenull_ _Post_invalid_ void*    mem)
{
  //HeapFree(_heapHndl, 0, mem);
  HeapFree( GetProcessHeap(), 0, mem);
}
void* __cdecl operator       new(size_t s)
{
  return HeapAlloc( GetProcessHeap(), 0, s);
}
void __cdecl operator     delete(void* p)
{
  HeapFree( GetProcessHeap(), 0, p);
}
// End malloc, realloc, calloc, free, new and delete

extern "C" double   _hypot(double x, double y){ return 0; }
extern "C" void     _fltused(){}  // symbol needs to be defined for some reason
//#endif  // end hypot function from musl (needed by std::vector for some reason)

//#if defined(_DEBUG)  // begin memcpy and memset (from musl)
void* memcpy(void* dest, const void* src, size_t n)
{
  unsigned char* d = (unsigned char*)dest;
  const unsigned char* s = (const unsigned char*)src;

  for(; n; n--) *d++ = *s++;
	return dest;
}
#pragma warning(disable:4146)  // hides msvc 'negative op on unsigned' warning
void* memset(void *dest, int c, size_t n)
{
	unsigned char* s = (unsigned char*)dest;
	size_t k;

	/* Fill head and tail with minimal branching. Each
	 * conditional ensures that all the subsequently used
	 * offsets are well-defined and in the dest region. */
	if (!n) return dest;
	s[0] = s[n-1] = c;
	if (n <= 2) return dest;
	s[1] = s[n-2] = c;
	s[2] = s[n-3] = c;
	if (n <= 6) return dest;
	s[3] = s[n-4] = c;
	if (n <= 8) return dest;

	/* Advance pointer to align it at a 4-byte boundary,
	 * and truncate n to a multiple of 4. The previous code
	 * already took care of any head/tail that get cut off
	 * by the alignment. */
  	k = -(uintptr_t)s & 3;

  //k = (-((uintptr_t)s)) & 3;
	s += k;
	n -= k;
	n &= -4;

  /* Pure C fallback with no aliasing violations. */
	for(; n; n--, s++) *s = c;

	return dest;
}
#pragma warning(default:4146)  
//#endif  // end memcpy and memset (from musl)

// begin various windows entry points
void entry(int argc, char** argv);

void DllMain(){}
void main(int argc, char** argv){ entry(argc, argv); }
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

// end various windows entry points
#endif  // end win32
#endif  // end header guard



//
//extern HANDLE _heapHndl;  // by using the process heap and not a separate heap, memory bugs are more likely to crash and be found?

//
//void WinMain(){ entry(); }

//
//extern "C" void* memset(void *dest, int c, size_t n);

//extern "C" void * __cdecl calloc(size_t nitems, size_t size)
//{
//    return HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, nitems * size );
//}

// begin hypot function from musl (needed by std::vector for some reason)
//#if FLT_EVAL_METHOD > 1U && LDBL_MANT_DIG == 64
//#define SPLIT (0x1p32 + 1)
//#else
//#define SPLIT (0x1p27 + 1)
//#endif

//static void sq(double_t *hi, double_t *lo, double x)
//{
//	double_t xh, xl, xc;
//
//	xc = (double_t)x*SPLIT;
//	xh = x - xc + xc;
//	xl = x - xh;
//	*hi = (double_t)x*x;
//	*lo = xh*xh - *hi + 2*xh*xl + xl*xl;
//}
//extern "C" double _hypot(double x, double y)
//{
//	union {double f; uint64_t i;} ux = {x}, uy = {y}, ut;
//	int ex, ey;
//	double_t hx, lx, hy, ly, z;
//
//	/* arrange |x| >= |y| */
//	ux.i &= -1ULL>>1;
//	uy.i &= -1ULL>>1;
//	if (ux.i < uy.i) {
//		ut = ux;
//		ux = uy;
//		uy = ut;
//	}
//
//	/* special cases */
//	ex = ux.i>>52;
//	ey = uy.i>>52;
//	x = ux.f;
//	y = uy.f;
//	/* note: hypot(inf,nan) == inf */
//	if (ey == 0x7ff)
//		return y;
//	if (ex == 0x7ff || uy.i == 0)
//		return x;
//	/* note: hypot(x,y) ~= x + y*y/x/2 with inexact for small y/x */
//	/* 64 difference is enough for ld80 double_t */
//	if (ex - ey > 64)
//		return x + y;
//
//	/* precise sqrt argument in nearest rounding mode without overflow */
//	/* xh*xh must not overflow and xl*xl must not underflow in sq */
//	z = 1;
//	if (ex > 0x3ff+510) {
//		z = 0x1p700;
//		x *= 0x1p-700;
//		y *= 0x1p-700;
//	} else if (ey < 0x3ff-450) {
//		z = 0x1p-700;
//		x *= 0x1p700;
//		y *= 0x1p700;
//	}
//	sq(&hx, &lx, x);
//	sq(&hy, &ly, y);
//	return z*sqrt(ly+lx+hy+hx);
//}

//#if !defined(_DEBUG) 
//extern "C" 
//uint8_t __fltused;
