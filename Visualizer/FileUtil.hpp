

#ifndef __FILEUTIL_HEADER_GUARD__
#define __FILEUTIL_HEADER_GUARD__

#include <SIM/Types.h>
#include <cstdio>
#include <fstream>

//std::ifstream::pos_type 
i64 FileSize(string const& filename)
{
  using namespace std;

  ifstream in(filename, ifstream::binary | ifstream::ate);
  
  return (i64)in.tellg(); 
}

vec<path> 
AllFiles(path dirPath)
{
  using namespace std::tr2::sys;

  vec<path> ret;

  directory_iterator dir( dirPath );
  directory_iterator endDir;
  for(; dir!=endDir; ++dir ) {
    ret.emplace_back( dir->path() );
  }

  return ret;
}

bool 
hasExtension(string const& s, string const& ext)
{
  return s.rfind(ext,s.size()) < s.size();
}

vec<ui8> 
LoadToBytes(string const& s)
{
  const int SZ = 65536; 
  ui8 buffer[SZ];
  
  vec<ui8> ret( FileSize(s) );
  FILE*  fp;
  auto   err    =  fopen_s( &fp, s.c_str(), "rb");
  size_t cnt    =  0;
  size_t total  =  0;
  do
  {
    cnt    =  fread(buffer, 1, SZ, fp);

    if( cnt+total > ret.size() )
      ret.resize( (ret.size()+cnt) * 2);
    
    memcpy( ret.data()+total, buffer, cnt);
    total  +=  cnt;
  } while(cnt > 0);
  fclose(fp);

  ret.shrink_to_fit();
  return ret;
}

#endif

