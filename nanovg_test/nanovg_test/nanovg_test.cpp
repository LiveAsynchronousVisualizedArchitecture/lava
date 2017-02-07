
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
// -todo: take out windows min and max
// -todo: draw connections
// -todo: make click and drag draw a box
// -todo: separate node drag and selection from nanovg drawing
// -todo: lerp connection bezier instad of divide by 2
// -todo: make drag not pick up nodes
// -todo: make selection drag box highlight fully inside nodes
// -todo: make selection by drag box
// -todo: use std::move_back in MoveToBack()? 
// -todo: make click drag short circuit on the top (back) node - need to loop in reverse? - yes, just FROM instead of TO
// -todo: make node referenceable connections structure - have two connection vectors, one for in and one for out? - need to be able to get all in and/or out connections for a node - need two heaps?
// -todo: make connections structure use sorted vectors of indices and and lower_bound + upper_bound - can just use one simple  multi-map?
// -todo: make all selected nodes drag - need a starting position for the pntr - that is what drgP is?
// -todo: take out px, py - just use vector v2
// -todo: make vector from center to a square border
// -todo: make line from center of node obey node ratio when clamped to 

// todo: make direction vector from center clamp to rounded corners
// todo: fix bounds being swapped
// todo: make circle on outer border of node
// todo: make dragging selected nodes work after the first time 
// todo: draw inputs
// todo: make function to find the normal of a rounded rect
// todo: make a function to find the position of a rounded rect border
// todo: make bezier point at normal of node border
// todo: make connections between nodes
// todo: change drg to primary selection
// todo: group ui state variables together - drg, connecting
// todo: change node drawing function to take v2 and str
// todo: make selection a vector for multi-selection - if the vector capacity is 3x the size, use reserve to shrink it down to 1.5x the size?
// todo: make nodes and connections save to json file
// todo: make nodes load from json file
// todo: make one node snap to another node
// todo: make two snapped nodes group together and be dragged together
// todo: make snapped/grouped nodes separate with right mouse button
// todo: make global state 
// todo: make nodes different shapes? 
// todo: make connections have different shapes? draw three thin lines for a scatter connection?
// todo: make selected indication a border effect and not a color change

#define  WIN32_LEAN_AND_MEAN
#define  NOMINMAX
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

#include <cstdio>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include "no_rt_util.h"
#include "vec.hpp"

const int   TITLE_MAX_LEN = 256;
const v2    NODE_SZ       = { 256.f, 64.f };
const auto  NODE_CLR      = nvgRGBf(.1f,.4f,.5f);
const float INFf          = std::numeric_limits<float>::infinity();
//const float INFf          = std::numeric_limits<float>:infinity();

template<class T> using vec = std::vector<T>;
using str = std::string;

union bnd 
{
  struct { float xmn, ymn, xmx, ymx; };  // todo: make into a union?
  struct { v2 mn; v2 mx; };
  struct { v4 mnmx; };
  float c[4];

  bnd() : xmn(INFf), ymn(INFf), xmx(-INFf), ymx(-INFf) {}
  bnd(float Xmn, float Ymn, float Xmx, float Ymx) :
    xmn(Xmn), ymn(Ymn), xmx(Xmx), ymx(Ymx)
  {}

  float&       operator[](int i)       {return c[i];}
  float const& operator[](int i) const {return c[i];}
  bnd& operator()(float Xmn, float Ymn, float Xmx, float Ymx)
  {
    xmn=Xmn; ymn=Ymn; xmx=Xmx; ymx=Ymx; return *this;
  }
  operator v4&(){ return mnmx; }

  float w(){return abs(xmx-xmn);}
  float h(){return abs(ymx-ymn);}
};

struct   node { v2 P; str txt; };
struct   cnct { 
  int src; int dest;
  bool operator<(cnct const& r){ return src < r.src; }

  static bool lessDest(cnct const& l, cnct const& r){
    return l.dest < r.dest;
  }
};


using     veci     =     vec<int>;
using   vec_nd     =    vec<node>;
using vec_nbnd     =     vec<bnd>;
using   vec_v2     =      vec<v2>;
using  vec_con     =    vec<cnct>;
using cnct_tbl     =    std::unordered_multimap<int, cnct>;

GLFWwindow*            win;
char              winTitle[TITLE_MAX_LEN];
int                premult = 0;
bnd                   nbnd;
v2                prevPntr;
bool                  rtDn = false;    // right mouse button down
bool                 lftDn = false;    // left mouse button down
bool              prevRtDn = false;    // right mouse button down
bool             prevLftDn = false;    // left mouse button down
float              ndOfstX;
float              ndOfstY;
float                  ndx = 512.f;
float                  ndy = 512.f;
vec_nd               nodes;
vec_nbnd             nbnds;
vec<bool>             sels;             // bitfield for selected nodes
vec<bool>             drgs;
vec<v2>           drgOfsts;
int                    drg = -1;
bool                 drgNd = false;
v2                    drgP;
v2                 drgofst;
veci               nd_ordr;
bool                drgbox = false;
vec_con              cncts;             // cncts is connections - the vector of connection structures
cnct_tbl           cnctTbl;
cnct_tbl           cnct_in;
cnct_tbl          cnct_out;
//bnd                 drgbnd;   // this is calculated data, not fundamental data
//float                prevX;
//float                prevY;


namespace{

template<class T> 
void MoveToBack(T* v, ui64 i)
{
  using namespace std;
  
  auto&  a = *v;
  auto  sz = a.size();
  auto tmp = move(a[i]);

  //move_backward(a.front()+i+1ul, a.back(), a.back()-1);
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
bool                isIn(bnd const& a, bnd const& b)
{
  return b.xmn<a.xmn && b.xmx>a.xmx && b.ymn<a.ymn && b.ymx>a.ymx;
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
                       //const char* text, 
                       str const& text,
                           //v2 P, v2 sz, 
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
	  tw = nvgTextBounds(vg, 0,0, text.c_str(), NULL, NULL);
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
	  nvgText(vg, x+w*0.5f-tw*0.5f+iw*0.25f,y+h*0.5f-1,text.c_str(), NULL);
	  nvgFillColor(vg, nvgRGBA(255,255,255,255));
	  nvgText(vg, x+w*0.5f-tw*0.5f+iw*0.25f,y+h*0.5f,text.c_str(), NULL);
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

    cncts.push_back( {0,1} );
    cncts.push_back( {1,2} );

    //cnct_in.insert( cnct_in.end(), ALL(cncts) );
    
    TO(cncts.size(),i){
      cnctTbl.insert( { (int)i, cncts[i] } );
    //  lower_bound( ALL(cnct_in), cncts[i], cnct::lessDest);
    //  lower_bound( ALL(cnct_in), cncts[i], cnct::lessDest);
    }
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

  glfwSetTime(0);
  SECTION(main loop)
  {
    v2 pntr = {0,0};
    double cx, cy, t, dt, prevt=0, cpuTime=0;
    //float px=0, py=0;
    float pxRatio;
		int ww, wh, fbWidth, fbHeight;

    while(!glfwWindowShouldClose(win))
    {
      int sz = (int)nd_ordr.size();

      SECTION(time)
      {
        t     = glfwGetTime();
		    dt    = t - prevt;
		    prevt = t;
      }
      SECTION(input)
      {
  	    glfwGetCursorPos(win, &cx, &cy);
        //px=(float)cx; py=(float)cy;
        //prevX=px; prevY=py; 
        prevPntr = pntr;
        pntr=Vec2((float)cx, (float)cy);

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
			    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
      }

      bnd drgbnd;
      SECTION(selection box)
      {
        if(!lftDn){ drgP=pntr; drgbox=false; }

        drgbnd = bnd( min(drgP.x, pntr.x),
                      min(drgP.y, pntr.y),
                      max(drgP.x, pntr.x),
                      max(drgP.y, pntr.y) );
      }
      SECTION(select)
      {
        if(drgbox){
          TO(sz,i){
            sels[i] = isIn(nbnds[i],drgbnd);
            //sels[ndOrdr] = inNode;
          }
        }
      }
      SECTION(drag)
      {
        FROM(sz,i)                                                // loop backwards so that the top nodes are dealt with first
        {
          int  ndOrdr = nd_ordr[i];
          node&     n = nodes[ndOrdr];
          bool inNode = isIn(pntr.x,pntr.y, nbnds[i]);

          //if(inNode && !lftDn){
          if(!lftDn){
            drgOfsts[i] = {0,0};
            drg         = -1;
          }

          if( (drg<0) && inNode && lftDn && !prevLftDn){
            MoveToBack( &nd_ordr, i);
            drg     = (int)(sz-1);                               // make the dragged node the last node and the drag index the last index 
            //drgofst = pntr - nbnds[i].mn;
          } 
                     
          //if(drg==i){ n.P = pntr - drgofst; }
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
            int ndOrdr = nd_ordr[i];
            node&    n = nodes[ndOrdr];

            auto clr = NODE_CLR;
            if(sels[ndOrdr]){
              if(drg>-1) n.P +=  pntr - prevPntr;
              clr = nvgRGBf(.5f,.4f,.1f);
              drw_node(vg, 0, n.txt.c_str(), n.P.x,n.P.y, NODE_SZ.x,NODE_SZ.y, clr, 1.f);
            }else{
              nbnds[i] = drw_node(vg, 0, n.txt.c_str(), n.P.x,n.P.y, NODE_SZ.x,NODE_SZ.y, NODE_CLR, 1.f);              
            }

            SECTION(border test)
            {
              v2 ncntr = n.P + NODE_SZ/2.f;
              v2 hlfsz = NODE_SZ / 2.f;
              v2  pdir = norm(pntr - ncntr) * len(NODE_SZ);       // * normsz;
              
              v2 ds = sign(pdir);                                  // ds is direction sign
              if( abs(pdir.x) > NODE_SZ.x ){
                pdir /= abs(pdir.x)/hlfsz.x;
              }else{
                pdir /= abs(pdir.y)/hlfsz.y;
              }

              v2 circCntr = n.P + NODE_SZ - (NODE_SZ.y/2.f);
              v2 circst = ncntr - circCntr;

              v2 dirEnd = ncntr + pdir*1.f;
              nvgBeginPath(vg);
               nvgMoveTo(vg, ncntr.x,ncntr.y);
               nvgLineTo(vg, dirEnd.x, dirEnd.y);
              nvgStrokeWidth(vg, 3.f);
              nvgStroke(vg);

              nvgBeginPath(vg);
               nvgCircle(vg, circCntr.x,circCntr.y, NODE_SZ.y/2.f);
              nvgStrokeWidth(vg, 2.f);
              nvgStroke(vg);
            }

          }
        }
        SECTION(connections)
        {
          TO(cncts.size(),i)
          {
            auto& cn = cncts[i];
            v2   src = nodes[cn.src].P;
            v2  dest = nodes[cn.dest].P;
            //v2  ofst = (sels[i] && drg>-1)?  (pntr-drgP) : Vec2(0,0);

            nvgBeginPath(vg);
             //v2 d = dest+ofst, s = src+ofst;
             //nvgMoveTo(vg,   s.x,s.y);
             //f32 halfx = lerp(.5f, d.x, s.x);
             //f32 halfy = lerp(.5f, d.y, s.y); 
             //nvgBezierTo(vg, halfx,s.y, halfx,d.y, d.x,d.y);
             nvgMoveTo(vg,   src.x,src.y);
             f32 halfx = lerp(.5f, dest.x, src.x);
             f32 halfy = lerp(.5f, dest.y, src.y); 
             nvgBezierTo(vg, halfx,src.y, halfx,dest.y, dest.x,dest.y);
             nvgStrokeWidth(vg, 8.f);
            nvgStrokeColor(vg, nvgRGBAf(0, .25f, 1.f, 1.f));
   	        nvgStroke(vg);
          }
        }
        SECTION(selection box)
        {
          if(lftDn && drg<0)
          {
            drgbox = true;

            nvgBeginPath(vg);
              float x,y,w,h;
              x = min(drgP.x, pntr.x); 
              y = min(drgP.y, pntr.y); 
              w = abs(drgP.x - pntr.x);
              h = abs(drgP.y - pntr.y);
              nvgRect(vg, x,y, w,h);
            nvgStrokeWidth(vg, 2.f);
            nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, .75f));
   	        nvgStroke(vg);
          }
        }
      }
      nvgEndFrame(vg);

      prevRtDn  =  rtDn;
      prevLftDn = lftDn;

      glfwSwapBuffers(win);
      glfwPollEvents();
    }
  }

  glfwTerminate();
  return 0;
}







//nvgBeginPath(vg);
// nvgCircle(vg, ncntr.x,ncntr.y, 10.f);
//nvgFill(vg);
//
//v2 normsz = NODE_SZ / max(NODE_SZ.x, NODE_SZ.y);
//pdir      = clamp_hi(pdir, normsz);
//
//pdir = clamp_hi(abs(pdir), NODE_SZ) * dirSign;
//
//if( abs(pdir.x * normsz.x) > abs(pdir.y * normsz.y) ){
//  pdir = pdir / abs(pdir.x) * NODE_SZ.x;
//}else{
//  pdir = pdir / abs(pdir.y) * NODE_SZ.y;
//}
//
//if( abs(pdir.x * normsz.x) > abs(pdir.y * normsz.y) ){
//  pdir = pdir / abs(pdir.x) * NODE_SZ.x;
//}else{
//  pdir = pdir / abs(pdir.y) * NODE_SZ.y;
//}
//
//pdir *= NODE_SZ;

//v2 ncntr = (NODE_SZ/Vec2(2.f,2.f)) + n.P;
//f32 side = dot( pdir, norm(Vec2(1.f,1.f)) );
//if(pdir.x>0) pdir /= pdir.x;
//else         pdir /= abs(pdir.y);
//if(pdir.y>0) pdir /= pdir.y;
//else         pdir /= abs(pdir.x);                
//ncntr.x += NODE_SZ.x / 2.f;

//v2    nP = sels[ndOrdr]?(n.P+(pntr-drgP))  :  n.P;
//v2 nP = drg>-1?(n.P+(pntr-drgP))  :  n.P;
//if(drg>-1) n.P = n.P + (pntr-drgP);

//px=(float)cx; 
//py=(float)cy;
//px=cx*ww; py=cy*wh;

//#include "glm/vec2.hpp"
//using v2 = glm::vec2;

//nvgMoveTo(vg,  drgP.x,drgP.y);
//nvgLineTo(vg, pntr.x, pntr.y);
//nvgBezierTo(vg, dest.x/2,dest.y, dest.x/2,dest.y, dest.x,dest.y);

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

