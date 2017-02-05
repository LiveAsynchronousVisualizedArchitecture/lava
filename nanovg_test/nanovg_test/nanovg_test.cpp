
// -todo: make click and drag node position relative to click position
// -todo: bring in vecf from lighting in a bottle
// -todo: try bezier
// -todo: make vector of bnds, nodes, etc.
// -todo: make ability to select node out of multiple nodes
// -todo: make node struct 
// -todo: make click and drag work with multple nodes
// -todo: make dragged node swap with top node
// -todo: make dragged node insert at top and keep sorted order
// -todo: make connections data

// todo: separate node drag and selection from nanovg drawing
// todo: make node referenceable connections structure - have two connection vectors, one for 
// todo: draw connections
// todo: make click and drag draw a box
// todo: make selection a vector 
// todo: make selection by drag box
// todo: make one node snap to another node
// todo: make two snapped nodes group together and be dragged together
// todo: make snapped/grouped nodes separate with right mouse button
// todo: make global state 
// todo: make circle on outer border of node
// todo: make bezier point at normal of node border

#include "glew_2.0.0.h"
#include "glfw3.h"
#include "nanovg.h"

#define NANOVG_GL3_IMPLEMENTATION   // Use GL2 implementation.
#include "nanovg_gl.h"

//
//#include "../nanovg-master/example/demo.h"

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

//#include "glm/vec2.hpp"
//using v2 = glm::vec2;

#include <cstdio>
#include <vector>
#include <string>
#include "no_rt_util.h"
#include "vec.hpp"

const int  TITLE_MAX_LEN = 256;
const v2   NODE_SZ       = { 256.f, 64.f };
const auto NODE_CLR      = nvgRGBf(.1f,.4f,.5f);

template<class T> using vec = std::vector<T>;
using str = std::string;

union bnd {
  struct { float xmn, ymn, xmx, ymx; };  // todo: make into a union?
  struct { v2 mn; v2 mx; };
  struct { v4 mnmx; };
  float c[4];

  float&       operator[](int i)       {return c[i];}
  float const& operator[](int i) const {return c[i];}
  operator v4&(){ return mnmx; }

  float w(){return xmx-xmn;}
  float h(){return ymx-ymn;}
};

struct   node { v2 P; str txt; };
struct conect { int src; int dest; };

using     veci  =     vec<int>;
using   vec_nd  =    vec<node>;
using vec_nbnd  =     vec<bnd>;
using   vec_v2  =      vec<v2>;
using  vec_con  =  vec<conect>;

GLFWwindow*            win;
char              winTitle[TITLE_MAX_LEN];
int                premult = 0;
bnd                   nbnd;
float                prevX;
float                prevY;
bool                  rtDn = false;    // right mouse button down
bool                 lftDn = false;    // left mouse button down
//bool                   drg = true;
float              ndOfstX;
float              ndOfstY;
float                  ndx = 512.f;
float                  ndy = 512.f;
vec_nd               nodes;
vec_nbnd             nbnds;
vec<bool>             sels;           // bitfield for selected nodes
vec<bool>             drgs;
vec<v2>           drgOfsts;
int                    drg = -1;
v2                    drgP;
v2                 drgofst;
vec_con            conects;
veci               nd_ordr;


namespace{

template<class T> 
void MoveToBack(T* v, ui64 i)
{
  using namespace std;
  
  auto&  a = *v;
  auto  sz = a.size();
  auto tmp = move(a[i]);

  for(auto j=i; j<sz-1; ++j)
    a[j] = move( a[j+1] );
  
  a[sz-1] = tmp;
}

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

void    mouseBtnCallback(GLFWwindow* window, int button, int action, int mods)
{
  if(button==GLFW_MOUSE_BUTTON_LEFT){
    if(action==GLFW_PRESS) lftDn = true;
    else if(action==GLFW_RELEASE) lftDn = false;
  }

  if(button==GLFW_MOUSE_BUTTON_RIGHT){
    if(action==GLFW_PRESS) rtDn = true;
    else if(action==GLFW_RELEASE) rtDn = false;
  }
}

bnd             drw_node(NVGcontext* vg,      // drw_node is draw node
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
    
    float bthk = rthk+2;
    nvgBeginPath(vg);
     nvgMoveTo(vg, x-bthk/2, cntrY);
     nvgArc(vg, cntrX, cntrY, rr, PIf*1.f, PIf*1.5f, NVG_CW);
     nvgStrokeWidth(vg, bthk);
    nvgStrokeColor(vg, nvgRGBAf(0, 0, 0, .5f) );
	  nvgStroke(vg);

    nvgBeginPath(vg);
     nvgMoveTo(vg, x-rthk/2, cntrY);
     nvgArc(vg, cntrX, cntrY, rr, PIf*1.f, PIf*1.5f, NVG_CW);
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

} // end namespace

ENTRY_DECLARATION
{
  using namespace std;
  
  SECTION(test data init)
  {
    nodes.push_back( { {100.f,100.f},"one"   } );
    nodes.push_back( { {200.f,200.f},"two"   } );
    nodes.push_back( { {300.f,300.f},"three" } );

    for(auto& n : nodes){
      nbnds.push_back( {n.P.x, n.P.y, n.P.x+NODE_SZ.x, n.P.y+NODE_SZ.y} );
    }

    auto sz = nodes.size();
    sels.resize(sz, false);
    drgOfsts.resize(sz, {0,0} );
    drgs.resize(sz, false);

    nd_ordr.resize(sz);
    TO((int)sz,i) nd_ordr[i]=i;

    conects.push_back( {0,1} );
    conects.push_back( {1,2} );
  }

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

  double prevt=0, cpuTime=0;
  glfwSetTime(0);
  SECTION(main loop)
  {
    while(!glfwWindowShouldClose(win))
    {
		  int ww, wh, fbWidth, fbHeight;
      double cx, cy, t, dt, prevt=0;
      float px=0, py=0, pxRatio;
      v2 pntr;

      SECTION(time)
      {
        t     = glfwGetTime();
		    dt    = t - prevt;
		    prevt = t;
      }
      SECTION(input)
      {
  	    glfwGetCursorPos(win, &cx, &cy);
        prevX=px; px=(float)cx; prevY=py; py=(float)cy;
        pntr = Vec2(px, py);
        //px=cx*ww; py=cy*wh;

        //sprintf(winTitle, "%.4f  %.4f", px, py);
        //glfwSetWindowTitle(win, winTitle);

		    glfwGetWindowSize(win, &ww, &wh);
		    glfwGetFramebufferSize(win, &fbWidth, &fbHeight);
		    glfwSetMouseButtonCallback(win, mouseBtnCallback);

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

      SECTION(selection box)
      {
        if(!lftDn){ drgP=pntr; }
      }
      SECTION(select and drag)
      {
        int sz = (int)nd_ordr.size();
        TO(sz,i)
        {
          node& n = nodes[nd_ordr[i]];
          bool inNode = isIn(px,py,nbnds[i]);
          if(inNode && !lftDn){
            drgOfsts[i] = {0,0};
            drg = -1;
          }

          if( (drg<0) && inNode && lftDn ){
            MoveToBack( &nd_ordr, i);

            drg     = (int)(sz-1);                               // make the dragged node the last node and the drag index the last index 
            drgofst = pntr - nbnds[i].mn;
          }
            
          if(drg==i){ n.P = pntr - drgofst; }
        }
      }
      

		  nvgBeginFrame(vg, ww, wh, pxRatio);
      SECTION(nanovg drawing)
      {
        SECTION(nodes)
        {
          int sz = (int)nd_ordr.size();
          TO(sz,i)
          {
            node& n = nodes[nd_ordr[i]];

            auto clr = sels[i]?nvgRGBf(.5f,.4f,.1f) : NODE_CLR;
            nbnds[i] = drw_node(vg, 0, n.txt.c_str(), n.P.x,n.P.y, NODE_SZ.x,NODE_SZ.y, clr, 1.f);
            //sels[i]  = isIn(px,py,nbnds[i]);
          }
        }
        SECTION(connections)
        {
          TO(conects.size(),i)
          {
            auto& cn = conects[i];
            v2   src = nodes[cn.src].P;
            v2  dest = nodes[cn.dest].P;

            nvgBeginPath(vg);
             nvgMoveTo(vg,   src.x,src.y);
             nvgBezierTo(vg, dest.x/2,dest.y, dest.x/2,dest.y, dest.x,dest.y);
            nvgStrokeColor(vg, nvgRGBAf(0,1.f,0,1.f));
   	        nvgStroke(vg);
          }
        }
        SECTION(selection box)
        {
          if(lftDn){
            nvgBeginPath(vg);
              float x,y,w,h;
              x = min(drgP.x, pntr.x); 
              y = min(drgP.y, pntr.y); 
              w = abs(drgP.x - pntr.x);
              h = abs(drgP.y - pntr.y);
              nvgRect(vg, x,y, w,h);
              //nvgMoveTo(vg,  drgP.x,drgP.y);
              //nvgLineTo(vg, pntr.x, pntr.y);
              //nvgBezierTo(vg, dest.x/2,dest.y, dest.x/2,dest.y, dest.x,dest.y);
            nvgStrokeWidth(vg, 2.f);
            nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, .5f));
   	        nvgStroke(vg);
          }
        }
      }
      nvgEndFrame(vg);

      glfwSwapBuffers(win);
      glfwPollEvents();
    }
  }

  glfwTerminate();
  return 0;
}







//sprintf(winTitle, "%.2f  %.2f", pntr.x, pntr.y);  //nodes[0].P.x, nodes[0].P.y );
//glfwSetWindowTitle(win, winTitle);

//
//TO(nodes.size(),i)

//auto tmp = nd_ordr[i];
//for(int j=i; j<sz-1; ++j) nd_ordr[j] = nd_ordr[j+1];
//nd_ordr[sz-1] = tmp;

//
//nbnd = drw_node(vg, 0, "BUTTON TIME", ndx,ndy, NODE_SZ.x,NODE_SZ.y, nclr, linNorm(px, 0,(float)ww) );

//SECTION(select and drag)
//{
//  bool inNode = isIn(px,py,nbnds[i]);
//  if(inNode){
//    if(!lftDn){ drgOfsts[i]={0,0}; }
//    else if(!drgs[i]){ drgOfsts[i] = pntr - nbnds[i].mn; }
//  }
//  drgs[i] = (drgs[i] || inNode) && lftDn;
//}

//auto   nclr = nvgRGBf(.1f,.4f,.5f);
//bool inNode = isIn(px,py,nbnd);
//if(inNode){
//  nclr = nvgRGBf(.5f, .4f, .1f);
//  if(!lftDn){ ndOfstX = ndOfstY = 0; }
//  else if(!drg){ 
//    ndOfstX = px - nbnd.xmn;
//    ndOfstY = py - nbnd.ymn;
//  }
//}
//drg = (drg || inNode) && lftDn;
//
//if(drg){
//  ndx = px-ndOfstX;
//  ndy = py-ndOfstY;
//}

//if(!lftDn && drg<0) drgP = pntr;
//v2   ndP = n.P;
// drgOfsts[i];

//else if( !(drg==i) ){ drgP }
//else if( !(drg==i) ){ drgOfsts[i] = pntr - nbnds[i].mn; }
//if( drg>-1 && drg)
//
//auto tmp = nodes.back();
//auto  sz = nodes.size();
//nodes[sz-1] = nodes[i];
//for(int j=i; j<sz-1; ++j) nodes[j] = nodes[j+1];
//nodes[sz-2];
//
//swap( nodes.back(), nodes[i] );

//drgOfsts[i].x = px - nbnds[i].xmn;
//drgOfsts[i].y = py - nbnds[i].ymn;

//void cursorPosCallback(GLFWwindow* window, double xposition, double yposition)
//{
//  const static float _2PI = 2.f* PIf;
//
//  glfwGetWindowUserPointer(window);
//  vec2 newMousePosition = vec2((float)xposition, (float)yposition);
//
//  if(vd->camera.leftButtonDown){
//    vd->camera.mouseDelta = (newMousePosition - vd->camera.oldMousePos);
//  }else{ vd->camera.mouseDelta = vec2(0,0); }
//    
//  if(vd->camera.rightButtonDown){
//    vd->camera.btn2Delta = (newMousePosition - vd->camera.oldMousePos);
//  }else{ vd->camera.btn2Delta  = vec2(0,0); }
//
//  vd->camera.oldMousePos = newMousePosition;
//}

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

