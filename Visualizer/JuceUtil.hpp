


#ifndef __JUCEUTIL_HEADER_GUARD__
#define __JUCEUTIL_HEADER_GUARD__

#include <functional>
#include <filesystem>
#include <SIM/Types.h>
#include "JuceHeader.h"

using namespace std::tr2::sys;

  template<class FUNC>
struct TimedCall : public Timer
{
  std::function<FUNC> func;
  TimedCall(){}
  TimedCall(FUNC _func) : func(func) {}
  void timerCallback() override { func(); }
};

inline path
JuceExecDir()
{
  auto cur = File::getSpecialLocation( File::SpecialLocationType::currentExecutableFile );
  path curPath( cur.getFullPathName().toStdString() );
  curPath.remove_filename();
  return curPath;
}

inline vec<ui8>
MemoryBlockToVec(MemoryBlock const& mb)
{
  vec<ui8> ret(mb.getSize());
  memcpy(ret.data(), mb.getData(), mb.getSize());
  return ret;
}

inline vec<path>    
GetExtensionPaths(string const& extension)
{
  vec<path> ret;

  directory_iterator libDir( JuceExecDir() );
  directory_iterator endDir;
  for(; libDir!=endDir; libDir++ )
  {
    auto& pstr = libDir->path().string();
    if( pstr.rfind(extension, pstr.size()) < pstr.size() )
      ret.emplace_back( libDir->path() );
  }
  return ret;
}


#endif

