//==========================================
// LIBCTINY - Matt Pietrek 2001
// MSDN Magazine, January 2001
//==========================================
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

// Force the linker to include USER32.LIB
#pragma comment(linker, "/defaultlib:user32.lib")

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

