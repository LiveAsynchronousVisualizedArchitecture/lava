
/* 
  This file will be more organized when looked at in an IDE with syntax folding 
*/

// -todo: try using generic libc dll - misses linked in C++ functions like __std_terminate
// -todo: make a roughness parameter as constant input for the shade ray hits 
// -todo: delete connections if there is no node available - fixed by not making connections if the nodes they need are not in the opposite buffers
// -todo: make shade ray hits use the weight to dictate length of visualized rays
// -todo: make slot viz not delete visualization
// -todo: give node creation buttons colors based on the same colors that their node types use
// -todo: make dragging a slot turn it into a constant, which writes a .type.const file, which is then memory mapped and live reloaded by lava
// -todo: debug why trace node is crashing when there is no shade rays parameters constant node loaded - when a frame was found, a packet could still be put into another frame
// -todo: debug why shade ray hits runs in infinite loop with two constant inputs
// -todo: organize nodes by types in a contex menu or side bar - sorted by name if type is the same too
// -todo: get node buttons out of the main bar
// -todo: switch back to GridLayout 
// -todo: make vector for spacer labels so they can be cleared - can just use empty labels and the existing label vector
// -todo: debug extra characters in the constant string array - needed to use strnlen() to take the minimum between the tbl array size and the length as a C string, to check for null/0/'\0' characters
// -todo: try changing .obj path constant
// -todo: make tbl editor be able to edit i8 strings as text - use a length limit?
// todo: -debug crash when trying to demo camera - is there a problem with the array parameter in the camera parameter constant? - seems to be with the tbl 
//        |  can't directly realloc the reference counted allocations - why is the camera node using a realloc function in the first place?
//        |  might have just been local allocations not being filled in with LavaParams
// -todo: debug why the camera ray visualization shows less rays than should be generated - number of points in visualization is rays*2 which should be correct, though number of lines is less, possibly half rounded up - some points are -inf and inf - inf in traced rays is possibly not fitlered out when visualizing as indexed verts - inf check and a separate ray index variable worked - should probably make a habit of visualizing the actual data going out 
// -todo: try slimming camera file size with sys_msvcrt.lib - need force option to link when there are multiply defined symbols
// -todo: try hardware random numbers to avoid <random> - only supported on ivy bridge and above
// -todo: figure out why ShadeRayHits is crashing - tbl::place_rh wasn't returning a pointer at all points, but the casting operator for KV was still letting it compile
// -todo: try OS specific system calls for Sleep instead of thread::sleep, so that there is one less C++ stdlib function that needs to be linked in to the shared libs
// -todo: try combining ShadeRayHits and Camera projects into a single shared library - http and xml parsing combined into single .dll which works fine
// -todo: give fissure live reloading inside the main loop
// -todo: give reloading a force option and use it to force the first reload
// -todo: put in live reloading that uses the Lava step function on reload
// -todo: work on and test live reloading - what thread reloads? - either the main loop of fissure or a separate thread that mostly sleeps - either way calling the reload function seems to best left out of LavaFlow.hpp
// -todo: debug why step function keeps looping on reload - do the LavaIds not get deleted from the vizId if the packet has no data? - was step only getting set by the step button callback and not the step function? - seems so
// -todo: fix high cpu usage - thread sleeping and yielding based on window state or main loop timing 

// todo: make string convenience function 
// todo: make console button re-open stdout so that printf will work without reloading a node
// todo: give const reloading the same structure as node reloading, or integrate them together
// todo: work out timed live reload checking - are there event callbacks that can be used - yes, windows seems to have a callback setup that could be used - could using separate directories for libs and live libs make it only neccesary to check the directory write time?
//       |  make a class that can special case windows
// todo: implement live reloading that copies the shared library, loads it, switches over atomically, unloads the previous live version, copies the tmp live version to the normal live file, maps the new live version file, automically switches over to that, then unloads the tmp live shared lib - whew!
// todo: make LavaQ into a struct that contains a function pointer to its own push function and can be passed as a C struct
// todo: put much of lava in its own name space
// todo: put globals struct into LavaParams that will hold the current number of threads running as well as the logical cores available - could also include a bitmask of which outputs are connected
// todo: implement optional arguments 
// todo: debug why BRDF rays is not outputting a visualization packet - node not being run because there is no frame for the node - need to implement optional arguments
// todo: debug why Demo_Trace does not stop looping - FilePath doesn't seem to get called - does deleting nodes delete them from the message/generator cache?
// todo: order generator nodes by traversing the graph backwards 
//       |  make a Lava function that returns a map of orders for graph nodes by their positions in the graph
//       |  could recursivly follow the graph backwards, traversing the first inputs first for a consistent ordering
// todo: make a Lava function or queue that outputs errors - use the LavaOut struct for errors and warnings
// todo: make errors put a node in an error or warning state 
// todo: make errors or warnings print with their color in the status bar
// todo: change node colors to be based off of profiling information while holding 'p' key
// todo: make step button function take a node or list of node ids to start with 
// todo: make Tbl Editor step only the node it is editing
// todo: make step button step only the selected nodes
// todo: make dragging a .const file into the UI copy it to the path 
// todo: build in data type visualization - part needs to be lava, part needs to be UI - does any need to be in lava? - is checking for 'tb' at the start of a binary blob, then checking for a "type" key enough to determine types ? - if it is a project, a window that shows visualized and unvisualized data types could be used
// todo: make sure zooming center is affected by cursor placement - now have the cursor in world space thanks to drgWrld
// todo: try using windows API to slow cursor movement when inside nodes and slots 
// todo: investigate if Trace node is spending most of its time in the BVH building and figure out what to do about it
// todo: integrate AddConst into RefreshFlowLibs function so that there are .live versions
// todo: make a lava function to incrementally load a single lib and another function to load the rest of the queue
// todo: make fissure or lava be able to incrementally load shared libraries
// todo: change constructor to happen on play and not on load (or after the destructor runs on stop)
// todo: re-orient nodes on resize of the window so they line up with the grid in the same place 
//       | maybe the scale and pan need to be changed instead 
//       | if the window size is simply shrinking closer to 0, maybe the center point needs to be normalized according to where it was in the window
//       | might just need to shift the pan by the change in the center point
// todo: make nodes have their own scale that dictates the text size and not the other way around
// todo: put slot name in db key - maybe the name should have two lines
// todo: make message node's text split to new lines on white space
// todo: make an IdxVerts helper header file
// todo: redo atomic bitset now that slots are separated and inputs should be packed together
// todo: make Tbl Editors pop up for all selected constants that point to tbls - need a vector of tbl windows and tbl layouts as well as a vector of vectors for the widgets of each key value
// todo: add heiarchy of tables - recursive function and indentation
// todo: put thread pointers into message node instances and work out how to lock and unlock them

// todo: give cache a buffer of at least one extra allocation, so that if it runs first as a generator in the cycle it can still catch the next allocation that comes through
// todo: should there be a ONCE node type too? - should there be a parameter of how many times the node is allowed to run? - only offers convenience, though to make it properly, an atomic needs to be used for the boolean of whether or not to run
// todo: add cursor member variable to LavaFrame - this would only be neccesary if slots weren't done with a one packet to one slot strucutre 
// todo: make a modal window that will set the text for a note 
// todo: keep track of loops through message nodes that don't generate any packets - if a thread goes through 2 empty loops, sleep - maybe sleep 1ms longer for each empty loop, up to 100ms
// todo: change slot placement so that output slots always point directly at the center average of their target nodes
// todo: add tool tips to node buttons containing the description string of the node
// todo: find way to add a tbl to a tbl by reference / direct assignment - should it immediatly copy and flatten()
// todo: make freezing packets at inputs visualized by a light blue circle larger than the yellow circle for visualizing in flight packets - use blue 'sunshine' lines going out from the center like a snowflake
// todo: make a settings file that is read on load if it in the same directory
// todo: make list of nodes a side window, right click menu, hot box, etc
// todo: make slots connected to the same node loop with a circle from one side to the other so they don't overlap
// todo: make each variable in the graph individually double buffered or even multi-buffered according to readers?
// todo: have exec() spinlock until readers of the opposite buffer drops to 0 - could also just skip the command buffer in the rare case that it catches readers as more than 0
// todo: look into techniques for keeping data local to CPU cores, and CPU sockets
// todo: put each thread's owned memory vector into a global vector that other threads can access - can the LavaQ be used or broken into a single writer multi-reader array?
// todo: make shared libraries only try to load one per frame
// todo: design packet freezing and packet visualization interface - maybe have three states - neutral, visualized, and frozen
// todo: test live reloading by compiling a shared lib while lava is running
// todo: integrate rapid json

// todo: make lava memory allocation aligned to 64 byte boundaries
// todo: come up with locking system so that message nodes have their own threads that are only run when a looping thread visits them - how should memory allocation be done? passing the thread's allocator in the exact same way?
// todo: prototype API for message nodes
//       | do message nodes need some extra way to hold their state? will there ever be more than a single instance of a message node?
//       | initially just make them thread safe or make them lock with mutexes
//       | do messages need some sort of 8 byte number to be able to do occasionally do without heap or simdb allocated values?
// todo: transition to better json support to write formatted json/.lava files?
// todo: put in error checking for connecting dest to dest or src to src?
// todo: make a node to read text from a file name 
// todo: make a node to split text into lines and scatter the result
// todo: make message node diameter dependant on text bounds
// todo: make unconnected slots not overlap
// todo: make one node snap to another node
// todo: make two snapped nodes group together and be dragged together
// todo: make multiple slots avoid each other - might need to have discreet sections around a node for a slot to sit in
// todo: change NODE_ERROR to NODE_NONE 

#include "FissureStatic.cpp"

// glew might include windows.h
#define  WIN32_LEAN_AND_MEAN
#define  WIN32_EXTRA_LEAN
#define  NOMINMAX
#include "glew_2.0.0.h"
#include "glfw3.h"
#include "nanovg.h"

#define NANOVG_GL3_IMPLEMENTATION   // Use GL2 implementation.
//#include "nanovg_gl.h"              // nanogui includes this, not sure why it is needed now and wasn't in the past

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
#include <thread>
#include <regex>
#include <filesystem>
#include "nfd.h"
#include "jzon.h"
#include "vec.hpp"
#include "../no_rt_util.h"
#include "../Transform.h"
#include "Watcher.hpp"
#include "FissureDecl.h"

namespace  fs = std::experimental::filesystem;

using vec_ids = std::vector<LavaId>;

static FisData    fd;
static simdb   fisdb;

namespace{

void step(u64 num=1);

// Util
template<class T> T strToNum(str const& s)
{
  std::istringstream iss(s);
  T val;
  iss >> val;
  return val;
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
bool              hasInf(v2   v)
{
  TO(2,i) if(v[i]==INFf || v[i]==-INFf) return true;
  return false;
}
bool              hasNaN(v2   v)
{
  TO(2,i) if(v[i]==SIG_NANf || v[i]==-SIG_NANf) return true;
  return false;
}
v2      lineCircleIntsct(v2   P, v2 dir, v2 crcl, f32 r)  // only works for circles to the sides
{
  using namespace std;
  
  if(dir.x==0) return {INFf, INFf};

  //v2    PtoCrcl = P - crcl;
  //f32 lenToCrcl = len(PtoCrcl);
  //if( lenToCrcl <= r ){                                        // if the point is inside the circle, just make a vector that goes from the circle center through the point that has a length of r (radius) 
  //  v2  nrmToCrcl = PtoCrcl / lenToCrcl;
  //  v2     radDir = nrmToCrcl * r;
  //  v2 cntrToEdge = crcl + radDir;
  //  return radDir; 
  //}

  v2       st = (P - crcl) / r;
  f32     mlt = abs(st.x) / dir.x;                            // mlt = multiplier - the multiplier to get dir.x to equal st.x 
  f64       C = (st + dir*mlt).y;
  if(C > r) return v2(INFf, INFf);
  f64       m = dir.y / dir.x;
  f64       a = SQR(m) + 1;
  f64       b = 2.0 * m * C;
  f64       c = SQR(C) - 1.0;
  f64      q2 = SQR(b) - 4.0*a*c;
  if(q2 < 0) return v2(INFf, INFf);
  f64       x = ((-b + sqrt(q2)) / 2.0*a); // - 0.00315;
  x = min(1.,max(-1.,x));
  //if(x<-1 || x>1) return v2(INFf, INFf);
  //f64  xfract = x - (i64)x;
  //f64   acosX = acos(xfract);
  //f64       y = sin(acosX);
  f64       y = sin(acos(x));
  if(isnan(y)) return v2(INFf, INFf);
  v2  intrsct = v2( (f32)(sign(dir.x)*x), (f32)(sign(dir.y)*y) ) * r + crcl;

  return intrsct;
}
f32        normalToAngle(v2   N)
{
  return atan2(N.y, N.x);
}
v2         angleToNormal(f32 angle)
{
  return { cos(angle), sin(angle) };
}
f64        timeToSeconds(u64  t)
{
  return t / 1000000000.0;
}
// End Util

// Flow Control
u64                nxtId()
{
  return fd.nxtId++;
}
void     stopFlowThreads()
{
  fd.flow.stop();
  for(auto& t : fd.flowThreads){
    if(t.joinable()){
      t.join();
    }
  }
  fd.flowThreads.clear();
  fd.flowThreads.shrink_to_fit();

  fd.ui.stopBtn->setBackgroundColor(  Color(e3f(.19f, .16f, .17f)) ); 
  fd.ui.stopBtn->setEnabled(false);

  fd.ui.playBtn->setEnabled(true);
  fd.ui.playBtn->setTextColor( Color(e3f(1.f, 1.f, 1.f)) );
  fd.ui.playBtn->setBackgroundColor(  Color(e3f(.15f, .2f,  .15f)) ); // set play button back to normal

  fd.ui.pauseBtn->setEnabled(false);
  fd.ui.stopBtn->setEnabled(false);
}
void    startFlowThreads(u64 num=1)
{
  stopFlowThreads();
  fd.lgrph.clearTime();
  fd.flow.start();

  TO(num,i){
    fd.flowThreads.emplace_back([](){
      LavaLoop(fd.flow);
    });
  }

  fd.ui.playBtn->setEnabled(false);
  fd.ui.playBtn->setTextColor(       Color(e3f(1.f, 1.f, 1.f)) );
  fd.ui.playBtn->setBackgroundColor( Color(e3f(.1f, 1.f, .1f)) ); 

  fd.ui.pauseBtn->setEnabled(true);
  fd.ui.stopBtn->setEnabled(true);
}
void                step(u64 num)                                    // num defaults to 1 in the prototype
{
  fd.step = true;
  if( fd.vizIds.count()>0  &&  !fd.flow.m_running.load() ){
    fd.stepIds = fd.vizIds;
    startFlowThreads(num);
  }
}
// End Flow Control

void         draw_radial(NVGcontext* vg, NVGcolor clr, f32 x, f32 y, f32 rad)
{
  auto  hlf = rad/2; 

  nvgFillColor(vg, nvgRGBA(0,255,255,255));
  nvgBeginPath(vg);
  nvgCircle(vg, x, y, rad);
  auto radial = nvgRadialGradient(vg,x,y,0,hlf,clr,nvgRGBA(0,0,0,0));
  nvgFillPaint(vg, radial);
  nvgFill(vg);
}

// State manipulation
v2               in_cntr(Node const& n, f32 r)
{
  //return v2(n.P.x + NODE_SZ.x/2, n.P.y-r);
  return v2(n.P.x + n.b.w()/2, n.P.y-r);
}
v2              out_cntr(Node const& n, f32 r)
{
  //return v2(n.P.x + NODE_SZ.x/2, n.P.y + NODE_SZ.y + r);
  return v2(n.P.x + n.b.w()/2, n.P.y + n.b.h() + r);
}

void            slot_add(bool isDest)
{
  LavaCommand::Arg arg;
  arg.slotDest = isDest;
  fd.lgrph.put(LavaCommand::ADD_SLOT, arg);
}
void           slot_draw(NVGcontext* vg, Slot const& s, Slot::State drawState, f32 slot_rad=10.f)
{
  nvgSave(vg);

  nvgStrokeColor(vg, nvgRGBAf(0,0,0,1.f));
  nvgStrokeWidth(vg, 3.f);

  v2     out = s.P;
  v2       N = s.N;

  NVGcolor fillClr;
  if(s.in){
    switch(drawState){
    case Slot::SELECTED:    fillClr = nvgRGBAf(1.f,   1.f,   .5f,  1.f); break;
    case Slot::HIGHLIGHTED: fillClr = nvgRGBAf( .36f,  .9f, 1.f,   1.f); break;
    case Slot::NORMAL:
    default:                fillClr = nvgRGBAf( .18f,  .6f,  .75f, 1.f); break;
    }
  }else{
    switch(drawState){
    case Slot::SELECTED:    fillClr = nvgRGBAf(1.f,   1.f,   .5f,   1.f); break;
    case Slot::HIGHLIGHTED: fillClr = nvgRGBAf( .36f, 1.f,   .36f,  1.f); break;
    case Slot::NORMAL:      
    default:                fillClr = nvgRGBAf( .18f,  .75f, .18f,  1.f); break;
    }
  }
  nvgFillColor(vg, fillClr);

  nvgBeginPath(vg);
    nvgCircle(vg, out.x, out.y, slot_rad);  //io_rad);
    nvgFill(vg);
  nvgStroke(vg);

  // slot triangle drawing
  f32  triRad = slot_rad - 2.f;
  auto inrClr = fillClr;
  inrClr.r += 0.2f;
  inrClr.g += 0.2f;
  inrClr.b += 0.2f;
  nvgFillColor(vg, inrClr);

  //f32 tfm[6];
  //nvgCurrentTransform(vg, tfm);

  nvgBeginPath(vg);

    //nvgResetTransform(vg);
    nvgTranslate(vg, out.x, out.y);             // translate comes before rotate here because nanovg 'premultiplies' transformations instead of multiplying them in for some reason. this reverses the order of transformations and can be seen in the source for nanovg
    nvgRotate(vg, normalToAngle(N) + (s.in? PIf/2.f : -PIf/2) );

    nvgMoveTo(vg, -0.707f*triRad, -0.707f*triRad);
    nvgLineTo(vg,  0.707f*triRad, -0.707f*triRad);
    nvgLineTo(vg,  0, triRad);
  nvgClosePath(vg);
  nvgFill(vg);

  nvgRestore(vg);
  //nvgResetTransform(vg);
}
Slot*           slot_get(LavaId id)
{
  //auto& slots = fd.graph.slots;
  auto& slots = id.isIn? fd.graph.inSlots  :  fd.graph.outSlots;

  auto si = slots.find(id);
  if(si == slots.end()) 
    return nullptr;

  return &si->second;
}

u64        node_nxtOrder()
{
  auto& nodes = fd.graph.nds;
  auto& ordrs = fd.graph.ordr;

  u64 order = 1;
  if(ordrs.size() > 0)
    order = ordrs.rbegin()->order + 1;

  return order;
}
auto        node_getPtrs() -> vec_ndptrs
{
  auto& nodes = fd.graph.nds;
  auto     sz = fd.graph.nds.size();  // nds.nsz();
  //u64       i = 0;
  vec_ndptrs nds; 
  nds.reserve(sz);
  for(auto& ido : fd.graph.ordr){                     // ido is id order - an IdOrdr struct
    if(nodes.find(ido.id) != end(nodes)){
      nds.push_back( &fd.graph.nds[ido.id] );
      //++i;
    }
  }

  return nds;

  //vec_ndptrs nds(sz,nullptr);
  //
  //nds[i++] = &fd.graph.nds[ido.id];

  //for(auto& kv : fd.graph.nds){
  //  nds[i++] = &kv.second;
  //}
}
Node&   node_moveToFront(u64 idx)
{
  using namespace std;

  Node& n = fd.graph.nds[idx];
  auto nxt = node_nxtOrder();

  fd.graph.ordr.erase( {idx, n.order} );
  n.order = nxt;
  fd.graph.ordr.insert( {idx, n.order} );

  return n;
}

auto            node_add(str node_name,  Node n) -> uint64_t
{
  // this effectivly adds a node to the ui graph, adds a add_node command to the LavaGraph, adds slot commands, then on exec adds the slots to the ui graph - far from ideal
  using namespace std;

  auto          pi = fd.flow.nameToPtr.find( node_name );                               // pi is pointer iterator
  uint64_t instIdx = n.id; 
  LavaNode*     ln = nullptr;
  if( pi != end(fd.flow.nameToPtr) )
  {
    ln = pi->second;
    if(ln)
    {
      if(instIdx == LavaNode::NODE_ERROR) 
        instIdx = fd.lgrph.nxtId();

      FisData::IdOrder ido;                                                          //ido is id order
      ido.id    = instIdx;
      ido.order = node_nxtOrder();
      fd.graph.ordr.insert(ido);
      
      if(n.txt==""){ n.txt = "" + node_name; }
      n.id    = instIdx;
      n.order = ido.order;
      n.ln    = ln; 
      fd.graph.nds[instIdx] = move(n);

      LavaCommand::Arg A,B;
      A.ndptr = ln;
      B.val   = instIdx;
      fd.lgrph.put(LavaCommand::ADD_NODE, A, B);

      auto out_types = ln->out_types;                    // do these first so that the output slots start at 0
      for(; out_types  &&  *out_types; ++out_types){ 
        slot_add(false);                                 // this will put a slot command into the LavaGraph command queue - the graph_apply() function will actually end up creating the slots in the ui graph
      }

      auto in_types = ln->in_types;
      for(; in_types  &&  *in_types; ++in_types){
        slot_add(true);
      }
    }

  }

  return instIdx;
}
Bnd             node_bnd(NVGcontext* vg, Node const&  n)
{
  using namespace std;
  
  f32 x=n.P.x, y=n.P.y; // w=n.b.w(), h=n.b.h();
  //Bnd b;
  //b = { x, y, x+w, y+h };
  //return b;

  f32   txtsz = fd.graph.textSize;
  f32 wMargin = fd.graph.textMarginW;
  f32 hMargin = fd.graph.textMarginH;
  nvgFontSize(vg, txtsz);
  nvgFontFace(vg, "sans-bold");
  f32 txtBnds[4];
  f32      tw = nvgTextBounds(vg, 0,0, n.txt.c_str(), NULL, txtBnds);
  f32       w = tw + wMargin;
  f32       h = (txtBnds[3]-txtBnds[1]) + hMargin;  // nvgTextBounds puts minx, miny, maxx, maxy in the txtBnds float array
  //f32       w = max<float>(n.b.w(), tw + wMargin);
  //f32       h = max<float>(n.b.h(), (txtBnds[3]-txtBnds[1]) + hMargin );  // nvgTextBounds puts minx, miny, maxx, maxy in the txtBnds float array

  Bnd b;
  if(n.type == LavaNode::MSG){
    b = { x, y, x+w, y+w };
  }else{
    //f32 x=n.P.x, y=n.P.y; // h=n.b.h();
    b = { x, y, x+w, y+h };
  }

  return b;
}
void           node_draw(NVGcontext* vg,      // drw_node is draw node
                            int preicon,
                          Node const& n,
                         float      rnd,      // rnd is corner rounding
                         bool  forceSel=false,
                         f32     border=3.5f,
                         bool highlight=false)
{
  using namespace std;

  const float   rthk = 8.f;    // rw is rail thickness

  nvgSave(vg);

  //nvgResetTransform(vg);
  nvgGlobalAlpha(vg, 1.f);
  
  float wMargin=50.f, hMargin=20.f;
	
  NVGpaint bg;
	char icon[8];
  float tw=0, iw=0, x=n.P.x, y=n.P.y, h=n.b.h(); // w=n.b.w(),

  f32 txtsz = fd.graph.textSize;
  nvgFontSize(vg, txtsz);
  nvgFontFace(vg, "sans-bold");
  tw = nvgTextBounds(vg, 0,0, n.txt.c_str(), NULL, NULL);
  float w  = max<float>(n.b.w(), tw + wMargin);

	float rad = lerp(rnd, 0.f, h/2.f);                                  // rad is corner radius
  f32 cntrX = x + w/2.f;
  f32 cntrY = y + h/2.f; 
  f32    rr = rad;                                                    // rr is rail radius
  bool  sel = forceSel || n.sel;

  NVGcolor col = fd.ui.nd_selclr; //fd.ui.nd_color;
  if(!sel){
    switch(n.type){
    case Node::Type::CONSTANT:  col = fd.ui.nd_const_clr;  break;
    case Node::Type::GENERATOR: col = fd.ui.nd_gen_clr;    break;
    case Node::Type::MSG:       col = fd.ui.nd_gather_clr; break;
    case Node::Type::FLOW:
    default: 
      col = fd.ui.nd_color; break;
    }
  }

  switch(n.type)
  {
  case Node::Type::MSG: {
    SECTION(draw message node)
    {
      f32 msgRad = w/2.f;

      nvgStrokeColor(vg, fd.ui.lineClr);
      nvgStrokeWidth(vg, border);

      SECTION(linear gradient from upper left)
      {
        nvgBeginPath(vg);
        nvgCircle(vg, cntrX, cntrY, msgRad);
        auto lin = nvgLinearGradient(vg, 
          cntrX, cntrY-msgRad, x, y+msgRad,
          sel? fd.ui.msgnd_selclr : fd.ui.msgnd_gradst,
          sel? fd.ui.msgnd_selclr : fd.ui.msgnd_graden );
        nvgFillPaint(vg, lin);
        nvgFill(vg);
      }
      SECTION(radial gradient)
      {
        nvgBeginPath(vg);
        nvgCircle(vg, cntrX, cntrY, msgRad);
        auto radial = nvgRadialGradient(vg,
          cntrX, cntrY, msgRad*.5f, msgRad,
          //fd.ui.msgnd_gradst,
          col,
          fd.ui.msgnd_graden  );
        nvgFillPaint(vg, radial);
        nvgFill(vg);

        if(highlight){
          nvgBeginPath(vg);
          nvgCircle(vg, cntrX, cntrY, msgRad);
          auto radial = nvgRadialGradient(vg,
            cntrX, cntrY, msgRad*.5f, msgRad,
            fd.ui.msgnd_gradst,
            fd.ui.nd_hilgt );
            //fd.ui.msgnd_graden  );
          nvgFillPaint(vg, radial);
          nvgFill(vg);
        }
      }
      SECTION(circle outline)
      {
        nvgBeginPath(vg);
          nvgCircle(vg, cntrX, cntrY, msgRad);
        nvgStroke(vg);
      }

    }
  } break;
  case Node::Type::FLOW:
  case Node::Type::GENERATOR: 
  case Node::Type::CONSTANT:
  default: {
    SECTION(draw flow node)
    {
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
        auto topClr = nvgRGBA(255,255,255, isBlack(col)?16:grad );
        auto botClr = nvgRGBA(0,0,0, isBlack(col)?16:grad );
        bg = nvgLinearGradient(vg, x,y,x,y+h, topClr, botClr);
        nvgBeginPath(vg);
        nvgRoundedRect(vg, x+border,y+border, w-(border*2),h-(border*2), rad-border);
        col.a = 0.8f;
        if(!isBlack(col)){
          nvgFillColor(vg, col);
          nvgFill(vg);
        }
        nvgFillPaint(vg, bg);
        nvgFill(vg);
      }
    }
  } break;
  }

  SECTION(text)
  {
   // f32 txtsz = fd.graph.textSize;
	  //nvgFontSize(vg, txtsz);
	  //nvgFontFace(vg, "sans-bold");
	  //tw = nvgTextBounds(vg, 0,0, n.txt.c_str(), NULL, NULL);
	  if(preicon != 0){
		  //nvgFontSize(vg, h*1.3f);
      nvgFontSize(vg, h);
		  nvgFontFace(vg, "icons");
		  iw = nvgTextBounds(vg, 0,0, cpToUTF8(preicon,icon), NULL, NULL);
		  //iw += h*0.15f;
	  }

    f32 txtX = x+w*0.5f - tw*0.5f + iw*0.25f;   // + fd.ui.slot_rad;
    f32 txtY = y + h*0.5f;
	  nvgFontSize(vg, txtsz);
	  nvgFontFace(vg, "sans-bold");
	  nvgTextAlign(vg, NVG_ALIGN_MIDDLE);  // NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
	  nvgFillColor(vg, nvgRGBA(0,0,0,160));
    nvgText(vg, txtX, txtY-1, n.txt.c_str(), NULL);
	  nvgFillColor(vg, nvgRGBA(255,255,255,255));
    nvgText(vg, txtX, txtY, n.txt.c_str(), NULL);
  }
  SECTION(debug circle sides and bnd)
  {
    //nvgStrokeColor(vg, nvgRGBAf(1,1,1,1) );
    //nvgStrokeWidth(vg, 1.f);
    //
    //nvgBeginPath(vg);
    //  nvgRect(vg, b.xmn, b.ymn, b.w(), b.h());
    //nvgStroke(vg);
    //
    //nvgBeginPath(vg);
    // nvgMoveTo(vg, n.P.x+rad,n.P.y);
    // //nvgLineTo(vg, n.P.x+rad,n.P.y + NODE_SZ.y);
    // nvgLineTo(vg, n.P.x+rad,n.P.y + h);
    //nvgStroke(vg);
    //
    //nvgBeginPath(vg);
    //  //nvgMoveTo(vg, n.P.x+NODE_SZ.x-rad, n.P.y);
    //  //nvgLineTo(vg, n.P.x+NODE_SZ.x-rad, n.P.y + NODE_SZ.y);
    //  nvgMoveTo(vg, n.P.x + w - rad, n.P.y);
    //  nvgLineTo(vg, n.P.x + w - rad, n.P.y + h);
    //nvgStroke(vg);
    //
    //nvgBeginPath(vg);
    //  nvgCircle(vg, n.P.x+rad, n.P.y+rad, rad);
    //nvgStroke(vg);
    //
    //nvgBeginPath(vg);
    //  //nvgCircle(vg, n.P.x+NODE_SZ.x-rad, n.P.y+NODE_SZ.y-rad, rad);
    //  nvgCircle(vg, n.P.x + w - rad, n.P.y + h - rad, rad);
    //nvgStroke(vg);
  }

  nvgRestore(vg);
}
v2           node_border(Node const& n, v2 dir, v2* out_nrml=nullptr)
{
  v2       wh = n.b.wh();
  v2      hlf = wh / 2;
  v2       nP = n.P;
  v2 borderPt = {0,0};
  v2     ndir = norm(dir);
  v2    ncntr = nP + hlf;      // n.b.mx/2.f; // hlf; // n.mx/2.f; // NODE_HALF_SZ;

  switch(n.type)
  {
  case Node::Type::MSG: 
  {
    f32  rad = wh.x/2;
    borderPt = ncntr + ndir*rad; 
    if(out_nrml){ *out_nrml = ndir; }
  } break;
  case Node::Type::GENERATOR:
  case Node::Type::FLOW: 
  case Node::Type::CONSTANT:
  default: 
  {
    f32  rad  =  hlf.y;
    v2  pdir  =  ndir;
    v2    ds  =  sign(pdir);                                  // ds is direction sign
    f32   ax  =  abs(pdir.x);
    if( ax > hlf.x ){
      pdir /= ax/hlf.x;                                       // can this never be 0, since ax is positive, hlf.x is positive, and ax is greater than hlf.x ? 
    }else{
      f32 ay = abs(pdir.y);
      pdir /= ay==0.f?  1.f  :  ay/hlf.y;
    }

    v2  circCntr = (pdir.x<0)? nP+v2(rad,rad)  :  nP + wh - v2(rad,rad);
    //v2   intrsct = lineCircleIntsct(ncntr, pdir, circCntr, rad);
    v2   intrsct = lineCircleIntsct(ncntr, ndir, circCntr, rad);
    bool     hit = !hasInf(intrsct) && !hasNaN(intrsct)  &&  (intrsct.x < nP.x+rad || intrsct.x > nP.x + wh.x - rad); 
    //bool     hit = !hasInf(intrsct);
    //if(hit){
    //  borderPt = ncntr + intrsct;
    //}else{
    //  borderPt = ncntr + pdir;
    //}

    if(hit){ pdir = intrsct - ncntr; }
    borderPt = ncntr + pdir;

    if(out_nrml){
      if(hit) *out_nrml = norm(borderPt - circCntr);
      else    *out_nrml = { 0.f, sign(ndir).y };
    }
  } break;
  }

  return borderPt;
}
auto        node_slotsIn(u64 nid) -> decltype(fd.graph.inSlots.begin())
{
  using namespace std;
  return lower_bound(ALL(fd.graph.inSlots), nid, [](auto a,auto b){ return a.first.nid < b; } );
}
auto       node_slotsOut(u64 nid) -> decltype(fd.graph.outSlots.begin())
{
  using namespace std;
  return lower_bound(ALL(fd.graph.outSlots), nid, [](auto a,auto b){ return a.first.nid < b; } );
}
auto          node_slots(vec_ndptrs const& nds) -> vec_ids
{
  using namespace std;

  auto&  inSlots = fd.graph.inSlots;
  auto& outSlots = fd.graph.outSlots;
  vec_ids sidxs;                                            // sidxs is slot indexes
  for(auto np : nds){                                       // np is node pointer and nds is nodes    
    SECTION(in slots)
    {
      auto si = node_slotsIn(np->id);    
      for(; si != end(inSlots)  &&  si->first.nid==np->id; ++si){
        auto id = si->first;
        id.isIn = true;
        sidxs.push_back(id);     
      }
    }
    SECTION(out slots)
    {
      auto si = node_slotsOut(np->id);    
      for(; si != end(outSlots)  &&  si->first.nid==np->id; ++si){
        auto id = si->first;
        id.isIn = false;
        sidxs.push_back(id);     
      }
    }
  }
  return sidxs;                                        // RVO
}
auto        node_slotsIn(vec_ndptrs const& nds) -> vec_ids
{
  using namespace std;

  auto& slots = fd.graph.inSlots;
  vec_ids sidxs;                                            // sidxs is slot indexes
  for(auto np : nds){                                       // np is node pointer and nds is nodes
    auto si = node_slotsIn(np->id);
    for(; si != end(slots)  &&  si->first.nid==np->id; ++si){
      Slot& s = si->second;
      sidxs.push_back(si->first);     
    }
  }
  return sidxs;                                        // RVO
}
auto       node_slotsOut(vec_ndptrs const& nds) -> vec_ids
{
  using namespace std;

  auto& slots = fd.graph.outSlots;
  vec_ids sidxs;                                            // sidxs is slot indexes
  for(auto np : nds){                                       // np is node pointer and nds is nodes
    auto si = node_slotsOut(np->id);
    for(; si != end(slots)  &&  si->first.nid==np->id; ++si){
      Slot& s = si->second;
      sidxs.push_back(si->first);     
    }
  }
  return sidxs;                                        // RVO
}
void         node_delete()
{
  
}
void    node_draw_radial(Node const& n, NVGcontext* vg, NVGcolor clr)
{
  auto    w = n.b.w();
  auto    h = n.b.h();
  auto   cx = n.P.x + w/2;
  auto   cy = n.P.y + h/2;

  draw_radial(vg, clr, cx, cy, w*1.25f);
}

void           cnct_draw(NVGcontext* vg, v2 srcP, v2 destP, v2 srcN, v2 destN, f32 minCenterDist=INFf)
{ 
  using namespace std;
  
  f32  halfx = lerp(.5f, srcP.x, destP.x);
  f32  halfy = lerp(.5f, srcP.y, destP.y);
  f32   dist = len(srcP - destP);
  v2  outNxt = srcP  + srcN  * min(minCenterDist, (dist/3));         // divide by 3 because there are 3 sections to the bezier
  v2   inNxt = destP + destN * min(minCenterDist, (dist/3));

  nvgBeginPath(vg);
    nvgMoveTo(vg,   srcP.x, srcP.y);
    nvgBezierTo(vg, outNxt.x,outNxt.y, inNxt.x,inNxt.y, destP.x,destP.y);
    //nvgStrokeWidth(vg, 3.f);
  nvgStroke(vg);
}
void           grid_draw(NVGcontext* vg, f32 lineSpace = 128.f)
{
  // the lines will already be transformed, so they just need to wrap around and create the illusion of an infinite canvas
  
  //f32 grdSz = 0.05f;
  Bnd     b = { 0, 0, (f32)fd.ui.w, (f32)fd.ui.h };
  nvgCurrentTransform(vg,           fd.ui.tfm);

  nvgSave(vg);
    nvgResetTransform(vg);
    nvgTransformInverse(fd.ui.invTfm, fd.ui.tfm);
    nvgTransformPoint( &b.mn.x, &b.mn.y, fd.ui.invTfm, b.mn.x, b.mn.y);   
    nvgTransformPoint( &b.mx.x, &b.mx.y, fd.ui.invTfm, b.mx.x, b.mx.y);

    f32    hlfW = (f32)fd.ui.w / 2.f;
    f32    hlfH = (f32)fd.ui.h / 2.f;
    f32 cntrX, cntrY;
    nvgTransformPoint( &cntrX, &cntrY, fd.ui.tfm, hlfW, hlfH);

    SECTION(draw centered grid lines)
    {
      nvgStrokeWidth(vg, 2.f);
      nvgStrokeColor(vg, nvgRGBAf( .18f, .18f, .18f, 1.f));

      nvgBeginPath(vg);
      nvgMoveTo(vg, cntrX,   0);
      nvgLineTo(vg, cntrX,   (f32)fd.ui.h);
      nvgStroke(vg);

      nvgBeginPath(vg);
      nvgMoveTo(vg, 0,            cntrY);
      nvgLineTo(vg, (f32)fd.ui.w, cntrY);
      nvgStroke(vg);
    }

    nvgStrokeWidth(vg, 1.f);
    nvgStrokeColor(vg, nvgRGBAf( .12f, .12f, .12f, 1.f));

    SECTION(draw X)
    {
      f32   ofstX = (b.mx.x+b.mn.x)/2.f - hlfW;
      f32    curX = b.mn.x + fmodf(b.w()/2.f - ofstX,lineSpace);
      while(curX < b.mx.x){
        f32 reX = remap(curX, b.mn.x, b.mx.x, 0, (f32)fd.ui.w);
        nvgBeginPath(vg);
        nvgMoveTo(vg, reX, 0);
        nvgLineTo(vg, reX, (f32)fd.ui.h);
        nvgStroke(vg);
        curX += lineSpace;
      }
    }
    SECTION(draw Y)
    {
      f32   ofstY = (b.mx.y+b.mn.y)/2.f - hlfH;
      f32    curY = b.mn.y + fmodf(b.h()/2.f - ofstY,lineSpace);
      while(curY < b.mx.y){
        f32 reY = remap(curY, b.mn.y, b.mx.y, 0, (f32)fd.ui.h);
        nvgBeginPath(vg);
        nvgMoveTo(vg, 0,            reY);
        nvgLineTo(vg, (f32)fd.ui.w, reY);
        nvgStroke(vg);
        curY += lineSpace;
      }
    }
  nvgRestore(vg);
}

auto           sel_nodes() -> vec_ndptrs
{
  vec_ndptrs nds;                                      // nids is node ids
  for(auto& on : fd.graph.nds){                        // on is order and node - order is on.first    node is on.second
    if(on.second.sel) nds.push_back(&on.second);
  }

  auto& nodes = fd.graph.nds;
  if(fd.sel.pri != LavaNode::NODE_ERROR && 
    fd.sel.pri != LavaId::NODE_NONE)
  { 
    if(nodes.find(fd.sel.pri) != end(nodes))
      nds.push_back( &fd.graph.nds[fd.sel.pri] );
  }

  return nds;                                          // counting on RVO (return value optimization) here

  //for(auto& on : m_nodes){                           // on is order and node - order is on.first    node is on.second
}
u64           sel_delete()
{
  using namespace std;

  u64    cnt = 0;
  auto   nds = sel_nodes();           // accumulate nodes
  auto   ids = node_slots(nds);       // accumulate dest slots  // accumulate slots
  
  for(auto id : ids){                 // delete cncts with dest slots
    auto s = slot_get(id);
    //if(s){
    //  if(s->in) fd.lgrph.delDestCnct(id);  //){ ++cnt; }
    //  else      fd.lgrph.delSrcCncts(id);
    //}
    if(s){
      if(s->in){
        //fd.lgrph.delDestCnct(id);  //){ ++cnt; }
        LavaCommand::Arg A;
        A.id = id;
        fd.lgrph.put(LavaCommand::DEL_CNCT, A);
      }else{
        //fd.lgrph.delSrcCncts(id);
        LavaCommand::Arg A;
        A.id = id;
        fd.lgrph.put(LavaCommand::DEL_CNCT, A);
      }
    }
  }

  // delete slots
  for(auto id : ids){                  // are LavaGraph slots deleted when deleting their node?
    if(id.isIn) fd.graph.inSlots.erase(id);
    else        fd.graph.outSlots.erase(id);
    
    fd.vizIds.del(id.asInt);
  }

  // delete nodes
  for(auto n : nds){  // does deleting from the graph.nds unordered map invalidate the pointers? - standard says no - how is memory reclaimed? - rehash()
    auto  nid = n->id;
    auto ordr = n->order;
    fd.graph.ordr.erase({nid, ordr});
    fd.graph.nds.erase(nid);
    //fd.lgrph.delNode(nid);

    LavaCommand::Arg A;
    A.id = nid;
    fd.lgrph.put(LavaCommand::DEL_NODE, A);
  }
  fd.graph.nds.reserve( fd.graph.nds.size() * 2 );

  return cnt;
}
void           sel_clear()
{
  fd.sel.slotOutSel = LavaId();
  fd.sel.slotInSel  = LavaId();
  fd.sel.pri        = LavaNode::NODE_ERROR;
  fd.sel.sec        = LavaNode::NODE_ERROR;

  for(auto& kv : fd.graph.nds){
    kv.second.sel = false;
  }

  for(auto& kv : fd.graph.inSlots){
    kv.second.state = Slot::NORMAL;
  }
  for(auto& kv : fd.graph.outSlots){
    kv.second.state = Slot::NORMAL;
  }
}
void      sel_clearSlots()
{
  fd.sel.slotInSel.sidx  = LavaId::SLOT_NONE; 
  fd.sel.slotOutSel.sidx = LavaId::SLOT_NONE;

  //for(auto& kv : fd.graph.slots){
  //  kv.second.state = Slot::NORMAL;
  //}
  for(auto& kv : fd.graph.inSlots){
    kv.second.state = Slot::NORMAL;
  }
  for(auto& kv : fd.graph.outSlots){
    kv.second.state = Slot::NORMAL;
  }
}

void         graph_clear()
{
  fd.graph.nds.clear();
  //fd.graph.slots.clear();
  fd.graph.inSlots.clear();
  fd.graph.outSlots.clear();
  fd.graph.ordr.clear();

  fd.lgrph.clear();
}
void         graph_apply(LavaGraph::ArgVec args)
{
  using namespace std;

  for(auto& a : args){
    if(a.id.sidx == LavaId::SLOT_NONE && a.id.nid != LavaId::NODE_NONE)
    {
      LavaInst li = fd.lgrph.node(a.id.nid);
      if(li.node) fd.graph.nds[a.id.nid].type = li.node->node_type;                                      // node arg, not a slot arg - need to check the type
    }else if(a.id.sidx != LavaId::SLOT_NONE){
      LavaFlowSlot* ls = fd.lgrph.slot(a.id);
      if(ls){
        Slot s(a.id.nid, ls->in);
        s.P = {0,0};
        //fd.graph.slots.insert({a.id, s});
        if(ls->in) fd.graph.inSlots.insert({a.id, s});
        else       fd.graph.outSlots.insert({a.id, s});
      }
    }
  }
}
void         graph_apply()
{
  LavaGraph::ArgVec av = fd.lgrph.exec();
  graph_apply(move(av));
}
// End state manipulation

// Serialize to and from json - put in FisTfm.hpp file?
bool           writeFile(str path, void* buf, u64 size)
{
  FILE* f = fopen(path.c_str(), "wb");
  if(!f) return false;

  size_t writeSz = fwrite(buf, 1, size, f);
  if(writeSz != size) return false;

  int closeRet = fclose(f);
  if(closeRet == EOF) return false;

  return true;
}
void    normalizeIndices()
{
  using namespace std;
  
  auto   nmap = fd.lgrph.normalizeIndices();                              // nmap is normalization map - this contains a map of previous indices to new indices
  //auto& nodes = fd.graph.nds;

  decltype(fd.graph.nds) nxtNds;
  for(auto& kv : fd.graph.nds){
    auto nxtId = nmap[kv.first];
    nxtNds[nxtId] = move(kv.second);
  }
  
  // make sure the fissure ui nodes have matching ids
  for(auto& idNd : nxtNds){
    idNd.second.id    = idNd.first;
    //idNd.second.order = idNd.first;
  }

  // just clear vizIds and stepIds for now
  fd.vizIds.clear();
  fd.stepIds.clear();

  fd.graph.nds = move(nxtNds);

  SECTION(slots)
  {
    decltype(fd.graph.inSlots) nxtInSlots;
    for(auto& kv : fd.graph.inSlots){
      LavaId    nxtId = kv.first;
      nxtId.nid       = nmap[nxtId.nid];
      nxtInSlots.insert({ nxtId, move(kv.second) });
    }
    fd.graph.inSlots = move(nxtInSlots);

    decltype(fd.graph.outSlots) nxtOutSlots;
    for(auto& kv : fd.graph.outSlots){
      LavaId    nxtId = kv.first;
      nxtId.nid       = nmap[nxtId.nid];
      nxtOutSlots.insert({ nxtId, move(kv.second) });
    }
    fd.graph.outSlots = move(nxtOutSlots);
  }

  decltype(fd.graph.ordr) nxtOrdr;
  for(auto& o : fd.graph.ordr){
    auto nxtId = nmap[o.id];
    nxtOrdr.insert( {nxtId, o.order} );
  }
  fd.graph.ordr = move(nxtOrdr);


  //for(auto const& kv : nmap){
  //}
}
str           graphToStr(LavaGraph const& lg)
{
  using namespace std;

  Jzon::Node jnodes = Jzon::object();
  SECTION(nodes)
  {
    vec_ndptrs   nps = node_getPtrs();     // fg.nds;
    auto        lnds = lg.nodes();         // lnds is Lava Nodes
    auto          sz = lnds.size();

    assert(nps.size() == sz);

    Jzon::Node  nd_func = Jzon::array();
    TO(sz,i){
      assert( lg.curNodes().find( nps[i]->id ) != lg.curNodes().end() );
      LavaInst linst = lg.node(nps[i]->id);
      nd_func.add( linst.node->name );
    }

    Jzon::Node    nd_id = Jzon::array();
    TO(sz,i) nd_id.add(nps[i]->id);

    Jzon::Node   nd_txt = Jzon::array();
    TO(sz,i) nd_txt.add(nps[i]->txt);

    Jzon::Node     nd_x = Jzon::array();
    TO(sz,i) nd_x.add(nps[i]->P.x);

    Jzon::Node     nd_y = Jzon::array();
    TO(sz,i) nd_y.add(nps[i]->P.y);

    jnodes.add("function", nd_func);
    jnodes.add("id",         nd_id);
    jnodes.add("txt",       nd_txt);
    jnodes.add("x",           nd_x);
    jnodes.add("y",           nd_y);
  }

  Jzon::Node jcncts = Jzon::object();
  SECTION(connections)
  {
    auto sz = lg.cnctsz();

    Jzon::Node srcId   = Jzon::array();
    Jzon::Node srcIdx  = Jzon::array();
    Jzon::Node destId  = Jzon::array();
    Jzon::Node destIdx = Jzon::array();

    for(auto const& kv : lg.cncts()){
      destId.add(kv.first.nid);
      destIdx.add(kv.first.sidx);
      srcId.add(kv.second.nid);
      srcIdx.add(kv.second.sidx);
    }

    jcncts.add("destId",    destId);
    jcncts.add("destIdx",  destIdx);
    jcncts.add("srcId",      srcId);
    jcncts.add("srcIdx",    srcIdx);
  }

  Jzon::Node jViz = Jzon::object();
  SECTION(slot visualization) // because some slots might be visualized even though they aren't connected, the slot visualization needs to be separate - because they should all be output, isIn shouldn't be needed
  {
    Jzon::Node vizNids  = Jzon::array();
    Jzon::Node vizSlts  = Jzon::array();
    TO(fd.vizIds.sz,i){
      if(fd.vizIds.buf[i] != fd.vizIds.null_val ){
        LavaId id;
        id.asInt = fd.vizIds.buf[i];
        vizNids.add(id.nid);
        vizSlts.add(id.sidx);
      }
    }

    jViz.add("vizNids", vizNids);
    jViz.add("vizSlts", vizSlts);
  }

  Jzon::Node graph = Jzon::object();
  graph.add("nodes",       jnodes);
  graph.add("connections", jcncts);
  graph.add("visualize",     jViz);

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
void          strToGraph(str const& s)
{
  using namespace std;

  graph_clear();

  Jzon::Parser prs;
  auto graph = prs.parseString(s);

  auto nd_func    = graph.get("nodes").get("function");
  auto nd_id      = graph.get("nodes").get("id");
  auto nd_txt     = graph.get("nodes").get("txt");
  auto nd_x       = graph.get("nodes").get("x");
  auto nd_y       = graph.get("nodes").get("y");
  //auto nd_type    = graph.get("nodes").get("type");
  auto ordr       = graph.get("nodes").get("order");
  auto destId     = graph.get("connections").get("destId");
  auto destIdx    = graph.get("connections").get("destIdx");
  auto srcId      = graph.get("connections").get("srcId");
  auto srcIdx     = graph.get("connections").get("srcIdx");
  auto sltDestId  = graph.get("slots").get("destId");
  auto sltDestIdx = graph.get("slots").get("destIdx");
  auto sltSrcId   = graph.get("slots").get("srcId");
  auto sltSrcIdx  = graph.get("slots").get("srcIdx");
  auto vizNids    = graph.get("visualize").get("vizNids");
  auto vizSlts    = graph.get("visualize").get("vizSlts");

  auto   cnt = nd_id.getCount();
  u64 mxNdId = 0;
  TO(cnt,i){        
    Node n;
    n.id   = nd_id.get(i).toInt();
    mxNdId = max(mxNdId, n.id);

    n.txt  = nd_txt.get(i).toString();
    n.P.x  = nd_x.get(i).toFloat();
    n.P.y  = nd_y.get(i).toFloat();
    //n.type = (Node::Type)nd_type.get(i).toInt();

    str funcName = nd_func.get(i).toString();

    node_add( funcName, n);
  }


  auto cnct_cnt = destId.getCount();
  TO(cnct_cnt,i){
    LavaCommand::Arg  dest;
    LavaCommand::Arg   src;
    src.id.nid   = srcId.get(i).toInt();
    src.id.sidx  = srcIdx.get(i).toInt();
    src.id.isIn  = false;
    dest.id.nid  = destId.get(i).toInt();
    dest.id.sidx = destIdx.get(i).toInt();
    dest.id.isIn = true;
    fd.lgrph.put(LavaCommand::TGL_CNCT, dest, src);
  }

  auto viz_cnt = vizNids.getCount();
  TO(viz_cnt,i){
    LavaId id;
    id.nid  = vizNids.get(i).toInt();
    id.sidx = vizSlts.get(i).toInt();
    fd.vizIds.put(id.asInt);
  }

  fd.lgrph.setNextNodeId(mxNdId + 1);
}
bool            saveFile(LavaGraph const& lg, str path)
{
  stopFlowThreads();

  str fileStr = graphToStr(lg);

  FILE* f = fopen(path.c_str(), "w");
  if(!f) return false;

  size_t writeSz = fwrite(fileStr.c_str(), 1, fileStr.size(), f);
  if(writeSz != fileStr.size()) return false;

  int closeRet = fclose(f);
  if(closeRet == EOF) return false;

  //startFlowThreads(1);

  return true;
}
bool            loadFile(str path, LavaGraph* out_g)
{
  stopFlowThreads();

  FILE* f = fopen(path.c_str(), "r");
  if(!f) return false;
  fseek(f, 0, SEEK_END);
  str s;
  s.resize( ftell(f) );
  fseek(f, 0, SEEK_SET);

  size_t redRet = fread( (void*)s.data(), 1, s.size(), f);
  if(redRet != s.size()) return false; 

  if(fclose(f) == EOF) return false;

  strToGraph(s);

  graph_apply();

  return true;
}
// End serialize to and from json 

// UI Util
auto           nvgToNGUI(NVGcolor clr) -> nanogui::Color
{
  nanogui::Color nclr;
  TO(3,i){ nclr[i] = clr.rgba[i]; }
  nclr[3] = 1.f;

  return nclr;
}
str       makeStatusText(u64 nid, f64 totalTime, vec_ndptrs const& nds, u64 nIdx)
{
  using namespace std;

  auto&     n  =  fd.lgrph.node(nid);

  char percentStr[256] = {};
  char secondsStr[256] = {};
  SECTION(format the node run time percentage string)
  {
    f64 seconds  =  timeToSeconds(n.time);
    f64 percent  =  totalTime>0?  (seconds/totalTime)*100  :  0;
    seconds      =  (int)(seconds*1000) / 1000.0;    // should clamp the seconds to 3 decimal places 

    sprintf(percentStr, "%0.2f", (f32)percent);
    sprintf(secondsStr, "%0.2f", (f32)seconds);
    //percent      =  max(100.0, percent);
  }

  str     err;
  switch(n.getState())
  {
  case LavaInst::COMPILE_ERROR:{
    err = "COMPILE ERROR";
  }break;
  case LavaInst::LOAD_ERROR:{
    err = "LOAD ERROR (the node could not be loaded)";
  }break;
  case LavaInst::RUN_ERROR:{
    err = "RUN ERROR (did the node output a value that had no memory allocated?)";
  }break;
  case LavaInst::NORMAL:
  default:
    break;
  }

  auto status = toString(
    "Node [",nid,"]  ",nds[nIdx]->txt,
    " | ",secondsStr," seconds  %",
    percentStr,"   ",
    err);

  if(n.node->description){ 
    status += toString(" -  ", n.node->description);
  }

  return status;
}
void      clearTblEditor()
{
  fd.ui.cnstWin->setVisible(false);

  for(auto const& lbl : fd.ui.cnstLbls)
    fd.ui.cnstWin->removeChild(lbl);
  fd.ui.cnstLbls.clear();
  fd.ui.cnstLbls.shrink_to_fit();

  for(auto const& edt : fd.ui.cnstEdit)
    fd.ui.cnstWin->removeChild(edt);
  fd.ui.cnstEdit.clear();
  fd.ui.cnstEdit.shrink_to_fit();

  if(fd.ui.cnstClose){ fd.ui.cnstWin->removeChild(fd.ui.cnstClose); }
  fd.ui.cnstClose = nullptr;
 
  if(fd.ui.cnstStr){ fd.ui.cnstWin->removeChild(fd.ui.cnstStr); }
  fd.ui.cnstStr = nullptr;

  fd.ui.cnstWin->setLayout(fd.ui.cnstLay);
}
template<class T> void setInc(tbl::KVOfst& kvo, TextBox* tb, str const& s)
{
  using namespace std;

  auto val = strToNum<T>(s);
  kvo      = val;
  auto inc = T( max(abs(val*.1),1.) );
  ((IntBox<T>*)tb)->setValueIncrement(inc);
}
template<class T> void setFloatInc(tbl::KVOfst& kvo, TextBox* tb, str const& s)
{
  using namespace std;

  auto val = strToNum<T>(s);
  kvo      = val;
  auto inc = T( abs(val*.1) );
  ((FloatBox<T>*)tb)->setValueIncrement(inc);
}
bool       makeTblEditor(LavaNode* n)
{
  using namespace std;
  using nanogui::AdvancedGridLayout;

  clearTblEditor();

  if(fd.sel.pri==LavaNode::NODE_ERROR){ return false; }

  LavaInst   li = fd.lgrph.node(fd.sel.pri);
  LavaNode*  ln = li.node;  
  if( !ln || ln->node_type!=LavaNode::CONSTANT){ return false; }         // if the primary selected node isn't a constant, don't do anything
  if( !(ln->filePtr) ){ return false; }                                  // if it is a constant but its pointer is nullptr, do nothing
  if( !tbl::isTbl(ln->filePtr) ){ return false; }

  auto lay = fd.ui.cnstLay;
  tbl cnstTbl(ln->filePtr);
  SECTION(set up a text box if the tbl array is a string)
  {
    fd.ui.cnstLbls.emplace_back( new Label(fd.ui.cnstWin, "") );
    fd.ui.cnstStr = new TextBox(fd.ui.cnstWin, "");
    fd.ui.cnstStr->setFixedWidth(400);
    fd.ui.cnstLbls.emplace_back( new Label(fd.ui.cnstWin, "") );

    if(cnstTbl.arrayType()==tbl::TblType::I8)
    {
      auto  sz = cnstTbl.size();
      auto cSz = strnlen( cnstTbl.data<const char>(), sz);                   // get the smaller value between the size of the table's array and the length as a C string (in case a '\0' character is somewhere in the array)
      str tblStr;
      tblStr.resize(cSz);
      TO(cSz,i)
        tblStr[i] = (i8)cnstTbl[i];

      //Println("tblStr: <", tblStr,">");

      fd.ui.cnstStr->setValue(tblStr);
      fd.ui.cnstStr->setEditable(true);

      fd.ui.cnstLay->preferredSize(fd.vg, fd.ui.cnstStr);

      auto  cnstStr = fd.ui.cnstStr;
      fd.ui.cnstStr->setCallback([cnstStr, ln](str const& s)
      {
        tbl t(ln->filePtr);
        Println("s: <", s,">");

        auto   cap = t.capacity();
        auto strSz = s.size(); 
        if(strSz > cap){
          str nxt = s;
          nxt.resize(cap);
          Println("nxt: <", nxt,">");
          cnstStr->setValue(nxt);
        }else{
          t.size(cap);

          TO(strSz,i){ t[i] = (i8)s[i]; }
          auto zeroSz = cap - strSz;
          TO(zeroSz,i){ t[i+strSz] = '\0'; }

          t.size(strSz);

          //memcpy(t.data<i8>(), s.data(), strSz);
          //memset(t.data<i8>()+strSz, 0, cap - strSz);          
        }
      
        return true;
      });
    }
  }

  vecstr keys; 
  SECTION(extract the keys and sort them)
  {
    for(auto kv : cnstTbl){                                                // use the iterators to go through the key-value pairs, then build the controls from those pairs
      keys.emplace_back(kv.key);
    }
    sort(ALL(keys));
  }

  SECTION(build a control for each key)
  {
    TO(keys.size(),i)
    {
      auto const& k = keys[i];
      tbl::KV*   kv = cnstTbl(k.c_str()).kv;
      if( kv->hasTypeAttr(tbl::TblType::INTEGER) || 
        kv->hasTypeAttr(tbl::TblType::SIGNED) )                           // SIGNED but not INTEGER would be a float
      {
        //fd.ui.cnstLay->appendRow(0);
        auto col = (i32)(i+1);
  
        auto txtLbl = new Label(fd.ui.cnstWin, k.c_str());
        fd.ui.cnstLbls.emplace_back(txtLbl);
        //fd.ui.cnstLay->setAnchor(txtLbl, AdvancedGridLayout::Anchor(col,0, Alignment::Fill, Alignment::Fill) );
  
        auto    type = kv->type;
        TextBox*  tb = nullptr;
        switch(type){
          case tbl::TblType::I8:  tb = new   IntBox<i8>(fd.ui.cnstWin,   (i8)*kv); break;
          case tbl::TblType::U8:  tb = new   IntBox<u8>(fd.ui.cnstWin,   (u8)*kv); break;
          case tbl::TblType::I16: tb = new   IntBox<i16>(fd.ui.cnstWin, (i16)*kv); break;
          case tbl::TblType::U16: tb = new   IntBox<u16>(fd.ui.cnstWin, (u16)*kv); break;
          case tbl::TblType::I32: tb = new   IntBox<i32>(fd.ui.cnstWin, (i32)*kv); break;
          case tbl::TblType::U32: tb = new   IntBox<u32>(fd.ui.cnstWin, (u32)*kv); break;
          case tbl::TblType::I64: tb = new   IntBox<i64>(fd.ui.cnstWin, (i64)*kv); break;
          case tbl::TblType::U64: tb = new   IntBox<u64>(fd.ui.cnstWin, (u64)*kv); break;
          case tbl::TblType::F32: tb = new FloatBox<f32>(fd.ui.cnstWin, (f32)*kv); break;
          case tbl::TblType::F64: tb = new FloatBox<f64>(fd.ui.cnstWin, (f64)*kv); break;
        }
        tb->setSpinnable(true);
        tb->setEditable(true);
        tb->setFixedWidth(200);
        tb->setCallback([tb, type, li, k](str const& s)
        {
          tbl t(li.node->filePtr);
  
          tbl::KVOfst kvo = t(k.c_str());
          switch(type){
            case tbl::TblType::I8:  setInc<i8>(kvo,tb,s);  break;
            case tbl::TblType::U8:  setInc<u8>(kvo,tb,s);  break;
            case tbl::TblType::I16: setInc<i16>(kvo,tb,s); break; 
            case tbl::TblType::U16: setInc<u16>(kvo,tb,s); break;
            case tbl::TblType::I32: setInc<i32>(kvo,tb,s); break; 
            case tbl::TblType::U32: setInc<u32>(kvo,tb,s); break;
            case tbl::TblType::I64: setInc<i64>(kvo,tb,s); break; 
            case tbl::TblType::U64: setInc<u64>(kvo,tb,s); break;
            case tbl::TblType::F32: setFloatInc<f32>(kvo,tb,s); break; 
            case tbl::TblType::F64: setFloatInc<f64>(kvo,tb,s); break;
          }
  
          if(fd.vizIds.count() > 0)
            step(fd.threadCount);
  
          return true;
        });
        //fd.ui.cnstLay->setAnchor(tb, AdvancedGridLayout::Anchor(col,1, Alignment::Fill, Alignment::Fill) );
        fd.ui.cnstEdit.emplace_back(tb);
  
        str  typeStr = tbl::TblType::type_str(type);
        auto typeLbl = new Label(fd.ui.cnstWin, typeStr.c_str() );
        fd.ui.cnstLbls.emplace_back(typeLbl);
        //fd.ui.cnstLay->setAnchor(typeLbl, AdvancedGridLayout::Anchor(col,2, Alignment::Fill, Alignment::Fill) );
      }
    }
  }

  SECTION(add close button set position make visible)
  {
    fd.ui.cnstLbls.emplace_back( new Label(fd.ui.cnstWin, "") );  // spacer label
    fd.ui.cnstClose = new Button(fd.ui.cnstWin, "Close");
    fd.ui.cnstClose->setCallback([](){
      fd.ui.cnstWin->setVisible(false);
      clearTblEditor();
    });
    fd.ui.cnstLbls.emplace_back( new Label(fd.ui.cnstWin, "") );

    //auto spcr3 = new Label(fd.ui.cnstWin, "");
    //auto spcr4 = new Label(fd.ui.cnstWin, " ");
    
    //lay->setAnchor(fd.ui.cnstClose,
    //               AdvancedGridLayout::Anchor(0,1, Alignment::Fill, Alignment::Fill) );
    //lay->setColStretch(1, 1.f);

    fd.ui.cnstLbls.emplace_back( new Label(fd.ui.cnstWin, "") );  // extra space at the bottom
    fd.ui.cnstLbls.emplace_back( new Label(fd.ui.cnstWin, "") );
    fd.ui.cnstLbls.emplace_back( new Label(fd.ui.cnstWin, "") );

    fd.ui.cnstWin->setPosition(Vector2i(0,200));
    fd.ui.cnstWin->performLayout(fd.vg);
    fd.ui.cnstWin->setVisible(true);
  }

  return true;
}
void    clearNodeInstWin()
{
  fd.ui.ndinstWin->setVisible(false);
  for(auto& b : fd.ui.ndinstBtns){
    fd.ui.ndinstWin->removeChild(b);
  }
  fd.ui.ndinstBtns.clear();                                             // delete interface buttons from the nanogui window

  //fd.ui.ndinstWin->removeChild(fd.ui.ndinstClose);
}
void     makeNodeInstWin()
{
  using namespace std;

  clearNodeInstWin();

  SECTION(redo interface node buttons)
  {
    vec<LavaNode*> lns;
    SECTION(put LavaNode pointers into a vector and sort them by type and name)
    {
      lns.reserve(fd.flow.flow.size());
      for(auto& kv : fd.flow.flow)
        lns.push_back(kv.second);

      sort(ALL(lns), [](LavaNode* a, LavaNode* b){
        if(a->node_type==b->node_type)
          return str(a->name) < str(b->name);
        return a->node_type < b->node_type;
      });
    }

    TO(lns.size(),i)
    {
      LavaNode*    ln = lns[i];
      auto      ndBtn = new Button(fd.ui.ndinstWin, ln->name);
      auto        clr = fd.ui.nd_color;
      switch(ln->node_type){
        case LavaNode::CONSTANT:   clr = fd.ui.nd_const_clr;  break;
        case LavaNode::GENERATOR:  clr = fd.ui.nd_gen_clr;    break;
        case LavaNode::MSG:        clr = fd.ui.nd_gather_clr; break;
        case LavaNode::FLOW:
      default: break;
      }
      ndBtn->setBackgroundColor(nvgToNGUI(clr));

      ndBtn->setCallback([ln](){ 
        v2 stPos = {fd.ui.w/2.f,  fd.ui.h/2.f};                                                // stPos is starting position
        node_add(ln->name, Node("", (Node::Type)((u64)ln->node_type), stPos) );
      });
      fd.ui.ndinstBtns.push_back(ndBtn);
    }
  }
  SECTION(finish with positioning, visibility, then layout)
  {
    auto btnPos = fd.ui.ndinstBtn->position();
    fd.ui.ndinstWin->setPosition( btnPos + Vector2i(-200,50));
    fd.ui.ndinstWin->setVisible(true);
    fd.ui.ndinstWin->performLayout(fd.vg);
    fd.ui.screen.performLayout();
  }

  //SECTION(make the close button at the bottom)
  //{
  //  fd.ui.ndinstClose = new Button(fd.ui.ndinstWin, "Close");
  //  fd.ui.ndinstClose->setCallback([](){
  //    fd.ui.ndinstWin->setVisible(false);
  //    //clearNodeInstWin();
  //  });
  //}
}
void  refreshNodeButtons()
{
  //SECTION(get the buttons out of the GUI and clear the button widgets from memory)
  //{
  //  for(auto& b : fd.ui.ndinstBtns){
  //    fd.ui.ndinstWin->removeChild(b);
  //  }
  //  fd.ui.ndinstBtns.clear();                                             // delete interface buttons from the nanogui window
  //}

  bool wasVisible = fd.ui.ndinstWin->visible();

  clearNodeInstWin();

  SECTION(redo interface node buttons)
  {
    vec<LavaNode*> lns;
    lns.reserve(fd.flow.flow.size());
    for(auto& kv : fd.flow.flow)
      lns.push_back(kv.second);

    sort(ALL(lns), [](LavaNode* a, LavaNode* b){
      if(a->node_type==b->node_type)
        return str(a->name) < str(b->name);
      return a->node_type < b->node_type;
    });

    TO(lns.size(),i)
    {
      LavaNode*    ln = lns[i];
      auto      ndBtn = new Button(fd.ui.ndinstWin, ln->name);
      auto        clr = fd.ui.nd_color;
      switch(ln->node_type){
        case LavaNode::CONSTANT:   clr = fd.ui.nd_const_clr;  break;
        case LavaNode::GENERATOR:  clr = fd.ui.nd_gen_clr;    break;
        case LavaNode::MSG:        clr = fd.ui.nd_gather_clr; break;
        case LavaNode::FLOW:
        default: break;
      }
      nanogui::Color nclr;
      TO(3,i){ nclr[i] = clr.rgba[i]; }
      nclr[3] = 1.f;

      ndBtn->setBackgroundColor(nclr);
      ndBtn->setCallback([ln](){ 
        v2 stPos = {fd.ui.w/2.f,  fd.ui.h/2.f};                                                // stPos is starting position
        node_add(ln->name, Node("", (Node::Type)((u64)ln->node_type), stPos) );
      });
      fd.ui.ndinstBtns.push_back(ndBtn);
    }
  }
  fd.ui.screen.performLayout();

  fd.ui.ndinstWin->setVisible(wasVisible);
}
bool    reloadSharedLibs(bool force=false)
{
  bool newlibs  = RefreshFlowLibs(fd.flow, force);
  //newlibs      |= RefreshFlowConsts(fd.flow);
  if(force){ RefreshFlowConsts(fd.flow); }                   // todo: redo this so that consts are handles more elegantly

  if(!newlibs){ return false; }

  refreshNodeButtons();

  return true;
}
// End UI Util

void               keyCallback(GLFWwindow* win,    int key, int scancode, int action, int modbits)
{
  bool used = fd.ui.screen.keyCallbackEvent(key, scancode, action, modbits);

  if(used) return;

  if(action==GLFW_RELEASE) return;

  switch(key){
  case 'E':
  case 'e':{
    makeTblEditor(nullptr);
  }break;  
  case 'H':
  case 'h':{
    nvgTransformIdentity( fd.ui.tfm );
    nvgTransformIdentity( fd.ui.invTfm );
    fd.ui.grphTx  =  1.0f;       // grphTx and grphTy are graph transform X and Y - these will determine how zoomed out or zoomed in the graph window is
    fd.ui.grphTy  =  1.0f;
    fd.ui.grphCx  =  0.0f;       // grphCx and grphCy are graph center X and Y 
    fd.ui.grphCy  =  0.0f;
  }break;
  case 'J':{
  }break;
  case 'K':{
  }break;
  case 'L':{
    reloadSharedLibs();
  }break;
  case 'Y':{
  }break;
  case GLFW_KEY_BACKSPACE:
  case GLFW_KEY_DELETE: {
    //fd.lgrph.exec(); // todo: might have to deal with the returned argument vector here
    //fd.lgrph.exec(); // todo: might have to deal with the returned argument vector 

    sel_delete();
    sel_clear();

    //LavaGraph::ArgVec av = fd.lgrph.exec();
    //graph_apply(move(av));
  }break;
  default:
    ;
  }
}
void          mouseBtnCallback(GLFWwindow* window, int button, int action, int mods)
{
  bool used = fd.ui.screen.mouseButtonCallbackEvent(button, action, mods);
  if(used){ return; }

  if(button==GLFW_MOUSE_BUTTON_LEFT){
    if(action==GLFW_PRESS){ fd.mouse.lftDn = true; }
    else if(action==GLFW_RELEASE){ fd.mouse.lftDn = false; }
  }

  if(button==GLFW_MOUSE_BUTTON_RIGHT){
    if(action==GLFW_PRESS){ fd.mouse.rtDn = true; }
    else if(action==GLFW_RELEASE){ fd.mouse.rtDn = false; }
  }

  if(button==GLFW_MOUSE_BUTTON_MIDDLE){
    if(action==GLFW_PRESS){ fd.mouse.midDn = true; }
    else if(action==GLFW_RELEASE){ fd.mouse.midDn = false; }
  }
}
void             errorCallback(int e, const char *d) {
  printf("Error %d: %s\n", e, d);
  fflush(stdout);
}
void            scrollCallback(GLFWwindow* window, double xofst, double yofst)
{
  using namespace std;
  bool used = fd.ui.screen.scrollCallbackEvent(xofst, yofst);
  if(used){ return; }

  //f32    scale2   =  (f32)(1.f  +  yofst /  25.f);
  f32 scrlSens   =  fd.ui.zoomSensitivity * 25.f; // * 10000.f;
  f32   scale2   =  (f32)(1.f  +  (yofst * scrlSens) );
  fd.ui.grphTx  *=  scale2;
  fd.ui.grphTy  *=  scale2;
}
void         cursorPosCallback(GLFWwindow* window, double x, double y)
{
  const static float _2PI = 2.f* PIf; //  pi<float>();
  
  bool used = fd.ui.screen.cursorPosCallbackEvent(x,y);
  if( !used ){
    fd.mouse.pos.x = (f32)x;
    fd.mouse.pos.y = (f32)y;
  }
}
void              charCallback(GLFWwindow* window, unsigned int codepoint)
{
  fd.ui.screen.charCallbackEvent(codepoint);
}
void              dropCallback(GLFWwindow* window, int count, const char** filenames)
{
  bool   used = fd.ui.screen.dropCallbackEvent(count, filenames);

  if(count>0){
    bool ok = loadFile(filenames[0], &fd.lgrph);
    Println(filenames[0], " loaded ",  ok? "successfully"  :  "unsuccessfully");
  }
}
void   framebufferSizeCallback(GLFWwindow* window, int w, int h)
{
  fd.ui.screen.resizeCallbackEvent(w, h);

  //fd.ui.grphCx  +=  ((f32)wdif / (f32)fd.ui.w) * .5f;
  //fd.ui.grphCy  +=  ((f32)hdif / (f32)fd.ui.h) * .5f;

  //f32 wdif = (f32)(w - fd.ui.w); 
  //f32 hdif = (f32)(h - fd.ui.h);
  //f32 difAvg = ((wdif/fd.ui.w)+(hdif/fd.ui.h)) / 2.f;
  //fd.ui.grphTx  *=  1.f + difAvg;
  //fd.ui.grphTy  *=  1.f + difAvg;
}

// Fis DB interaction
str                   genDbKey(LavaId      sid)            // genDbKey is generate database key, sid is slot Id
{
  auto ni = fd.graph.nds.find(sid.nid); // todo: this is called from the lava looping threads and would need to be thread safe - it is also only used for getting text labels, which may make things easier
  if(ni == end(fd.graph.nds)) return ""; 

  auto label  =  toString("[",sid.nid,"-",sid.sidx,"]",ni->second.txt);

  return label;
}
str                getSlotType(LavaId      sid)
{
  using namespace std;

  if(sid.sidx == LavaId::SLOT_NONE){ return ""; }
  auto ni = fd.graph.nds.find(sid.nid);
  if(ni == end(fd.graph.nds)){ return ""; }

  str type;
  LavaNode* ln = ni->second.ln;
  if(!ln || !ln->out_types){ return ""; }

  auto  typeLst = ln->out_types;
  for(int i=0; *typeLst && i<sid.sidx; ){
    ++typeLst; ++i;
  }

  if(*typeLst){ type = str( *typeLst ); }

  return type;
}
LavaControl        cb_lavaPckt(LavaPacket* pkt)
{
  LavaId  srcid;
  LavaId destid;
  if(pkt)
  {
    srcid  = LavaId(pkt->src_node,  pkt->src_slot,  false);
    destid = LavaId(pkt->dest_node, pkt->dest_slot,  true);
    if( fd.vizIds.has(srcid.asInt) ){
      auto label  =  genDbKey(srcid);
      if(pkt->val.value!=0 && pkt->val.type==LavaArgType::MEMORY){
        auto  lm  =  LavaMem::fromDataAddr(pkt->val.value);
        bool  ok  =  fisdb.put(label.data(), (u32)label.size(), lm.data(), (u32)lm.sizeBytes() );
      }
    }else if( fd.vizIds.has(destid.asInt) ){
      auto label  =  genDbKey(destid);
      if(pkt->val.value!=0 && pkt->val.type==LavaArgType::MEMORY){
        auto  lm  =  LavaMem::fromDataAddr(pkt->val.value);
        bool  ok  =  fisdb.put(label.data(), (u32)label.size(), lm.data(), (u32)lm.sizeBytes() );
      }
    }
  }

  if(fd.step){
    fd.stepIds.del(srcid.asInt);                    // should just need source ids since inputs/destination slots can't be visualized
    fd.stepIds.del(destid.asInt);
    if(fd.stepIds.count() == 0){
      fd.step = false;
      return LavaControl::STOP;
    }
  }

  return LavaControl::GO;
}
LavaNode*           sidToConst(LavaId      sid)
{
  using namespace fs;
  
  str  key = genDbKey(sid);
  if(key==""){ return nullptr; }

  auto dat = fisdb.get<u8>(key);
  if(dat.size()==0){ return nullptr; }

  str  type = getSlotType(sid);
  str   dir = path(GetSharedLibPath()).remove_filename().generic_string();
  str   pth = dir + "/" + key + "." + type + ".const";
  bool   ok = writeFile(pth, dat.data(), dat.size());
  if(!ok){ return nullptr; }
  
  LavaNode* ln = AddFlowConst(pth, fd.flow);
  Node n;
  n.txt = ln->name;
  node_bnd(fd.vg, n);
  //n.P   = fd.mouse.pos - n.b.wh()/2;
  n.P   = fd.mouse.drgWrld - n.b.wh()/2;
  node_add(ln->name, n);

  refreshNodeButtons();

  return ln;
}
// End Fis DB interaction

void              debug_coords(v2 a)
{
  char  winTitle[TITLE_MAX_LEN];  // does glfw copy this string or just use the pointer? looks like it converts to a different character format which copies it / transforms it

  sprintf(winTitle, "%.2f  %.2f", a.x, a.y);
  glfwSetWindowTitle(fd.win, winTitle);
}

} // end namespace

void PrintAB(LavaQ<int>& q, str label="")
{
  //TO(q.capA(),i) sA += toString(q.atA(i)," ");
  //while(i < q.capA()){
  //
  //TO(q.capB(),i) sB += toString(q.atB(i)," ");
  //while(i < q.capB()){

  str sA = "A:  ";
  int i=0;
  auto  buf = q.m_buf;
  auto capA =  buf.useA? LavaQ<int>::Capacity(buf.cap) : LavaQ<int>::Capacity(buf.cap-1);
  auto capB = !buf.useA? LavaQ<int>::Capacity(buf.cap) : LavaQ<int>::Capacity(buf.cap-1);

  if(q.m_memA.addr() != nullptr)
    while(i < capA ){
      sA += toString(q.atA(i)," ");
      ++i;
    }

  str sB = "\nB:  ";
  i=0;
  if(q.m_memB.addr() != nullptr)
    while(i < capB){
      sB += toString(q.atB(i)," ");
      ++i;
    }

  Println(label,":\n",sA,"\n",sB,"\n");
}

ENTRY_DECLARATION // main or winmain
{
  using namespace std;
  using namespace fs;
  
  NVGcontext*& vg = fd.vg;
  SECTION(initialization)
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
      glfwWindowHint(GLFW_SAMPLES, 16);
      //glfwWindowHint(GLFW_DECORATED, GL_FALSE);

      //fd.win = glfwCreateWindow(1536, 1024, "Fissure", NULL, NULL);        // assert(win!=nullptr);
      fd.win = glfwCreateWindow(fd.ui.w, fd.ui.h, "Fissure", NULL, NULL);        // assert(win!=nullptr);
      glfwSwapBuffers(fd.win);
      glfwMakeContextCurrent(fd.win);
      glfwSwapBuffers(fd.win);

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

      glfwSwapInterval(1); // turning this to 1 clamps the programs to the vertical sync rate and slows down the interactivity, though setting it to 0 will use up a full core as the main loop will spin infinitely 
      
      glfwPollEvents();
      glViewport(0, 0, fd.ui.w, fd.ui.h);
      glClearColor(.075f, .075f, .075f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
      glfwSwapBuffers(fd.win);
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

      SECTION(create Tbl Editor window)
      {
        fd.ui.cnstWin = new Window(&fd.ui.screen, "Tbl Editor");
        //fd.ui.cnstWin->setWidth(600);
        //fd.ui.cnstWin->setFixedWidth(400);

        fd.ui.cnstLay = new GridLayout(Orientation::Horizontal, 3, Alignment::Fill, 10,10);
        //fd.ui.cnstLay = new AdvancedGridLayout({1}, {}, 10);
        //fd.ui.cnstLay->setColStretch(0, 1.f);
        //fd.ui.cnstLay->setRowStretch(0, 1.f);
        //fd.ui.cnstLay->setColStretch(1, 1.f);
        fd.ui.cnstWin->setLayout(fd.ui.cnstLay);
        fd.ui.cnstWin->setVisible(false);
      }
      SECTION(create Node Buttons window)
      {
        fd.ui.ndinstWin = new Window(&fd.ui.screen, "Create Node Instance");
        fd.ui.ndinstLay = new GridLayout(Orientation::Horizontal, 5, Alignment::Fill, 10,10);
        //fd.ui.ndinstLay = new AdvancedGridLayout({1,3}, {0}, 10);
        fd.ui.ndinstWin->setLayout(fd.ui.ndinstLay);
        fd.ui.ndinstWin->setVisible(false);
      }

      fd.ui.keyLay    = new BoxLayout(Orientation::Horizontal, Alignment::Fill, 0,10);      //fd.ui.keyLay   = new BoxLayout(Orientation::Vertical, Alignment::Fill, 0,10);
      fd.ui.keyWin    = new  Window(&fd.ui.screen,    "");
      auto spcr1      = new   Label(fd.ui.keyWin,     "");
      auto spcr2      = new   Label(fd.ui.keyWin,     "");
      auto loadBtn    = new  Button(fd.ui.keyWin,      "Load");
      auto saveBtn    = new  Button(fd.ui.keyWin,      "Save");
      auto& stepBtn   = fd.ui.stepBtn  = new  Button(fd.ui.keyWin,   "Step  ||>");
      auto& playBtn   = fd.ui.playBtn  = new  Button(fd.ui.keyWin,     "Play  >");
      auto& pauseBtn  = fd.ui.pauseBtn = new  Button(fd.ui.keyWin,   "Pause  ||");
      auto& stopBtn   = fd.ui.stopBtn  = new  Button(fd.ui.keyWin,   "Stop  |_|");
      fd.ui.ndinstBtn = new  Button(fd.ui.keyWin,   "Node List");
      auto cnstBtn    = new  Button(fd.ui.keyWin,  "Create Constant");
      auto nodeBtn    = new  Button(fd.ui.keyWin,  "Create Node");
      auto thrdsLabel = new   Label(fd.ui.keyWin,  ""); //toString(fd.threadCount) );
      auto thrdsSldr  = new  Slider(fd.ui.keyWin);
      auto nodeTxt    = fd.ui.nodeTxt = new TextBox(fd.ui.keyWin,  "");

      SECTION(load and save callbacks)
      {
        loadBtn->setCallback([](){ 
          stopFlowThreads();

          nfdchar_t  *inPath = NULL;
          nfdresult_t result = NFD_OpenDialog("lava", NULL, &inPath );

          if(inPath){
            bool ok = loadFile(inPath, &fd.lgrph);
            if(ok) printf("\nFile loaded from %s\n", inPath);
            else   printf("\nLoad did not read successfully from %s\n", inPath);
          }
        });
        saveBtn->setCallback([](){
          //fd.grph.normalizeIndices();

          nfdchar_t *outPath = NULL;
          nfdresult_t result = NFD_SaveDialog("lava", NULL, &outPath );
          //printf("\n\nfile dialog: %d %s \n\n", result, outPath);
          if(outPath){
            //stopFlowThreads();
            normalizeIndices(); // todo: put this into the save function
            bool ok = saveFile(fd.lgrph, outPath);
            if(ok) printf("\nFile Written to %s\n", outPath);
            else   printf("\nSave did not write successfully to %s\n", outPath);
          }
        });
      }
      SECTION(set up new node and node name changing controls)
      {
        cnstBtn->setCallback([](){

        });
        nodeBtn->setCallback([nodeTxt]() // creates a new node shared library using templates
        {
          //Println("nodeTxt: ", nodeTxt->value());

          regex whiteSpace("[ |\t]+");
          regex whiteSpaceTrail("[ |\t]+$");

          str nodeName = nodeTxt->value();
          nodeName = regex_replace( nodeName, whiteSpaceTrail, ""); 
          nodeName = regex_replace( nodeName, whiteSpace, "_"); 

          nodeTxt->setValue( nodeName );

          str     nodeDir = "../" + nodeName;
          create_directory(nodeDir);
          str      cppPth = nodeDir+"/"+nodeName+".cpp";
          str   vcprojPth = nodeDir+"/"+nodeName+".vcxproj";

          FILE*    cppHndl = fopen(cppPth.c_str(),    "wb");
          if(!cppHndl) return;

          FILE* vcprojHndl = fopen(vcprojPth.c_str(), "wb");
          if(!vcprojHndl) return;

          str    cppFile = regex_replace( Template_cpp,     regex("\\|_NAME_\\|"), nodeName);
          str vcprojFile = regex_replace( Template_vcxproj, regex("\\|_NAME_\\|"), nodeName);

          fwrite(cppFile.c_str(),     1, cppFile.length(),        cppHndl);                   // don't want to write the '\0' null character at the end of the string into the file
          fwrite(vcprojFile.c_str(),  1, vcprojFile.length(),  vcprojHndl);

          fclose(cppHndl);
          fclose(vcprojHndl);

          nodeTxt->setValue("");
        });

        nodeTxt->setEditable(true);
        nodeTxt->setFixedWidth(150);
        nodeTxt->setAlignment(TextBox::Alignment::Left);
        nodeTxt->setTooltip("The name of the created node");
        nodeTxt->setCallback( [](str const& s){
          auto& n = fd.graph.nds[ fd.sel.pri ];
          n.txt   = s;
          n.b     = node_bnd(fd.vg, n);
          return true;
        });
      }
      SECTION(init flow control button colors and callbacks)
      {
        playBtn->setBackgroundColor(  Color(e3f(.15f, .25f,  .15f)) ); 
        playBtn->setTextColor( Color(e3f(1.f, 1.f, 1.f)) );

        stepBtn->setBackgroundColor(  Color(e3f(.2f, .25f,  .2f)) ); 
        stepBtn->setTextColor( Color(e3f(1.f, 1.f, 1.f)) );
        
        pauseBtn->setBackgroundColor( Color(e3f(.2f,  .2f,  .15f)) ); 
        pauseBtn->setEnabled(false);
        
        stopBtn->setBackgroundColor(  Color(e3f(.19f, .16f, .17f)) ); 
        stopBtn->setEnabled(false);

        playBtn->setCallback([playBtn,pauseBtn,stopBtn](){
          fd.step = false;
          startFlowThreads( fd.threadCount );
        });
        pauseBtn->setCallback([playBtn,pauseBtn,stopBtn](){
          stopFlowThreads();
          playBtn->setEnabled(true);
          pauseBtn->setEnabled(false);
          stopBtn->setEnabled(false);
        });
        stopBtn->setCallback([playBtn,pauseBtn,stopBtn]()
        {
          stopFlowThreads();
          fd.flow.m_curId.nid = LavaNode::NODE_ERROR;
          auto         nInsts = fd.lgrph.nodes();
          for(auto const& n : nInsts){
            fd.lgrph.setState(n.id.nid, LavaInst::NORMAL);
          }
          fd.flow.m_qLck.lock();
            if(fd.flow.q.size() > 0){
              auto& pckt = fd.flow.q.top();
              fd.graph.qPacketBytes = 0;
              fd.graph.qPacketBytes += pckt.sz_bytes;
              //fd.flow.q.pop();
            }
          fd.flow.m_qLck.unlock();
          fd.flow.runDestructors(false);
          fd.flow.runConstructors();
        });
        stepBtn->setCallback([](){
          step(fd.threadCount);
        });
      }
      SECTION(init thread slider and thread label)
      {
        auto hardThreads = thread::hardware_concurrency();

        thrdsSldr->setFixedWidth(100);
        thrdsSldr->setRange( {1.f, (f32)hardThreads} );
        thrdsSldr->setCallback( [thrdsLabel, thrdsSldr](f32 i){
            fd.threadCount = (i32)i;
            thrdsSldr->setValue( (f32)fd.threadCount );
            
            auto    thrdStr = toString(fd.threadCount);
            char thrdLbl[4] = {'0','0','0','0'};
            //char thrdLbl[4] = {' ',' ',' ',' '};
            thrdLbl[3]      = '\0';
            auto         st = 3 - thrdStr.size();
            TO(thrdStr.length(),i){
              thrdLbl[st+i] = thrdStr[i];
            }
            thrdsLabel->setCaption( thrdLbl );
        });
        thrdsSldr->setValue(1.f);

        thrdsSldr->setTooltip("Threads");
        thrdsLabel->setTooltip("Threads");
      }

      SECTION(node instance window button)
      {
        fd.ui.ndinstClose = new Button(fd.ui.ndinstWin, "Close");
        fd.ui.ndinstClose->setCallback([](){
          fd.ui.ndinstWin->setVisible(false);
          //clearNodeInstWin();
        });

        fd.ui.ndinstBtn->setBackgroundColor( nvgToNGUI(fd.ui.nd_color) );
        fd.ui.ndinstBtn->setCallback([](){
          makeNodeInstWin();
        });
      }
      SECTION(console button)
      {
        fd.ui.cnslBtn = new Button(fd.ui.keyWin, "Console");
        fd.ui.cnslBtn->setCallback([](){
          #ifdef _WIN32
            AllocConsole();
            freopen("CONOUT$", "w", stdout);
            //stdout = GetStdHandle(STD_OUTPUT_HANDLE);
          #endif
        });
      }
      fd.ui.keyWin->setLayout(fd.ui.keyLay);

      SECTION(set up theme for the window)
      {
        Theme* thm = fd.ui.keyWin->theme();
        thm->mButtonFontSize      = 16;
        thm->mTransparent         = e4f( .0f,  .0f,  .0f,    0.0f   );
        thm->mWindowFillUnfocused = e4f( .2f,  .2f,  .225f,  0.97f  );
        thm->mWindowFillFocused   = e4f( .3f,  .28f, .275f,  1.0f   );
      }
      SECTION(set up the status bar at the bottom of the screen)
      {
        fd.ui.statusWin = new  Window(&fd.ui.screen,    "");
        fd.ui.statusTxt = new TextBox(fd.ui.statusWin,  "");
        fd.ui.statusTxt->setEditable(false);
        fd.ui.statusTxt->setDefaultValue("");
        fd.ui.statusTxt->setValue("");
        fd.ui.statusLay = new BoxLayout(Orientation::Horizontal, Alignment::Fill, 0,0);
        fd.ui.statusWin->setLayout(fd.ui.statusLay);
      }

      fd.ui.screen.setVisible(true);
      fd.ui.screen.performLayout();

      thrdsSldr->callback()((f32)fd.threadCount);

      glfwPollEvents();
      fd.ui.screen.drawContents();
      fd.ui.screen.drawWidgets();
      glfwSwapBuffers(fd.win);
    }
    SECTION(init nanovg and font)
    {
      vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	    if(vg == NULL) {
        printf("Could not init nanovg.\n");
		    return -1;
	    }
      int font = nvgCreateFontMem(vg, "sans-bold", Roboto_Bold_ttf, Roboto_Bold_ttf_len, 0);
      if(font == -1) {
		    printf("Could not add font bold.\n");
		    return -1;
	    }

      SECTION(draw the grid and nanogui for more immediate window loading feedback)
      {
        glfwPollEvents();

        SECTION(draw grid for the first time)
        {
          f32 cx = fd.ui.grphCx * fd.ui.w;
          f32 cy = fd.ui.grphCy * fd.ui.h;
          nvgBeginFrame(vg,  fd.ui.w,   fd.ui.h, 1.f);
            nvgResetTransform(vg);
            nvgTranslate(vg,    fd.ui.w/2.f,      fd.ui.h/2.f);
            nvgScale(vg,       fd.ui.grphTx,     fd.ui.grphTy);
            nvgTranslate(vg,   -fd.ui.w/2.f,     -fd.ui.h/2.f);
            nvgTranslate(vg,             cx,               cy);

            grid_draw(vg, fd.graph.gridSpace);
          nvgEndFrame(vg);
        }

        fd.ui.screen.drawContents();
        fd.ui.screen.drawWidgets();

        glfwSwapBuffers(fd.win);
      }
    }
    SECTION(test data init)
    {
    }
    SECTION(lava and db)
    {
      reloadSharedLibs(true);                                          // force reloading here because it is the first time the shared libs are loaded

      new (&fisdb)     simdb("Fissure", 4096, 1<<16);                  // 4096 * 65,536 = 268,435,456

      //printdb(fisdb);

      new (&fd.vizIds) AtmSet( LavaId().asInt );
      fd.flow.packetCallback = cb_lavaPckt;
      LavaInit();

      glfwPollEvents();
      glfwSwapBuffers(fd.win);
    }
  }
  
  glfwSetTime(0);
  SECTION(main loop)
  {
    auto&       g = fd.lgrph;
    f64 t, dt, avgFps=60, prevt=0, cpuTime=0;
    f32 pxRatio;
    int fbWidth, fbHeight;

    while(!glfwWindowShouldClose(fd.win))
    {
      auto&   ms = fd.mouse;
      ms.prevPos = ms.pos;

      if(fd.flow.m_threadCount == 0)
        stopFlowThreads();

      SECTION(poll events through glfw and apply commands to the graph)
      {
        glfwPollEvents();                                                    // PollEvents must be done after zeroing out the deltas
        LavaGraph::ArgVec av = fd.lgrph.exec();
        graph_apply(move(av));
      }

      auto      nds = node_getPtrs();
      auto       sz = nds.size();
      bool isInNode = false;
      u64      nIdx = LavaId::NODE_NONE;
      v2       pntr;
      LavaId    nid;                                           // has a default constructor

      SECTION(input that isnt from glfw events)
      {
        SECTION(time)
        {
          t     = glfwGetTime();
		      dt    = t - prevt;
		      prevt = t;
        }
        SECTION(input and pointer transform)
        {
          fd.ui.prevPntr = pntr;
          nvgTransformPoint(&pntr.x, &pntr.y, fd.ui.invTfm, ms.pos.x, ms.pos.y);

		      glfwGetWindowSize(fd.win, &fd.ui.w, &fd.ui.h);
		      glfwGetFramebufferSize(fd.win, &fbWidth, &fbHeight);

          pxRatio = (f32)fbWidth / (f32)fd.ui.w;          // Calculate pixel ration for hi-dpi devices.
        }
        SECTION(get lava graph visualization data)
        {        
          if(fd.flow.m_running ){ // && fd.flow.m_threadCount>0 
            fd.ui.stopBtn->setEnabled(true);
            fd.ui.stopBtn->setBackgroundColor( Color(e3f(.75f, .1f, .1f)) );

            fd.graph.curNode  =  fd.flow.m_curId.nid;          // todo: make atomic, although this may just work since it is only reading 8 bytes

            auto& slts = fd.graph.packetSlots;
            slts.clear();
            fd.flow.m_qLck.lock();
              if(fd.flow.q.size() > 0){
                auto& pckt = fd.flow.q.top();
                fd.graph.qPacketBytes = 0;
                fd.graph.qPacketBytes += pckt.sz_bytes;
                slts.emplace( pckt.dest_node, pckt.dest_slot );
                slts.emplace( pckt.src_node, pckt.src_slot );
              }
            fd.flow.m_qLck.unlock();
          }
          else
          {
            fd.graph.packetSlots.clear();
            fd.graph.qPacketBytes = 0;
          }
        }
        SECTION(reload and step)
        {
          fd.libReloadRem += dt;
          if(fd.libReloadRem > fd.libReloadTime){
            fd.libReloadRem -= fd.libReloadTime;
            if(reloadSharedLibs()){
              step(fd.threadCount);
            }              
          }
        }
      }
      SECTION(gl frame setup)
      {
        // can this be moved to the start of the drawing loop?
        glViewport(0, 0, fbWidth, fbHeight);
        glClearColor(.075f, .075f, .075f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
      }
      SECTION(selection)
      {
        // figure out all the information that needs to be known before figuring what to do with it
        bool lftClkDn  =   ms.lftDn && !ms.prevLftDn;      // lftClkDn is left click down
        bool lftClkUp  =  !ms.lftDn &&  ms.prevLftDn;      // lftClkDn is left click up
        bool rtClkDn   =   ms.rtDn  && !ms.prevRtDn;       // rtClkDn is right click down
        bool rtClkUp   =  !ms.rtDn  &&  ms.prevRtDn;       // rtClkDn is right click up
        bool midClkDn  =   ms.midDn && !ms.prevMidDn;      // midClkDn is middle click down
        bool midClkUp  =  !ms.midDn &&  ms.prevMidDn;      // midClkDn is middle click up

        LavaId   sid;                                      // sid is slot id
        bool isInSlot = false;
        SECTION(slot inside check: if inside a slot, early exit on the first found) // todo: does this need to loop through in the node ordr ? 
        {
          for(auto& kv : fd.graph.inSlots){
            Slot&    s  =  kv.second;
            isInSlot    =  len(pntr - s.P) < fd.ui.slot_rad;
            if(isInSlot){ 
              sid = kv.first;
              break;
            }
          }
          
          if(!isInSlot) for(auto& kv : fd.graph.outSlots){
            Slot&    s  =  kv.second;
            isInSlot    =  len(pntr - s.P) < fd.ui.slot_rad;
            if(isInSlot){ 
              sid = kv.first;
              if(midClkDn && ms.drgSlot.asInt==LavaNode::NODE_ERROR){ 
                fd.mouse.drgSlot = sid;
                fd.mouse.drgSlotP = s.P;
              }
              break;
            }
          }
        }

        if(!isInSlot) SECTION(box selection to primary and node inside check: if inside a node in node ordr from top to bottom)
        {
          SECTION(handle slot dragging)
          {
            if(!fd.mouse.midDn){
              if(fd.mouse.drgSlot.asInt != LavaNode::NODE_ERROR){
                auto drgSlt = fd.mouse.drgSlot;
                Println("slot dragged: ", drgSlt.nid, " ", drgSlt.sidx);
                // if the pointer is not inside a slot and the mid mouse button is up and the drag slot is not null, do constant writing stuff here
                sidToConst(drgSlt);
                //if(ok){ AddFlowConst( fd.flow  }
              }
              fd.mouse.drgSlot = LavaNode::NODE_ERROR;        // false;  // todo: change this to an ID
            }
          }

          SECTION(check what nodes the pointer is inside and fill in the primary selection with selected nodes if it is blank)
          {
            FROM(sz,i)                                                          // loop backwards so that the top nodes are dealt with first
            {
              Node* n = nds[i];

              if(lftClkUp && fd.sel.pri==LavaNode::NODE_ERROR && n->sel)  // todo: break this out of the !isInSlot section
                fd.sel.pri = n->id;

              isInNode     =  isIn(pntr.x,pntr.y, n->b);
              if(isInNode){
                nid  = n->id;
                nIdx = i;
                break;
              }
            }
          }
        }

        bool    slotClk  =  isInSlot && lftClkDn;
        bool    nodeClk  =  isInNode && lftClkDn;
        bool  slotRtClk  =  isInSlot &&  rtClkDn;
        // end per loop data

        SECTION(mouse ui state)
        {
          if(!ms.rtDn){ 
            ms.rtDragging = false;
          }
          ms.rtDragging  |=  rtClkDn;                            // only change from false to true - already true will stay true

          if(ms.rtDragging){ ms.drgbox = false; }

          if(!ms.lftDn){ 
            ms.dragging = false;
            ms.drgbox   = false;
          }

          ms.dragging  |=  lftClkDn;                             // only change from false to true - already true will stay true
          ms.drgbox    |=  !slotClk && !nodeClk && lftClkDn;     // only change from false to true - already true will stay true


          //if(!ms.dragging){ ms.drgP = pntr; }                    // if not dragging, the drag pointer and pointer are the same
          if(!ms.dragging){ ms.drgP = ms.pos; }                    // if not dragging, the drag pointer and pointer are the same

          if(ms.drgbox){
            //ms.drgbnd = Bnd( min(ms.drgP.x, pntr.x),
            //  min(ms.drgP.y, pntr.y),
            //  max(ms.drgP.x, pntr.x),
            //  max(ms.drgP.y, pntr.y) );
            ms.drgbnd = Bnd( min(ms.drgP.x, ms.pos.x),
              min(ms.drgP.y, ms.pos.y),
              max(ms.drgP.x, ms.pos.x),
              max(ms.drgP.y, ms.pos.y) );
          }else{
            ms.drgbnd = Bnd();
          }
        }
        SECTION(select src and dest slots)
        {          
          if(slotClk){
            Slot* s = slot_get(sid);
            if(s->in) fd.sel.slotInSel  = sid;
            else      fd.sel.slotOutSel = sid;
            s->state = Slot::SELECTED;
          }
        }
        SECTION(node primary select and move to top)
        {
          if(nodeClk)
          {
            Node* n     =  &(node_moveToFront(nid.nid));
            nds         =  node_getPtrs();                    // move to the front will invalidate some pointers in the nds array so it needs to be remade
            fd.sel.pri  =  n->id;
            ms.drgP     =  ms.pos; //pntr;

            fd.ui.nodeTxt->setValue( n->txt );
          }
        }
        SECTION(select from drg box)
        {
          if(ms.drgbox) TO(sz,i)
          {
            Bnd tfmBnd;
            nvgTransformPoint( &tfmBnd.mn.x, &tfmBnd.mn.y, fd.ui.invTfm, ms.drgbnd.mn.x, ms.drgbnd.mn.y );
            nvgTransformPoint( &tfmBnd.mx.x, &tfmBnd.mx.y, fd.ui.invTfm, ms.drgbnd.mx.x, ms.drgbnd.mx.y );

            Node& n       =  *(nds[i]);
            bool inside   =  isIn(n.b, tfmBnd);
            n.sel        |=  inside;
          }
        }
        SECTION(connection creation)
        {
          if(fd.sel.slotInSel.sidx  != LavaId::SLOT_NONE && 
             fd.sel.slotOutSel.sidx != LavaId::SLOT_NONE)
          {
            LavaCommand::Arg dest;
            LavaCommand::Arg  src;
            dest.id = fd.sel.slotInSel;
            src.id  = fd.sel.slotOutSel;
            fd.lgrph.put(LavaCommand::TGL_CNCT, dest, src);
            fd.sel.slotOutSel = fd.sel.slotInSel = LavaId(0,0);

            sel_clearSlots();
          }
        }
        SECTION(selection clearing)
        {
           if(lftClkDn && !slotClk && !nodeClk){
             sel_clear();
             fd.ui.statusTxt->setValue("");
             fd.ui.nodeTxt->setValue("");
           }
        }
        SECTION(slot visualization output simdb writing)
        {
          if(slotRtClk)
          {
            auto vizSid = sid;
            if( slot_get(sid)->in ){
              auto&   cncts = g.cncts();
              vizSid = cncts[sid];
            }

            if( fd.vizIds.has(vizSid.asInt) ){
              fd.vizIds.del(vizSid.asInt);
              //fisdb.del( genDbKey(vizSid) );
            }else{
              fd.vizIds.put( vizSid.asInt );
            }
          }
        }
        SECTION(nanogui status bar text)
        {
          str    status = "";
          f64 totalTime = timeToSeconds(fd.lgrph.totalTime());
          if(isInSlot)
          {
            str slotName; 
            str slotType;
            SECTION(figure out slot name)
            {
              LavaInst&   li  =  fd.lgrph.node(sid.nid);
              LavaNode*    n  =  li.node;
              if(sid.isIn){
                if(n->in_names) slotName = n->in_names[sid.sidx];
                if(n->in_types) slotType = n->in_types[sid.sidx];
              }else{
                if(n->out_names) slotName = n->out_names[sid.sidx];
                if(n->out_types) slotType = n->out_types[sid.sidx];
              }
            }
            status = toString("Slot [",sid.nid,":",sid.sidx,"]    ",slotName,"  <",slotType,">");
            fd.ui.statusTxt->setValue( status );
          }else if(nid.nid == LavaId::NODE_NONE){
            fd.ui.statusTxt->setValue( toString("Queued Packets: ",fd.graph.qPacketBytes," bytes") );
          }else if(slotRtClk){
            fd.ui.statusTxt->setValue( toString(" right click on slot ") );
          }else if(isInNode){
            status = makeStatusText(nid.nid, totalTime, nds, nIdx);
            fd.ui.statusTxt->setValue( status );
          }else if(fd.sel.pri != LavaNode::NODE_ERROR){
            fd.ui.statusTxt->setValue( status );
          }else{
            fd.ui.statusTxt->setValue("");
          }
        }
      }
      SECTION(movement)
      {
        // SetCursorPos(200, 200);
        
        SECTION(node movement)
        {
          TO(sz,i)
          {
            Node&     n = *(nds[i]);
            bool selctd = n.id==fd.sel.pri || n.sel;

            if(ms.dragging && fd.sel.pri!=LavaNode::NODE_ERROR && selctd ){           // if a node is primary selected (left mouse down on a node) or the selected flag is set
              n.P +=  pntr - fd.ui.prevPntr;
            }

            n.b = node_bnd(vg, n);
          }
        }
        SECTION(slot movement)
        {
          auto sltThick = (fd.ui.slot_rad + fd.ui.slot_border) * 2;

          SECTION(input / dest slots)
          {
            for(auto& kv : fd.graph.inSlots)
            {
              LavaId    nid = kv.first;
              Slot&       s = kv.second;
              Node const& n = fd.graph.nds[nid.nid];
              v2 wh = n.b.wh();
              v2 nP = n.P + wh/2;                                 //NODE_SZ/2; // n.b.mx; // w()/2; // NODE_SZ/2;
              v2 nrml;

              LavaFlowSlot* src = g.srcSlot(nid);
              if(src){
                auto srcIter = fd.graph.outSlots.find(src->id);
                if(srcIter != fd.graph.outSlots.end() ){
                  auto  srcNdP = fd.graph.nds[src->id.nid].P;
                  s.P = node_border(n, srcNdP - nP, &nrml);
                  s.N = nrml;
                }else{
                  s.P = node_border(n, {0,-1.f}, &nrml);
                  s.N = {0,-1.f};
                }
              }else{
                s.P = node_border(n, {0,-1.f}, &nrml);
                s.N = {0,-1.f};
              }
            }
          }
          SECTION(output / src slots)
          {
            for(auto& kv : fd.graph.outSlots)
            {
              LavaId    nid = kv.first;
              Slot&       s = kv.second;
              Node const& n = fd.graph.nds[nid.nid];
              v2 wh = n.b.wh();
              v2 nP = n.P + wh/2;                                      //NODE_SZ/2; // n.b.mx; // w()/2; // NODE_SZ/2;
              v2 nrml;

              auto cnctEn = fd.lgrph.destCnctEnd();
              auto     ci = fd.lgrph.destSlots(kv.first);              // queries the destination / input connection map for possible connections

              if(ci==cnctEn){
                f32 sltOfst = nid.sidx * sltThick;                         // sltOfst is slot offset - this is to make sure output slots with no connections don't overlap each other
                f32 slotRot = (-PIf/3.f) + ((PIf/3.f) * nid.sidx);         // slotRot is slot rotation
                auto    dir = v2( sinf(slotRot), cosf(slotRot) );
                s.P = node_border(n, dir, &nrml);
                s.N = nrml;
              }else{
                v2  destP={0,0}, destN={0,0};
                int   cnt = 0;
                for(; ci != cnctEn && ci->first==nid; ++ci)
                {
                  if(!fd.lgrph.outSlot(ci->second)){ cnt -= 1; continue; }   // todo: does this need to subtract 1 from count?

                  auto si = fd.graph.inSlots.find(ci->second);
                  if(si != fd.graph.inSlots.end()){
                    auto curP  =  si->second.P;
                    destP     +=  curP; 
                    destN     +=  norm(curP - nP);
                    ++cnt;
                  }
                }
                destP /= (f32)cnt;
                destN /= (f32)cnt;
                s.N = norm(destN);
                s.P = node_border(n, s.N);
              }
            }
          }
        }
        SECTION(node graph canvas movement)
        {
          //v2 pntrDif = pntr - fd.ui.prevPntr;
          v2 dif = ms.pos - ms.prevPos;
          if(ms.rtDragging){
            if(ms.dragging){
              f32 grphScale = (dif.x + dif.y) * fd.ui.zoomSensitivity; //0.0025f;
              fd.ui.grphTx *= 1.f + grphScale;
              fd.ui.grphTy *= 1.f + grphScale;
            }else{
              fd.ui.grphCx += (dif.x/fd.ui.w) / fd.ui.grphTx;
              fd.ui.grphCy += (dif.y/fd.ui.h) / fd.ui.grphTy;
            }
          }
        }
      }
      SECTION(drawing)
      {
        SECTION(nanovg drawing - |node graph|)
        {
          f32 scrnToWrld[6]={};
          f32     curTfm[6]={};
          f32 cx = fd.ui.grphCx * fd.ui.w;
          f32 cy = fd.ui.grphCy * fd.ui.h;
          ms.drgWrld = ms.pos;                                               // a drag slot position that will be transformed from the 'world space' to screen coordinates
          nvgBeginFrame(vg,  fd.ui.w,   fd.ui.h, pxRatio);
            nvgResetTransform(vg);
                        
            nvgTranslate(vg,    fd.ui.w/2.f,      fd.ui.h/2.f);
            nvgScale(vg,       fd.ui.grphTx,     fd.ui.grphTy);
            nvgTranslate(vg,   -fd.ui.w/2.f,     -fd.ui.h/2.f);
            nvgTranslate(vg,             cx,               cy);
            nvgCurrentTransform(vg,         curTfm);
            nvgTransformInverse(scrnToWrld, curTfm);

            grid_draw(vg, fd.graph.gridSpace);

            SECTION(draw a line when dragging a slot)
            {
              if(fd.mouse.drgSlot.asInt != LavaNode::NODE_ERROR)
              {
                nvgTransformPoint(&ms.drgWrld.x, &ms.drgWrld.y, scrnToWrld, ms.drgWrld.x, ms.drgWrld.y);

                nvgStrokeColor(vg, fd.ui.nd_const_clr);
                nvgStrokeWidth(vg, 4.f);
                nvgBeginPath(vg);
                  nvgMoveTo(vg, ms.drgSlotP.x, ms.drgSlotP.y);
                  nvgSave(vg);
                  nvgResetTransform(vg);
                    nvgLineTo(vg, ms.pos.x,      ms.pos.y);
                  nvgRestore(vg);
                nvgStroke(vg);
              }
            }
            SECTION(current node highlights)
            {
              auto nid = fd.graph.curNode;
              if(nid != LavaNode::NONE  &&
                 fd.graph.nds.count(nid) > 0)
              {
                auto const& n = fd.graph.nds[nid];
                node_draw_radial(n, vg, nvgRGBA(0,255,128,48));
              }
            }
            SECTION(highlights behind visualized slots)
            {
              TO(fd.vizIds.sz,i)
              {
                LavaId id;
                id.asInt  = fd.vizIds.load(i);
                Slot*   s = nullptr;
                if(id.asInt == fd.vizIds.null_val){ continue; }
                s = slot_get(id);
                if(!s){ continue; }

                draw_radial(vg, nvgRGBA(0,128,228,255), 
                  s->P.x, s->P.y, fd.ui.slot_rad*5); 
              }
            }
            SECTION(draw outstanding packets as circle segments around slots)
            {
              nvgStrokeColor(vg, nvgRGBAf(0,0,0,1.f));
              nvgStrokeWidth(vg, 1.f);
              nvgFillColor(vg, nvgRGBAf(1.f,1.f,0,.75f) );

              SECTION(input / dest slots)
              {
                auto const& inSlots = fd.graph.inSlots;
                auto const& outSlots = fd.graph.outSlots;
                for(auto lid : fd.graph.packetSlots)
                {
                  SECTION(input / dest slots)
                  {
                    auto sIter = node_slotsIn(lid.nid);
                    for(; sIter!=end(inSlots) && sIter->first.nid==lid.nid; ++sIter){
                      auto sIdx = sIter->first;                        // todo: needs to be redone
                      if(lid.sidx != sIdx.sidx){ continue; }                  
                      Slot const& s = sIter->second;
                      nvgBeginPath(vg);                       // draw a circle larger than the slot circle, which will show up as a sort of halo when the slot is drawn over it in the next section
                        nvgCircle(vg, s.P.x, s.P.y, fd.ui.slot_rad+5.f);
                        nvgFill(vg);
                        nvgStroke(vg);
                      nvgStroke(vg);
                    }
                  }
                  SECTION(output / src slots)
                  {
                    auto sIter = node_slotsOut(lid.nid);
                    for(; sIter!=end(outSlots) && sIter->first.nid==lid.nid; ++sIter){
                      auto sIdx = sIter->first;                        // todo: needs to be redone
                      if(lid.sidx != sIdx.sidx){ continue; }                  
                      Slot const& s = sIter->second;
                      nvgBeginPath(vg);                       // draw a circle larger than the slot circle, which will show up as a sort of halo when the slot is drawn over it in the next section
                        nvgCircle(vg, s.P.x, s.P.y, fd.ui.slot_rad+5.f);
                        nvgFill(vg);
                        nvgStroke(vg);
                      nvgStroke(vg);
                    }
                  }
                }
              }

              //auto const& slots = fd.graph.slots;
              //for(auto lid : fd.graph.packetSlots)
              //{
              //  auto sIter = node_slots(lid.nid);
              //  for(; sIter!=end(slots) && sIter->first.nid==lid.nid; ++sIter)
              //  {
              //    auto sIdx = sIter->first;                        // todo: needs to be redone
              //    if(lid.sidx != sIdx.sidx){ continue; }
              //    
              //    Slot const& s = sIter->second;
              //
              //    // draw a circle larger than the slot circle, which will show up as a sort of halo when the slot is drawn over it in the next section
              //    nvgBeginPath(vg);
              //      nvgCircle(vg, s.P.x, s.P.y, fd.ui.slot_rad+5.f);
              //      nvgFill(vg);
              //      nvgStroke(vg);
              //    nvgStroke(vg);
              //  }
              //}
            }
            SECTION(draw connections)
            {
              auto di = g.srcCnctsMap().begin();                                    // di is destination iterator
              auto en = g.srcCnctsMap().end();
              while(di != en)
              {
                LavaId  srcIdx = di->first;
                LavaId destIdx = di->second;

                auto pcktSltIter = find( ALL(fd.graph.packetSlots), srcIdx);  // todo: this is a linear search, it could be a hash lookup
                if( pcktSltIter !=  end(fd.graph.packetSlots) ){
                  nvgStrokeWidth(vg, 4.f);
                  nvgStrokeColor(vg, nvgRGBAf(1.f, .85f, 0, 0.8f));
                }else{
                  nvgStrokeWidth(vg, 3.f);
                  nvgStrokeColor(vg, nvgRGBAf(.7f, 1.f, .9f, .5f));
                }

                u64  cnt = 0; v2 srcP(0,0); v2 srcN(0,0);
                auto const& si = fd.graph.outSlots.find(srcIdx);     // si is source iterator - iterates over output slots
                if(si != fd.graph.outSlots.end()){
                  cnt  = g.destCnctCount(srcIdx);
                  srcP = si->second.P;
                  srcN = si->second.N;
                }

                if(cnt==1)
                {
                  auto sltIter = fd.graph.inSlots.find(destIdx);
                  if(sltIter != fd.graph.inSlots.end()){
                    Slot const& dest = sltIter->second; // todo: check for end() here?
                    f32 w = fd.graph.nds[destIdx.nid].b.w();
                    cnct_draw(vg, srcP, dest.P, srcN, dest.N, w/2);
                  }
                  ++di;
                }
                else
                {
                  v2 avgP=srcP; v2 avgN={0,0}; u32 cnt=0;
                  auto avgIter=di;
                  for(; avgIter!=en && avgIter->first==srcIdx; ++avgIter )
                  {    
                    if(! slot_get(avgIter->second)){ continue; }

                    Slot const& dest = *(slot_get(avgIter->second));
                    avgP += dest.P;
                    avgP += srcP;
                    ++cnt;
                  }
                  avgP    /= (f32)(cnt*2+1);             // can't forget the first position for averaging - the src position - the avgP is weighted 1:1 with the srcP and all the destination positions combined
                  v2 midN  = norm(srcP - avgP);
                  f32   w  = fd.graph.nds[destIdx.nid].b.w();

                  cnct_draw(vg, srcP, avgP, srcN, midN, w/2);

                  for(auto dhIter=di; di!=en && di->first == srcIdx; ++di){        // dhIter is draw half iterator - this is where the the connections are drawn from the average position of all slots 
                    const v2 hlfsz = fd.ui.slot_rad/2.f;

                    Slot* dest = slot_get(di->second);

                    if(dest) cnct_draw(vg, avgP, dest->P, -1.f*midN, dest->N, w/2);
                  }

                  nvgBeginPath(vg);
                    nvgCircle(vg, avgP.x, avgP.y, 6.f); 
                    nvgFillColor(vg, nvgRGBAf(.18f, .18f, .18f, 1.f) );
                    nvgStrokeWidth(vg, 3.f);
                    nvgStrokeColor(vg, nvgRGBAf(0,0,0,1.f) );
                  nvgStroke(vg);
                  nvgFill(vg);
                }
              }
            }
            SECTION(draw nodes and slots)
            {
              TO(sz,i)
              {
                Node&     n = *(nds[i]);
                bool selctd = n.id==fd.sel.pri || n.sel;

                LavaInst::State state = fd.lgrph.getState(n.id);
                if(state == LavaInst::RUN_ERROR){
                  node_draw_radial(n, vg, nvgRGBA(255,48,0,255));
                }

                bool highlight = isInNode && nIdx==i;
                node_draw(vg, 0, n, 1.f, selctd, fd.ui.nd_border, highlight);

                SECTION(draw input / dest node slots)
                {
                  auto const& slots = fd.graph.inSlots;

                  nvgStrokeColor(vg, nvgRGBAf(0,0,0,1.f));
                  nvgStrokeWidth(vg, fd.ui.slot_border);

                  auto sIter = node_slotsIn(n.id);
                  for(; sIter!=end(slots) && sIter->first.nid==n.id; ++sIter)
                  {
                    auto     sIdx = sIter->first;                    // todo: needs to be redone
                    Slot const& s = sIter->second;
                    bool   inSlot = len(pntr - s.P) < fd.ui.slot_rad;

                    Slot::State drawState = Slot::NORMAL;
                    if(s.state==Slot::SELECTED) drawState = Slot::SELECTED;
                    else if(inSlot)             drawState = Slot::HIGHLIGHTED;
                    slot_draw(vg, s, drawState, fd.ui.slot_rad);
                  }
                }
                SECTION(draw output / src node slots)
                {
                  auto const& slots = fd.graph.outSlots;

                  nvgStrokeColor(vg, nvgRGBAf(0,0,0,1.f));
                  nvgStrokeWidth(vg, fd.ui.slot_border);

                  auto sIter = node_slotsOut(n.id);
                  for(; sIter!=end(slots) && sIter->first.nid==n.id; ++sIter)
                  {
                    auto     sIdx = sIter->first;                         // todo: needs to be redone
                    Slot const& s = sIter->second;
                    bool   inSlot = len(pntr - s.P) < fd.ui.slot_rad;

                    Slot::State drawState = Slot::NORMAL;
                    if(s.state==Slot::SELECTED) drawState = Slot::SELECTED;
                    else if(inSlot)             drawState = Slot::HIGHLIGHTED;
                    slot_draw(vg, s, drawState, fd.ui.slot_rad);
                  }
                }
              }
            }
            nvgResetTransform(vg);
            SECTION(draw selection box)
            {
              if(fd.mouse.lftDn && fd.sel.pri==LavaNode::NODE_ERROR)
              {
                nvgBeginPath(vg);
                  float x,y,w,h;
                  x = min(ms.drgP.x, ms.pos.x); 
                  y = min(ms.drgP.y, ms.pos.y); 
                  w = abs(ms.drgP.x - ms.pos.x);
                  h = abs(ms.drgP.y - ms.pos.y);
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
              nvgFontSize(vg, 12.0f);
              nvgFontFace(vg, "sans-bold");
              nvgTextAlign(vg,  NVG_ALIGN_LEFT);  // NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
              nvgFillColor(vg, nvgRGBA(255,255,255,255));
              nvgText(vg,  fd.ui.w - tb , 10, fpsStr, NULL);
            }
          nvgEndFrame(vg);
        }
        SECTION(nanogui)
        {
          fd.ui.screen.performLayout();

          e2i statusSz = fd.ui.statusWin->size();
          fd.ui.statusWin->setPosition( e2i(0, fd.ui.h - statusSz[1]) );
          fd.ui.statusWin->setSize( e2i(fd.ui.w, statusSz[1]) );
          fd.ui.statusTxt->setWidth( fd.ui.w - fd.ui.statusLay->margin() );

          fd.ui.screen.drawContents();
          fd.ui.screen.drawWidgets();
        }
      }

      fd.mouse.prevRtDn  = fd.mouse.rtDn;
      fd.mouse.prevLftDn = fd.mouse.lftDn;

      glfwSwapBuffers(fd.win);

      SECTION(frame rate limiting and thread sleeping)
      {
        if( glfwGetWindowAttrib(fd.win, GLFW_ICONIFIED) ){
          this_thread::sleep_for( 84ms );                              // 84 milliseconds is about 12 frames per second -  40 ms is about 25hz / 25fps, which should be more than enough if it is iconified (which should also mean minimized)
        }else if( !glfwGetWindowAttrib(fd.win, GLFW_FOCUSED) ){
          this_thread::sleep_for( 16ms );                              // 16 milliseconds is slightly more than 60hz - 7ms is about 144hz
        }else if(dt < 0.005){                                          // if the loop took less than a certain amount of seconds, yield the thread to reduce CPU usage
          this_thread::yield();
        }
      }
    }
  }
  SECTION(shutdown)
  {
    stopFlowThreads();
    nanogui::shutdown();
    glfwTerminate();
  }

  return 0;
}






//
//bool    rtClk = (ms.rtDn  && !ms.prevRtDn);  // todo: take this out

//auto    spcr1 = new Label(fd.ui.cnstWin, " ");
//auto    spcr2 = new Label(fd.ui.cnstWin, "");

//fd.ui.cnstLay->setAnchor(fd.ui.cnstStr, 
//   AdvancedGridLayout::Anchor(0, lay->rowCount()-1) );             // 4, 1 );       // Alignment::Minimum, Alignment::) );
//fd.ui.cnstLay->setColStretch(0,1.f);

//nanogui::Color nclr;
//TO(3,i){ nclr[i] = clr.rgba[i]; }
//nclr[3] = 1.f;
//ndBtn->setBackgroundColor(nclr);

//SECTION(get the buttons out of the GUI and clear the button widgets from memory)
//{
//}

//auto  sIter = node_slots(n.id);
//
//auto& slots = fd.graph.slots;
//auto  sIter = slots.find(sid); //slots.find(sid);
//for(; sIter!=end(slots) && sIter->first==sid; ++sIter){                  // if the slot is an input slot, highlight the outputs connected to it
//  if(sIter->second.in){
//  }
//}
