
//#include <iostream>
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

//int __stdcall WinMain(
//  _In_ HINSTANCE hInstance,
//  _In_ HINSTANCE hPrevInstance,
//  _In_ LPSTR     lpCmdLine,
//  _In_ int       nCmdShow
//  )
int main()
{  
  auto wm = new windowManager();
  wm->keyEvent = HandleKeyPresses;

  auto wn = wm->AddWindow("Example", wm); // nullptr, vec2_t<ui32>(512,512), 8, 8, 8); 
  wn->SetResolution(vec2_t<ui32>(512,512));
  wn->SetPosition(vec2_t<i32>(1024,768));

  glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
  while(!wn->shouldClose)
  {
    wm->PollForEvents();
    wn->SwapDrawBuffers();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  wm->ShutDown();
  
  //int a;
  //std::cin >> a;

  return 0;
}
