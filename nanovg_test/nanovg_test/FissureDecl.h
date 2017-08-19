
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
const auto    NODE_CLR      = nvgRGBf(.2f, .3f, .375f);
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

  float w(){return abs(xmx-xmn);}
  float h(){return abs(ymx-ymn);}
};
using  vec_bnd     =    vec<Bnd>;

struct    Node
{
  enum Type { MSG=0, FLOW=1 };

  v2        P = {0,0};
  //bnd       B;
  str     txt; 
  Type   type = FLOW;
  //LavaNode ln;

  Node(){}
  Node(str _txt, Type _type=FLOW, v2 _P=v2(0,0) ) : txt(_txt), P(_P), type(_type) {}
};
using   vec_nd     =    vec<Node>;

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

struct    Slot { 
  enum State { NORMAL=0, HIGHLIGHTED, SELECTED, SLOT_ERROR };

  v2 P; u64 nidx; v2 N; bool in=false; State state=NORMAL;

  Slot(u64 nIdx, bool In=false) : nidx(nIdx), in(In), P(0,0), N(0,1), state(NORMAL) {}
};
using vec_slot     =    vec<Slot>;

class  GraphDB
{
public:
  using NodeSlotMap = std::multimap<u64, u64>;          // The key is the index into the node array, the value is the index into the slot array.  Every node can have 0 or more slots. Slots can only have 1 and only 1 node. Slots have their node index in their struct so getting the node from the slots is easy. To get the slots that a node has, this multimap is used.
  using CnctMap     = std::multimap<u32, u32>;
  using SrcMap      = std::multimap<u32, u32>;          // todo: change this to a single map

private:
  vec_nd             m_nodes;
  vec_bnd             m_bnds;
  veci                m_ordr;
  vec<bool>       m_selected;             // bitfield for selected nodes
  vec_slot           m_slots;
  NodeSlotMap    m_nodeSlots;
  CnctMap         m_outCncts;
  SrcMap           m_inCncts;

  void mv(GraphDB&& rval)
  {
    using namespace std;

    m_nodes     = move(rval.m_nodes); 
    m_bnds      = move(rval.m_bnds); 
    m_ordr      = move(rval.m_ordr); 
    m_selected  = move(rval.m_selected); 
    m_slots     = move(rval.m_slots); 
    m_nodeSlots = move(rval.m_nodeSlots); 
    m_outCncts  = move(rval.m_outCncts); 
    m_inCncts   = move(rval.m_inCncts);
  }

public:
  GraphDB(){}
  GraphDB(GraphDB&& rval){ mv(std::move(rval)); }
  GraphDB& operator=(GraphDB&& rval){ mv(std::move(rval)); return *this; }

  u64       addNode(Node n)
  {
    m_nodes.push_back(n);
    u64 nodeIdx = (u64)(m_nodes.size()-1);
    m_ordr.push_back( (i32)nodeIdx );
    m_bnds.emplace_back();
    m_selected.push_back(false);

    return nodeIdx;
  }
  auto         node(u64 nIdx)  -> struct Node& { return m_nodes[nIdx]; }
  auto         node(u64 nIdx) const -> struct Node const& { return m_nodes[nIdx]; }
  auto  orderedNode(u64 order) -> struct Node& { return m_nodes[m_ordr[order]]; }
  void   moveToBack(u64 nIdx)
  {
    using namespace std;

    auto  sz = m_ordr.size();
    auto tmp = move(m_ordr[nIdx]);
    for(auto j=nIdx; j<sz-1; ++j)
      m_ordr[j] = move( m_ordr[j+1] );

    m_ordr[sz-1] = tmp;
  }
  auto     getNodes() -> vec_nd&   { return m_nodes; }
  auto        nodes() -> vec_nd&   { return m_nodes; }
  u64           nsz() const { return m_nodes.size(); }

  u64       addSlot(Slot s)
  {
    m_slots.push_back(s);
    u64 slotIdx = (u64)(m_slots.size() - 1);

    auto iter = m_nodeSlots.insert({s.nidx, slotIdx});
    
    return slotIdx;
  }
  bool      hasSlot(u64 sIdx)
  {
    return sIdx < m_slots.size();
  }
  auto         slot(u64 sIdx) -> Slot& 
  {
    //static Slot slot_error(0,false);  // = { {0,0}, 0, {0,0}, false, Slot::SLOT_ERROR };

    //if(sIdx<m_slots.size())
      return m_slots[sIdx];
    //else{
    //  slot_error.state = Slot::SLOT_ERROR;
    //  return slot_error;
    //}
  }
  auto    nodeSlots(u64 nIdx) -> decltype(m_nodeSlots.find(nIdx))
  {
    auto iter = m_nodeSlots.find(nIdx);
    return iter;
  }
  auto        slots() -> vec_slot& { return m_slots; }
  auto        slots() const -> vec_slot const& { return m_slots; }
  auto     getSlots() -> vec_slot& { return m_slots; }
  auto      srcSlot(u32 destSlotIdx) -> struct Slot* 
  {
    auto ci = m_inCncts.find(destSlotIdx);
    if(ci != m_inCncts.end()){
      struct Slot& s = slot(ci->second);
      return &s;
    }else 
      return nullptr;
  }
  auto    destSlots(u32 srcSlotIdx) -> decltype(m_outCncts.end())
  {
    return m_outCncts.find(srcSlotIdx);
  }
  auto      slotEnd() -> decltype(m_nodeSlots.end()) { return m_nodeSlots.end(); }
  u64           ssz() const { return m_slots.size(); }

  void      addCnct(u32 src, u32 dest)
  {
    auto srcIter = m_inCncts.find(dest);
    if(srcIter != m_inCncts.end()){
      m_outCncts.erase(src);
      m_inCncts.erase(dest);
    }

    m_outCncts.insert({src, dest});
    m_inCncts.insert({dest, src});
  }
  void   toggleCnct(u32 src, u32 dest)
  {
    auto srcIter = m_inCncts.find(dest);
    if(srcIter != m_inCncts.end()){
      //m_outCncts.erase(src);

      auto iter = m_outCncts.find(src);
      for(; iter != m_outCncts.end() && iter->first==src; ){
        auto cpy = iter;
        ++iter;
        if(cpy->second == dest) m_outCncts.erase(cpy);
      }

      //if(iter != m_outCncts.end()){
      //}

      //m_outCncts.erase(src);
      m_inCncts.erase(dest);
    }else{
      m_outCncts.insert({src, dest});
      m_inCncts.insert({dest, src});
    }
  }
  auto      cnctEnd() -> decltype(m_outCncts.end())  { return m_outCncts.end(); }
  auto    cnctBegin() -> decltype(m_outCncts.begin()) { return m_outCncts.begin(); }
  auto        cncts() -> CnctMap& { return m_outCncts; }
  auto        cncts() const -> CnctMap const& { return m_outCncts; }
  u64        cnctsz() const { return m_outCncts.size(); }

  auto          bnd(u64 idx) -> Bnd& { return m_bnds[idx]; }
  auto       bounds() -> vec_bnd& { return m_bnds; }

  bool          sel(u64 idx){ return m_selected[idx]; }
  void          sel(u64 idx, bool s){ m_selected[idx] = s; }
  auto         sels() -> vec<bool>& { return m_selected; }
  void    clearSels()
  {
    for(auto& sel : m_selected) sel = false;
    for(auto& slt : m_slots) slt.state = Slot::NORMAL;
  }
  u64         selsz() const { return m_selected.size(); }

  i32         order(u64 idx){ return m_ordr[idx]; }
  void        order(u64 idx, i32 o){ m_ordr[idx] = o; }

  void        clear()
  {
    m_nodes.clear();
    m_bnds.clear();
    m_ordr.clear();
    m_selected.clear();
    m_slots.clear();
    m_nodeSlots.clear();
    m_outCncts.clear();
    m_inCncts.clear();
  }
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






//auto&  o = order;
//move_backward(a.front()+i+1ul, a.back(), a.back()-1);

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

