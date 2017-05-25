
#include <Windows.h>
#include <string>

auto      GetLastErrorStdStr() -> std::string
{
  DWORD error = GetLastError();
  if (error)
  {
    LPVOID lpMsgBuf;
    DWORD bufLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );
    if (bufLen)
    {
      LPCSTR lpMsgStr = (LPCSTR)lpMsgBuf;
      std::string result(lpMsgStr, lpMsgStr+bufLen);
  
      LocalFree(lpMsgBuf);

      return result;
    }
  }
  return std::string();
}
#include <iostream>
#define printWinError std::cout << "\n" <<  __LINE__ << " windows error " << GetLastErrorStdStr() << std::endl;

#include <cstdint>
#include "TinyWindow.h"

using ui32 = uint32_t;
using  i32 =  int32_t;

using namespace TinyWindow;

void HandleKeyPresses(tWindow* window, int key, keyState_t keyState)
{
  auto wm = (windowManager*)window->userData;

	if(keyState == keyState_t::down && key == spacebar)
	{
		//printf("spacebar has been pressed \n");
	}
  else if(key==del || key== ('q'-32) ){
    wm->ShutDown(); 
  }
}


int main()
{
  printWinError
  std::unique_ptr<windowManager> manager(new windowManager());
  printWinError
  std::unique_ptr<tWindow> window(manager->AddWindow("Example"));
  //PrintMonitorInfo(manager.get());

  //manager->keyEvent = HandleKeyPresses;
  //manager->mouseButtonEvent = HandleMouseClick;
  //manager->mouseWheelEvent = HandleMouseWheel;
  //manager->destroyedEvent = HandleShutdown;
  //manager->maximizedEvent = HandleMaximized;
  //manager->minimizedEvent = HandleMinimized;
  //manager->focusEvent = HandleFocus;
  //manager->movedEvent = HandleMovement;
  //manager->resizeEvent = HandleResize;
  ////manager->mouseMoveEvent = HandleMouseMovement;

  glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
  while (!window->shouldClose)
  {
    manager->PollForEvents();

    //if (spacePressed)
    //{
    //  window->SetStyle(style_t::popup);
    //  window->SetPosition(vec2_t<int>::Zero());
    //  window->SetResolution(vec2_t<unsigned int>(manager->GetMonitors().back()->extents.right, manager->GetMonitors().back()->extents.bottom));
    //  window->ToggleFullscreen(manager->GetMonitors()[0]);
    //  spacePressed = false;
    //}

    window->SwapDrawBuffers();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  manager->ShutDown();
  tWindow* tempWindow = window.release();
  delete tempWindow;

  return 0;
}

//int __stdcall WinMain(
//  _In_ HINSTANCE hInstance,
//  _In_ HINSTANCE hPrevInstance,
//  _In_ LPSTR     lpCmdLine,
//  _In_ int       nCmdShow
//  )
//int main()
//{  
//  auto wm = new windowManager();
//  wm->keyEvent = HandleKeyPresses;
//
//  auto wn = wm->AddWindow("Example", wm); // nullptr, vec2_t<ui32>(512,512), 8, 8, 8); 
//  wn->SetResolution(vec2_t<ui32>(512,512));
//  wn->SetPosition(vec2_t<i32>(1024,768));
//
//  glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
//  while(!wn->shouldClose)
//  {
//    wm->PollForEvents();
//    wn->SwapDrawBuffers();
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//  }
//
//  wm->ShutDown();
//  
//  //int a;
//  //std::cin >> a;
//
//  return 0;
//}


