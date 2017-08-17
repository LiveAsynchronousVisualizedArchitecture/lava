
#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __FISSURE_DECL_HEADERGUARD_HPP__
#define __FISSURE_DECL_HEADERGUARD_HPP__

#include "../no_rt_util.h"
#include "vec.hpp"
#include <nanogui/nanogui.h>

using namespace nanogui;

const int     TITLE_MAX_LEN = 256;
const v2      NODE_SZ       = { 256.f, 64.f };
const v2      NODE_HALF_SZ  = NODE_SZ/2.f;
const auto    NODE_CLR      = nvgRGBf(.15f,.3f,.4f);
const float   INFf          = std::numeric_limits<float>::infinity();
const float   SIG_NANf      = std::numeric_limits<float>::signaling_NaN();
const f32     IORAD         = 15.f;
const float   BORDER        = 3.5f;

extern "C" unsigned char Roboto_Regular_ttf[];
extern "C" unsigned int  Roboto_Regular_ttf_len;

extern "C" unsigned char Roboto_Bold_ttf[];
extern "C" unsigned int  Roboto_Bold_ttf_len;

extern "C" unsigned char entypo_ttf[];
extern "C" unsigned int  entypo_ttf_len;

template<class T> using vec = std::vector<T>;
using str = std::string;

using e2i  =  Eigen::Vector2i;
using e4f  =  Eigen::Vector4f;

using     veci     =    vec<i32>;
using   vecstr     =    std::vector<str>;
using   vec_v2     =    vec<v2>;
using cnct_tbl     =    std::unordered_multimap<int,int>;

union      bnd 
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
using vec_nbnd     =    vec<bnd>;

struct    node
{
  enum Type { MSG=0, FLOW=1 };

  v2        P = {0,0};
  //bnd       B;
  str     txt; 
  Type   type = FLOW;
  //LavaNode ln;

  node(){}
  node(str _txt, Type _type=FLOW, v2 _P=v2(0,0) ) : txt(_txt), P(_P), type(_type) {}
};
using   vec_nd     =    vec<node>;

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

struct    slot { 
  enum State { NORMAL=0, HIGHLIGHTED, SELECTED };

  v2 P; u64 nidx; v2 N; bool in=false; State state=NORMAL;

  slot(u64 nIdx, bool In=false) : nidx(nIdx), in(In), P(0,0), N(0,1) {}
};
using vec_slot     =    vec<slot>;


class  GraphDB
{
public:
  using NodeSlotMap = std::multimap<u64, u64>;   // The key is the index into the node array, the value is the index into the slot array.  Every node can have 0 or more slots. Slots can only have 1 and only 1 node. Slots have their node index in their struct so getting the node from the slots is easy. To get the slots that a node has, this multimap is used.

private:
  vec_nd               nodes;
  vec_nbnd              bnds;
  veci                  ordr;
  vec<bool>         selected;             // bitfield for selected nodes
  vec_slot             slots;
  NodeSlotMap     node_slots;

public:
  GraphDB(){}

  i32      addNode(node n)
  {
    nodes.push_back(n);
    i32 nodeIdx = (i32)(nodes.size()-1);
    ordr.push_back( nodeIdx );
    bnds.emplace_back();
    selected.push_back(false);

    return nodeIdx;
  }
  i32      addSlot(slot s)
  {
    slots.push_back(s);
    i32 slotIdx = (i32)(slots.size() - 1);

    auto iter = node_slots.insert({s.nidx, slotIdx});
    
    return slotIdx;
  }
  auto   nodeSlots(u64 nIdx) -> decltype(node_slots.find(nIdx))
  {
    auto iter = node_slots.find(nIdx);
    return iter;
  }
  auto         end() -> decltype(node_slots.end()) { return node_slots.end(); } 
  auto orderedNode(u64 order) -> node& { return nodes[ordr[order]]; }
  auto        node(u64 nIdx) -> node& { return nodes[nIdx]; }
  auto        slot(u64 sIdx) -> slot& { return slots[sIdx]; }
  auto    getNodes() -> vec_nd&   { return nodes; }
  auto    getSlots() -> vec_slot& { return slots; }
  auto         bnd(u64 idx) -> bnd& { return bnds[idx]; }
  bool         sel(u64 idx){ return selected[idx]; }
  void         sel(u64 idx, bool s){ selected[idx] = s; }
  i32        order(u64 idx){ return ordr[idx]; }
  void       order(u64 idx, i32 o){ ordr[idx] = o; }
  u64          nsz(){ return nodes.size(); }
  u64          ssz(){ return slots.size(); }
};

struct FisData
{
  GLFWwindow*         win = nullptr;                     // Platform 
  GraphDB            grph;

  struct
  {
    int w=0, h=0;
    Screen         screen;
    Window*        keyWin = nullptr;
    Window*         dbWin = nullptr;
    BoxLayout*     keyLay = nullptr;
  } ui;
};

#endif






//using NodeIdex    = i32;
//
//vec_slot          slots_in;
//vec_slot         slots_out;
//
//void  getSlots(i32 nIdx)

//nbnds.emplace_back( { n.P.x, n.P.y, n.P.x + , n.P.y  } );
//
//v2 P = v2(512,512);
//nodes.push_back( {P,txt} );
//nodes.back().type = type;

//f32 ptSz, hudSz, guideSz;                              // ptSz is point size  |  hudSz is  heads up display size
//
//ComboBox*       dbLst = nullptr;
//int          dbLstIdx = -1;
//NVGcontext*       nvg = nullptr;
//veci           dbIdxs;
//vecstr        dbNames;
//int         dbNameIdx;
//str            dbName;
//bool        showGuide;

