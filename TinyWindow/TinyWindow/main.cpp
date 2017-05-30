
#include <Windows.h>
#include <string>
#include "no_rt_util.h"

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

//#include "GL/glew.h"
//#include "glfw3.h"
//void            errorCallback(int e, const char *d) {
//  printf("Error %d: %s\n", e, d);
//  fflush(stdout);
//}


#include "TinyWindow.h"
using namespace TinyWindow;
void HandleKeyPresses(tWindow* window, int key, keyState_t keyState)
{
  auto wm = (windowManager*)window->userData;

  if (keyState == keyState_t::down && key == spacebar)
  {
    //printf("spacebar has been pressed \n");
  }
  else if (key == del || key == ('q' - 32)) {
    wm->ShutDown();
  }
}
void PrintMonitorInfo(windowManager* manager)
{
  for (auto monitorIter : manager->GetMonitors())
  {
    printf("%s \n", monitorIter->deviceName.c_str());
    printf("%s \n", monitorIter->monitorName.c_str());
    printf("%s \n", monitorIter->displayName.c_str());
    printf("resolution:\t current width: %i | current height: %i \n", monitorIter->currentSetting->resolution.width, monitorIter->currentSetting->resolution.height);
    printf("extents:\t top: %i | left: %i | bottom: %i | right: %i \n", monitorIter->extents.top, monitorIter->extents.left, monitorIter->extents.bottom, monitorIter->extents.right);
    /*for (auto settingIter : monitorIter->settings)
    {
    printf("width %i | height %i | frequency %i | pixel depth: %i \n",
    settingIter->resolution.width, settingIter->resolution.height, settingIter->displayFrequency, settingIter->bitsPerPixel);
    }*/
    printf("\n");
  }
}

using ui32 = uint32_t;
using  i32 =  int32_t;


int main()
{
  //SECTION(glfw)
  //{
  //  glfwSetErrorCallback(errorCallback);
  //  if (!glfwInit()) {
  //    fprintf(stdout, "[GFLW] failed to init!\n");
  //    exit(1);
  //  }
  //
  //  GLFWwindow* win = glfwCreateWindow(512, 512, "Visualizer", NULL, NULL);
  //  glfwMakeContextCurrent(win);
  //
  //  while(!glfwWindowShouldClose(win))
  //  {
  //    glfwPollEvents();
  //
  //    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
  //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //
  //    glfwSwapBuffers(win);
  //  }
  //}

  SECTION(TinyWindow)
  {
    printWinError
    std::unique_ptr<windowManager> manager(new windowManager());
    printWinError
    std::unique_ptr<tWindow> window(manager->AddWindow("Example"));

    PrintMonitorInfo(manager.get());
    //
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

    glClearColor(0.25f, 0.25f, 0.25f, 0.0f);
    while (!window->shouldClose)
    {
      manager->PollForEvents();
    
      glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
      //if (spacePressed)
      //{
      //  window->SetStyle(style_t::popup);
      //  window->SetPosition(vec2_t<int>::Zero());
      //  window->SetResolution(vec2_t<unsigned int>(manager->GetMonitors().back()->extents.right, manager->GetMonitors().back()->extents.bottom));
      //  window->ToggleFullscreen(manager->GetMonitors()[0]);
      //  spacePressed = false;
      //}
    
      window->SwapDrawBuffers();
    }
    
    manager->ShutDown();
    tWindow* tempWindow = window.release();
    delete tempWindow;
  }

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


