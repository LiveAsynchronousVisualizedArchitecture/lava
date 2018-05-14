

#define  _NO_CRT_STDIO_INLINE
#define  WIN32_LEAN_AND_MEAN
#define  WIN32_EXTRA_LEAN
#include <windows.h>
#include <shellapi.h>
#include <cstring>
#include "../../tbl.hpp"

//int main(int argc, char** argv)

extern "C" void entry(void)
{
  //if(argc < 2) return 0;
  //auto len = strlen(argv[1]);

  int      winArgc  =  0;
  LPWSTR*  winArgv  =  CommandLineToArgvW(GetCommandLineW(), &winArgc);
  if(winArgc < 2){ ExitProcess(0); }

  auto len = wcslen(winArgv[1]);

  char s[1024];
  sprintf(s, "%ls", winArgv[1]);

  tbl t;
  t("type") = tbl::StrToInt("ASCII");
  t.resize<int8_t>(len+1, 0);
  memcpy( t.data<void>(), s, len);

  //memcpy( t.data<void>(), argv[1], len);

  FILE *f = fopen("string.tbl", "wb");
  if(!f) ExitProcess(0);
  //if(!f) return 0;

  auto writeSz = fwrite(t.memStart(), 1, t.sizeBytes(), f);

  fclose(f);

  return ExitProcess(0);
}

