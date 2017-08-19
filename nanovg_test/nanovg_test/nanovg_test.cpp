
// -todo: fix selection of slots
// -todo: make a deselect all function that clears globals and calls fd.grph.clearSels()
// -todo: fix deselection of slots
// -todo: organized graphdb functions according to which part they operate on
// -todo: change graphdb private variables to use m_
// -todo: try renaming types with upper case first letter
// -todo: fix slots not staying selected - slots clicked need to turn off clearSelection - clearSelection turned off on left click up when inside a slot
// -todo: solve assert(win==NULL) on window resize - was using void* attached to glfw instead of static state
// -todo: fix in/dest connections when not connected
// -todo: make selection of one dest and one src slot trigger connection create 
// -todo: make a GraphDB function that toggles a selection on or off
// -todo: make drawing of one src to multiple connections draw first to the average of all the slots, then draw to all the dest slots - works and works well
// -todo: use previous drawing technique for connection to one in/dest slot
// -todo: make bnd also work for message passing nodes
// -todo: debug toggle killing all connection drawing - just needed to only delete the outCncts that had the same dest
// -todo: make graphToStr and saveFile use graphdb
// -todo: make strToGraph and loadFile use graphdb

// todo: make slots part of file writing 
// todo: make slots part of file reading
// todo: make node type part of file writing
// todo: make node type part of file reading
// todo: make 'delete' and 'backspace' delete selected nodes
// todo: make 'delete' and 'backspace' delete selected connections
// todo: put node type into written file
// todo: draw message node slots as sliding angles
// todo: make one node snap to another node
// todo: use scroll wheel and nanovg scale transforms to zoom in and out
// todo: group ui state variables together - priSel, connecting
// todo: make two snapped nodes group together and be dragged together
// todo: separate finding node the pointer is inside from the action to take
// todo: print to console with ReadFile.cpp function
// todo: make two nodes execute in order
// todo: make a node to read text from a file name 
// todo: make a node to split text into lines and scatter the result
// todo: add data to node for inputs
// todo: add data to connection for input and output indices
// todo: make function to draw a bezier from one slot to another with normals

// idea: make connection delete and create trigger when 1 or more in/dest slots are selected and 1 out/src slot is connected
// idea: make a single file include all nanoui into one compilation unit
// idea: make msg loop that would deal with selections, clicks and drags? 
// idea: take out redudant node position + bnd information 
// idea: look into quantized gradients of nanovg
// idea: don't select a slot if it is under an existing node
// idea: switch over to using json.hpp
// idea: make load file take the state in
// idea: make an io file
// idea: make a recently opened menu
// idea: drag json file into window to open file
// idea: draw arrows (triangles) to show direction with connections
// idea: make connection a set
// idea: slow down cursor over nodes
// idea: speed up cursor while dragging
// idea: highlight connections and slots when a node is selected
// idea: half highlight when mouse over of node, slot or connection
// idea: make snapped/grouped nodes separate with right mouse button
// idea: make mouse slow down over slots more than it slows down over nodes
// idea: make click and drag for connections
// idea: make selected indication a border effect and not a color change
// idea: make selection a vector for multi-selection - if the vector capacity is 3x the size, use reserve to shrink it down to 1.5x the size?
// idea: make connections have different shapes? draw three thin lines for a scatter connection?
// idea: make nodes different shapes? - make data input into vertical columns?
// idea: look into openGL input latency technique
// idea: convert general data structures of nodes, slots, and connections to use tbl?
// idea: make drawing order of slots dictated by node order
// idea: make connection class that keeps two connection arrays, each sorted by src or dest for fast searching

// glew might include windows.h
#define  WIN32_LEAN_AND_MEAN
#define  NOMINMAX
#include "glew_2.0.0.h"
#include "glfw3.h"
#include "nanovg.h"

#define NANOVG_GL3_IMPLEMENTATION   // Use GL2 implementation.
//#include "nanovg_gl.h"  // nanogui includes this already

#define ENTRY_DECLARATION int main(void)
#ifdef _MSC_VER
  // not needed due to having glfw and glew compiled into single files
  //#pragma comment(lib, "glfw3dll.lib")
  //#pragma comment(lib, "glew32.lib")

  #pragma comment(lib, "opengl32.lib")

  #if defined(_DEBUG)
    #define USE_CONSOLE                                 // turning this off will use the windows subsystem in the linker and change the entry point to WinMain() so that no command line/console will appear
  #endif

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
#include <unordered_set>
#include <algorithm>
#include "nfd.h"
#include "jzon.h"
#include "vec.hpp"
#include "../no_rt_util.h"
#include "../Transform.h"
#include "../LavaNode.h"
#include "FissureDecl.h"

char              winTitle[TITLE_MAX_LEN];
int                premult = 0;
Bnd                   nbnd;
v2                prevPntr;
bool                  rtDn = false;    // right mouse button down
bool                 lftDn = false;    // left mouse button down
bool              prevRtDn = false;    // right mouse button down
bool             prevLftDn = false;    // left mouse button down
float              ndOfstX;
float              ndOfstY;
float                  ndx = 512.f;
float                  ndy = 512.f;

i32              slotInSel = -1;
i32             slotOutSel = -1;
int                 priSel = -1;
int                 secSel = -1;
f32                 io_rad;

bool                 drgNd = false;
v2                    drgP;
v2                 drgofst;
bool                drgbox = false;

static FisData fd;

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
bool                isIn(float x, float y, Bnd const& b)
{
  return x>b.xmn && x<b.xmx && y>b.ymn && y<b.ymx;
}
bool                isIn(Bnd const& a, Bnd const& b)
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
bool              hasInf(v2 v)
{
  TO(2,i) if(v[i]==INFf || v[i]==-INFf) return true;
  return false;
}
bool              hasNaN(v2 v)
{
  TO(2,i) if(v[i]==SIG_NANf || v[i]==-SIG_NANf) return true;
  return false;
}
v2      lineCircleIntsct(v2 P, v2 dir, v2 crcl, f32 r)  // only works for circles to the sides
{
  if(dir.x==0) return {INFf, INFf};

  v2       st = (P - crcl) / r;
  f32     mlt = abs(st.x) / dir.x;                 // mlt = multiplier - the multiplier to get dir.x to equal st.x 
  f32       C = (st + dir*mlt).y;
  if(C > r) return v2(INFf, INFf);
  f32       m = dir.y / dir.x;
  f32       a = SQR(m) + 1;
  f32       b = 2.f * m * C;
  f32       c = SQR(C) - 1.f;
  f32      q2 = SQR(b) - 4.f*a*c;
  if(q2 < 0) return v2(INFf, INFf);
  f32       x = ((-b + sqrt(q2)) / 2.f*a);
  f32       y = sin(acos(x));
  v2  intrsct = v2( sign(dir.x)*x, sign(dir.y)*y)*r + crcl;

  return intrsct;

  //f32       r = NODE_SZ.y/2.f;
  //v2  dirCirc = abs(pdir) / r;
  //v2 circCntr = n.P + NODE_SZ - r;
}
bool              insUnq(cnct_tbl* cnct, int a, int b)   // insUnq is insert unique - this inserts into a multi-set only if the combination of int key and int val does not already exist
{
  using namespace std;
  
  auto rnge = cnct->equal_range(a);
  if(rnge.first != rnge.second){
    for(auto i=rnge.first; i!=rnge.second; ++i){
      if( i->second == b ) return false;
    }
  }
  cnct->insert( {a, b} );

  return true;

  //auto bkt = find(rnge.first, rnge.second, );
  //if(bkt!=rnge.second){
  //  cnct->insert( {a, b} );
  //  return true;
  //} 
}
f32        normalToAngle(v2 N)
{
  return atan2(N.y, N.x);
}
v2        angleToNormal(f32 angle)
{
  return { cos(angle), sin(angle) };
}

// serialize to and from json
str           graphToStr(GraphDB const& g)
{
  using namespace std;
  
  Jzon::Node nds = Jzon::object();
  SECTION(nodes)
  {
    //auto sz = nodes.size();
    auto sz = g.nsz();

    Jzon::Node nd_txt = Jzon::array();
    TO(sz,i) nd_txt.add(g.node(i).txt);
    //TO(sz,i) nd_txt.add(nodes[i].txt);

    Jzon::Node   nd_x = Jzon::array();
    TO(sz,i) nd_x.add(g.node(i).P.x);

    Jzon::Node   nd_y = Jzon::array();
    TO(sz,i) nd_y.add(g.node(i).P.y);

    //Jzon::Node ordr = Jzon::array();
    //TO(sz,i) ordr.add(nd_ordr[i]);

    nds.add("x",      nd_x);
    nds.add("y",      nd_y);
    nds.add("txt",  nd_txt);
    //nds.add("order",  ordr);
  }
  Jzon::Node jcncts = Jzon::object();
  SECTION(connections)
  {
    //auto sz = cncts.size();
    auto sz = g.cnctsz();

    Jzon::Node src  = Jzon::array();
    Jzon::Node dest = Jzon::array();

    for(auto kv : g.cncts()){
      src.add(kv.first);
      dest.add(kv.second);
    }
    //TO(sz,i) src.add(cncts[i].src);
    //TO(sz,i) dest.add(cncts[i].dest);

    jcncts.add("src",   src);
    jcncts.add("dest", dest);
  }
  
  Jzon::Node graph = Jzon::object();
  graph.add("nodes", nds);
  graph.add("connections", jcncts);

  //Jzon::Format format;
  //format.newline    = true;
  //format.indentSize = 1;
  //format.spacing    = true;
  //format.useTabs    = false;

  Jzon::Writer w;
  //w.setFormat(format);  // breaks reading for some reason
  str s;
  w.writeString(graph, s);

  return s;
}
GraphDB       strToGraph(str const& s)
{
  using namespace std;

  GraphDB g;

  Jzon::Parser prs;
  auto graph = prs.parseString(s);

  auto nd_x   = graph.get("nodes").get("x");
  auto nd_y   = graph.get("nodes").get("y");
  auto nd_txt = graph.get("nodes").get("txt");
  auto ordr   = graph.get("nodes").get("order");
  auto src    = graph.get("connections").get("src");
  auto dest   = graph.get("connections").get("dest");
  
  auto cnt = nd_x.getCount();
  TO(cnt,i){
    Node n;
    n.P.x = nd_x.get(i).toFloat();
    n.P.y = nd_y.get(i).toFloat();
    n.txt = nd_txt.get(i).toString();
    g.addNode(n);
  }
  
  //g.nodes().resize(cnt);
  //nodes.resize(cnt);  
  //TO(cnt,i) nodes[i].P.x = nd_x.get(i).toFloat();
  //TO(cnt,i) nodes[i].P.y = nd_y.get(i).toFloat();
  //TO(cnt,i) nodes[i].txt = nd_txt.get(i).toString();

  auto cnct_cnt = src.getCount();
  TO(cnct_cnt,i){
    g.addCnct(src.get(i).toInt(), dest.get(i).toInt());
  }

  //cncts.resize(cnct_cnt);
  //TO(cnct_cnt,i) cncts[i].src  = src.get(i).toInt();
  //TO(cnct_cnt,i) cncts[i].dest = dest.get(i).toInt();
  //
  //for(auto c : cncts){
  //  cnct_src.insert( {c.src, c.dest} );
  //  cnct_dest.insert( {c.dest, c.src} );
  //}

  return move(g);

  //cnct_src.clear();
  //cnct_dest.clear();
  //
  //sels.resize(cnt);
  //nbnds.resize(cnt);
  //
  //nd_ordr.resize(cnt);
  //TO(cnt,i) nd_ordr[i] = ordr.get(i).toInt();
  //
  //TO(cnt,i) nd_ordr[i] = (int)i;
}
bool            saveFile(GraphDB const& g, str path)
{
  str fileStr = graphToStr(g);
  
  FILE* f = fopen(path.c_str(), "w");
  if(!f) return false;

  size_t writeSz = fwrite(fileStr.c_str(), 1, fileStr.size(), f);
  if(writeSz != fileStr.size()) return false;

  int closeRet = fclose(f);
  if(closeRet == EOF) return false;

  return true;
}
bool            loadFile(str path, GraphDB* out_g)
{
  FILE* f = fopen(path.c_str(), "r");
  if(!f) return false;
  fseek(f, 0, SEEK_END);
  str s;
  s.resize( ftell(f) );
  fseek(f, 0, SEEK_SET);

  size_t redRet = fread( (void*)s.data(), 1, s.size(), f);
  if(redRet != s.size()) return false; 

  if(fclose(f) == EOF) return false;

  *out_g = strToGraph(s);

  return true;
}

// state manipulation
void            node_add(str txt, Node::Type type=Node::FLOW)
{
  //v2 P = v2(512,512);
  Node n(txt, type);
  n.P = v2(512,512);
  //nodes.push_back(n);

  //nodes.push_back( {P,txt} );
  //nodes.back().type = type;

  //sels.push_back(false);
  //nbnds.emplace_back();

  //nd_ordr.push_back( (int)(nodes.size()-1) );
}
void    clear_selections(GraphDB* inout_grph)
{
  inout_grph->clearSels();
  slotInSel   =  -1;
  slotOutSel  =  -1;
  priSel      =  -1;
  secSel      =  -1;
}

void         keyCallback(GLFWwindow* win, int key, int scancode, int action, int modbits)
{
  if(action==GLFW_RELEASE) return;

  char sngl[256]; // = {'\0', '\0'};
  memset(sngl, 0, 256);
  sngl[0] = key;
  glfwSetWindowTitle(win, sngl);
  
  switch(key){
  case 'J':
  {
    //_s = graphToStr();
    //glfwSetWindowTitle(win, _s.c_str() );
    //FILE* f = fopen("nanovg_test.lava", "w");
    //fwrite(_s.c_str(), 1, _s.size(), f);
    //fclose(f);
  }break;
  case 'K':
  {
    //FILE* f = fopen("nanovg_test.lava", "r");
    //fseek(f, 0, SEEK_END);
    //_s.resize( ftell(f) );
    //fseek(f, 0, SEEK_SET);
    //fread( (void*)_s.data(), 1, _s.size(), f);
    //fclose(f);
    //strToGraph(_s);
  }break;
  case 'L':
  {
    //#ifdef _WIN32
    //  HMODULE lib = LoadLibrary(TEXT("TfmTestLib.dll"));
    //  if(lib){
    //    auto   getNds = (GetLavaNodes_t)GetProcAddress(lib, TEXT("GetNodes") );
    //    LavaNode* nds = getNds();
    //    sprintf(sngl, "%s    %s    %s", nds[0].name, nds[0].in_types[0], nds[0].out_types[0] );
    //    while(nds && nds->name)
    //      node_add( (nds++)->name );
    //  }else{ sprintf(sngl, "zero", lib); }
    //
    //  glfwSetWindowTitle(win, sngl);
    //#endif


    #ifdef _WIN32
      HMODULE lib = LoadLibrary(TEXT("ReadFile.dll"));
      if(lib){
        auto   getNds = (GetLavaNodes_t)GetProcAddress(lib, TEXT("GetNodes") );
        LavaNode* nds = getNds();
        printf("%s    %s    %s", nds[0].name, nds[0].in_types[0], nds[0].out_types[0] );
        while(nds && nds->name)
          node_add( (nds++)->name );
      }else{ printf("zero"); }
      //}else{ printf("zero", lib); }


    #endif
  }break;
  case 'Y':
  {
    //sprintf(sngl, "sizeof LavaData %d", sizeof(LavaData) );
    //glfwSetWindowTitle(win, sngl);    
    printf("sizeof LavaData %lld \n", sizeof(LavaData) );
  }break;
  default:
    ;
  }

  fd.ui.screen.keyCallbackEvent(key, scancode, action, modbits);
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

  fd.ui.screen.mouseButtonCallbackEvent(button, action, mods);
}
void            errorCallback(int e, const char *d) {
  printf("Error %d: %s\n", e, d);
  fflush(stdout);
}
void           scrollCallback(GLFWwindow* window, double xofst, double yofst)
{
  using namespace std;
  fd.ui.screen.scrollCallbackEvent(xofst, yofst);
}
void        cursorPosCallback(GLFWwindow* window, double x, double y)
{
  //using namespace glm;
  const static float _2PI = 2.f* PIf; //  pi<float>();

  //FisData* fd = (FisData*)glfwGetWindowUserPointer(window);

  fd.ui.screen.cursorPosCallbackEvent(x,y);
}
void             charCallback(GLFWwindow* window, unsigned int codepoint)
{
  FisData* fd = (FisData*)glfwGetWindowUserPointer(window);

  fd->ui.screen.charCallbackEvent(codepoint);
}
void             dropCallback(GLFWwindow* window, int count, const char** filenames)
{
  FisData* fd = (FisData*)glfwGetWindowUserPointer(window);

  fd->ui.screen.dropCallbackEvent(count, filenames);
}
void  framebufferSizeCallback(GLFWwindow* window, int w, int h)
{
  //FisData* fd = (FisData*)glfwGetWindowUserPointer(window);
  //fd->ui.screen.resizeCallbackEvent(w, h);

  fd.ui.screen.resizeCallbackEvent(w, h);
}

v2               in_cntr(Node const& n, f32 r)
{
  return v2(n.P.x + NODE_SZ.x/2, n.P.y-r);
}
v2              out_cntr(Node const& n, f32 r)
{
  return v2(n.P.x + NODE_SZ.x/2, n.P.y + NODE_SZ.y + r);
}
Bnd            node_draw(NVGcontext* vg,      // drw_node is draw node
                            int preicon,
                          Node const& n,
                           NVGcolor col,
                         float      rnd)     // rnd is corner rounding
{
  const float   rthk = 8.f;    // rw is rail thickness

	NVGpaint bg;
	char icon[8];
  float tw=0, iw=0, x=n.P.x, y=n.P.y, w=NODE_SZ.x, h=NODE_SZ.y;
	float rad = lerp(rnd, 0.f, h/2.f);           // rad is corner radius
  float cntrX=x+w/2, cntrY=y+h/2, rr=rad;      // rr is rail radius
  //float io_rad=10.f;
  Bnd b;

  nvgResetTransform(vg);
  nvgGlobalAlpha(vg, 1.f);
  switch(n.type)
  {
  case Node::FLOW: {
    SECTION(grey border)
    {
	    nvgBeginPath(vg);
       nvgRoundedRect(vg, x,y,w,h, rad);
      nvgFillColor(vg, nvgRGBA(0,0,0,128));
	    nvgFill(vg);
    }
    SECTION(shaded color inside)
    {
      int    grad = (int)lerp(rnd, 0, 48);
      auto topClr = nvgRGBA(255,255,255,isBlack(col)?16:grad);
      auto botClr = nvgRGBA(0,0,0,isBlack(col)?16:grad);
	    bg = nvgLinearGradient(vg, x,y,x,y+h, topClr, botClr);
	    nvgBeginPath(vg);
	      nvgRoundedRect(vg, x+BORDER,y+BORDER, w-(BORDER*2),h-(BORDER*2), rad-BORDER);
        col.a = 0.8f;
	      if(!isBlack(col)){
		      nvgFillColor(vg, col);
		      nvgFill(vg);
	      }
	    nvgFillPaint(vg, bg);
	    nvgFill(vg);
    }
    b = {x,y, x+w, y+h};
  } break;
  case Node::MSG: {
    f32 msgRad = NODE_SZ.x / 2;

    nvgStrokeColor(vg, nvgRGBAf(.04f, .04f, .04f, 1.f));
    nvgStrokeWidth(vg, BORDER);

    nvgBeginPath(vg);
    nvgCircle(vg, cntrX, cntrY, msgRad);
    auto radial = nvgRadialGradient(vg,
      cntrX, cntrY, msgRad*.5f, msgRad,
      nvgRGBAf( .15f, .15f,  .15f,   .95f ),
      nvgRGBAf( .2f, .2f,    .2f,   1.f)  );
    nvgFillPaint(vg, radial);
    nvgFill(vg);

    nvgBeginPath(vg);
      nvgCircle(vg, x+w/2,y+h/2, msgRad);
      auto lin = nvgLinearGradient(vg, 
                                   cntrX, cntrY-msgRad, x, y+msgRad,
                                   nvgRGBAf( .3f,   .3f,   .3f,  0.5f), 
                                   nvgRGBAf( .15f,  .15f,  .15f,  .45f) );
      nvgFillPaint(vg, lin);
    nvgFill(vg);

    nvgBeginPath(vg);
      nvgCircle(vg, cntrX, cntrY, msgRad);
    nvgStroke(vg);

    b = {cntrX-msgRad/1.2f, cntrY-msgRad/1.2f, cntrX+msgRad/1.2f, cntrY+msgRad/1.2f};

    //nvgBeginPath(vg);
    //  nvgRect(vg, b.xmn, b.ymn, b.w(), b.h());
    //nvgStroke(vg);
  } break;
  }

  SECTION(text)
  {
	  nvgFontSize(vg, 30.0f);
	  nvgFontFace(vg, "sans-bold");
	  tw = nvgTextBounds(vg, 0,0, n.txt.c_str(), NULL, NULL);
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
	  nvgText(vg, x+w*0.5f-tw*0.5f+iw*0.25f,y+h*0.5f-1, n.txt.c_str(), NULL);
	  nvgFillColor(vg, nvgRGBA(255,255,255,255));
	  nvgText(vg, x+w*0.5f-tw*0.5f+iw*0.25f,y+h*0.5f,n.txt.c_str(), NULL);
  }

  SECTION(debug circle sides)
  {
    //nvgStrokeColor(vg, nvgRGBAf(1,1,1,1) );
    //nvgStrokeWidth(vg, 1.f);
    //nvgBeginPath(vg);
    // nvgMoveTo(vg, n.P.x+rad,n.P.y);
    // nvgLineTo(vg, n.P.x+rad,n.P.y + NODE_SZ.y);
    //nvgStroke(vg);
    //
    //nvgBeginPath(vg);
    //  nvgMoveTo(vg, n.P.x+NODE_SZ.x-rad, n.P.y);
    //  nvgLineTo(vg, n.P.x+NODE_SZ.x-rad, n.P.y + NODE_SZ.y);
    //nvgStroke(vg);
    //
    //nvgBeginPath(vg);
    //  nvgCircle(vg, n.P.x+rad, n.P.y+rad, rad);
    //nvgStroke(vg);
    //
    //nvgBeginPath(vg);
    //  nvgCircle(vg, n.P.x+NODE_SZ.x-rad, n.P.y+NODE_SZ.y-rad, rad);
    //nvgStroke(vg);
  }

  return b;
}

v2           node_border(Node const& n, v2 dir, v2* out_nrml=nullptr)
{
  v2       nP = n.P;
  v2 borderPt = {0,0};
  v2     ndir = norm(dir);
  v2    ncntr = nP + NODE_HALF_SZ;

  switch(n.type)
  {
  case Node::FLOW: {
    v2   hlf = NODE_HALF_SZ;
    f32  rad = hlf.y;

    v2 pdir = ndir;
    v2   ds = sign(pdir);                                  // ds is direction sign
    f32  ax = abs(pdir.x);
    if( ax > hlf.x ){
      pdir /= ax/hlf.x;                                    // can this never be 0, since ax is positive, hlf.x is positive, and ax is greater than hlf.x ? 
    }else{
      f32 ay = abs(pdir.y);
      pdir /= ay==0.f?  1.f  :  ay/hlf.y;
    }

    v2  circCntr = (pdir.x<0)? nP+v2(rad,rad)  :  nP+NODE_SZ-v2(rad,rad);
    v2   intrsct = lineCircleIntsct(ncntr, pdir, circCntr, rad);
    bool     hit = !hasInf(intrsct)  &&  (intrsct.x < nP.x+rad || intrsct.x > nP.x+NODE_SZ.x-rad); 
    if(hit){ pdir = intrsct - ncntr; }

    borderPt = ncntr + pdir;

    if(out_nrml){
      if(hit) *out_nrml = norm(borderPt - circCntr);
      //if(hit) *out_nrml = norm(circCntr - borderPt);
      else    *out_nrml = { 0.f, sign(ndir).y };
    }
  } break;
  case Node::MSG: 
  default: 
  {
    f32  rad = NODE_SZ.x/2;
    borderPt = ncntr + ndir*rad; 
    if(out_nrml){ *out_nrml = ndir; }
  } break;
  }

  return borderPt;

  //  && !hasNaN(intrsct);
  //
  //f32        r = hlf.y;
  //
  //if(hit && borderPt.x > (nP.x+rad) && borderPt.x < (nP.x-rad) ){
  //  if(ds.y > .5f) borderPt.y = nP.y + NODE_SZ.y;
  //  else           borderPt.y = nP.y;
  //}
}
void            drw_rail(NVGcontext* vg, v2 P, v2 nP)                     // drw_rail is draw_rail
{
  using namespace std;
  
  const f32    hPi = PIf/2;      // hPi is half pi 
  const int border = 2;
  const f32   rthk = 8.f;        // rthk is rail thickness

  f32 rnd = 1.f;
  f32 tw=0, iw=0, x=nP.x, y=nP.y, w=NODE_SZ.x, h=NODE_SZ.y;
  v2   hlf = NODE_HALF_SZ;
  f32 vMid = nP.y +  NODE_SZ.y/2;
  f32  rad = hlf.y; 
  f32   rr = NODE_SZ.y/2;          // rr is rail radius
  f32 rlen = NODE_SZ.x/2;          // rlen is rail length
  
  v2 ndCntr = {nP.x + hlf.x, nP.y + hlf.y};

  bool leftSide = P.x < ndCntr.x;
  v2    lftCirc = { nP.x+rad, ndCntr.y };
  v2    rgtCirc = { nP.x+NODE_SZ.x-rad, ndCntr.y };

  bool inLeftCircle = P.x < (nP.x + rad);
  bool  inRgtCircle = P.x > (nP.x+NODE_SZ.x - rad);
  bool        onTop = P.y < ndCntr.y;
  enum Place { BOTTOM=0, LEFT, TOP, RIGHT };
  Place place = BOTTOM;
  if(inLeftCircle)     place = LEFT;
  else if(inRgtCircle) place = RIGHT;
  else if(onTop)       place = TOP;

  v2 dir;
  if(inLeftCircle)      dir = norm(P-lftCirc);
  else if(inRgtCircle)  dir = norm(P-rgtCirc);
  else dir = norm(P-ndCntr);

  f32 railRad, arcSt, lftDist, lftArc, lftOpp, lftArcSt=hPi, rgtDist, rgtArc, rgtOpp, rgtArcSt=hPi; 
  railRad = rlen/2;


  if(inLeftCircle)     arcSt = PIf - asin(dir.y);
  else if(inRgtCircle) arcSt = asin(dir.y);

  if(inLeftCircle) lftArcSt = PIf - asin(dir.y);
  lftDist = (inLeftCircle || inRgtCircle)? 0 : max(0.f, min(railRad, P.x-(nP.x+rad)) );
  lftArc  = min(hPi*3 - lftArcSt, (railRad-lftDist) / rad );          // maximum arc amount is half a circle, which is PI radians
  if(inRgtCircle){
    lftArc  = max(0.f, min(hPi - lftArcSt, (railRad-lftDist)/rad) );
  }
  lftOpp  = railRad - lftArc*rad - lftDist;                           // lftOpp is left opposite side segment length

  if(inRgtCircle) rgtArcSt = asin(dir.y);
  rgtDist = inRgtCircle? 0 : max(0.f, min(railRad, (nP.x+NODE_SZ.x-rad)-P.x) );
  rgtArc  = min(rgtArcSt + hPi, (railRad-rgtDist)/rad );              // maximum arc amount is half a circle, which is PI radians
  rgtOpp  = railRad - rgtArc*rad - rgtDist;                           // rgtOpp is right opposite side segment length


  //if(inLeftCircle)     arcSt = PIf - asin(dir.y);
  //else if(inRgtCircle) arcSt = asin(dir.y);

  Bnd nb(nP.x, nP.y, nP.x+NODE_SZ.x, nP.y+NODE_SZ.y);
  
  // relative distances
  f32 lDst = max(0.f, min(railRad, (P.x-(nb.xmn+rad))) );
  f32 rDst = max(0.f, min(railRad, ((nb.xmx-rad)-P.x)) );
  f32 lArc = (railRad-lDst) / rad;
  f32 rArc = (railRad-rDst) / rad;
  f32 lOpp = railRad - lDst - lArc*rad;
  f32 rOpp = railRad - rDst - rArc*rad;

  if(inLeftCircle){
    f32 ang = P.y>vMid?  acos(P.x)  :  PIf+acos(P.x);
    lArc = min(hPi*3.f - ang, railRad);
    rArc = min(ang - hPi, railRad);
  }

  // absolute positions
  //v2  lArcPt =  node_border(nP, dir, io_rad);

  v2  lPt    = { P.x - lDst, P.y };
  v2  rPt    = { P.x + rDst, P.y };
  f32 lArcSt = hPi + lArc;
  f32 lArcEn = hPi; 
  f32 rArcSt = hPi;
  f32 rArcEn = hPi - rArc;
  v2  lOppPt = { nb.xmn + rad + lOpp , nb.ymn };
  v2  rOppPt = { nb.xmx - rad - rOpp , nb.ymn };
  v2  lArcPt = { acos(lArcSt), asin(lArcSt) };
  v2  rArcPt = { acos(rArcSt), asin(rArcSt) };

  if(inLeftCircle){
    
  }

  //SECTION(drawing fourth attempt)
  //{
  //  // maybe the rounded rectangle should be broken down into one number that represents a position on the border
  //  // then taking the number backwards and forwards would at least give the start and end points 
  //
  //
  //  v2 orthDir = { dir.y, -dir.x };
  //  v2 lBrdr = node_border(nP, orthDir, io_rad);
  //
  //  nvgStrokeWidth(vg, rthk);
  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
  //
  //  nvgBeginPath(vg);
  //    nvgMoveTo(vg, P.x, P.y);
  //    nvgLineTo(vg, lBrdr.x, lBrdr.y);
  //    //nvgRoundedRect(vg, nP.x, nP.y, NODE_SZ.x, NODE_SZ.y, NODE_SZ.y/2 );
  //    //nvgRect(vg, nP.x, nP.y, NODE_SZ.x, NODE_SZ.y); 
  //  nvgStroke(vg);
  //}

  //SECTION(drawing third attempt)
  //{
  //  nvgStrokeWidth(vg, rthk);
  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
  //
  //  nvgBeginPath(vg);
  //    if(lOpp>0){
  //      nvgMoveTo(vg, lOppPt.x, lOppPt.y);
  //    }else if(lArc>0){
  //      nvgMoveTo(vg, lArcPt.x, lArcPt.y);
  //      nvgArc(vg, lftCirc.x, lftCirc.y, rad, lArcSt, lArcEn, NVG_CCW);
  //    }else{
  //      nvgMoveTo(vg, lPt.x, lPt.y);
  //    }
  //    if(rDst>0) nvgLineTo(vg, rPt.x, rPt.y);
  //
  //    if(rArc>0){
  //      nvgArc(vg, rgtCirc.x, rgtCirc.y, rad, rArcSt, rArcEn, NVG_CCW);
  //    }
  //    if(rOpp>0) nvgLineTo(vg, rOppPt.x, rOppPt.y);
  //
  //    //if(lArc>0) nvgArc(vg, lftCirc.x, lftCirc.y, rad, lArcSt, lArcEn, NVG_CCW);
  //  nvgStroke(vg);  
  //}

  //SECTION(drawing second attempt)
  //{
  //  nvgStrokeWidth(vg, rthk);
  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
  //
  //  nvgBeginPath(vg);  // left side
  //    nvgMoveTo(vg, P.x, P.y);
  //    //switch(place){      
  //    if(onTop && !inLeftCircle)
  //    { 
  //      if(lftDist>0){ nvgLineTo(vg, P.x - lftDist, P.y); }
  //      if(lftArc>0)   nvgArc(vg, lftCirc.x, lftCirc.y, rad, hPi*3, hPi*3 - lftArc, NVG_CCW);
  //      if(lftOpp>0)   nvgLineTo(vg, nP.x+rad+lftOpp, nP.y+NODE_SZ.y);
  //    }
  //    else{
  //      if(lftDist>0){ nvgLineTo(vg, P.x - lftDist, P.y); }
  //      if(inLeftCircle){
  //        if(lftArc>0) nvgArc(vg, lftCirc.x, lftCirc.y, rad, arcSt, arcSt + lftArc, NVG_CW);
  //        if(lftOpp>0) nvgLineTo(vg, nP.x+rad+lftOpp, nP.y);
  //      }else if(inRgtCircle){
  //        if(lftArc>0) nvgArc(vg, rgtCirc.x, rgtCirc.y, rad, arcSt, arcSt + lftArc, NVG_CW);
  //        if(lftOpp>0) nvgLineTo(vg, nP.x+rad+lftOpp, nP.y);
  //      }else{
  //        if(lftArc>0) nvgArc(vg, lftCirc.x, lftCirc.y, rad, hPi, hPi + lftArc, NVG_CW);
  //        if(lftOpp>0) nvgLineTo(vg, nP.x+rad+lftOpp, nP.y);
  //      }
  //    }
  //    //}
  //  nvgStroke(vg);
  //
  //  nvgBeginPath(vg);  // left side
  //  nvgMoveTo(vg, P.x, P.y);
  //  //switch(place){      
  //  if(onTop && !inRgtCircle)
  //  { 
  //    if(rgtDist>0){ nvgLineTo(vg, P.x + rgtDist, P.y); }
  //    if(rgtArc>0)   nvgArc(vg, rgtCirc.x, rgtCirc.y, rad, -hPi, -hPi + rgtArc, NVG_CW);
  //    if(rgtOpp>0)   nvgLineTo(vg, nP.x+rad+rgtOpp, nP.y+NODE_SZ.y);
  //  }
  //  else{
  //    if(rgtDist>0){ nvgLineTo(vg, P.x + rgtDist, P.y); }
  //    if(inRgtCircle){
  //      if(rgtArc>0) nvgArc(vg, rgtCirc.x, rgtCirc.y, rad, arcSt, arcSt - rgtArc, NVG_CCW);
  //    }else{
  //      if(rgtArc>0) nvgArc(vg, rgtCirc.x, rgtCirc.y, rad, hPi, hPi - rgtArc, NVG_CCW);
  //    }
  //    if(rgtOpp>0) nvgLineTo(vg, nP.x+NODE_SZ.x-rad-rgtOpp, nP.y);
  //  }
  //  //}
  //  nvgStroke(vg);
  //}

  //f32 rad = lerp(rnd, 0.f, h/2.f);                               // rad is corner radius
  //f32 cntrX = x+border+rad, cntrY = y+border+h/2, 
  //f32 rr=rad;        // rr is rail radius

  //v2  hlf = NODE_HALF_SZ;
  //v2 ndir = norm(nP - P);
  //v2 pdir = ndir;
  //v2   ds = sign(pdir);                                  // ds is direction sign
  //f32  ax = abs(pdir.x);
  //if( ax > hlf.x ){
  //  pdir /= ax/hlf.x;                                    // can this never be 0, since ax is positive, hlf.x is positive, and ax is greater than hlf.x ? 
  //}else{
  //  f32 ay = abs(pdir.y);
  //  pdir /= ay==0.f?  1.f  :  ay/hlf.y;
  //}
  //f32       r = hlf.y;
  //v2 circCntr = (pdir.x<0)? nP+v2(r,r)  :  nP+NODE_SZ-r;

  //v2 lftCirc;
  //lftCirc.y = nP.y + hlf.y;
  //lftCirc.x = leftSide? nP.x+rad  :  nP.x+NODE_SZ.x-rad;
  //lftCirc.y = nP.y + hlf.y;

  //f32      leftLine = P.x - rlen/2;
  //f32 leftLineLimit = max(nP.x + rad, leftLine);
  //f32     leftExtra = abs(leftLineLimit - leftLine);
  ////f32       leftArc = inLeftCircle? (rlen/2)/rad  :  min(PIf*1.5f, leftExtra/rad);
  //f32       leftArc = min(PIf*1.5f, leftExtra/rad);
  //f32         arcSt = inLeftCircle? hPi + hPi-asin(dir.y) :  hPi;
  //f32         arcEn = min(arcSt+leftArc, PIf*1.5f);
  //f32       arcDist = (arcEn - arcSt) * rad;
  //f32      leftWrap = max(0.f, leftExtra -  arcDist);

  //st      = PIf - asin(dir.y);
  //f32   lftSeg = P.x - lftDist;
  //
  //f32 leftLineLen = P.x - rlen/2 - leftLimit;  // nP.x + 2*rad rlen/2 - 
  //
  // black border
  //f32 bthk = rthk+2;                        // bthk is black thickness 
  //nvgBeginPath(vg);
  //  nvgMoveTo(vg, x-bthk/2, P.y);
  //  nvgArc(vg, P.x, P.y, rr, PIf*1.f, PIf*1.5f, NVG_CW);
  //  nvgStrokeWidth(vg, bthk);
  //nvgStrokeColor(vg, nvgRGBAf(0, 0, 0, 1.f) );
  //nvgStroke(vg);
  //
  // inner color
  //nvgBeginPath(vg);
  //  nvgMoveTo(vg, x-rthk/2, P.y);
  //  nvgArc(vg, P.x, P.y, rr, PIf*1.f, PIf*1.5f, NVG_CW);
  //  nvgStrokeWidth(vg, rthk);
  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
  //nvgStroke(vg);
  //
  //v2 brdr = node_border(nP, P - ndCntr, io_rad);      // brdr is border
  //
  //nvgBeginPath(vg);
  //  nvgCircle(vg, P.x, P.y, 5.f);
  //  nvgStrokeWidth(vg, rthk);
  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
  //nvgStroke(vg);
  //
  //nvgBeginPath(vg);
  //  nvgCircle(vg, circCntr.x, circCntr.y, 5.f);
  //  nvgStrokeWidth(vg, rthk);
  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
  //nvgStroke(vg);
  //
  //f32 angle = inLeftCircle? asin(dir.y) :  PIf/2;
  //f32 arcSt = PIf/2.0f;
  //f32 arcEn = PIf*1.5f;
  //NVGwinding windDirection = leftSide? NVG_CW  :  NVG_CCW;
  //if(onTop){
  //  swap(arcSt, arcEn);
  //  windDirection = NVG_CCW;
  //}

  //SECTION(drawing first attempt)
  //{
  //  nvgBeginPath(vg);
  //    //if(inLeftCircle){
  //    //  nvgMoveTo(vg, P.x, P.y);
  //    //  if(leftArc>0){
  //    //    nvgArc(vg, circCntr.x, circCntr.y, rad, arcSt, arcEn, windDirection);    // PIf*1.f, PIf*1.5f, NVG_CW);
  //    //    if(leftWrap>0)
  //    //      nvgLineTo(vg, nP.x+rad+leftWrap, nP.y);
  //    //  }
  //    //}else{
  //    //if(leftSide){
  //
  //    //if(onTop && !inLeftCircle){
  //    //  nvgMoveTo(vg, P.x, P.y);
  //    //  nvgLineTo(vg, leftLineLimit, P.y);
  //    //  nvgArc(vg, circCntr.x, circCntr.y, rad, arcEn, arcSt, NVG_CCW);      // PIf*1.f, PIf*1.5f, NVG_CW);
  //    //  if(leftWrap>0)
  //    //    nvgLineTo(vg, nP.x+rad+leftWrap, nP.y+NODE_SZ.y);
  //    //}else{
  //      nvgMoveTo(vg, P.x, P.y);
  //      if(!inLeftCircle){ nvgLineTo(vg, leftLineLimit, P.y); }
  //      if(leftArc>0){
  //        if(onTop) nvgArc(vg, circCntr.x, circCntr.y, rad, arcEn, arcSt, NVG_CCW);
  //        else      nvgArc(vg, circCntr.x, circCntr.y, rad, arcSt, arcEn, windDirection);
  //        if(leftWrap>0)
  //          nvgLineTo(vg, nP.x+rad+leftWrap, nP.y);
  //      }
  //    //}
  //    //}
  //    //}else if(leftSide && onTop){
  //    //  
  //    //}
  //
  //    //nvgMoveTo(vg, leftLineLimit, P.y);
  //    //nvgLineTo(vg, P.x, P.y);
  //    //nvgArc(vg, circCntr.x, circCntr.y, rr, arcSt, arcEn, windDirection);    // PIf*1.f, PIf*1.5f, NVG_CW);
  //    nvgStrokeWidth(vg, rthk);
  //    nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
  //  nvgStroke(vg);
  //}
  //
  //f32 arcEn = leftSide? PIf*1.5f  :  -PIf*0.5f;
  //if(leftSide==false){ swap(arcSt, arcEn); }

  //nvgBeginPath(vg);
  //  nvgCircle(vg, nP.x, nP.y, 5.f);
  //  nvgStrokeWidth(vg, rthk);
  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
  //nvgStroke(vg);
  //
  //nvgBeginPath(vg);
  //  nvgCircle(vg, P.x, P.y, 5.f);
  //  nvgStrokeWidth(vg, rthk);
  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
  //nvgStroke(vg);
}
void           slot_draw(NVGcontext* vg, Slot const& s, Slot::State drawState) //bool highlight=false, bool selected=false)
{
  nvgStrokeColor(vg, nvgRGBAf(0,0,0,1.f));
  nvgStrokeWidth(vg, 3.f);

  v2     out = s.P;
  v2       N = s.N;

  NVGcolor fillClr;
  if(s.in){
    //switch(s.state){
    switch(drawState){
    case Slot::SELECTED:    fillClr = nvgRGBAf(1.f,   1.f,   .5f,  1.f); break;
    case Slot::HIGHLIGHTED: fillClr = nvgRGBAf( .36f,  .9f, 1.f,   1.f); break;
    case Slot::NORMAL:
    default:                fillClr = nvgRGBAf( .18f,  .6f,  .75f, 1.f); break;
    }
  }else{
    //switch(s.state){
    switch(drawState){
    case Slot::SELECTED:    fillClr = nvgRGBAf(1.f,   1.f,   .5f,   1.f); break;
    case Slot::HIGHLIGHTED: fillClr = nvgRGBAf( .36f, 1.f,   .36f,  1.f); break;
    case Slot::NORMAL:      
    default:                fillClr = nvgRGBAf( .18f,  .75f, .18f,  1.f); break;
    }
  }
  nvgFillColor(vg, fillClr);

  nvgBeginPath(vg);
    nvgCircle(vg, out.x, out.y, io_rad);
  nvgFill(vg);
  nvgStroke(vg);

  // slot triangle drawing
  f32  triRad = io_rad - 2.f;
  auto inrClr = fillClr;
  inrClr.r += 0.2f;
  inrClr.g += 0.2f;
  inrClr.b += 0.2f;
  nvgFillColor(vg, inrClr);

  nvgBeginPath(vg);
    nvgResetTransform(vg);
    nvgTranslate(vg, out.x, out.y);             // translate comes before rotate here because nanovg 'premultiplies' transformations instead of multiplying them in for some reason. this reverses the order of transformations and can be seen in the source for nanovg
    nvgRotate(vg, normalToAngle(N) + (s.in? PIf/2.f : -PIf/2) );

    nvgMoveTo(vg, -0.707f*triRad, -0.707f*triRad);
    nvgLineTo(vg,  0.707f*triRad, -0.707f*triRad);
    nvgLineTo(vg,  0, triRad);

    nvgClosePath(vg);
  nvgFill(vg);
  nvgResetTransform(vg);
}

void        debug_coords(v2 a)
{
  sprintf(winTitle, "%.2f  %.2f", a.x, a.y);
  glfwSetWindowTitle(fd.win, winTitle);
}

} // end namespace

ENTRY_DECLARATION
{
  using namespace std;
  
	NVGcontext* vg = NULL;
  SECTION(initialization)
  {
    SECTION(test data init)
    {
      io_rad = IORAD;

      // nodes
      fd.grph.addNode( Node("one",   Node::FLOW, {400.f,300.f}) );
      fd.grph.addNode( Node("two",   Node::FLOW, {200.f,500.f}) );
      fd.grph.addNode( Node("three", Node::FLOW, {700.f,500.f}) );
      fd.grph.addNode( Node("four",  Node::FLOW, {700.f,700.f}) );

      // slots
      fd.grph.addSlot( Slot(0, false) );
      fd.grph.addSlot( Slot(1,  true) );
      fd.grph.addSlot( Slot(2,  true) );
      fd.grph.addSlot( Slot(3,  true) );

      fd.grph.addCnct(0, 1);
      fd.grph.addCnct(0, 2);
      fd.grph.addCnct(0, 3);

      auto sz = fd.grph.nsz();
    }
    SECTION(FisData)
    { 
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
      glfwWindowHint(GLFW_SAMPLES, 16);

      fd.win = glfwCreateWindow(1024, 768, "Demo", NULL, NULL);        // assert(win!=nullptr);
      glfwMakeContextCurrent(fd.win);

      glfwSetKeyCallback(fd.win,                keyCallback);
      glfwSetMouseButtonCallback(fd.win,   mouseBtnCallback);
      glfwSetCharCallback(fd.win,              charCallback);          // in glfw charCallback is for typing letters and is different than the keyCallback for keys like backspace 
      glfwSetKeyCallback(fd.win,                keyCallback);
      glfwSetScrollCallback(fd.win,          scrollCallback);
      glfwSetCursorPosCallback(fd.win,    cursorPosCallback);
      glfwSetMouseButtonCallback(fd.win,   mouseBtnCallback);
      glfwSetDropCallback(fd.win,              dropCallback);
      glfwSetFramebufferSizeCallback(fd.win, framebufferSizeCallback);

      glfwSetWindowUserPointer(fd.win, &fd.ui.screen);

      #ifdef _WIN32
        //GLFWimage images[2];
        //images[0] = LoadIcon("lava.jpg");
        //images[1] = LoadIcon("lava.jpg");
        //glfwSetWindowIcon(win, 2, images);
      #endif

      glfwSwapInterval(0); // turning this to 1 clamps the programs to the vertical sync rate and slows down the interactivity
    }
    SECTION(init glew)
    {
      //glewExperimental = 1;
      if(glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
      }
    }
    SECTION(nanogui)
    {
      fd.ui.screen.initialize(fd.win, false);

      fd.ui.keyLay   = new BoxLayout(Orientation::Vertical, Alignment::Fill, 0,10);
      fd.ui.keyWin   = new Window(&fd.ui.screen,  "");
      auto spcr1     = new Label(fd.ui.keyWin,    "");
      auto spcr2     = new Label(fd.ui.keyWin,    "");
      auto spcr3     = new Label(fd.ui.keyWin,    "");
      auto loadBtn   = new Button(fd.ui.keyWin,    "Load");
      auto saveBtn   = new Button(fd.ui.keyWin,    "Save");
      auto flowBtn   = new Button(fd.ui.keyWin,    "Flow Node");
      auto msgBtn    = new Button(fd.ui.keyWin,    "Message Node");

      loadBtn->setCallback([](){ 
        nfdchar_t *inPath = NULL;
        nfdresult_t result = NFD_OpenDialog("lava", NULL, &inPath );

        //printf("\n\nfile dialog: %d %s \n\n", result, inPath);

        if(inPath){
          bool ok = loadFile(inPath, &fd.grph);
          if(ok) printf("\nFile loaded from %s\n", inPath);
          else   printf("\nLoad did not read successfully from %s\n", inPath);
        }
      });
      saveBtn->setCallback([](){
        nfdchar_t *outPath = NULL;
        nfdresult_t result = NFD_SaveDialog("lava", NULL, &outPath );

        //printf("\n\nfile dialog: %d %s \n\n", result, outPath);

        if(outPath){
          bool ok = saveFile(fd.grph, outPath);
          if(ok) printf("\nFile Written to %s\n", outPath);
          else   printf("\nSave did not write successfully to %s\n", outPath);
        }
      });

      GraphDB* grphPtr = &fd.grph;
      flowBtn->setCallback([grphPtr](){
        //node_add("new node");
        grphPtr->addNode( Node("new node", Node::FLOW) );
      });
      msgBtn->setCallback([grphPtr](){
        //node_add("message node", node::MSG);
        grphPtr->addNode( Node("message node", Node::MSG) );
      });

      fd.ui.keyWin->setLayout(fd.ui.keyLay);

      Theme* thm = fd.ui.keyWin->theme();
      thm->mButtonFontSize      = 20;
      thm->mTransparent         = e4f( .0f,  .0f,  .0f,    .0f );
      thm->mWindowFillUnfocused = e4f( .2f,  .2f,  .225f,  .3f );
      thm->mWindowFillFocused   = e4f( .3f,  .28f, .275f,  .3f );

      fd.ui.screen.setVisible(true);
      fd.ui.screen.performLayout();
    }
    SECTION(init nanovg and font)
    {
      vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	    if(vg == NULL) {
        printf("Could not init nanovg.\n");
		    return -1;
	    }
      int font = nvgCreateFontMem(vg, "sans-bold", Roboto_Bold_ttf, Roboto_Bold_ttf_len, 0);
      //int font = nvgCreateFont(vg, "sans-bold", "Roboto-Bold.ttf");
      if(font == -1) {
		    printf("Could not add font bold.\n");
		    return -1;
	    }
    }
  }

  glfwSetTime(0);
  SECTION(main loop)
  {
    GraphDB& grph = fd.grph;
    v2       pntr = {0,0};
    double cx, cy, t, dt, avgFps=60, prevt=0, cpuTime=0;
    float pxRatio;
    int fbWidth, fbHeight;

    while(!glfwWindowShouldClose(fd.win))
    {
      bool lftClk = (lftDn && !prevLftDn);
      bool  rtClk = (rtDn  && !prevRtDn);
      bool    clk = lftClk || rtClk;
      auto     sz =  grph.nsz();

      SECTION(time)
      {
        t     = glfwGetTime();
		    dt    = t - prevt;
		    prevt = t;
      }
      SECTION(input)
      {
        glfwPollEvents();                                             // PollEvents must be done after zeroing out the deltas
  	    glfwGetCursorPos(fd.win, &cx, &cy);

        //px=(float)cx; py=(float)cy;
        //prevX=px; prevY=py; 

        prevPntr = pntr;
        pntr=Vec2((float)cx, (float)cy);

        //sprintf(winTitle, "%.4f  %.4f", px, py);
        //glfwSetWindowTitle(win, winTitle);

		    glfwGetWindowSize(fd.win, &fd.ui.w, &fd.ui.h);
		    glfwGetFramebufferSize(fd.win, &fbWidth, &fbHeight);

        pxRatio = (float)fbWidth / (float)fd.ui.w;          // Calculate pixel ration for hi-dpi devices.
      }
      SECTION(gl frame setup)
      {
		    glViewport(0, 0, fbWidth, fbHeight);
		    if(premult)
			    glClearColor(0,0,0,0);
		    else
			    glClearColor(.075f, .075f, .075f, 1.0f);
		    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
      }
      SECTION(selection)
      {
        bool lftClkDn =  lftDn && !prevLftDn;    // lftClkDn is left click down
        bool lftClkUp = !lftDn &&  prevLftDn;    // lftClkDn is left click up
        bool clearSelections = true;
        Bnd  drgbnd;
        SECTION(selection box)
        {
          if(!lftDn){ drgP=pntr; drgbox=false; }

          drgbnd = Bnd( min(drgP.x, pntr.x),
                        min(drgP.y, pntr.y),
                        max(drgP.x, pntr.x),
                        max(drgP.y, pntr.y) );
        }
        SECTION(select)
        {
          if(drgbox){
            TO(sz,i){
              bool selected = isIn(grph.bnd(i), drgbnd);
              grph.sel(i, selected);               
            }
            clearSelections = false;
          }
        
          if(!lftDn){
            priSel = -1;
          }
        }
        SECTION(slot selection and connection creation)
        {
          i32  inClk = -1;
          i32 outClk = -1;
          TO(grph.ssz(), i)
          {
            if(lftClkDn){  //lftDn && !prevLftDn)
              Slot&    s = grph.slot(i);
              bool inSlt = len(pntr - s.P) < io_rad;
              if(inSlt){
                outClk  = (i32)i;
                s.state = Slot::SELECTED;
                if(s.in)
                  slotInSel  = i;
                else
                  slotOutSel = i;
                clearSelections = false;
              }
            }else if(lftClkUp){
              Slot&    s = grph.slot(i);
              bool inSlt = len(pntr - s.P) < io_rad;
              if(inSlt) clearSelections = false;
            }
          }

          if(slotInSel>-1 && slotOutSel>-1){
            grph.toggleCnct(slotOutSel, slotInSel);
            slotOutSel = slotInSel = -1;
            clear_selections(&grph);
            clearSelections = false;
          }

          //if(outClk > -1){
          //  if(outClk==slotOutSel) 
          //    slotOutSel = -1;                                           // deselects if clicking elsewhere or if inside the selected slot - this creates a toggle
          //  else if(slotInSel > -1){
          //    grph.addCnct(slotInSel, outClk);
          //    slotOutSel = slotInSel = -1;
          //    clearSelections = false;
          //  }else{
          //    slotOutSel = outClk;
          //    //clearSelections = false;
          //  }
          //  //clearSelections = false;
          //}

          //if(inClk > -1){
          //  if(inClk==slotInSel)
          //    slotInSel = -1;                                            // deselects if clicking elsewhere or if inside the selected slot - this creates a toggle
          //  else if(slotOutSel > -1){
          //    grph.addCnct(inClk, slotInSel);
          //    slotOutSel = slotInSel = -1;
          //    clearSelections = false;
          //  }else{
          //    slotInSel  = inClk;
          //    //clearSelections = false;
          //  }
          //  //clearSelections = false;
          //}
        }
        SECTION(node selection)
        {
          bool inAny = false;
          FROM(sz,i)                                                // loop backwards so that the top nodes are dealt with first
          {
            int  ndOrdr = grph.order(i);
            Node&     n = grph.node(i);
            bool inNode = isIn(pntr.x,pntr.y, grph.bnd(ndOrdr) );
            inAny      |= inNode;

            SECTION(primary selection and group selection effects)
            {
              if(inNode && clk && (priSel<0||priSel!=ndOrdr) )
              {
                grph.moveToBack(i);
                priSel     = ndOrdr;
                drgP       = pntr;

                if(!grph.sel(ndOrdr)){
                  TO(grph.selsz(),i) grph.sel(i,false);
                  grph.sel(ndOrdr,true);
                  clearSelections = true;
                }
                break;                                                  // without breaking from the loop, a node could be moved down and hit again
              }
            } 
          }

          if(!inAny){
            if(lftDn && priSel<0){ drgbox=true; clearSelections=false; }
            if(rtDn && !prevRtDn){ secSel = -1; }
          }else{
            clearSelections=false;
          }
        }

        if(clearSelections && !lftDn && prevLftDn){ 
          clear_selections(&grph);
        }
      }
      SECTION(movement)
      {
        SECTION(node movement)
        {
          TO(grph.nsz(),i)
          {
            int  ndOrdr = grph.order(i);
            Node&     n = grph.node(ndOrdr);
            bool selctd = ndOrdr==priSel || grph.sel(ndOrdr);

            if( priSel>-1 && selctd ){           // if a node is primary selected (left mouse down on a node) or the selected flag is set
              n.P +=  pntr - prevPntr;
            }
          }
        }
        SECTION(slot movement)
        {
          TO(grph.ssz(),i)
          {
            Slot& s      = grph.slot(i);
            auto    nidx = s.nidx;
            if(nidx < grph.nsz()){
              v2        nrml;
              Node const& n = grph.node(nidx);
              v2 nP = n.P + NODE_SZ/2;
              
              if(s.in){                                              // dest / in / blue slots
                Slot* src = grph.srcSlot(i);
                if(src){
                  auto srcNdP = grph.node(src->nidx).P + NODE_SZ/2;
                  s.P = node_border(n, srcNdP - nP, &nrml);
                  s.N = nrml;
                }else{
                  s.P = node_border(n, {0,-1.f}, &nrml);
                  s.N = {0,-1.f};
                }
              }else{
                auto ci = grph.destSlots(i);
                if(ci == grph.cnctEnd()){
                  s.P = node_border(n, v2(0,s.in? -1.f : 1.f), &nrml);
                  s.N = nrml;
                }else{
                  v2  destP={0,0}, destN={0,0};
                  int   cnt = 0;
                  for(; ci != grph.cnctEnd() && ci->first==i; ++cnt, ++ci){
                    if(!grph.hasSlot(ci->second)){ cnt -= 1; continue; }   // todo: does this need to subtract 1 from count?

                    v2 curP = grph.slot(ci->second).P;
                    destP  += curP; 
                    destN  += norm(curP - nP);
                  }
                  destP /= (f32)cnt;
                  destN /= (f32)cnt;
                  s.N = norm(destN);
                  s.P = node_border(n, s.N);
                }
              }
            }
          }
        }
      }
      SECTION(drawing)
      {
        SECTION(nanovg drawing)
        {
          nvgBeginFrame(vg, fd.ui.w, fd.ui.h, pxRatio);
          SECTION(draw connections)
          {                                 // ci is connection iterator 
            auto en = grph.cnctEnd();
            for(auto ci = grph.cnctBegin(); ci != en; )
            {
              auto     srcIdx = ci->first;
              Slot const& src = grph.slot(srcIdx);
              auto      count = grph.cncts().count(ci->first);
              
              if(count==1){
                Slot const& dest = grph.slot(ci->second);
                f32  halfx = lerp(.5f, src.P.x, dest.P.x);
                f32  halfy = lerp(.5f, src.P.y, dest.P.y);
                f32   dist = len(src.P - dest.P);
                v2  outNxt = src.P  + src.N  * min(NODE_SZ.x/2, (dist/3));         // divide by 3 because there are 3 sections to the bezier
                v2   inNxt = dest.P + dest.N * min(NODE_SZ.x/2, (dist/3));         // todo: make these limited by a fraction of the node's size

                nvgBeginPath(vg);
                  nvgMoveTo(vg,   src.P.x, src.P.y);
                  nvgBezierTo(vg, outNxt.x,outNxt.y, inNxt.x,inNxt.y, dest.P.x,dest.P.y);
                  nvgStrokeWidth(vg, 3.f);
                  nvgStrokeColor(vg, nvgRGBAf(.7f, 1.f, .9f, .5f));
                nvgStroke(vg);

                ++ci; continue;
              }

              v2 avgP=src.P; v2 avgN={0,0}; u32 cnt=0;
              auto avgIter=ci;
              for(; avgIter!=en && avgIter->first == srcIdx; ++avgIter, ++cnt){
                if(!grph.hasSlot(avgIter->second)){ --cnt; continue; }

                Slot const& dest = grph.slot(avgIter->second);
                avgP += dest.P;
                avgP += src.P;
                //avgN += norm(dest.P
              }
              avgP    /= (f32)(cnt*2+1);             // can't forget the first position for averaging - the src position - the avgP is weighted 1:1 with the srcP and all the destination positions combined
              v2 midN  = norm(src.P - avgP);

              //f32  halfx = lerp(.5f, dest.P.x, src.P.x);
              //f32  halfy = lerp(.5f, dest.P.y, src.P.y);
              //f32   dist = len(src.P - dest.P);
              f32  halfx = lerp(.5f, src.P.x, avgP.x);
              f32  halfy = lerp(.5f, src.P.y, avgP.y);
              f32   dist = len(src.P - avgP);
              v2  outNxt = src.P  + src.N * min(NODE_SZ.x/2, (dist/3));         // divide by 3 because there are 3 sections to the bezier
              v2   inNxt = avgP   +  midN * min(NODE_SZ.x/2, (dist/3));         // todo: make these limited by a fraction of the node's size

              nvgBeginPath(vg);
                nvgMoveTo(vg,   src.P.x, src.P.y);
                //nvgBezierTo(vg, outNxt.x,outNxt.y, inNxt.x,inNxt.y, dest.P.x,dest.P.y);
                nvgBezierTo(vg, outNxt.x,outNxt.y, inNxt.x,inNxt.y, avgP.x, avgP.y);
                nvgStrokeWidth(vg, 3.f);
                nvgStrokeColor(vg, nvgRGBAf(.7f, 1.f, .9f, .5f));
              nvgStroke(vg);

              for(auto dhIter=ci; ci!=en && ci->first == srcIdx; ++ci){   // dhIter is draw half iterator - this is where the the connections are drawn from the average position of all slots 
                const v2 hlfsz = io_rad/2.f;
                Slot const& dest = grph.slot(ci->second);

                f32  halfx = lerp(.5f, dest.P.x, avgP.x);
                f32  halfy = lerp(.5f, dest.P.y, avgP.y);
                f32   dist = len(avgP - dest.P);
                v2  outNxt = dest.P  + dest.N * min(NODE_SZ.x/2, (dist/3));         // divide by 3 because there are 3 sections to the bezier
                v2   inNxt = avgP + -1.f*midN * min(NODE_SZ.x/2, (dist/3));        // todo: make these limited by a fraction of the node's size
                nvgBeginPath(vg);
                  nvgMoveTo(vg,   dest.P.x,dest.P.y);
                  nvgBezierTo(vg, outNxt.x,outNxt.y, inNxt.x,inNxt.y, avgP.x, avgP.y);
                  nvgStrokeWidth(vg, 3.f);
                  nvgStrokeColor(vg, nvgRGBAf(.7f, 1.f, .9f, .5f));
                nvgStroke(vg);
              }

              nvgBeginPath(vg);
                //nvgMoveTo(vg, avgP.x, avgP.y);
                nvgCircle(vg, avgP.x, avgP.y, 6.f); 
                nvgFillColor(vg, nvgRGBAf(.18f, .18f, .18f, 1.f) );
                nvgStrokeWidth(vg, 3.f);
                nvgStrokeColor(vg, nvgRGBAf(0,0,0,1.f) );
              nvgStroke(vg);
              nvgFill(vg);
            }
          }
          //SECTION(draw connections)
          //{
          //  auto ci = grph.cncts();                                 // ci is connection iterator 
          //  for(; ci!=grph.cnctEnd(); ++ci)
          //  {
          //    const v2 hlfsz = io_rad/2.f;
          //    Slot const&  src = grph.slot(ci->first);
          //    Slot const& dest = grph.slot(ci->second);
          //
          //    f32  halfx = lerp(.5f, dest.P.x, src.P.x);
          //    f32  halfy = lerp(.5f, dest.P.y, src.P.y);
          //    f32   dist = len(src.P - dest.P);
          //    v2  outNxt = src.P  + src.N * min(NODE_SZ.x/2, (dist/3));         // divide by 3 because there are 3 sections to the bezier
          //    v2   inNxt = dest.P + dest.N * min(NODE_SZ.x/2, (dist/3));        // todo: make these limited by a fraction of the node's size
          //
          //    nvgBeginPath(vg);
          //      nvgMoveTo(vg,   src.P.x,src.P.y);
          //      nvgBezierTo(vg, outNxt.x,outNxt.y, inNxt.x,inNxt.y, dest.P.x,dest.P.y);
          //      nvgStrokeWidth(vg, 3.f);
          //      nvgStrokeColor(vg, nvgRGBAf(.7f, 1.f, .9f, .5f));
          //    nvgStroke(vg);
          //  }
          //}
          SECTION(draw nodes)
          {
            auto sz = fd.grph.nsz();
            TO(sz,i)
            {
              int  ndOrdr = fd.grph.order(i);      // nd_ordr[i];
              auto      n = fd.grph.node(ndOrdr);
              bool selctd = ndOrdr==priSel || fd.grph.sel(ndOrdr);

              auto clr = NODE_CLR;
              if(selctd){ clr = nvgRGBf(.5f,.4f,.1f); }

              float round = secSel==ndOrdr? 0 : 1.f;
              fd.grph.bnd(ndOrdr) = node_draw(vg, 0, n, clr, round);

              SECTION(draw node slots)
              {
                nvgStrokeColor(vg, nvgRGBAf(0,0,0,1.f));
                nvgStrokeWidth(vg, BORDER);
                auto sIter = grph.nodeSlots(ndOrdr);            // sIter is slot iterator
                for(; sIter!=grph.slotEnd() && sIter->first==ndOrdr; ++sIter)
                {
                  auto     sIdx = sIter->second;                    // sIdx is slot index
                  Slot const& s = grph.slot(sIdx);
                  bool   inSlot = len(pntr - s.P) < io_rad;

                  Slot::State drawState = Slot::NORMAL;
                  if(s.state==Slot::SELECTED) drawState = Slot::SELECTED;
                  else if(inSlot)             drawState = Slot::HIGHLIGHTED;
                  slot_draw(vg, s, drawState);
                }
              }

            }
          }
          SECTION(draw selection box)
          {
            if(lftDn && priSel<0)
            {
              nvgBeginPath(vg);
                float x,y,w,h;
                x = min(drgP.x, pntr.x); 
                y = min(drgP.y, pntr.y); 
                w = abs(drgP.x - pntr.x);
                h = abs(drgP.y - pntr.y);
                nvgRect(vg, x,y, w,h);
                nvgStrokeWidth(vg, 1.f);
                nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, .75f));
              nvgStroke(vg);
            }
          }
          SECTION(draw fps - frames per second counter)
          {
            avgFps *= 0.9;
            avgFps += (1.0/dt)*0.1;
            int fps = (int)avgFps;

            char fpsStr[TITLE_MAX_LEN];
            sprintf(fpsStr, "%d", fps);

            f32 tb = nvgTextBounds(vg, 10,0, fpsStr, NULL, NULL);
            nvgFontSize(vg, 15.0f);
            nvgFontFace(vg, "sans-bold");
            nvgTextAlign(vg,  NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);  // NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
            nvgFillColor(vg, nvgRGBA(255,255,255,255));
            nvgText(vg, tb-40, 10, fpsStr, NULL);
          }
          nvgEndFrame(vg);
        }
        SECTION(nanogui)
        {
          fd.ui.screen.drawContents();
          fd.ui.screen.drawWidgets();
        }
      }

      prevRtDn  =  rtDn;
      prevLftDn = lftDn;

      glfwSwapBuffers(fd.win);
      glfwPollEvents();
    }
  }

  nanogui::shutdown();
  glfwTerminate();
  return 0;
}








//nvgBeginPath(vg);
//  nvgCircle(vg, x+w/2,y+h/2, msgRad);
//  //nvgRoundedRect(vg, x,y,w,h, rad);
//nvgFillColor(vg, nvgRGBAf(.15f, .15f, .15f, .95f));
//nvgFill(vg);
//
//x-msgRad*.1f, y-msgRad*.1f, x+msgRad*.1f, y+msgRad*.1f, 

//nvgRoundedRect(vg, x,y,w,h, rad+BORDER); //-.5f);
//nvgRoundedRect(vg, x+0.5f,y+0.5f, w-1,h-1, cornerRad-.5f);
//
//nvgStrokeColor(vg, nvgRGBA(0,0,0,128));
//nvgStroke(vg);

//int sz = (int)nd_ordr.size();
//auto sz = fd.grph.getNodes().size();
//auto sz = grph.nsz();
//
//int  ndOrdr = nd_ordr[i];
//node&     n = nodes[ndOrdr];
//bool selctd = ndOrdr==priSel || sels[ndOrdr];

//  for(; ci!=en && ci->first == srcIdx; ++ci)
//  {
//    f32  halfx = lerp(.5f, dest.P.x, src.P.x);
//    f32  halfy = lerp(.5f, dest.P.y, src.P.y);
//    f32   dist = len(src.P - dest.P);
//    v2  outNxt = src.P  + src.N * min(NODE_SZ.x/2, (dist/3));         // divide by 3 because there are 3 sections to the bezier
//    v2   inNxt = dest.P + dest.N * min(NODE_SZ.x/2, (dist/3));        // todo: make these limited by a fraction of the node's size
//    nvgBeginPath(vg);
//    nvgMoveTo(vg,   src.P.x,src.P.y);
//    nvgBezierTo(vg, outNxt.x,outNxt.y, inNxt.x,inNxt.y, dest.P.x,dest.P.y);
//    nvgStrokeWidth(vg, 3.f);
//    nvgStrokeColor(vg, nvgRGBAf(.7f, 1.f, .9f, .5f));
//    nvgStroke(vg);
//  }
//}
//
//for(auto ci = grph.cncts(); ci != en; ){
//
//f32  halfx = lerp(.5f, dest.P.x, src.P.x);
//f32  halfy = lerp(.5f, dest.P.y, src.P.y);
//f32   dist = len(src.P - dest.P);
//v2  outNxt = src.P  + src.N * min(NODE_SZ.x/2, (dist/3));         // divide by 3 because there are 3 sections to the bezier
//v2   inNxt = dest.P + dest.N * min(NODE_SZ.x/2, (dist/3));        // todo: make these limited by a fraction of the node's size
//nvgBeginPath(vg);
//  nvgMoveTo(vg,   src.P.x,src.P.y);
//  nvgBezierTo(vg, outNxt.x,outNxt.y, inNxt.x,inNxt.y, dest.P.x,dest.P.y);
//  nvgStrokeWidth(vg, 3.f);
//  nvgStrokeColor(vg, nvgRGBAf(.7f, 1.f, .9f, .5f));
//nvgStroke(vg);

//auto&   cn = cncts[i];
//v2     out = slots_out[cn.src].P;
//
//v2 outNrml = slots_out_nrmls[cn.src];
//v2      in = slots_in[cn.dest].P;
//v2  inNrml = slots_in_nrmls[cn.dest];

//TO(cncts.size(),i)
//{
//  const v2 hlfsz = io_rad/2.f;
//  auto&   cn = cncts[i];
//  v2     out = slots_out[cn.src].P;
//  v2 outNrml = slots_out_nrmls[cn.src];
//  v2      in = slots_in[cn.dest].P;
//  v2  inNrml = slots_in_nrmls[cn.dest];
//  f32  halfx = lerp(.5f, in.x, out.x);
//  f32  halfy = lerp(.5f, in.y, out.y);
//  f32   dist = len(out - in);
//  v2  outNxt = out + outNrml*(dist/3);              // divide by 3 because there are 3 sections to the bezier
//  v2   inNxt = in  +  inNrml*(dist/3);
//
//  // draw normal
//  //nvgBeginPath(vg);
//  //  nvgMoveTo(vg,   out.x,out.y);
//  //  nvgLineTo(vg,  outNxt.x,outNxt.y);
//  //  nvgStrokeColor(vg, nvgRGBAf(.7f, 1.f, .9f, .5f));
//  //nvgStroke(vg);
//
//  nvgBeginPath(vg);
//    nvgMoveTo(vg,   out.x,out.y);
//    nvgBezierTo(vg, outNxt.x,outNxt.y, inNxt.x,inNxt.y, in.x,in.y);
//    nvgStrokeWidth(vg, 3.f);
//    nvgStrokeColor(vg, nvgRGBAf(.7f, 1.f, .9f, .5f));
//  nvgStroke(vg);
//
//  //v2 nrmlEnd = in + inNrml*100.f;
//  //nvgBeginPath(vg);
//  //  nvgMoveTo(vg,   in.x,in.y);
//  //  nvgLineTo(vg,  nrmlEnd.x, nrmlEnd.y);
//  //nvgStroke(vg);
//  //
//  //nvgBezierTo(vg, halfx,out.y, halfx,in.y, in.x,in.y);
//}

//if(lftDn && !prevLftDn){
//  TO(grph.getSlots().size(), i){
//    Slot&    s = grph.slot(i);
//    bool inSlt = len(pntr - s.P) < io_rad;
//    if(inSlt){
//      outClk          = (i32)i;
//      s.state         = Slot::SELECTED;
//      clearSelections = false;
//    }
//  }
//}

//auto destNdIdx = grph.slot(ci->dest).nidx;
//
//auto destNdIdx  = grph.slot(ci->second).nidx;
//destP          += grph.node(destNdIdx).P;

// grph.node(destNdIdx).P;
//destN += norm(curP - s.P);
//v2  P  = hasNaN(s.P)? n.P  :  s.P;

//s.P = node_border(n, destP - n.P);
//s.P = node_border(n, destP - n.P, &nrml);
//s.N = nrml;

//auto  cnctIter = find_if(ALL(cncts), [&i](cnct const& c){return c.src==i;});       // find connect in the naive way for now 
//if(cnctIter==end(cncts)){
//  s.P = node_border(n, v2(0,s.in? -1.f : 1.f), &nrml);
//  s.N = nrml;
//}else{
//  v2  destP = {0,0};
//  int   cnt = 0; 
//  for(;cnctIter != end(cncts); ++cnt, ++cnctIter){
//    auto destNdIdx = grph.slot(cnctIter->dest).nidx;
//    destP         += grph.node(destNdIdx).P;
//  }
//  destP              /= (f32)cnt;
//  s.P = node_border(n, destP - n.P, &nrml);
//  s.N = nrml;
//}

//fd.grph.addNode( { {300.f,300.f}, "one",   node::FLOW } );
//fd.grph.addNode( { {300.f,500.f}, "two",   node::FLOW } );
//fd.grph.addNode( { {300.f,300.f}, "three", node::MSG  } );
//
//nodes.push_back( node("one",   node::FLOW, {300.f,300.f}) );
//nodes.push_back( node("two",   node::FLOW, {300.f,500.f}) );
//nodes.push_back( node("three", node::FLOW, {300.f,300.f}) );
//
//nodes.push_back( { {300.f,300.f}, "one",   node::FLOW } );
//nodes.push_back( { {300.f,500.f}, "two",   node::FLOW } );
//nodes.push_back( { {300.f,300.f}, "three", node::MSG  } );
//
//slots_out.push_back( {{0.f,0.f}, 0, {0,1.f} } );
//slots_out_nrmls.push_back( {0,0} );
//
//slots_in.push_back( {{0.f,0.f}, 1, {0,1.f} } );
//slots_in_nrmls.push_back( {0,0} );
//
//slots_in.push_back( {{0.f,0.f}, 2, {0,1.f} } );
//slots_in_nrmls.push_back( {0,0} );
//
//for(auto& n : nodes){
//nbnds.push_back( {n.P.x, n.P.y, n.P.x+NODE_SZ.x, n.P.y+NODE_SZ.y} );
//}
//
//auto sz = nodes.size();
//sels.resize(sz, false);
//
//nd_ordr.resize(sz);
//TO(sz,i) nd_ordr[i] = (i32)i;

//GLFWwindow*            win;
//vec_bnd              nbnds;
//vec<bool>             sels;             // bitfield for selected nodes
//veci               nd_ordr;

//vec_nd               nodes;
//vec_con              cncts;             // cncts is connections - the vector of connection structures
//cnct_tbl          cnct_src;
//
//vec_slot          slots_in;
//vec_slot         slots_out;
//vec_v2      slots_in_nrmls;
//vec_v2     slots_out_nrmls;
//
//cnct_tbl         cnct_dest;

//sels[i] = isIn(nbnds[i],drgbnd);
//sels[ndOrdr] = inNode;

//int      sz = (int)nd_ordr.size();
//
// (int)nd_ordr.size();

//cnct c = {slotOutSel, inClk};
//auto curCnct = find(ALL(cncts), c);
//if(curCnct == end(cncts)) 
//  cncts.push_back(c);
//else
//  cncts.erase(curCnct);                                   // have to find another way to index connections if they are going to be referenced, since any deletion will invalidate their positions
//
//inAny = true;

//MoveToBack( &nd_ordr, i);
//
//if(!sels[ndOrdr]){
//  TO(sels.size(),i) sels[i]=false;
//  sels[ndOrdr] = true;
//}
//
//TO(sels.size(),i) grph.sel(i,false);

//int  ndOrdr = nd_ordr[i];
//node&     n = grph.orderedNode(i);
//node&     n = nodes[ndOrdr];
//bool inNode = isIn(pntr.x,pntr.y, nbnds[ndOrdr]);

//SECTION(secondary selection (for connections) )
//{
//  if(inNode && rtDn && !prevRtDn)
//  {
//    if(secSel<0) secSel=ndOrdr;
//    else{ // create a connection between secSel and i
//      if( insUnq(&cnct_src,  ndOrdr, secSel) &&
//          insUnq(&cnct_dest, secSel, ndOrdr) ){
//        cncts.push_back( {ndOrdr, secSel} );
//      }
//  
//      secSel = -1;
//    }
//    break;
//  }
//}

//slot_draw(vg, s, inSlot);
//                  
//bool slot_selected = false;
//
//inSlot? slot::HIGHLIGHTED : slot::NORMAL

//int sz = (int)nd_ordr.size();
//auto sz = fd.grph.getNodes().size();
//
//node&     n = nodes[ndOrdr];
//bool selctd = ndOrdr==priSel || sels[ndOrdr];
//
//nbnds[ndOrdr] = node_draw(vg, 0, n, clr, round);

//cnct c = {outClk, slotInSel};
//
//auto curCnct = find(ALL(cncts), c);
//if(curCnct==end(cncts)) 

//auto curCnct = find(ALL(grph.cncts), c);              
//if(curCnct==end(cncts)) 
//  cncts.push_back( {outClk, slotInSel} );
//else cncts.erase(curCnct);                                 // have to find another way to index connections if they are going to be referenced, since any deletion will invalidate their positions

//bool inSlt = len(pntr - slots_out[i].P) < io_rad;
//
// else s.state = slot::NORMAL;

//TO(slots_out.size(), i){
//
//TO(slots_in.size(),  i){ 
//  bool inSlt = len(pntr - slots_in[i].P) < io_rad;
//  if(inSlt) inClk = (i32)i;           
//}

//SECTION(draw slots)
//{
//nvgStrokeColor(vg, nvgRGBAf(0,0,0,1.f));
//nvgStrokeWidth(vg, BORDER);
//TO(grph.ssz(),i){
//  slot const& s = grph.slot(i);
//  bool inSlot = len(pntr - s.P) < io_rad;
//  slot_draw(vg, s, inSlot);
//}
//}

//TO(slots_out.size(),i){
//  bool inSlot = len(pntr-slots_out[i].P) < io_rad;
//  slot_draw(vg, i, true, inSlot);
//
//  //slot&  slt = slots_out[i];
//  //v2     out = slt.P;
//  //v2       N = slt.N;
//  //bool inSlt = len(pntr-out) < io_rad;
//  //
//  //NVGcolor fillClr;
//  //if(i==slotInSel) fillClr = nvgRGBAf(1.f,   1.f,   .5f,   1.f);
//  //else if(inSlt)   fillClr = nvgRGBAf( .36f, 1.f,   .36f,  1.f);
//  //else             fillClr = nvgRGBAf( .18f,  .75f, .18f,  1.f);
//  //nvgFillColor(vg, fillClr);
//  //
//  //nvgBeginPath(vg);
//  //nvgCircle(vg, out.x, out.y, io_rad);
//  //nvgFill(vg);
//  //nvgStroke(vg);
//  //
//  //// slot triangle drawing
//  //f32  triRad = io_rad - 2.f;
//  //auto inrClr = fillClr;
//  //inrClr.r += 0.2f;
//  //inrClr.g += 0.2f;
//  //inrClr.b += 0.2f;
//  //nvgFillColor(vg, inrClr);
//  //
//  //nvgBeginPath(vg);
//  //nvgResetTransform(vg);
//  //nvgTranslate(vg, out.x, out.y);             // translate comes before rotate here because nanovg 'premultiplies' transformations instead of multiplying them in for some reason. this reverses the order of transformations and can be seen in the source for nanovg
//  //nvgRotate(vg, normalToAngle(N) -PIf/2.f );
//  //
//  //nvgMoveTo(vg, -0.707f*triRad, -0.707f*triRad);
//  //nvgLineTo(vg,  0.707f*triRad, -0.707f*triRad);
//  //nvgLineTo(vg,  0, triRad);
//  //
//  //nvgClosePath(vg);
//  //nvgFill(vg);
//  //nvgResetTransform(vg);
//}
//TO(slots_in.size(),i){
//  bool inSlot = len(pntr-slots_in[i].P) < io_rad;
//  slot_draw(vg, i, false, inSlot);
//
//  //slot&  slt = slots_in[i];
//  //v2      in = slt.P;
//  //v2       N = slt.N;
//  //bool inSlt = len(pntr-in) < io_rad;
//  //
//  //NVGcolor fillClr;
//  //if(i==slotInSel) fillClr = nvgRGBAf(1.f,   1.f,   .5f,  1.f);
//  //else if(inSlt)   fillClr = nvgRGBAf( .36f,  .9f, 1.f,   1.f);
//  //else             fillClr = nvgRGBAf( .18f,  .6f,  .75f, 1.f);
//  //nvgFillColor(vg, fillClr);
//  //
//  //nvgBeginPath(vg);
//  //nvgCircle(vg, in.x, in.y, io_rad);
//  //nvgFill(vg);
//  //nvgStroke(vg);
//  //
//  //// slot triangle drawing
//  //f32  triRad = io_rad - 2.f;
//  //auto inrClr = fillClr;
//  //inrClr.r += 0.2f;
//  //inrClr.g += 0.2f;
//  //inrClr.b += 0.2f;
//  //nvgFillColor(vg, inrClr);
//  //
//  //nvgBeginPath(vg);
//  //nvgResetTransform(vg);
//  //nvgTranslate(vg, in.x, in.y);             // translate comes before rotate here because nanovg 'premultiplies' transformations instead of multiplying them in for some reason. this reverses the order of transformations and can be seen in the source for nanovg
//  //nvgRotate(vg, normalToAngle(N)+PIf/2.f );
//  //
//  //nvgMoveTo(vg, -0.707f*triRad, -0.707f*triRad);
//  //nvgLineTo(vg,  0.707f*triRad, -0.707f*triRad);
//  //nvgLineTo(vg,  0, triRad);
//  //
//  //nvgClosePath(vg);
//  //nvgFill(vg);
//  //nvgResetTransform(vg);
//  //
//  ////drw_rail(vg, slt.P, nodes[slt.nidx].P);
//}

//
//slot&  destSlt =  grph.slot(cnctIter->dest)

//TO(slots_out.size(),i){
//  auto nidx = slots_out[i].nidx;
//  if(nidx < nodes.size()){
//    v2        nrml;
//    node&        n = nodes[nidx];
//    auto  cnctIter = find_if(ALL(cncts), [&i](cnct const& c){ return c.src==i; } );            // find connect in the naive way for now 
//    if(cnctIter == end(cncts)){
//      slots_out[i].P     = node_border(n, v2(0,1.f), &nrml);
//      slots_out[i].N     = nrml;
//      slots_out_nrmls[i] = nrml;
//    }else{
//      //slots_out[i].P     = {0,0};
//      v2       destP     = {0,0};
//      //slots_out[i].N     = {0,0};
//      //slots_out_nrmls[i] = {0,0};
//      int   cnt = 0; 
//      for(;cnctIter != end(cncts); ++cnt, ++cnctIter){
//        slot&  destSlt = slots_in[cnctIter->dest];
//        destP              += nodes[destSlt.nidx].P;
//        //slots_out[i].N     += nrml;
//        //slots_out_nrmls[i] += nrml;
//      }
//      destP              /= (f32)cnt;
//      slots_out[i].P      = node_border(n, destP - n.P, &nrml);
//      slots_out[i].N      = nrml;
//      slots_out_nrmls[i]  = nrml;
//
//      //slots_out[i].P     /= (f32)cnt;
//      //slots_out[i].N     /= (f32)cnt;
//      //slots_out_nrmls[i] /= (f32)cnt;
//    }
//  }
//
//  //v2       destP = nodes[destSlt.nidx].P;
//  //
//  //slots_out[i].P     = out_cntr(n, io_rad);
//  //slots_out[i].N     = {0, 1.f};
//  //slots_out_nrmls[i] = {0, 1.f};
//  //
//  //if(n.type==node::FLOW){
//  //slots_out[i].P = node_border(n.P, destP - n.P, io_rad, &nrml);
//  //
//  //}else{
//  //  f32 rad = NODE_SZ.x/2;                // todo: will need to be changed to use a per node size
//  //  slots_out[i].P = node_border(n.P, destP - n.P, io_rad, &nrml);
//  //  slots_out[i].N = nrml;
//  //}
//}
//TO(slots_in.size(),i){
//  auto nidx = slots_in[i].nidx;
//  if(nidx < nodes.size()){
//    v2        nrml;
//    node&        n = nodes[nidx];
//    auto  cnctIter = find_if(ALL(cncts), [&i](cnct const& c){ return c.dest==i; } );            // find connect in the naive way for now 
//    if(cnctIter == end(cncts)){
//      slots_in[i].P = node_border(n, v2(0,-1.f), &nrml);
//      slots_in[i].N = nrml;
//      slots_in_nrmls[i] = nrml;
//    }else{
//      slot&  srcSlt = slots_out[cnctIter->src];
//      v2       srcP = srcSlt.P;
//      //v2       srcP = nodes[srcSlt.nidx].P;
//      slots_in[i].P = node_border(n, srcP - n.P, &nrml);
//      slots_in[i].N = nrml;
//      slots_in_nrmls[i] = nrml;
//    }
//  }
//  //slots_in[i].P  = node_border(n.P, srcP - n.P, io_rad, &nrml);
//  //
//  //slots_in[i].P  = in_cntr(n, io_rad);
//  //slots_in[i].N = {0,1.f};
//  //slots_in_nrmls[i] = {0, 1.f};
//}

//
//v2           node_border(node const& n, v2 dir, f32 slot_rad, v2* out_nrml=nullptr)

//void           slot_draw(NVGcontext* vg, u64 idx, bool isOut=true, bool highlight=false)
//
//slot&  slt = isOut? slots_out[idx]  :  slots_in[idx];
//bool inSlt = len(pntr-out) < io_rad;
//
//if(idx==slotInSel) fillClr = nvgRGBAf(1.f,   1.f,   .5f,  1.f);
//else if(highlight) fillClr = nvgRGBAf( .36f,  .9f, 1.f,   1.f);
//else               fillClr = nvgRGBAf( .18f,  .6f,  .75f, 1.f);
//
//if(idx==slotInSel) fillClr = nvgRGBAf(1.f,   1.f,   .5f,   1.f);
//else if(highlight) fillClr = nvgRGBAf( .36f, 1.f,   .36f,  1.f);
//else               fillClr = nvgRGBAf( .18f,  .75f, .18f,  1.f);

//if(inSlt && slotOutSel != i){
//  if(slotInSel<0){ slotOutSel = (i32)i; }                    // deselects if clicking elsewhere or if inside the selected slot - this creates a toggle
//  else{ 
//    cncts.push_back( {(i32)i, slotInSel} ); 
//  }
//}else slotOutSel = -1;
//
//if(lftDn && !prevLftDn){
//  TO(slots_in.size(),  i){ 
//    bool inSlt = len(pntr - slots_in[i].P) < io_rad;
//
//    if(inSlt && slotInSel != i) slotInSel =  (i32)i;             // deselects if clicking elsewhere or if inside the selected slot - this creates a toggle
//    else                        slotInSel = -1;
//  }
//
//  TO(slots_out.size(), i){ 
//    bool inSlt = len(pntr - slots_out[i].P) < io_rad;
//
//    if(inSlt && slotOutSel != i){ 
//      
//      if(slotInSel<0){ slotOutSel = (i32)i; }                    // deselects if clicking elsewhere or if inside the selected slot - this creates a toggle
//      else{ 
//        cncts.push_back( {(i32)i, slotInSel} ); 
//      }
//    }else slotOutSel = -1;
//  }
//}

//else if(slotOutSel > -1){
//  cncts.push_back( {slotOutSel, inClk} );
//  slotOutSel = slotInSel = -1;

//
//int ww, wh,

//SECTION(border test)
//{
//  //  v2 ncntr = n.P + NODE_SZ/2.f;
//  //  v2 hlfsz = NODE_SZ / 2.f;
//  //  v2  pdir = norm(pntr - ncntr) * len(hlfsz);          // * normsz;
//  //  
//  //  v2 ds = sign(pdir);                                  // ds is direction sign
//  //  if( abs(pdir.x) > hlfsz.x ){
//  //    pdir /= abs(pdir.x)/hlfsz.x;
//  //  }else{
//  //    pdir /= abs(pdir.y)/hlfsz.y;
//  //  }
//  //  
//  //  f32        r = NODE_SZ.y/2.f;
//  //  v2  circCntr = (pdir.x<0)? n.P+v2(r,r)  :  n.P+NODE_SZ-r;
//  //  v2   intrsct = lineCircleIntsct(ncntr, pdir, circCntr, r);
//  //  bool     hit = !hasInf(intrsct);
//  //  if(hit) pdir = intrsct - ncntr;
//  //  else continue;
//
//  //  v2 dirEnd = ncntr + pdir*1.f;
//  //  nvgBeginPath(vg);
//  //   nvgMoveTo(vg, ncntr.x,ncntr.y);
//  //   nvgLineTo(vg, dirEnd.x, dirEnd.y);
//  //  nvgStrokeWidth(vg, 3.f);
//  //  nvgStroke(vg);
//
//  //  nvgBeginPath(vg);
//  //   nvgCircle(vg, circCntr.x,circCntr.y, r);
//  //  nvgStrokeWidth(vg, 1.f);
//  //  nvgStroke(vg);
//
//  //  nvgBeginPath(vg);
//  //   nvgCircle(vg, intrsct.x,intrsct.y, 4.f);
//  //  nvgFill(vg);
//  //  
//  //  v2 brdr = ncntr + pdir + norm(pdir)*8.f;
//  //  nvgBeginPath(vg);
//  //   nvgCircle(vg, brdr.x,brdr.y, 8.f);
//  //  nvgFill(vg);
//}

//f32  triRad = io_rad - 2.f;
//auto inrClr = fillClr;
//inrClr.r += 0.2f;
//inrClr.g += 0.2f;
//inrClr.b += 0.2f;
//nvgFillColor(vg, inrClr);
//nvgBeginPath(vg);
//  nvgMoveTo(vg, in.x-0.707f*triRad, in.y-0.707f*triRad);
//  nvgLineTo(vg, in.x+0.707f*triRad, in.y-0.707f*triRad);
//  nvgLineTo(vg, in.x, in.y+triRad);
//  nvgClosePath(vg);
//nvgFill(vg);

//auto inrClr = fillClr;
//inrClr.a = inrClr.r = inrClr.g = inrClr.b = 1.f;
//nvgFillColor(vg, inrClr);
//nvgBeginPath(vg);
//  nvgCircle(vg, in.x, in.y, io_rad);
//  nvgRadialGradient(vg, in.x, in.y, 0, io_rad/2, inrClr, fillClr);
//nvgFill(vg);
//
//nvgTransformRotate()
//
//inrClr.a = inrClr.r = inrClr.g = inrClr.b = 1.f;

//if(i==slotOutSel) nvgFillColor(vg, nvgRGBAf(1.f,   1.f,  .5f,  1.f));
//else if(inSlt)    nvgFillColor(vg, nvgRGBAf( .36f, 1.f,  .36f, 1.f));
//else              nvgFillColor(vg, nvgRGBAf( .18f,  .75f, .18f, 1.f));
//
//inrClr.a = inrClr.r = inrClr.g = inrClr.b = 1.f;

//nvgMoveTo(vg, out.x-0.707*triRad, out.y-0.707*triRad);
//nvgLineTo(vg, out.x+0.707*triRad, out.y-0.707*triRad);
//nvgLineTo(vg, out.x, out.y+triRad);
//nvgClosePath(vg);
