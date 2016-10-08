
#ifndef __DYNLIB_HEADER_GUARD__
#define __DYNLIB_HEADER_GUARD__

//#include "Viz.h"
#include <SIM/SIM_GeneralTemplateUtil.hpp>
#include <SIM/SIM_Concurrency.hpp>
#include "JuceHeader.h"
#include <SIM/Types.h>

class  DynLib : public juce::Timer
{
private:
  using SharedDynLib  =  shared<DynamicLibrary>;
  using FileTime      =  juce::Time;

  SharedDynLib   m_lib;
  bool           m_loaded          =  false;
  path           m_livePath;
  path           m_origPath;
  FileTime       m_lastReload;
  double         m_minReloadSecs   =  0.0;
  mutable bool   m_fresh           =  false;

  void      mv(DynLib&& rval)
  {
    m_lib          =  move(rval.m_lib);
    m_loaded       =  rval.m_loaded;
    m_livePath     =  move(rval.m_livePath);
    m_origPath     =  move(rval.m_origPath);
    //m_lastMod      =  rval.m_lastMod;
    m_lastReload   =  rval.m_lastReload;

    rval.m_lib         =  nullptr;
    rval.m_loaded      =  false;
    //rval.m_lastMod     =  FileTime();
    rval.m_lastReload  =  FileTime();
  }
  void      waitForNoMoreWrite()
  {
    auto lastMod = lastModOrig();
    SleepMs(200);
    while( (lastModOrig()-lastMod).inSeconds() > 0 ) {
      lastMod = lastModOrig();
      SleepMs(200);
    }
  }
  bool      copyToLive()
  {
    bool ok = true;
    if(!exists(m_livePath) && exists(m_origPath))
    {
      File orig(m_origPath.string());
      File live(m_livePath.string());
      path tmpPath = m_origPath;
      tmpPath.replace_extension(".tmp.dll");
      File tmp(tmpPath.string());

      waitForNoMoreWrite();
      
      ok = ok && orig.copyFileTo(tmp);
      ok = ok && rename(tmpPath, m_livePath);
    }    
    return ok;
    //if(!ok) return false;
    //copy_file(m_origPath, m_livePath, copy_option::overwrite_if_exists);
    //ok = ok && orig.copyFileTo(live);
    //ok = ok && tmp.copyFileTo(orig);
  }
  FileTime  lastModOrig()
  {
    if(exists(m_origPath))
      return File(m_origPath.string()).getLastModificationTime();
    else
      return FileTime();
  }
  double    ReloadModifiedDif()
  {
    return (m_lastReload - lastModOrig()).inSeconds();
  }

public:
  DynLib(){}
  DynLib(path const& p, double minReloadSecs = 0.0) : 
    m_lastReload(),
    m_minReloadSecs(minReloadSecs)
  {
    m_lib       =  make_shared<DynamicLibrary>();
    m_origPath  =  p;
    m_livePath  =  m_origPath;
    m_livePath.replace_extension(".live.dll");
    removeLive();
  }
  ~DynLib()
  {
    //m_lib->close();
    removeLive();
  }

  DynLib           (DynLib&& rval)
  { mv(move(rval)); }
  DynLib& operator=(DynLib&& rval)
  { mv(move(rval)); return *this; }

  void   timerCallback() override
  {
    reload();
  }

  bool   load()
  {
    if(!m_loaded && copyToLive())
    {
      auto& pstr    =  m_livePath.string();
      m_lib->open(pstr);
      m_lastReload  =  Time::getCurrentTime();
      m_loaded      =  true;
      m_fresh       =  true;
    }
    
    //m_lib          =  make_shared<DynamicLibrary>(pstr);
    //m_lastMod      =  File(pstr).getLastModificationTime();
    //
    //if(m_lib.get()!=nullptr) {
    //  m_lastReload  =  Time::getCurrentTime();
    //  m_loaded      =  true;
    //}

    return m_loaded;
  }
  void   close()
  {
    if(!this->empty()) {
      m_lib->close();
      m_loaded = false;
    }
  }
  bool   removeLive()
  {
    close();
    return std::tr2::sys::remove(m_livePath);
  }
  void*  getFunction(string const& name) const
  {
    m_fresh  =  false;
    if(!this->empty()) {
      return m_lib->getFunction(name);
    }
    else
      return nullptr;
  }
  bool   reload()
  {
    bool   reloaded  =  false;
    auto   dif       =  (Time::getCurrentTime()-m_lastReload);
    double difS      =  dif.inSeconds();
    if(difS>m_minReloadSecs && ReloadModifiedDif()<0) // this should short circuit and not check the modification time more than minReloadSecs
    {
      removeLive();
      load();
      //if(copyToLive()) load();
      m_lastReload  =  Time::getCurrentTime();
      reloaded      =  true;
    }

    return reloaded;
  }
  bool   empty()    const  { return !m_loaded || m_lib.get()==nullptr; }
  bool   fresh()    const  { return m_fresh; }
  void   fresh(bool b)     { m_fresh = b; }
  path   livePath() const  { return m_livePath; }
  path   origPath() const  { return m_origPath; }
};


#endif







//m_origPath  =  JuceExecDir() / p;
//load();
//if(copyToLive()) load();
