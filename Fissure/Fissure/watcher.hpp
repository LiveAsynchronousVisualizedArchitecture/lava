
#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __WATCHER_HEADERGUARD_HPP__
#define __WATCHER_HEADERGUARD_HPP__

#define  WIN32_LEAN_AND_MEAN
#define  WIN32_EXTRA_LEAN
#define  NOMINMAX
#include <windows.h>

struct Watcher
{
  std::thread      m_thrd;
  bool           m_change = false;
  bool            m_error = false;

#ifdef _WIN32
  HANDLE     m_dir = nullptr;
  DWORD  m_status  = 0; 
#endif

  Watcher() : m_dir(nullptr){}
  Watcher(const char* path)
  {
    using namespace std;
    
    #ifdef _WIN32
      m_dir = FindFirstChangeNotification( 
        path,                                   // directory to watch 
        FALSE,                                  // do not watch subtree 
        FILE_NOTIFY_CHANGE_LAST_WRITE);

      if(m_dir == INVALID_HANDLE_VALUE){
        m_error = true;
        m_dir   = nullptr;
      }
    #endif

    //m_thrd = thread([m_dir](){
      //WaitForSingleObject(m_dir, INFINITE);
    //});
  }
  ~Watcher()
  {
    #ifdef _WIN32
      if(m_dir){
        FindCloseChangeNotification(m_dir);
      }
    #endif

    if(m_thrd.joinable()){
      m_thrd.join();
    }
  }

  //bool check(int waitTimeMillis=0)
  //{
  //  WaitForSingleObject(m_dir, waitTimeMillis); // 
  //}
  bool wait()
  {
    #ifdef _WIN32
      WaitForSingleObject(m_dir, INFINITE);
    #endif
  }
};

#endif



//
//FILE_NOTIFY_CHANGE_FILE_NAME);     // watch file name changes 
