
// printf
extern "C" int __cdecl printf(const char * format, ...);

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
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
// printf

//extern int WINAPI WinMain (
//    _In_ HINSTANCE hInstance,
//    _In_opt_ HINSTANCE hPrevInstance,
//    _In_ LPSTR lpCmdLine,
//    _In_ int nShowCmd
//    );

void entry();

void DllMain(){}
void main(){ entry(); }
int WINAPI WinMain (
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
    )
{
  entry();
}
//void WinMain(){ entry(); }

static char* strmsg = "hello tiny world";
void entry()
{

  auto mem = malloc()
  printf("hello tiny world");
}
