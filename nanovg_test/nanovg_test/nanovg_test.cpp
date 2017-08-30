
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
// -todo: make box selection stick - needed to check if the box was being dragged before drgBox gets set to false for the left mouse button not being down 
// -todo: make slots part of file writing 
// -todo: make slots part of file reading
// -todo: make node type part of file writing
// -todo: make node type part of file reading
// -todo: put node type into written file
// -todo: change m_inCncts to unordered_map instead of multi-map
// -todo: make a delete node function - will need to find the slots on the node and the connections on the slots, then delete the connection, then delete the slot then delete the node
// -todo: make 'delete' and 'backspace' delete selected nodes
// -todo: make 'delete' and 'backspace' delete selected connections
// -todo: add data to node for inputs - slots not done with per node data
// -todo: add data to connection for input and output indices - connections done with maps
// -todo: print to console with ReadFile.cpp function - not sure what this was about
// -todo: make a node Id map that maps ids on to the ordered node set
// -todo: change nodes to be a set or ordered set that counts from 1 on on every node creation
// -todo: make bnd part of Node struct
// -todo: take out bnd array
// -todo: figure out node creation in main loop - was deleting with new order, which didn't delete anything
// -todo: fix selection turning off on click and drag of node
// -todo: fix node movement and drawing 
// -todo: make main loop use an array of node pointers
// -todo: make bounds be properly set in main loop
// -todo: fix srcSlot() function to fix slot movement so that in/dest slot ends up on the node - actual problem was that the node id was being compared to nsz() so ever increasing node IDs quickly go out of the bounds of the size of the node structure
// -todo: test saving 
// -todo: test connection drawing 
// -todo: test multiple nodes
// -todo: make sure NodeSlotMap is a map from a node Id to a slot index and not from a node index to a slot index - if Slot::id is a node id then the NodeSlotMap will be a node.id -> slotIdx map
// -todo: debug file save not writing node four text and writing an empty string - needed to use the node pointer vector or iterate over nodes - using an index won't get all the nodes because they are looked up by ID now
// -todo: test loading - seems to work
// -todo: make selected a boolean in the Node struct since there is no linear ordering with the node being a map based on order with access based on an unordered_map based on id
// -todo: test node deletions again - slot deletion is problematic due to linear indexing
// -todo: investigate making slots into an unordered set - is this neccesary for proper slot deletion?  what data and access is needed ?
//       |- a slot struct has a node id associated with it - a node can have multiple slots - slots are going to be accessed by the node id they are attached to - slots can not exist without a node - a node.id to slot multi-map should hold all the slots?
//       |- how do connections access the slots? they have a slot index - should slots have a unique Id also? - if slots are deleted in the linear indexed vector, all their positions change and their indices are invalidated 
//       |- slots don't need to be ordered - need to be accessed by their own id and by their node id - if slots are always part of a particular node, should their id be a subset of the nodeID? should there only be one id between them? - should one struct hold a 48 bit integer and a 16 bit integer for slots?
//       |- when drawing slots, use a ranged lookup (lower_bound) to find all slots that have that the same node? 
//       |- lookup slots when drawing by a multi-map of slotId -> to nodeOrder - look up actual slots with an Id to slot unordered_map
// -todo: debug slots not drawing - needed lower bound instead of exact search for node id
// -todo: debug slot movement - needed to use a reference to the iterator so that the internal slot could be modified and wouldn't be copied
// -todo: redo clearSelected loop over slots
// -todo: put fps in the upper right corner
// -todo: try gui button layout horizontal across the top
// -todo: fix slot disappearing after normalization - find() with only a node id doesn't find the exact slot - need lower_bound through a call to nodeSlots()
// -todo: make saving normalize the node id numbers - make normalize function
// -todo: correct unconnected src slot drawing
// -todo: change global selection variables to be Id
// -todo: test and fix connection creation - crashes - NaNs fixed and infinite loop fixed
// -todo: test node deletion
// -todo: fix deletion of src node hanging connection
// -todo: fix crashing on deletion of node with src slot and connection - for loop was incrementing iterator after it had been invalidated by m_destCncts.erase()
// -todo: fix crash on node deletion when src node has 2 connections - deleting from destCncts multi-map only needs to happen once since it will match all the connections to that src slot
// -todo: fix crash on deletion of dest node when connected to a multi-connected node - delDestCnct needed to be redone after underlying data structure changes
// -todo: try multiple nodes and connections
// -todo: redo delNode() - delSelection works for now
// -todo: fix and put back yellow highlighting of slots that are selected - for loop needed to use auto& so that the slot state could be altered
// -todo: make click in blank space clear selections - needed to do more than just test drgbox boolean - made a hasLen() function of bound that makes sure there is positive length of the bounding box
// -todo: retest saving
// -todo: fix in/dest slots pointing the wrong way - negated normal in case of no connection
// -todo: fix slots not moving around borders - in/dest slot finding was done in the src case
// -todo: put back intermediate position with multiple connections
// -todo: fix node becoming deselected after drag - inAny was being combined with clicks
// -todo: make sure normalization also normalizes ordering
// -todo: fix connections disappearing on saving/normalization - need to use an unordered_map to associate Ids
// -todo: figure out why src slot stops pointing toward dest after normalization - m_destCncts loop was switching src and dest
// -todo: fix high numbers in connection saving - Id struct being cast to an integer? - yes, but it is intentional serialization
// -todo: make loading find the highest node id and set the current id of the GraphDB
// -todo: fix drag selection from clearing selections - put back check for drgbox == true to clear selections with addition of drgbox && lftClkUp
// -todo: clean comments to graveyard
// -todo: separate connection node ids and slot indices - have to do the same thing for slots?
// -todo: investigate crash on drawing after file load - possibly doesn't work due to javascript having double as a number limitation
// -todo: write node index to json file
// -todo: write order array with nodes - is the order in the file good enough - should be since add node will increment the order
// -todo: add idx to addSlot()
// -todo: figure out why slot types get flipped when loading file - addNode() needed newId to have false passed to it
// -todo: write slot indices into json file
// -todo: test saving and loading with normalization back on
// -todo: debug flashing connections - possibly due to numeric error handling - have to repeat first - can't repeat
// -todo: fix duplicate connections being created - delCnct needed to be redone after switching cncts and destCncts
// -todo: make clear_selections clear selected slots - clear_selections not triggering right, split out clearing slots into separate function
// -todo: draw bg grid
// -todo: make function to modularize drawing a bezier from one slot to another with normals
// -todo: make addSlot check for current slots to make its slot index sequential
// -todo: clean comments out of main functions
// -todo: profile - all time coming from nvg drawing functions
// -todo: test two slots - two connections can be made to one dest
// -todo: fix multiple connections to a single dest - made toggleCnct delete any connections to the dest Id
// -todo: group ui state variables together - priSel, secSel, slot selections
// -todo: fix warnings
// -todo: make io_rad into slot_rad in FisData
// -todo: put color and border size drawing constants into FisData as variables
// -todo: put mouse state and prev mouse state variable into FisData
// -todo: group global input state variables into FisData
// -todo: take out unused functions
// -todo: fix connections not getting deleted - toggle was doing delCnt twice instead of once and checking the return value
// -todo: put message node colors into FisData
// -todo: make selected color for message passing nodes
// -todo: take color argument out of node_draw
// -todo: make node size draw using node bnds
// -todo: put up screen shot on github
// -todo: try without libc - linking in opengl32.lib works for openGL symbols - some C++ like operator new and possibly vtables, vector constructor, basic_ostream 
// -todo: try a smaller font for buttons - works
// -todo: change slot movement to follow node bnds
// -todo: make nodes smaller 

// todo: try compiling nanogui into one file
// todo: change project to be named Fissure 
// todo: put slots on message passing node

// todo: make two nodes execute in order
// todo: make a node to read text from a file name 
// todo: make a node to split text into lines and scatter the result
// todo: make message node diameter dependant on text bounds
// todo: make flow node size dependant on text bounds
// todo: make unconnected slots not overlap
// todo: make one node snap to another node
// todo: make two snapped nodes group together and be dragged together
// todo: use scroll wheel and nanovg scale transforms to zoom in and out - will need to scale mouse pointer position as well to 'canvas' coordinates
// todo: make multiple slots avoid each other - might need to have discreet sections around a node for a slot to sit in
// todo: don't select a slot if it is under an existing node

// idea: separate finding node the pointer is inside from the action to take
// idea: make nodes snap to a grid
// idea: combine src and dest slots when writing json and just use a boolean to separate them 
// idea: have a panel or window that shows information about the selected node and the shared library it represents
// idea: make connections thicker when there is more data and brighter when there are more packets
// idea: build in focus as information separate from selection
// idea: draw message node slots as sliding angles
// idea: make connection delete and create trigger when 1 or more in/dest slots are selected and 1 out/src slot is connected
// idea: make a single file include all nanoui into one compilation unit
// idea: make msg loop that would deal with selections, clicks and drags? 
// idea: take out redudant node position + bnd information 
// idea: look into quantized gradients of nanovg
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

using Id = GraphDB::Id;

static FisData fd;

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
}
f32        normalToAngle(v2 N)
{
  return atan2(N.y, N.x);
}
v2         angleToNormal(f32 angle)
{
  return { cos(angle), sin(angle) };
}

// serialize to and from json - put in FisTfm.hpp file?
str           graphToStr(GraphDB const& g)
{
  using namespace std;
  
  Jzon::Node nds = Jzon::object();
  SECTION(nodes)
  {
    auto ndp = g.nodes();
    auto  sz = g.nsz();

    Jzon::Node  nd_id = Jzon::array();
    TO(sz,i) nd_id.add(ndp[i]->id);

    Jzon::Node  nd_txt = Jzon::array();
    TO(sz,i) nd_txt.add(ndp[i]->txt);

    Jzon::Node    nd_x = Jzon::array();
    TO(sz,i) nd_x.add(ndp[i]->P.x);

    Jzon::Node    nd_y = Jzon::array();
    TO(sz,i) nd_y.add(ndp[i]->P.y);

    Jzon::Node nd_type = Jzon::array();
    TO(sz,i) nd_type.add(ndp[i]->type);

    nds.add("id",     nd_id);
    nds.add("txt",   nd_txt);
    nds.add("x",       nd_x);
    nds.add("y",       nd_y);
    nds.add("type", nd_type);
  }
  Jzon::Node jslots = Jzon::object();
  SECTION(slots)
  {
    Jzon::Node srcId   = Jzon::array();
    Jzon::Node srcIdx  = Jzon::array();
    Jzon::Node destId  = Jzon::array();
    Jzon::Node destIdx = Jzon::array();

    for(auto kv : g.slots()){
      Id  sid = kv.first;
      auto& s = kv.second;
      if(s.in){
        destId.add(sid.id);
        destIdx.add(sid.idx);
      }else{
        srcId.add(sid.id);
        srcIdx.add(sid.idx);
      }
    }

    jslots.add("destId",   destId);
    jslots.add("destIdx", destIdx);
    jslots.add("srcId",     srcId);
    jslots.add("srcIdx",   srcIdx);
  }
  Jzon::Node jcncts = Jzon::object();
  SECTION(connections)
  {
    auto sz = g.cnctsz();

    Jzon::Node srcId   = Jzon::array();
    Jzon::Node srcIdx  = Jzon::array();
    Jzon::Node destId  = Jzon::array();
    Jzon::Node destIdx = Jzon::array();

    for(auto kv : g.cncts()){
      destId.add(kv.first.id);
      destIdx.add(kv.first.idx);
      srcId.add(kv.second.id);
      srcIdx.add(kv.second.idx);
    }

    jcncts.add("destId",   destId);
    jcncts.add("destIdx", destIdx);
    jcncts.add("srcId",     srcId);
    jcncts.add("srcIdx",   srcIdx);
  }
  
  Jzon::Node graph = Jzon::object();
  graph.add("nodes", nds);
  graph.add("slots", jslots);
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

  auto nd_id      = graph.get("nodes").get("id");
  auto nd_txt     = graph.get("nodes").get("txt");
  auto nd_x       = graph.get("nodes").get("x");
  auto nd_y       = graph.get("nodes").get("y");
  auto nd_type    = graph.get("nodes").get("type");
  auto ordr       = graph.get("nodes").get("order");
  auto destId     = graph.get("connections").get("destId");
  auto destIdx    = graph.get("connections").get("destIdx");
  auto srcId      = graph.get("connections").get("srcId");
  auto srcIdx     = graph.get("connections").get("srcIdx");
  auto sltDestId  = graph.get("slots").get("destId");
  auto sltDestIdx = graph.get("slots").get("destIdx");
  auto sltSrcId   = graph.get("slots").get("srcId");
  auto sltSrcIdx  = graph.get("slots").get("srcIdx");

  auto cnt = nd_id.getCount();
  TO(cnt,i){
    Node n;
    n.id   = nd_id.get(i).toInt();
    n.txt  = nd_txt.get(i).toString();
    n.P.x  = nd_x.get(i).toFloat();
    n.P.y  = nd_y.get(i).toFloat();
    n.type = (Node::Type)nd_type.get(i).toInt();
    g.addNode(n, false);
  }
  
  TO(sltSrcId.getCount(),i){
    Id sltId;
    sltId.id  = sltSrcId.get(i).toInt();
    sltId.idx = sltSrcIdx.get(i).toInt();
    Slot s(sltId.id,false);
    g.addSlot(s, sltId.idx);
  }
  TO(sltDestId.getCount(),i){
    Id sltId;
    sltId.id  = sltDestId.get(i).toInt();
    sltId.idx = sltDestIdx.get(i).toInt();
    Slot s(sltId.id,true);
    g.addSlot(s, sltId.idx);
  }

  auto cnct_cnt = destId.getCount();
  TO(cnct_cnt,i){
    Id src, dest;
    src.id   = srcId.get(i).toInt();
    src.idx  = srcIdx.get(i).toInt();
    dest.id  = destId.get(i).toInt();
    dest.idx = destIdx.get(i).toInt();
    //g.addCnct(src, dest);
    g.toggleCnct(src, dest);
  }

  g.setNextNodeId( g.maxId() );

  return move(g);
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
void    clear_selections(GraphDB* inout_grph, FisData* inout_fd)
{
  inout_grph->clearSels();
  inout_fd->sel.slotOutSel = Id(0,0);
  inout_fd->sel.slotInSel  = Id(0,0);
  inout_fd->sel.pri        =  -1;
  inout_fd->sel.sec        =  -1;
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
    #ifdef _WIN32
      HMODULE lib = LoadLibrary(TEXT("ReadFile.dll"));
      if(lib){
        auto   getNds = (GetLavaNodes_t)GetProcAddress(lib, TEXT("GetNodes") );
        LavaNode* nds = getNds();
        printf("%s    %s    %s", nds[0].name, nds[0].in_types[0], nds[0].out_types[0] );
        //while(nds && nds->name)
          //node_add( (nds++)->name );
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

  if(key==GLFW_KEY_BACKSPACE || key==GLFW_KEY_DELETE){
    fd.grph.delSelected();
  }

  fd.ui.screen.keyCallbackEvent(key, scancode, action, modbits);
}
void    mouseBtnCallback(GLFWwindow* window, int button, int action, int mods)
{
  if(button==GLFW_MOUSE_BUTTON_LEFT){
    if(action==GLFW_PRESS) fd.mouse.lftDn = true;
    else if(action==GLFW_RELEASE) fd.mouse.lftDn = false;
  }

  if(button==GLFW_MOUSE_BUTTON_RIGHT){
    if(action==GLFW_PRESS) fd.mouse.rtDn = true;
    else if(action==GLFW_RELEASE) fd.mouse.rtDn = false;
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
  const static float _2PI = 2.f* PIf; //  pi<float>();
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
                         float      rnd,      // rnd is corner rounding
                         f32     border=3.5f)
{
  const float   rthk = 8.f;    // rw is rail thickness

	NVGpaint bg;
	char icon[8];
  float tw=0, iw=0, x=n.P.x, y=n.P.y, w=n.b.w(), h=n.b.h();
	float rad = lerp(rnd, 0.f, h/2.f);           // rad is corner radius
  f32 cntrX = (n.P.x + n.b.xmx)/2;
  f32 cntrY = (n.P.y + n.b.ymx)/2; 
  f32 rr    = rad;      // rr is rail radius
  Bnd b;

  nvgResetTransform(vg);
  nvgGlobalAlpha(vg, 1.f);
  switch(n.type)
  {
  case Node::FLOW: {
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
        auto    col = n.sel? fd.ui.nd_selclr  : fd.ui.nd_color;
        int    grad = (int)lerp(rnd, 0, 48);
        auto topClr = nvgRGBA(255,255,255,isBlack(col)?16:grad);
        auto botClr = nvgRGBA(0,0,0,isBlack(col)?16:grad);
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
      b = {x,y, x+w, y+h};
    }
  } break;
  case Node::MSG: {
    SECTION(draw message node)
    {
      f32 msgRad = NODE_SZ.x / 2;

      nvgStrokeColor(vg, fd.ui.lineClr); // nvgRGBAf(.04f, .04f, .04f, 1.f));
      nvgStrokeWidth(vg, border);

      SECTION(linear gradient from upper left)
      {
        nvgBeginPath(vg);
        nvgCircle(vg, x+w/2,y+h/2, msgRad);
        auto lin = nvgLinearGradient(vg, 
          cntrX, cntrY-msgRad, x, y+msgRad,
          n.sel? fd.ui.msgnd_selclr : fd.ui.msgnd_gradst,
          n.sel? fd.ui.msgnd_selclr : fd.ui.msgnd_graden );

        //nvgRGBAf( .3f,   .3f,   .3f,  0.5f), 
        //nvgRGBAf( .15f,  .15f,  .15f,  .45f) );
        nvgFillPaint(vg, lin);
        nvgFill(vg);
      }
      SECTION(radial gradient)
      {
        nvgBeginPath(vg);
        nvgCircle(vg, cntrX, cntrY, msgRad);
        auto radial = nvgRadialGradient(vg,
          cntrX, cntrY, msgRad*.5f, msgRad,
          fd.ui.msgnd_gradst,
          fd.ui.msgnd_graden  );
        //nvgRGBAf( .15f, .15f,  .15f,   .95f ),
        //nvgRGBAf( .2f, .2f,    .2f,   1.f)  );
        nvgFillPaint(vg, radial);
        nvgFill(vg);
      }
      SECTION(circle outline)
      {
        nvgBeginPath(vg);
          nvgCircle(vg, cntrX, cntrY, msgRad);
        nvgStroke(vg);
      }

      b = {cntrX-msgRad/1.2f, cntrY-msgRad/1.2f, cntrX+msgRad/1.2f, cntrY+msgRad/1.2f};
    }
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
  f32 w=n.b.w(), h=n.b.h();
  v2       wh = {w,h};
  //v2      hlf = { n.b.xmx/2, n.b.ymx/2 };
  //v2      hlf = NODE_SZ/2;
  v2      hlf = wh / 2;
  v2       nP = n.P;
  v2 borderPt = {0,0};
  v2     ndir = norm(dir);
  v2    ncntr = nP + hlf;      // n.b.mx/2.f; // hlf; // n.mx/2.f; // NODE_HALF_SZ;

  switch(n.type)
  {
  case Node::FLOW: {
    //v2   hlf  =  NODE_HALF_SZ;
    f32  rad  =  hlf.y;
    v2  pdir  =  ndir;
    v2    ds  =  sign(pdir);                                  // ds is direction sign
    f32   ax  =  abs(pdir.x);
    if( ax > hlf.x ){
      pdir /= ax/hlf.x;                                    // can this never be 0, since ax is positive, hlf.x is positive, and ax is greater than hlf.x ? 
    }else{
      f32 ay = abs(pdir.y);
      pdir /= ay==0.f?  1.f  :  ay/hlf.y;
    }

    //v2  circCntr = (pdir.x<0)? nP+v2(rad,rad)  :  nP + NODE_SZ - v2(rad,rad);  // NODE_SZ
    v2  circCntr = (pdir.x<0)? nP+v2(rad,rad)  :  nP + wh - v2(rad,rad);
    v2   intrsct = lineCircleIntsct(ncntr, pdir, circCntr, rad);
    //bool     hit = !hasInf(intrsct)  &&  (intrsct.x < nP.x+rad || intrsct.x > nP.x + NODE_SZ.x - rad); 
    bool     hit = !hasInf(intrsct)  &&  (intrsct.x < nP.x+rad || intrsct.x > nP.x + wh.x - rad); 
    if(hit){ pdir = intrsct - ncntr; }

    borderPt = ncntr + pdir;

    if(out_nrml){
      if(hit) *out_nrml = norm(borderPt - circCntr);
      else    *out_nrml = { 0.f, sign(ndir).y };
    }
  } break;
  case Node::MSG: 
  default: 
  {
    //f32  rad = NODE_SZ.x/2;  // n.b.xmx/2.f;   // NODE_SZ.x/2;
    f32  rad = wh.x/2;  // n.b.xmx/2.f;   // NODE_SZ.x/2;
    borderPt = ncntr + ndir*rad; 
    if(out_nrml){ *out_nrml = ndir; }
  } break;
  }

  return borderPt;
}
void           slot_draw(NVGcontext* vg, Slot const& s, Slot::State drawState, f32 slot_rad=10.f)
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
void           draw_cnct(NVGcontext* vg, v2 srcP, v2 destP, v2 srcN, v2 destN, f32 minCenterDist=INFf)
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
    nvgStrokeWidth(vg, 3.f);
    nvgStrokeColor(vg, nvgRGBAf(.7f, 1.f, .9f, .5f));
  nvgStroke(vg);
}

void        debug_coords(v2 a)
{
  char  winTitle[TITLE_MAX_LEN];  // does glfw copy this string or just use the pointer? looks like it converts to a different character format which copies it / transforms it

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
      //io_rad = IORAD;
      fd.ui.slot_rad = 15.f;

      // nodes
      Node& n0 = fd.grph.addNode( Node("one",   Node::FLOW, {400.f,300.f}) );
      Node& n1 = fd.grph.addNode( Node("two",   Node::FLOW, {200.f,500.f}) );
      Node& n2 = fd.grph.addNode( Node("three", Node::FLOW, {700.f,500.f}) );
      Node& n3 = fd.grph.addNode( Node("four",  Node::FLOW, {700.f,700.f}) );

      // slots
      Id s0 = fd.grph.addSlot( Slot(n0.id, false) );
      Id s1 = fd.grph.addSlot( Slot(n1.id,  true) );
      Id s2 = fd.grph.addSlot( Slot(n2.id,  true) );
      Id s3 = fd.grph.addSlot( Slot(n3.id,  true) );
      Id s4 = fd.grph.addSlot( Slot(n0.id, false) );

      fd.grph.toggleCnct(s0, s1);
      fd.grph.toggleCnct(s0, s2);
      fd.grph.toggleCnct(s0, s3);
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
      //glfwWindowHint(GLFW_DECORATED, GL_FALSE);

      fd.win = glfwCreateWindow(1024, 768, "Fissure", NULL, NULL);        // assert(win!=nullptr);
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

      fd.ui.keyLay   = new BoxLayout(Orientation::Horizontal, Alignment::Fill, 0,10);      //fd.ui.keyLay   = new BoxLayout(Orientation::Vertical, Alignment::Fill, 0,10);
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
        //fd.grph.normalizeIndices();

        nfdchar_t *outPath = NULL;
        nfdresult_t result = NFD_SaveDialog("lava", NULL, &outPath );
        //printf("\n\nfile dialog: %d %s \n\n", result, outPath);
        if(outPath){
          fd.grph.normalizeIndices();
          bool ok = saveFile(fd.grph, outPath);
          if(ok) printf("\nFile Written to %s\n", outPath);
          else   printf("\nSave did not write successfully to %s\n", outPath);
        }
      });

      GraphDB* grphPtr = &fd.grph;
      flowBtn->setCallback([grphPtr](){
        grphPtr->addNode( Node("new node", Node::FLOW) );
      });
      msgBtn->setCallback([grphPtr](){
        grphPtr->addNode( Node("message node", Node::MSG) );
      });

      fd.ui.keyWin->setLayout(fd.ui.keyLay);

      Theme* thm = fd.ui.keyWin->theme();
      thm->mButtonFontSize      = 16;
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
      auto&    ms = fd.mouse;
      bool  rtClk = (ms.rtDn  && !ms.prevRtDn);
      auto    nds = grph.nodes();
      auto     sz = grph.nsz();

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

        fd.ui.prevPntr = pntr;
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
		    //if(premult)
			   // glClearColor(0,0,0,0);
		    //else
			    glClearColor(.075f, .075f, .075f, 1.0f);
		    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
      }
      SECTION(selection)
      {
        bool lftClkDn =  ms.lftDn && !ms.prevLftDn;    // lftClkDn is left click down
        bool lftClkUp = !ms.lftDn &&  ms.prevLftDn;    // lftClkDn is left click up
        bool clearSelections = true;
        Bnd  drgbnd;
        SECTION(selection box)
        {
          if(ms.drgbox || drgbnd.hasLen()){    // && lftClkUp
            clearSelections=false;
          }

          if(!ms.lftDn){ ms.drgP=pntr; ms.drgbox=false; }

          if(ms.drgbox)
            drgbnd = Bnd( min(ms.drgP.x, pntr.x),
                          min(ms.drgP.y, pntr.y),
                          max(ms.drgP.x, pntr.x),
                          max(ms.drgP.y, pntr.y) );
          else
            drgbnd = Bnd();
        }
        SECTION(select)
        {
          if(ms.drgbox){
            TO(sz,i){
              Node& n = *(nds[i]);
              n.sel   = isIn(n.b, drgbnd);
            }
            if(drgbnd.hasLen())
              clearSelections = false;
          }
        
          if(!fd.mouse.lftDn){
            fd.sel.pri = -1;
          }
        }
        SECTION(slot selection and connection creation)
        {
          bool inAnySlt = false;
          Id  inClk(0);
          Id outClk(0);
          for(auto& kv : grph.slots())
          {
            Id     sid = kv.first;                       // sid is slot id
            Slot&    s = kv.second;
            if(lftClkDn){  
              bool inSlt = len(pntr - s.P) < fd.ui.slot_rad; // io_rad;
              if(inSlt){
                outClk  = sid;  // Id(nid);
                s.state = Slot::SELECTED;
                if(s.in) fd.sel.slotInSel  = sid; // Id(nid, );
                else     fd.sel.slotOutSel = sid; // Id(nid, );
                clearSelections = false;
                inAnySlt = true;
              }
            }else if(lftClkUp){
              bool inSlt = len(pntr - s.P) < fd.ui.slot_rad;  // io_rad;
              if(inSlt){
                clearSelections = false;
                inAnySlt = true;
              }
              //else{
              //}
            }
          }

          if(!inAnySlt && lftClkUp){
            fd.sel.slotOutSel = fd.sel.slotInSel = Id(0,0);
            grph.clearSlotSels();
          }
          
          if(fd.sel.slotInSel.idx>0 && fd.sel.slotOutSel.idx>0){
            grph.toggleCnct(fd.sel.slotOutSel, fd.sel.slotInSel);
            fd.sel.slotOutSel = fd.sel.slotInSel = Id(0,0);
            clear_selections(&grph, &fd);
            clearSelections = false;
          }
        }
        SECTION(node selection)
        {
          bool inAny = false;
          FROM(sz,i)                                                // loop backwards so that the top nodes are dealt with first
          {
            Node*     n = nds[i];
            bool inNode = isIn(pntr.x,pntr.y, n->b);
            inAny      |= inNode;

            SECTION(primary selection and group selection effects)
            {
              if(inNode && lftClkDn && (fd.sel.pri<0 || fd.sel.pri!=n->id) ){
                n   = &(grph.moveToFront(n->id));
                nds = grph.nodes(); // move to the front will invalidate some pointers in the nds array so it needs to be remade
                
                fd.sel.pri = n->id;
                ms.drgP    = pntr;

                if(!n->sel){
                  TO(sz,j){ nds[j]->sel = false; }  // todo: move this out of the outer loop due to being O(n^2)
                  n->sel = true;
                }
                break;                              // without breaking from the loop, a node could be moved down and hit again
              }
            } 
          }

          if(!inAny){
            if( lftClkDn && !(fd.sel.pri>0) ){ ms.drgbox=true; }

            if(fd.mouse.rtDn && !fd.mouse.prevRtDn){ fd.sel.sec = -1; }
          }else{ // if(lftClkDn && lftClkUp){
            clearSelections=false;
          }
        }

        if(clearSelections && lftClkUp){ // !lftDn && prevLftDn){ 
          clear_selections(&grph, &fd);
        }
      }
      SECTION(movement)
      {
        SECTION(node movement)
        {
          TO(sz,i)
          {
            Node&     n = *(nds[i]);
            bool selctd = n.id==fd.sel.pri || n.sel;

            if( fd.sel.pri>-1 && selctd ){           // if a node is primary selected (left mouse down on a node) or the selected flag is set
              n.P +=  pntr - fd.ui.prevPntr;
            }
          }
        }
        SECTION(slot movement)
        {
          for(auto& kv : grph.slots())
          {
            Id     nid = kv.first; // s.nid;
            Slot&    s = kv.second;
            v2    nrml;
            Node const& n = grph.node(nid.id);
            v2 nP = n.P + NODE_SZ/2; // n.b.mx; // w()/2; // NODE_SZ/2;
              
            if(s.in)
            {                                                 // dest / in / blue slots
              Slot* src = grph.srcSlot(kv.first);
              if(src){
                auto srcNdP = grph.node(src->nid).P + NODE_SZ/2; // n.b.mx; // NODE_SZ/2;
                s.P = node_border(n, srcNdP - nP, &nrml);
                s.N = nrml;
              }else{
                s.P = node_border(n, {0,-1.f}, &nrml);
                s.N = {0,-1.f};
              }
            }else
            {
              auto ci = grph.destSlots(kv.first);
              if(ci==grph.destCnctEnd()){
                s.P = node_border(n, v2(0,1.f), &nrml);
                s.N = nrml;
              }else{
                v2  destP={0,0}, destN={0,0};
                int   cnt = 0;
                for(; ci != grph.destCnctEnd() && ci->first==nid; ++cnt, ++ci){
                  if(!grph.slot(ci->second)){ cnt -= 1; continue; }   // todo: does this need to subtract 1 from count?

                  v2 curP = grph.slot(ci->second)->P;
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
      SECTION(drawing)
      {
        SECTION(nanovg drawing)
        {
          nvgBeginFrame(vg, fd.ui.w, fd.ui.h, pxRatio);
          SECTION(draw background grid)
          {
            nvgStrokeWidth(vg, 1.f);
            nvgStrokeColor(vg, nvgRGBAf( .12f, .12f, .12f, 1.f));

            u32 lineCntX = (fd.ui.w / 100) + 1;
            f32 lineIncX  = fd.ui.w / (f32)lineCntX;
            f32    curX  = 0; 
            TO(lineCntX,i){
              nvgBeginPath(vg);
                nvgMoveTo(vg, curX, 0);
                nvgLineTo(vg, curX, (f32)fd.ui.h);
              nvgStroke(vg);

              curX += lineIncX;
            }

            u32 lineCntY = (fd.ui.h / 100) + 1;
            f32 lineIncY  = fd.ui.h / (f32)lineCntY;
            f32    curY  = 0; 
            TO(lineCntY,i){
              nvgBeginPath(vg);
              nvgMoveTo(vg, 0, curY);
              nvgLineTo(vg, (f32)fd.ui.w, curY);
              nvgStroke(vg);

              curY += lineIncY;
            }
          }
          SECTION(draw connections)
          {
            auto di = grph.srcCnctsMap().begin();                                    // di is destination iterator
            auto en = grph.srcCnctsMap().end();
            for(auto di = grph.srcCnctsMap().begin(); di != en; )
            {
              auto     srcIdx = di->first;
              auto    destIdx = di->second;
              Slot const& src = *(grph.slot(srcIdx));
              auto      count = grph.destCnctCount(srcIdx);

              if(count==1)
              {
                Slot const& dest = *(grph.slot(destIdx));
                draw_cnct(vg, src.P, dest.P, src.N, dest.N, NODE_SZ.x/2);

                ++di; //continue;
              }
              else
              {
                v2 avgP=src.P; v2 avgN={0,0}; u32 cnt=0;
                auto avgIter=di;
                for(; avgIter!=en && avgIter->first==srcIdx; ++avgIter ){     // ++cnt
                  if(!grph.slot(avgIter->second)){ continue; }

                  Slot const& dest = *(grph.slot(avgIter->second));
                  avgP += dest.P;
                  avgP += src.P;
                  ++cnt;
                }
                avgP    /= (f32)(cnt*2+1);             // can't forget the first position for averaging - the src position - the avgP is weighted 1:1 with the srcP and all the destination positions combined
                v2 midN  = norm(src.P - avgP);

                draw_cnct(vg, src.P, avgP, src.N, midN, NODE_SZ.x/2);

                for(auto dhIter=di; di!=en && di->first == srcIdx; ++di){   // dhIter is draw half iterator - this is where the the connections are drawn from the average position of all slots 
                  const v2 hlfsz = fd.ui.slot_rad/2.f;
                  Slot const& dest = *(grph.slot(di->second));

                  draw_cnct(vg, avgP, dest.P, -1.f*midN, dest.N, NODE_SZ.x/2);
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
          SECTION(draw nodes)
          {
            TO(sz,i)
            {
              Node&     n = *(nds[i]);
              bool selctd = n.id==fd.sel.pri || n.sel;

              //auto clr = fd.ui.nd_color;    // NODE_CLR;
              //if(selctd){ clr = fd.ui.nd_selclr; }  //nvgRGBf(.5f,.4f,.1f); }
              //
              //n.b = node_draw(vg, 0, n, clr, 1.f, fd.ui.nd_border);

              n.b = node_draw(vg, 0, n, 1.f, fd.ui.nd_border);

              SECTION(draw node slots)
              {
                nvgStrokeColor(vg, nvgRGBAf(0,0,0,1.f));
                nvgStrokeWidth(vg, fd.ui.slot_border);
                auto sIter = grph.nodeSlots(n.id);            // sIter is slot iterator
                for(; sIter!=grph.slotEnd() && sIter->first.id==n.id; ++sIter)
                {
                  auto     sIdx = sIter->first;                    // todo: needs to be redone
                  Slot const& s = *(grph.slot(sIdx));
                  bool   inSlot = len(pntr - s.P) < fd.ui.slot_rad; //io_rad;
                
                  Slot::State drawState = Slot::NORMAL;
                  if(s.state==Slot::SELECTED) drawState = Slot::SELECTED;
                  else if(inSlot)             drawState = Slot::HIGHLIGHTED;
                  slot_draw(vg, s, drawState, fd.ui.slot_rad);
                }
              }

            }
          }
          SECTION(draw selection box)
          {
            if(fd.mouse.lftDn && fd.sel.pri<0)
            {
              nvgBeginPath(vg);
                float x,y,w,h;
                x = min(ms.drgP.x, pntr.x); 
                y = min(ms.drgP.y, pntr.y); 
                w = abs(ms.drgP.x - pntr.x);
                h = abs(ms.drgP.y - pntr.y);
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
          fd.ui.screen.drawContents();
          fd.ui.screen.drawWidgets();
        }
      }

      fd.mouse.prevRtDn  =  fd.mouse.rtDn;
      fd.mouse.prevLftDn =  fd.mouse.lftDn;

      glfwSwapBuffers(fd.win);
      glfwPollEvents();
    }
  }

  nanogui::shutdown();
  glfwTerminate();
  return 0;
}







//f32 cntrX = (n.b.xmn+n.b.xmx)/2;
//f32 cntrY = (n.b.ymn+n.b.ymx)/2; 
//
//NVGcolor col,
//
//float tw=0, iw=0, x=n.P.x, y=n.P.y, w=NODE_SZ.x, h=NODE_SZ.y;
//float cntrX=x+w/2, cntrY=y+h/2, rr=rad;      // rr is rail radius

//fd.grph.addCnct(Id(1,1), Id(1,1));
//fd.grph.addCnct(Id(1,1), Id(2,1));
//fd.grph.addCnct(Id(1,1), Id(3,1));

//bool lftClk = (fd.mouse.lftDn && !fd.mouse.prevLftDn);
//bool    clk = lftClk || rtClk;

//nvgRGBAf( .15f, .15f,  .15f,   .95f ),
//nvgRGBAf( .2f, .2f,    .2f,   1.f)  );

//bool              insUnq(cnct_tbl* cnct, int a, int b)   // insUnq is insert unique - this inserts into a multi-set only if the combination of int key and int val does not already exist
//{
//  using namespace std;
//  
//  auto rnge = cnct->equal_range(a);
//  if(rnge.first != rnge.second){
//    for(auto i=rnge.first; i!=rnge.second; ++i){
//      if( i->second == b ) return false;
//    }
//  }
//  cnct->insert( {a, b} );
//
//  return true;
//}

//bool                 drgNd = false;
//v2                    drgP;
//v2                 drgofst;
//bool                drgbox = false;

//bool                  rtDn = false;    // right mouse button down
//bool                 lftDn = false;    // left mouse button down
//bool              prevRtDn = false;    // right mouse button down
//bool             prevLftDn = false;    // left mouse button down

//v2                prevPntr;
//

//char              winTitle[TITLE_MAX_LEN];
//
//int                premult = 0;
//
//Bnd                   nbnd;
//
//float              ndOfstX;
//float              ndOfstY;
//
//float                  ndx = 512.f;
//float                  ndy = 512.f;
//
//Id               slotInSel;
//Id              slotOutSel;
//int                 pri = -1;
//int                 sec = -1;
//f32                 io_rad;

//
//bool highlight=false, bool selected=false)

//px=(float)cx; py=(float)cy;
//prevX=px; prevY=py; 

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

//
//int font = nvgCreateFont(vg, "sans-bold", "Roboto-Bold.ttf");

//using namespace glm;
//
//FisData* fd = (FisData*)glfwGetWindowUserPointer(window);

//auto sz = nodes.size();
//
//TO(sz,i) nd_txt.add(g.node(i).txt);
//TO(sz,i) nd_txt.add(nodes[i].txt);
//
//TO(sz,i) nd_x.add(g.node(i).P.x);
//
//TO(sz,i) nd_y.add(g.node(i).P.y);
//
//TO(sz,i) nd_type.add(g.node(i).type);
//
//Jzon::Node ordr = Jzon::array();
//TO(sz,i) ordr.add(nd_ordr[i]);
//
//nds.add("order",  ordr);

//void            node_add(str txt, Node::Type type=Node::FLOW)
//{
//  //v2 P = v2(512,512);
//  Node n(txt, type);
//  n.P = v2(512,512);
//  //nodes.push_back(n);
//
//  //nodes.push_back( {P,txt} );
//  //nodes.back().type = type;
//
//  //sels.push_back(false);
//  //nbnds.emplace_back();
//
//  //nd_ordr.push_back( (int)(nodes.size()-1) );
//}

// draw rail
//void            drw_rail(NVGcontext* vg, v2 P, v2 nP)                     // drw_rail is draw_rail
//{
//  using namespace std;
//  
//  const f32    hPi = PIf/2;      // hPi is half pi 
//  const int border = 2;
//  const f32   rthk = 8.f;        // rthk is rail thickness
//
//  f32 rnd = 1.f;
//  f32 tw=0, iw=0, x=nP.x, y=nP.y, w=NODE_SZ.x, h=NODE_SZ.y;
//  v2   hlf = NODE_HALF_SZ;
//  f32 vMid = nP.y +  NODE_SZ.y/2;
//  f32  rad = hlf.y; 
//  f32   rr = NODE_SZ.y/2;          // rr is rail radius
//  f32 rlen = NODE_SZ.x/2;          // rlen is rail length
//  
//  v2 ndCntr = {nP.x + hlf.x, nP.y + hlf.y};
//
//  bool leftSide = P.x < ndCntr.x;
//  v2    lftCirc = { nP.x+rad, ndCntr.y };
//  v2    rgtCirc = { nP.x+NODE_SZ.x-rad, ndCntr.y };
//
//  bool inLeftCircle = P.x < (nP.x + rad);
//  bool  inRgtCircle = P.x > (nP.x+NODE_SZ.x - rad);
//  bool        onTop = P.y < ndCntr.y;
//  enum Place { BOTTOM=0, LEFT, TOP, RIGHT };
//  Place place = BOTTOM;
//  if(inLeftCircle)     place = LEFT;
//  else if(inRgtCircle) place = RIGHT;
//  else if(onTop)       place = TOP;
//
//  v2 dir;
//  if(inLeftCircle)      dir = norm(P-lftCirc);
//  else if(inRgtCircle)  dir = norm(P-rgtCirc);
//  else dir = norm(P-ndCntr);
//
//  f32 railRad, arcSt, lftDist, lftArc, lftOpp, lftArcSt=hPi, rgtDist, rgtArc, rgtOpp, rgtArcSt=hPi; 
//  railRad = rlen/2;
//
//
//  if(inLeftCircle)     arcSt = PIf - asin(dir.y);
//  else if(inRgtCircle) arcSt = asin(dir.y);
//
//  if(inLeftCircle) lftArcSt = PIf - asin(dir.y);
//  lftDist = (inLeftCircle || inRgtCircle)? 0 : max(0.f, min(railRad, P.x-(nP.x+rad)) );
//  lftArc  = min(hPi*3 - lftArcSt, (railRad-lftDist) / rad );          // maximum arc amount is half a circle, which is PI radians
//  if(inRgtCircle){
//    lftArc  = max(0.f, min(hPi - lftArcSt, (railRad-lftDist)/rad) );
//  }
//  lftOpp  = railRad - lftArc*rad - lftDist;                           // lftOpp is left opposite side segment length
//
//  if(inRgtCircle) rgtArcSt = asin(dir.y);
//  rgtDist = inRgtCircle? 0 : max(0.f, min(railRad, (nP.x+NODE_SZ.x-rad)-P.x) );
//  rgtArc  = min(rgtArcSt + hPi, (railRad-rgtDist)/rad );              // maximum arc amount is half a circle, which is PI radians
//  rgtOpp  = railRad - rgtArc*rad - rgtDist;                           // rgtOpp is right opposite side segment length
//
//
//  //if(inLeftCircle)     arcSt = PIf - asin(dir.y);
//  //else if(inRgtCircle) arcSt = asin(dir.y);
//
//  Bnd nb(nP.x, nP.y, nP.x+NODE_SZ.x, nP.y+NODE_SZ.y);
//  
//  // relative distances
//  f32 lDst = max(0.f, min(railRad, (P.x-(nb.xmn+rad))) );
//  f32 rDst = max(0.f, min(railRad, ((nb.xmx-rad)-P.x)) );
//  f32 lArc = (railRad-lDst) / rad;
//  f32 rArc = (railRad-rDst) / rad;
//  f32 lOpp = railRad - lDst - lArc*rad;
//  f32 rOpp = railRad - rDst - rArc*rad;
//
//  if(inLeftCircle){
//    f32 ang = P.y>vMid?  acos(P.x)  :  PIf+acos(P.x);
//    lArc = min(hPi*3.f - ang, railRad);
//    rArc = min(ang - hPi, railRad);
//  }
//
//  // absolute positions
//  //v2  lArcPt =  node_border(nP, dir, io_rad);
//
//  v2  lPt    = { P.x - lDst, P.y };
//  v2  rPt    = { P.x + rDst, P.y };
//  f32 lArcSt = hPi + lArc;
//  f32 lArcEn = hPi; 
//  f32 rArcSt = hPi;
//  f32 rArcEn = hPi - rArc;
//  v2  lOppPt = { nb.xmn + rad + lOpp , nb.ymn };
//  v2  rOppPt = { nb.xmx - rad - rOpp , nb.ymn };
//  v2  lArcPt = { acos(lArcSt), asin(lArcSt) };
//  v2  rArcPt = { acos(rArcSt), asin(rArcSt) };
//
//  if(inLeftCircle){
//    
//  }
//
//  //SECTION(drawing fourth attempt)
//  //{
//  //  // maybe the rounded rectangle should be broken down into one number that represents a position on the border
//  //  // then taking the number backwards and forwards would at least give the start and end points 
//  //
//  //
//  //  v2 orthDir = { dir.y, -dir.x };
//  //  v2 lBrdr = node_border(nP, orthDir, io_rad);
//  //
//  //  nvgStrokeWidth(vg, rthk);
//  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
//  //
//  //  nvgBeginPath(vg);
//  //    nvgMoveTo(vg, P.x, P.y);
//  //    nvgLineTo(vg, lBrdr.x, lBrdr.y);
//  //    //nvgRoundedRect(vg, nP.x, nP.y, NODE_SZ.x, NODE_SZ.y, NODE_SZ.y/2 );
//  //    //nvgRect(vg, nP.x, nP.y, NODE_SZ.x, NODE_SZ.y); 
//  //  nvgStroke(vg);
//  //}
//
//  //SECTION(drawing third attempt)
//  //{
//  //  nvgStrokeWidth(vg, rthk);
//  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
//  //
//  //  nvgBeginPath(vg);
//  //    if(lOpp>0){
//  //      nvgMoveTo(vg, lOppPt.x, lOppPt.y);
//  //    }else if(lArc>0){
//  //      nvgMoveTo(vg, lArcPt.x, lArcPt.y);
//  //      nvgArc(vg, lftCirc.x, lftCirc.y, rad, lArcSt, lArcEn, NVG_CCW);
//  //    }else{
//  //      nvgMoveTo(vg, lPt.x, lPt.y);
//  //    }
//  //    if(rDst>0) nvgLineTo(vg, rPt.x, rPt.y);
//  //
//  //    if(rArc>0){
//  //      nvgArc(vg, rgtCirc.x, rgtCirc.y, rad, rArcSt, rArcEn, NVG_CCW);
//  //    }
//  //    if(rOpp>0) nvgLineTo(vg, rOppPt.x, rOppPt.y);
//  //
//  //    //if(lArc>0) nvgArc(vg, lftCirc.x, lftCirc.y, rad, lArcSt, lArcEn, NVG_CCW);
//  //  nvgStroke(vg);  
//  //}
//
//  //SECTION(drawing second attempt)
//  //{
//  //  nvgStrokeWidth(vg, rthk);
//  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
//  //
//  //  nvgBeginPath(vg);  // left side
//  //    nvgMoveTo(vg, P.x, P.y);
//  //    //switch(place){      
//  //    if(onTop && !inLeftCircle)
//  //    { 
//  //      if(lftDist>0){ nvgLineTo(vg, P.x - lftDist, P.y); }
//  //      if(lftArc>0)   nvgArc(vg, lftCirc.x, lftCirc.y, rad, hPi*3, hPi*3 - lftArc, NVG_CCW);
//  //      if(lftOpp>0)   nvgLineTo(vg, nP.x+rad+lftOpp, nP.y+NODE_SZ.y);
//  //    }
//  //    else{
//  //      if(lftDist>0){ nvgLineTo(vg, P.x - lftDist, P.y); }
//  //      if(inLeftCircle){
//  //        if(lftArc>0) nvgArc(vg, lftCirc.x, lftCirc.y, rad, arcSt, arcSt + lftArc, NVG_CW);
//  //        if(lftOpp>0) nvgLineTo(vg, nP.x+rad+lftOpp, nP.y);
//  //      }else if(inRgtCircle){
//  //        if(lftArc>0) nvgArc(vg, rgtCirc.x, rgtCirc.y, rad, arcSt, arcSt + lftArc, NVG_CW);
//  //        if(lftOpp>0) nvgLineTo(vg, nP.x+rad+lftOpp, nP.y);
//  //      }else{
//  //        if(lftArc>0) nvgArc(vg, lftCirc.x, lftCirc.y, rad, hPi, hPi + lftArc, NVG_CW);
//  //        if(lftOpp>0) nvgLineTo(vg, nP.x+rad+lftOpp, nP.y);
//  //      }
//  //    }
//  //    //}
//  //  nvgStroke(vg);
//  //
//  //  nvgBeginPath(vg);  // left side
//  //  nvgMoveTo(vg, P.x, P.y);
//  //  //switch(place){      
//  //  if(onTop && !inRgtCircle)
//  //  { 
//  //    if(rgtDist>0){ nvgLineTo(vg, P.x + rgtDist, P.y); }
//  //    if(rgtArc>0)   nvgArc(vg, rgtCirc.x, rgtCirc.y, rad, -hPi, -hPi + rgtArc, NVG_CW);
//  //    if(rgtOpp>0)   nvgLineTo(vg, nP.x+rad+rgtOpp, nP.y+NODE_SZ.y);
//  //  }
//  //  else{
//  //    if(rgtDist>0){ nvgLineTo(vg, P.x + rgtDist, P.y); }
//  //    if(inRgtCircle){
//  //      if(rgtArc>0) nvgArc(vg, rgtCirc.x, rgtCirc.y, rad, arcSt, arcSt - rgtArc, NVG_CCW);
//  //    }else{
//  //      if(rgtArc>0) nvgArc(vg, rgtCirc.x, rgtCirc.y, rad, hPi, hPi - rgtArc, NVG_CCW);
//  //    }
//  //    if(rgtOpp>0) nvgLineTo(vg, nP.x+NODE_SZ.x-rad-rgtOpp, nP.y);
//  //  }
//  //  //}
//  //  nvgStroke(vg);
//  //}
//
//  //f32 rad = lerp(rnd, 0.f, h/2.f);                               // rad is corner radius
//  //f32 cntrX = x+border+rad, cntrY = y+border+h/2, 
//  //f32 rr=rad;        // rr is rail radius
//
//  //v2  hlf = NODE_HALF_SZ;
//  //v2 ndir = norm(nP - P);
//  //v2 pdir = ndir;
//  //v2   ds = sign(pdir);                                  // ds is direction sign
//  //f32  ax = abs(pdir.x);
//  //if( ax > hlf.x ){
//  //  pdir /= ax/hlf.x;                                    // can this never be 0, since ax is positive, hlf.x is positive, and ax is greater than hlf.x ? 
//  //}else{
//  //  f32 ay = abs(pdir.y);
//  //  pdir /= ay==0.f?  1.f  :  ay/hlf.y;
//  //}
//  //f32       r = hlf.y;
//  //v2 circCntr = (pdir.x<0)? nP+v2(r,r)  :  nP+NODE_SZ-r;
//
//  //v2 lftCirc;
//  //lftCirc.y = nP.y + hlf.y;
//  //lftCirc.x = leftSide? nP.x+rad  :  nP.x+NODE_SZ.x-rad;
//  //lftCirc.y = nP.y + hlf.y;
//
//  //f32      leftLine = P.x - rlen/2;
//  //f32 leftLineLimit = max(nP.x + rad, leftLine);
//  //f32     leftExtra = abs(leftLineLimit - leftLine);
//  ////f32       leftArc = inLeftCircle? (rlen/2)/rad  :  min(PIf*1.5f, leftExtra/rad);
//  //f32       leftArc = min(PIf*1.5f, leftExtra/rad);
//  //f32         arcSt = inLeftCircle? hPi + hPi-asin(dir.y) :  hPi;
//  //f32         arcEn = min(arcSt+leftArc, PIf*1.5f);
//  //f32       arcDist = (arcEn - arcSt) * rad;
//  //f32      leftWrap = max(0.f, leftExtra -  arcDist);
//
//  //st      = PIf - asin(dir.y);
//  //f32   lftSeg = P.x - lftDist;
//  //
//  //f32 leftLineLen = P.x - rlen/2 - leftLimit;  // nP.x + 2*rad rlen/2 - 
//  //
//  // black border
//  //f32 bthk = rthk+2;                        // bthk is black thickness 
//  //nvgBeginPath(vg);
//  //  nvgMoveTo(vg, x-bthk/2, P.y);
//  //  nvgArc(vg, P.x, P.y, rr, PIf*1.f, PIf*1.5f, NVG_CW);
//  //  nvgStrokeWidth(vg, bthk);
//  //nvgStrokeColor(vg, nvgRGBAf(0, 0, 0, 1.f) );
//  //nvgStroke(vg);
//  //
//  // inner color
//  //nvgBeginPath(vg);
//  //  nvgMoveTo(vg, x-rthk/2, P.y);
//  //  nvgArc(vg, P.x, P.y, rr, PIf*1.f, PIf*1.5f, NVG_CW);
//  //  nvgStrokeWidth(vg, rthk);
//  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
//  //nvgStroke(vg);
//  //
//  //v2 brdr = node_border(nP, P - ndCntr, io_rad);      // brdr is border
//  //
//  //nvgBeginPath(vg);
//  //  nvgCircle(vg, P.x, P.y, 5.f);
//  //  nvgStrokeWidth(vg, rthk);
//  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
//  //nvgStroke(vg);
//  //
//  //nvgBeginPath(vg);
//  //  nvgCircle(vg, circCntr.x, circCntr.y, 5.f);
//  //  nvgStrokeWidth(vg, rthk);
//  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
//  //nvgStroke(vg);
//  //
//  //f32 angle = inLeftCircle? asin(dir.y) :  PIf/2;
//  //f32 arcSt = PIf/2.0f;
//  //f32 arcEn = PIf*1.5f;
//  //NVGwinding windDirection = leftSide? NVG_CW  :  NVG_CCW;
//  //if(onTop){
//  //  swap(arcSt, arcEn);
//  //  windDirection = NVG_CCW;
//  //}
//
//  //SECTION(drawing first attempt)
//  //{
//  //  nvgBeginPath(vg);
//  //    //if(inLeftCircle){
//  //    //  nvgMoveTo(vg, P.x, P.y);
//  //    //  if(leftArc>0){
//  //    //    nvgArc(vg, circCntr.x, circCntr.y, rad, arcSt, arcEn, windDirection);    // PIf*1.f, PIf*1.5f, NVG_CW);
//  //    //    if(leftWrap>0)
//  //    //      nvgLineTo(vg, nP.x+rad+leftWrap, nP.y);
//  //    //  }
//  //    //}else{
//  //    //if(leftSide){
//  //
//  //    //if(onTop && !inLeftCircle){
//  //    //  nvgMoveTo(vg, P.x, P.y);
//  //    //  nvgLineTo(vg, leftLineLimit, P.y);
//  //    //  nvgArc(vg, circCntr.x, circCntr.y, rad, arcEn, arcSt, NVG_CCW);      // PIf*1.f, PIf*1.5f, NVG_CW);
//  //    //  if(leftWrap>0)
//  //    //    nvgLineTo(vg, nP.x+rad+leftWrap, nP.y+NODE_SZ.y);
//  //    //}else{
//  //      nvgMoveTo(vg, P.x, P.y);
//  //      if(!inLeftCircle){ nvgLineTo(vg, leftLineLimit, P.y); }
//  //      if(leftArc>0){
//  //        if(onTop) nvgArc(vg, circCntr.x, circCntr.y, rad, arcEn, arcSt, NVG_CCW);
//  //        else      nvgArc(vg, circCntr.x, circCntr.y, rad, arcSt, arcEn, windDirection);
//  //        if(leftWrap>0)
//  //          nvgLineTo(vg, nP.x+rad+leftWrap, nP.y);
//  //      }
//  //    //}
//  //    //}
//  //    //}else if(leftSide && onTop){
//  //    //  
//  //    //}
//  //
//  //    //nvgMoveTo(vg, leftLineLimit, P.y);
//  //    //nvgLineTo(vg, P.x, P.y);
//  //    //nvgArc(vg, circCntr.x, circCntr.y, rr, arcSt, arcEn, windDirection);    // PIf*1.f, PIf*1.5f, NVG_CW);
//  //    nvgStrokeWidth(vg, rthk);
//  //    nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
//  //  nvgStroke(vg);
//  //}
//  //
//  //f32 arcEn = leftSide? PIf*1.5f  :  -PIf*0.5f;
//  //if(leftSide==false){ swap(arcSt, arcEn); }
//
//  //nvgBeginPath(vg);
//  //  nvgCircle(vg, nP.x, nP.y, 5.f);
//  //  nvgStrokeWidth(vg, rthk);
//  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
//  //nvgStroke(vg);
//  //
//  //nvgBeginPath(vg);
//  //  nvgCircle(vg, P.x, P.y, 5.f);
//  //  nvgStrokeWidth(vg, rthk);
//  //  nvgStrokeColor(vg, nvgRGBAf(1.f, .7f, 0, 1.f));
//  //nvgStroke(vg);
//}

//GraphDB::Id id;
//printf("\n sizeof(GraphDB::Id): %d \n", sizeof(GraphDB::Id) );

//node_add("new node");
//
//node_add("message node", node::MSG);

//auto sIter = grph.nodeSlots(ndOrdr);            // sIter is slot iterator
//
//auto     sIdx = sIter->second;                    // sIdx is slot index

//
//nvgTextAlign(vg,  NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);  // NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);

//auto sz = cncts.size();
//TO(sz,i) src.add(cncts[i].src);
//TO(sz,i) dest.add(cncts[i].dest);

//FisData* fd = (FisData*)glfwGetWindowUserPointer(window);
//fd->ui.screen.resizeCallbackEvent(w, h);

//if(hit) *out_nrml = norm(circCntr - borderPt);
//
//  && !hasNaN(intrsct);
//
//f32        r = hlf.y;
//
//if(hit && borderPt.x > (nP.x+rad) && borderPt.x < (nP.x-rad) ){
//  if(ds.y > .5f) borderPt.y = nP.y + NODE_SZ.y;
//  else           borderPt.y = nP.y;
//}

//nvgBeginPath(vg);
//  nvgRect(vg, b.xmn, b.ymn, b.w(), b.h());
//nvgStroke(vg);

//if(preicon != 0){
//	nvgFontSize(vg, h*1.3f);
//	nvgFontFace(vg, "icons");
//	nvgFillColor(vg, nvgRGBA(255,255,255,96));
//	nvgTextAlign(vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
//	nvgText(vg, x+w*0.5f-tw*0.5f-iw*0.75f, y+h*0.5f, cpToUTF8(preicon,icon), NULL);
//}

// lineCircleIntersect
//f32       r = NODE_SZ.y/2.f;
//v2  dirCirc = abs(pdir) / r;
//v2 circCntr = n.P + NODE_SZ - r;

//auto bkt = find(rnge.first, rnge.second, );
//if(bkt!=rnge.second){
//  cnct->insert( {a, b} );
//  return true;
//} 

//TO(grph.selsz(),i) grph.sel(i,false);
//grph.sel(ndOrdr,true);
//
//clearSelections = true;

//if(inNode && clk && (priSel<0||priSel!=ndOrdr) )
// priSel!=ndOrdr) )
//
//priSel     = ndOrdr;
//
//if(!grph.sel(ndOrdr)){

//int  ndOrdr = grph.order(i);
//Node&     n = grph.node(i);
//bool inNode = isIn(pntr.x,pntr.y, grph.bnd(ndOrdr) );

//if( lftClkUp && !(priSel>0) ){
//  clearSelections=false;
//}

//if( (drgbnd.w()+drgbnd.h()) > 0 ){
//  clearSelections=false;
//}

//i32  inClk = -1;
//i32 outClk = -1;
//
//TO(grph.ssz(), i)
//auto   nid = kv.first.id;
//
//lftDn && !prevLftDn)
//Slot&    s = *(grph.slot(i));
//
//outClk  = (i32)nid;
//
//Slot&    s = *(grph.slot(i));

//else{
//slotOutSel = slotInSel = Id(0,0);
//grph.clearSlotSels();
//}

//auto sz = fd.grph.nsz();
//
//int  ndOrdr = fd.grph.order(i);            // nd_ordr[i];
//if( !(ndOrdr < grph.nsz()) ){ continue; }
//
//auto      n = fd.grph.node(ndOrdr);
//bool selctd = ndOrdr==priSel || fd.grph.sel(ndOrdr);
//
//float round = secSel==ndOrdr? 0 : 1.f;
//fd.grph.bnd(ndOrdr) = node_draw(vg, 0, n, clr, round);

//f32  halfx = lerp(.5f, src.P.x, dest.P.x);
//f32  halfy = lerp(.5f, src.P.y, dest.P.y);
//f32   dist = len(src.P - dest.P);
//v2  outNxt = src.P  + src.N  * min(NODE_SZ.x/2, (dist/3));         // divide by 3 because there are 3 sections to the bezier
//v2   inNxt = dest.P + dest.N * min(NODE_SZ.x/2, (dist/3));

//nvgBeginPath(vg);
//  nvgMoveTo(vg,   src.P.x, src.P.y);
//  nvgBezierTo(vg, outNxt.x,outNxt.y, inNxt.x,inNxt.y, dest.P.x,dest.P.y);
//  nvgStrokeWidth(vg, 3.f);
//  nvgStrokeColor(vg, nvgRGBAf(.7f, 1.f, .9f, .5f));
//nvgStroke(vg);

//f32  halfx = lerp(.5f, src.P.x, avgP.x);
//f32  halfy = lerp(.5f, src.P.y, avgP.y);
//f32   dist = len(src.P - avgP);
//v2  outNxt = src.P  + src.N * min(NODE_SZ.x/2, (dist/3));         // divide by 3 because there are 3 sections to the bezier
//v2   inNxt = avgP   +  midN * min(NODE_SZ.x/2, (dist/3));

//nvgBeginPath(vg);
//  nvgMoveTo(vg,   src.P.x, src.P.y);
//  nvgBezierTo(vg, outNxt.x,outNxt.y, inNxt.x,inNxt.y, avgP.x, avgP.y);
//  nvgStrokeWidth(vg, 3.f);
//  nvgStrokeColor(vg, nvgRGBAf(.7f, 1.f, .9f, .5f));
//nvgStroke(vg);

//f32  halfx = lerp(.5f, dest.P.x, avgP.x);
//f32  halfy = lerp(.5f, dest.P.y, avgP.y);
//f32   dist = len(avgP - dest.P);
//v2  outNxt = dest.P  + dest.N * min(NODE_SZ.x/2, (dist/3));         // divide by 3 because there are 3 sections to the bezier
//v2   inNxt = avgP + -1.f*midN * min(NODE_SZ.x/2, (dist/3));
//nvgBeginPath(vg);
//  nvgMoveTo(vg,   dest.P.x,dest.P.y);
//  nvgBezierTo(vg, outNxt.x,outNxt.y, inNxt.x,inNxt.y, avgP.x, avgP.y);
//  nvgStrokeWidth(vg, 3.f);
//  nvgStrokeColor(vg, nvgRGBAf(.7f, 1.f, .9f, .5f));
//nvgStroke(vg);

//auto en = grph.cnctEnd();
//for(auto const& ci : grph.cncts())
//for(auto ci = grph.cnctBegin(); ci != en; )
//for(auto di : grph.srcCnctsMap())  // multi-map of src->dest connections, di is destination iterator 

//avgN += norm(dest.P
//
//f32  halfx = lerp(.5f, dest.P.x, src.P.x);
//f32  halfy = lerp(.5f, dest.P.y, src.P.y);
//f32   dist = len(src.P - dest.P);
//
//nvgMoveTo(vg, avgP.x, avgP.y);
//
//nvgBezierTo(vg, outNxt.x,outNxt.y, inNxt.x,inNxt.y, dest.P.x,dest.P.y);

//auto     srcIdx = ci->second;
//auto      count = grph.cncts().count(ci->first);

//auto srcInt  = srcId.get(i).toInt();
//auto destInt = destId.get(i).toInt();

//auto st = src.get(i).getType();
//auto dt = dest.get(i).getType();
//auto srcInt  = src.get(i).toInt();
//auto destInt = dest.get(i).toInt();

//Id srcId, destId;
//srcId.asInt  = srcInt;
//destId.asInt = destInt;
//g.addCnct(srcId, destId);
