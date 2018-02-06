
#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __FISSURE_DECL_HEADERGUARD_HPP__
#define __FISSURE_DECL_HEADERGUARD_HPP__

#include <set>
#include <queue>
#include <nanogui/nanogui.h>
//#include "../no_rt_util.h"
//#include "../simdb.hpp"
#include "../../no_rt_util.h"
#include "../../simdb.hpp"
#include "vec.hpp"

#define __LAVAFLOW_IMPL__
#include "../LavaFlow.hpp"

using namespace nanogui;

const int     TITLE_MAX_LEN = 256;
const float   INFf          = std::numeric_limits<float>::infinity();
const float   SIG_NANf      = std::numeric_limits<float>::signaling_NaN();

extern "C" unsigned char Roboto_Regular_ttf[];
extern "C" unsigned int  Roboto_Regular_ttf_len;

extern "C" unsigned char Roboto_Bold_ttf[];
extern "C" unsigned int  Roboto_Bold_ttf_len;

extern "C" unsigned char entypo_ttf[];
extern "C" unsigned int  entypo_ttf_len;

template<class T> using vec = std::vector<T>;
using str  =  std::string;

using e2i  =  Eigen::Vector2i;
using e2f  =  Eigen::Vector2f;
using e3f  =  Eigen::Vector3f;
using e4f  =  Eigen::Vector4f;

using      veci    =    vec<i32>;
using     vecui    =    vec<u32>;
using    vecstr    =    std::vector<str>;
using    vec_v2    =    vec<v2>;
using  cnct_tbl    =    std::unordered_multimap<int,int>;
using   vec_btn    =    vec<Button*>;

union      Bnd 
{
  struct { float xmn, ymn, xmx, ymx; };  // todo: make into a union?
  struct { v2 mn; v2 mx; };
  struct { v4 mnmx; };
  float c[4];

  Bnd() : xmn(INFf), ymn(INFf), xmx(-INFf), ymx(-INFf) {}
  Bnd(float Xmn, float Ymn, float Xmx, float Ymx) :
    xmn(Xmn), ymn(Ymn), xmx(Xmx), ymx(Ymx)
  {}

  float&       operator[](int i)       {return c[i];}
  float const& operator[](int i) const {return c[i];}
  Bnd& operator()(float Xmn, float Ymn, float Xmx, float Ymx)
  {
    xmn=Xmn; ymn=Ymn; xmx=Xmx; ymx=Ymx; return *this;
  }
  operator v4&(){ return mnmx; }

  float      w() const { return abs(xmx-xmn); }
  float      h() const { return abs(ymx-ymn); }
  v2        wh() const { return {w(), h()};   }
  float   area() const { return w() * h();    }
  bool  hasLen() const
  { 
    using namespace std;
    return (max(0.f,xmx-xmn) + max(0.f,ymx-ymn)) > 0.f;
  }
};
using  vec_bnd     =    vec<Bnd>;

struct    Node
{
  //enum Type { MSG=0, FLOW=1, NODE_ERROR=0xFFFFFFFFFFFFFFFF };
  using Type = LavaNode::Type;

  LavaNode* lfn;                                     // lfn is Lava Flow Node
  u64       id  =  0;
  u64    order  =  0;
  v2         P  =  {0,0};
  bool     sel  =  false;                            // sel is selected
  Bnd        b  =  {0, 0, 128.f, 48.f};
  str      txt  =  "";
  Type    type  =  Type::NODE_ERROR;
  u16    incnt  =  0;
  u16   outcnt  =  0;

  void cp(Node const& l)
  {
    id    = l.id;
    order = l.order;
    P     = l.P;
    type  = l.type;
    sel   = l.sel;
    b     = l.b;
    txt   = l.txt;
  }
  void mv(Node&&      r)
  {
    using namespace std;

    id     = move(r.id);
    order  = move(r.order);
    P      = move(r.P);
    type   = move(r.type);
    sel    = move(r.sel);
    b      = move(r.b);
    txt    = move(r.txt);
    incnt  = r.incnt;
    outcnt = r.outcnt;
  }

  Node(){}
  Node(str _txt, Type _type=Node::Type::FLOW, v2 _P=v2(0,0) ) : txt(_txt), P(_P), type(_type)
  {
    if(type==Type::MSG) b.ymx = b.xmx;
  }
  Node(Node const& l){ cp(l); }
  Node(Node&&      r){ mv(std::move(r)); }

  Node& operator=(Node const& l){            cp(l); return *this; }
  Node& operator=(Node&&      r){ mv(std::move(r)); return *this; }
  bool  operator<(Node const& l){ return l.order; }
};
using   vec_nd      =   vec<Node>;
using   vec_ndptrs  =   vec<Node*>;

struct    cnct { 
  int src, dest;
  ui8 src_out, dest_in;
  bool operator<(cnct const& r){ return src < r.src; }
  bool operator==(cnct const& r){ return src==r.src && dest==r.dest; }

  static bool lessDest(cnct const& l, cnct const& r){
    return l.dest < r.dest;
  }
};
using  vec_con     =    vec<cnct>;

struct    Slot
{ 
  enum State { NORMAL=0, HIGHLIGHTED, SELECTED, SLOT_ERROR };

  u64 nid; v2 P; v2 N; bool in=false; State state=NORMAL;

  Slot(u64 nId, bool In=false) : nid(nId), in(In), P(0,0), N(0,1), state(NORMAL) {}
};
using vec_slot     =    vec<Slot>;

struct  AtmSet
{
  using au64 = std::atomic<u64>;

  static const u64 sz = 8;
  u64    null_val;
  u64     buf[sz];

  AtmSet(){}
  AtmSet(u64 nullVal) : null_val(nullVal)
  {
    TO(sz,i){ store(i,null_val); }
  }

  void   store(u64 i, u64 val){ ((au64*)(buf+i))->store(val); }
  bool  cmpSwp(u64 i, u64 val, u64 prev)
  {
    return ((au64*)(buf+i))->compare_exchange_strong(prev, val);
  }
  u64     load(u64 i){ return ((au64*)(buf+i))->load(); }
  bool     has(u64 val)
  {
    TO(sz,i){ if(load(i)==val){ return true; } }
    return false;
  }
  bool     put(u64 val)
  {
    TO(sz,i){
      auto cur = load(i);
      if( cur == val ){ return true; }
      if( cur == null_val ){
        if(cmpSwp(i, val, null_val)) return true;
      }
    }
    return false;
  }
  bool     del(u64 val)
  {
    TO(sz,i){
      u64 cur;
      while( (cur=load(i)) == val ){
        if(cmpSwp(i, null_val, val)){ return true; }
      }
    }
    return false;
  }
};

struct FisData
{
  struct IdOrder {
    u64      id = 0;
    u64   order = 0;
    bool operator<(IdOrder l) const { return order < l.order; }
  };

  using          Id  =  LavaId;
  using     NodeMap  =  std::unordered_map<uint64_t, Node>;
  using   NodeOrder  =  std::set<IdOrder>;
  using       Slots  =  std::multimap<LavaId, Slot>;              // The key is a node id, the value is the index into the slot array.  Every node can have 0 or more slots. Slots can only have 1 and only 1 node. Slots have their node index in their struct so getting the node from the slots is easy. To get the slots that a node has, this multimap is used
  using    vec_thrd  =  std::vector<std::thread>;
  using     vec_slt  =  std::vector<LavaId>;
  using PacketSlots  =  std::unordered_set<LavaId, LavaId>;       // uses LavaId as the hashing function object


  u64                 nxtId = 1;
  GLFWwindow*           win = nullptr;                            // Platform 
  LavaFlow             flow;
  LavaGraph&          lgrph = flow.graph;
  vec_thrd      flowThreads;
  AtmSet             vizIds;

  struct Graph
  {
    NodeMap             nds;       // nds  is nodes
    NodeOrder          ordr;       // ordr is orders 
    Slots             slots;       // 

    u64             curNode = LavaNode::NODE_ERROR;              // information from the running lava graph for visualization in the GUI
    u64        qPacketBytes = 0;
    //vec_slt     packetSlots;
    PacketSlots packetSlots;

    // text
    f32            textSize  =  18.f;  //18.f;
    f32         textMarginW  =  55.f; 
    f32         textMarginH  =  35.f; 

    // graph colors
    NVGcolor        lineClr  =  nvgRGBAf(.04f, .04f, .04f, 1.f);
    NVGcolor       nd_color  =  nvgRGBAf(.2f, .3f, .375f, 1.f);
    NVGcolor      nd_selclr  =  nvgRGBAf(.5f,.4f,.1f, 1.f);
    NVGcolor    msgnd_color  =  nvgRGBAf(.2f, .3f, .375f, 1.f);
    NVGcolor   msgnd_selclr  =  nvgRGBAf(.5f,.4f,.1f, .75f);
    NVGcolor   msgnd_gradst  =  nvgRGBAf(.3f, .3f, .3f, .5f);
    NVGcolor   msgnd_graden  =  nvgRGBAf(.15f, .15f, .15f, .45f); 
  } graph;
  struct
  {
    int w=0, h=0;
    Screen          screen;
    Window*         keyWin = nullptr;
    Window*          dbWin = nullptr;
    BoxLayout*      keyLay = nullptr;
    Window*      statusWin = nullptr;
    BoxLayout*   statusLay = nullptr;
    TextBox*     statusTxt = nullptr;
    vec_btn         ndBtns;


    // mouse
    v2                prevPntr;
    bool                  rtDn = false;    // right mouse button down
    bool                 lftDn = false;    // left mouse button down
    bool              prevRtDn = false;    // right mouse button down
    bool             prevLftDn = false;    // left mouse button down

    // drawing
    NVGcolor        lineClr  =  nvgRGBAf(.04f, .04f, .04f, 1.f);
    NVGcolor       nd_color  =  nvgRGBAf(.2f, .3f, .375f, 1.f);
    NVGcolor      nd_selclr  =  nvgRGBAf(.5f,.4f,.1f, 1.f);
    NVGcolor       nd_hilgt  =  nvgRGBAf(1.f,1.f,1.f, 0.025f);
    NVGcolor    msgnd_color  =  nvgRGBAf(.2f, .3f, .375f, 1.f);
    NVGcolor   msgnd_selclr  =  nvgRGBAf(.5f,.4f,.1f, .75f);
    NVGcolor   msgnd_gradst  =  nvgRGBAf(.3f, .3f, .3f, .5f);
    NVGcolor   msgnd_graden  =  nvgRGBAf(.15f, .15f, .15f, .45f); 

    f32           nd_border  =   3.5f;
    f32            slot_rad  =   15.f;
    f32         slot_border  =   3.5f;
  }          ui;
  struct
  {
    v2                prevPntr;
    bool                  rtDn = false;    // right mouse button down
    bool                 lftDn = false;    // left mouse button down
    bool              prevRtDn = false;    // right mouse button down
    bool             prevLftDn = false;    // left mouse button down

    v2                    drgP;
    v2                 drgofst;
    bool              dragging = false;
    bool                drgbox = false;
    bool            prevDrgbox = false;
    bool                 drgNd = false;
    Bnd                 drgbnd;
  }       mouse;
  struct 
  {
    u64              pri = LavaNode::NODE_ERROR;
    u64              sec = LavaNode::NODE_ERROR;
    LavaId     slotInSel;
    LavaId    slotOutSel;
  }         sel;
};


#endif






//
//Lava               lava;
