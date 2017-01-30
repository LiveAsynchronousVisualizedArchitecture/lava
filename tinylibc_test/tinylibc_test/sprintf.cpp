//==========================================
// LIBCTINY - Matt Pietrek 2001
// MSDN Magazine, January 2001
//==========================================
#include <windows.h>
#include <stdio.h>

#define EOF     (-1)

// Force the linker to include USER32.LIB
#pragma comment(linker, "/defaultlib:user32.lib")

extern "C" int __cdecl sprintf(char *buffer, const char * format, ...)
{
    int retValue;
    va_list argptr;
          
    va_start( argptr, format );
    retValue = wvsprintf( buffer, format, argptr );
    va_end( argptr );

    return retValue;
}
