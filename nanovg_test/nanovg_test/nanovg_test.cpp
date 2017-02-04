
//#include <GL/GL.h>
//#include "glfw_3.2.1.h"

#include "glew_2.0.0.h"
#include "glfw3.h"

#include "nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION   // Use GL2 implementation.
#include "nanovg_gl.h"

#include "../nanovg-master/example/demo.h"
//#include "../nanovg-master/example/perf.h"

#define ENTRY_DECLARATION int main(void)
#ifdef _MSC_VER
  //#pragma comment(lib, "glfw3dll.lib")
  #pragma comment(lib, "opengl32.lib")
  //#pragma comment(lib, "glew32.lib")

  //#define USE_CONSOLE                                 // turning this off will use the windows subsystem in the linker and change the entry point to WinMain() so that no command line/console will appear
  #ifndef USE_CONSOLE
    #pragma comment( linker, "/subsystem:windows" )
    #undef ENTRY_DECLARATION
    #define ENTRY_DECLARATION int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
  #endif
#endif

#include "no_rt_util.h"
#include <cstdio>

const int TITLE_MAX_LEN = 256;

struct bnd { float xmn, ymn, xmx, ymx; };

GLFWwindow* win;
char winTitle[TITLE_MAX_LEN];
int premult = 0;
bnd nbnd;

namespace{

float               lerp(float p, float lo, float hi)
{
  return lo*(1-p) + hi*p;
}
float            linNorm(float n, float lo, float hi)
{
  return (n-lo) / (hi-lo);
}
float              remap(float n, float lo, float hi, float toLo, float toHi)
{
  //float p = (n-lo) / (hi-lo);
  float p = linNorm(n, lo, hi);
  return lerp(p, toLo, toHi);
}
bool                isIn(float x, float y, bnd const& b)
{
  return x>b.xmn && x<b.xmx && y>b.ymn && y<b.ymx;
}
int              isBlack(NVGcolor col)
{
	if( col.r == 0.0f && col.g == 0.0f && col.b == 0.0f && col.a == 0.0f )
	{
		return 1;
	}
	return 0;
}
static char*    cpToUTF8(int cp, char* str)
{
	int n = 0;
	if (cp < 0x80) n = 1;
	else if (cp < 0x800) n = 2;
	else if (cp < 0x10000) n = 3;
	else if (cp < 0x200000) n = 4;
	else if (cp < 0x4000000) n = 5;
	else if (cp <= 0x7fffffff) n = 6;
	str[n] = '\0';
	switch (n) {
	case 6: str[5] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x4000000;
	case 5: str[4] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x200000;
	case 4: str[3] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x10000;
	case 3: str[2] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x800;
	case 2: str[1] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0xc0;
	case 1: str[0] = cp;
	}
	return str;
}

bnd                 node(NVGcontext* vg, 
                            int preicon, 
                       const char* text, 
                       float x, float y, 
                       float w, float h, 
                           NVGcolor col,
                        float       rnd)   // rnd is corner rounding
{
  const int   border = 2;
  const float   rthk = 8.f;    // rw is rail thickness

	NVGpaint bg;
	char icon[8];
	float rad = lerp(rnd, 0.f, h/2.f);         // rad is corner radius
	float  tw = 0, iw = 0;

  SECTION(grey border)
  {
	  nvgBeginPath(vg);
     nvgRoundedRect(vg, x,y,w,h, rad+border); //-.5f);
	   //nvgRoundedRect(vg, x+0.5f,y+0.5f, w-1,h-1, cornerRad-.5f);
    nvgFillColor(vg, nvgRGBA(0,0,0,128));
	  nvgFill(vg);
    //nvgStrokeColor(vg, nvgRGBA(0,0,0,128));
	  //nvgStroke(vg);
  }
  SECTION(shaded color inside)
  {
    int    grad = (int)lerp(rnd, 0, 48);
    auto topClr = nvgRGBA(255,255,255,isBlack(col)?16:grad);
    auto botClr = nvgRGBA(0,0,0,isBlack(col)?16:grad);
	  bg = nvgLinearGradient(vg, x,y,x,y+h, topClr, botClr);
	  nvgBeginPath(vg);
	    nvgRoundedRect(vg, x+border,y+border, w-(border*2),h-(border*2), rad); //-1);
	    if(!isBlack(col)){
		    nvgFillColor(vg, col);
		    nvgFill(vg);
	    }
	  nvgFillPaint(vg, bg);
	  nvgFill(vg);
  }
  SECTION(rounded rails)
  {
    //float cntrX=x+border+rad, cntrY=y+border+h/2, rr=rad;        // rr is rail radius
    float cntrX=x+rad, cntrY=y+h/2, rr=rad;        // rr is rail radius
    
    float bthk = rthk+1;
    nvgBeginPath(vg);
     nvgMoveTo(vg, x-bthk/2, cntrY);
     nvgArc(vg, cntrX, cntrY, rr, PIf*1, PIf*1.5, NVG_CW);
     nvgStrokeWidth(vg, bthk);
    nvgStrokeColor(vg, nvgRGBAf(0, 0, 0, 1.f) );
	  nvgStroke(vg);

    nvgBeginPath(vg);
     nvgMoveTo(vg, x-rthk/2, cntrY);
     nvgArc(vg, cntrX, cntrY, rr, PIf*1, PIf*1.5, NVG_CW);
     nvgStrokeWidth(vg, rthk);
    nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
	  nvgStroke(vg);
  }
  SECTION(text)
  {
	  nvgFontSize(vg, 30.0f);
	  nvgFontFace(vg, "sans-bold");
	  tw = nvgTextBounds(vg, 0,0, text, NULL, NULL);
	  if(preicon != 0){
		  nvgFontSize(vg, h*1.3f);
		  nvgFontFace(vg, "icons");
		  iw = nvgTextBounds(vg, 0,0, cpToUTF8(preicon,icon), NULL, NULL);
		  iw += h*0.15f;
	  }

    //if(preicon != 0){
	  //	nvgFontSize(vg, h*1.3f);
	  //	nvgFontFace(vg, "icons");
	  //	nvgFillColor(vg, nvgRGBA(255,255,255,96));
	  //	nvgTextAlign(vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
	  //	nvgText(vg, x+w*0.5f-tw*0.5f-iw*0.75f, y+h*0.5f, cpToUTF8(preicon,icon), NULL);
	  //}

	  nvgFontSize(vg, 30.0f);
	  nvgFontFace(vg, "sans-bold");
	  nvgTextAlign(vg, NVG_ALIGN_MIDDLE);  // NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
	  nvgFillColor(vg, nvgRGBA(0,0,0,160));
	  nvgText(vg, x+w*0.5f-tw*0.5f+iw*0.25f,y+h*0.5f-1,text, NULL);
	  nvgFillColor(vg, nvgRGBA(255,255,255,255));
	  nvgText(vg, x+w*0.5f-tw*0.5f+iw*0.25f,y+h*0.5f,text, NULL);
  }

  return {x,y, x+w, y+h};
}

}

ENTRY_DECLARATION
{
  SECTION(init glfw)
  {
    //glfwSetErrorCallback(errorCallback);
    if( !glfwInit() ){
      fprintf(stdout, "[GFLW] failed to init!\n");
      exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 32);

    //GLFWwindow* win = glfwCreateWindow(vd->ui.w, vd->ui.h, "Demo", NULL, NULL);    assert(win!=nullptr);
    win = glfwCreateWindow(1024, 1024, "Demo", NULL, NULL);    //assert(win!=nullptr);
    glfwMakeContextCurrent(win);
    glfwSetWindowPos(win, 384, 1800);

    //glfwGetWindowSize(win, &vd->ui.w, &vd->ui.h);
    //glfwSetKeyCallback(win, keyCallback);
    //glfwSetScrollCallback(win, scrollCallback);
    //glfwSetCursorPosCallback(win, cursorPosCallback);
    //glfwSetMouseButtonCallback(win, mouseBtnCallback);

    #ifdef _WIN32
      //GLFWimage images[2];
      //images[0] = LoadIcon("lava.jpg");
      //images[1] = LoadIcon("lava.jpg");
      //glfwSetWindowIcon(win, 2, images);
    #endif

    glfwSwapInterval(0);
  }
  SECTION(init glew)
  {
    //glewExperimental = 1;
    if(glewInit() != GLEW_OK) {
      fprintf(stderr, "Failed to setup GLEW\n");
      exit(1);
    }
  }

	NVGcontext*    vg = NULL;
  SECTION(init nanovg and font)
  {
    vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	  if(vg == NULL) {
      printf("Could not init nanovg.\n");
		  return -1;
	  }
    int font = nvgCreateFont(vg, "sans-bold", "Roboto-Bold.ttf");
    if(font == -1) {
		  printf("Could not add font bold.\n");
		  return -1;
	  }
  }

  double prevt = 0, cpuTime = 0;
  glfwSetTime(0);
  SECTION(main loop)
  {
    while(!glfwWindowShouldClose(win))
    {
		  int ww, wh, fbWidth, fbHeight;
      double cx, cy, t, dt, prevt=0;
      float px, py, pxRatio;

      SECTION(time)
      {
        t     = glfwGetTime();
		    dt    = t - prevt;
		    prevt = t;
      }
      SECTION(input)
      {
  	    glfwGetCursorPos(win, &cx, &cy);
        //px=cx*ww; py=cy*wh;
        px=cx; py=cy;

        sprintf(winTitle, "%.4f  %.4f", px, py);
        glfwSetWindowTitle(win, winTitle);

		    glfwGetWindowSize(win, &ww, &wh);
		    glfwGetFramebufferSize(win, &fbWidth, &fbHeight);
		    // Calculate pixel ration for hi-dpi devices.
		    pxRatio = (float)fbWidth / (float)ww;
      }
      SECTION(gl frame setup)
      {
		    glViewport(0, 0, fbWidth, fbHeight);
		    if(premult)
			    glClearColor(0,0,0,0);
		    else
			    glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
		    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
      }

		  nvgBeginFrame(vg, ww, wh, pxRatio);
      SECTION(nanovg drawing)
      {
        nvgBeginPath(vg);
        nvgFillColor(vg, nvgRGBA(0xFF,0,0,0));
   	    nvgFill(vg);

        auto nclr = nvgRGBf(.1f,.4f,.5f);
        if( isIn(px,py,nbnd) ) nclr = nvgRGBf(.5f, .4f, .1f);
        nbnd = node(vg, 0, "BUTTON TIME", 384,384,256,64, nclr, linNorm(px, 0,ww) ); // nvgRGBf(.1f,.4f,.5f) );
      }
      nvgEndFrame(vg);


      glfwSwapBuffers(win);
      glfwPollEvents();
    }
  }

  glfwTerminate();
  return 0;
}



//
// 32.f); // max(0.f, 48.f-dt*16.f ) );

//SECTION(rounded rails)
//{
//  nvgBeginPath(vg);
	//  nvgRoundedRect(vg, x+border,y+border, w-(border*2),h-(border*2), rad); //-1);
	//  nvgRoundedRect(vg, x+rthk,y+rthk, w-(rthk*2),h-(rthk*2),         rad); //-1);
//   nvgPathWinding(vg, NVG_HOLE);
//  nvgFillColor(vg, nvgRGBAf(1.f, .8f, 0, .75f));
	// nvgFill(vg);
//}

//SECTION(rounded rails)
//{
//  float cntrX=x+border+rad, cntrY=y+border+h/2, rr=rad;        // rr is rail radius
//  
//  nvgBeginPath(vg);
//   nvgMoveTo(vg, cntrX, cntrY);
//   nvgArc(vg, cntrX, cntrY, rr, PIf*1, PIf*1.5, NVG_CW);
//   nvgClosePath(vg);
//
//   nvgMoveTo(vg, cntrX, cntrY);
//   nvgArc(vg, cntrX+2, cntrY+2, rr-rthk+2, PIf*1, PIf*1.5, NVG_CW);
//   nvgPathWinding(vg, NVG_HOLE);
//  nvgFillColor(vg, nvgRGBAf(1.f, .7f, 0, .5f));
	// nvgFill(vg);
//}

//nvgLineTo(vg, cntrX, y+border+rthk);
//nvgClosePath(vg);
//
//nvgMoveTo(vg, cntrX, cntrY);
//nvgArc(vg, cntrX+2, cntrY+2, rr-rthk+2, PIf*1, PIf*1.5, NVG_CW);
//nvgPathWinding(vg, NVG_HOLE);

//nvgLineTo(vg, cntrX, y+border+rthk);
//nvgClosePath(vg);
//
//nvgMoveTo(vg, cntrX, cntrY);
//nvgArc(vg, cntrX+2, cntrY+2, rr-rthk+2, PIf*1, PIf*1.5, NVG_CW);
//nvgPathWinding(vg, NVG_HOLE);

// nvgFillColor(vg, nvgRGBAf(1.f, .7f, 0, .5f));
//nvgFill(vg);

//nvgBeginPath(vg);
// nvgMoveTo(vg, 50,50);
// nvgLineTo(vg, 400,50);
// nvgArc(vg, 256,256, 128.f, 0,NVG_PI/4.f, NVG_CW);
// nvgCircle(vg, 300,300,32.f);
// nvgPathWinding(vg, NVG_HOLE);
//nvgClosePath(vg);
//nvgFillColor(vg, nvgRGBA(0,0,0,255));
//nvgFill(vg);

//nvgBezierTo(vg, 100,200, 250,200, 350,350);
//nvgLineTo(vg, 200,700);
//nvgArc(vg, 384,384, 128.f, -NVG_PI/4.f,NVG_PI/4.f, NVG_CW);
//
//nvgLineCap(vg, NVG_BUTT);
//nvgLineJoin(vg, NVG_BEVEL);

//nvgStrokeWidth(vg, 2.f);
//nvgStrokeColor(vg, nvgRGBA(0,0,0,255));
//nvgStroke(vg);
//
//struct NVGcolor bclr; bclr.r=.1f; bclr.g=.4f; bclr.b=.5f; bclr.a=1.f;

//
//double prevt = 0; // cpuTime = 0;

//win = initGLFW();                        // assert(vd.win!=nullptr);
//
//DemoData     data;
//GPUtimer    gpuTimer;
//PerfGraph fps, cpuGraph, gpuGraph;

//initGraph(&fps, GRAPH_RENDER_FPS, "Frame Time");
//initGraph(&cpuGraph, GRAPH_RENDER_MS, "CPU Time");
//initGraph(&gpuGraph, GRAPH_RENDER_MS, "GPU Time");

//GLFWwindow*  initGLFW()
//{
//  //glfwSetErrorCallback(errorCallback);
//  if( !glfwInit() ){
//    fprintf(stdout, "[GFLW] failed to init!\n");
//    exit(1);
//  }
//
//  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
//  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//  glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
//  glfwWindowHint(GLFW_SAMPLES, 32);
//
//  //GLFWwindow* win = glfwCreateWindow(vd->ui.w, vd->ui.h, "Demo", NULL, NULL);    assert(win!=nullptr);
//  GLFWwindow* win = glfwCreateWindow(1024, 1024, "Demo", NULL, NULL);    //assert(win!=nullptr);
//  glfwMakeContextCurrent(win);
//
//  //glfwGetWindowSize(win, &vd->ui.w, &vd->ui.h);
//  //glfwSetKeyCallback(win, keyCallback);
//  //glfwSetScrollCallback(win, scrollCallback);
//  //glfwSetCursorPosCallback(win, cursorPosCallback);
//  //glfwSetMouseButtonCallback(win, mouseBtnCallback);
//
//  #ifdef _WIN32
//    //GLFWimage images[2];
//    //images[0] = LoadIcon("lava.jpg");
//    //images[1] = LoadIcon("lava.jpg");
//    //glfwSetWindowIcon(win, 2, images);
//  #endif
//
//  glfwSwapInterval(0);
//
//  return win;
//}
//void         initGlew()
//{
//  //glewExperimental = 1;
//  if(glewInit() != GLEW_OK) {
//    fprintf(stderr, "Failed to setup GLEW\n");
//    exit(1);
//  }
//}

