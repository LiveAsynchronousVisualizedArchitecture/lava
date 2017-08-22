
#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __FISSURE_DECL_HEADERGUARD_HPP__
#define __FISSURE_DECL_HEADERGUARD_HPP__

#include <set>
#include <nanogui/nanogui.h>
#include "../no_rt_util.h"
#include "vec.hpp"

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
  enum Type { MSG=0, FLOW=1, NODE_ERROR };

  u64      id = 0;
  u64   order = 0;
  v2        P = {0,0};
  str     txt; 
  Type   type = NODE_ERROR;

  //bnd       B;
  //LavaNode ln;

  Node(){}
  Node(str _txt, Type _type=FLOW, v2 _P=v2(0,0) ) : txt(_txt), P(_P), type(_type) {}
  bool operator<(Node const& l){ return l.order; }
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
  using NodeMap      = std::map<u64, Node>;
  using NodeIdMap    = std::unordered_map<u64, u64>;
  using NodeSlotMap  = std::multimap<u64, u64>;            // The key is the index into the node array, the value is the index into the slot array.  Every node can have 0 or more slots. Slots can only have 1 and only 1 node. Slots have their node index in their struct so getting the node from the slots is easy. To get the slots that a node has, this multimap is used.
  using CnctMap      = std::multimap<u32, u32>;
  using SrcMap       = std::unordered_map<u32, u32>;
  //using NodeSet      = std::set<Node>;
  //using SrcMap      = std::multimap<u32, u32>;          // todo: change this to a single map

  //static const Node CONST_ERR_NODE;
  static Node ERR_NODE;

private:
  u64                m_nxtId;             // nxtId is next id - a counter for every node created that only increases, giving each node a unique id
  NodeMap            m_nodes;
  NodeIdMap            m_ids;
  vec_bnd             m_bnds;
  vec<bool>       m_selected;             // bitfield for selected nodes
  vec_slot           m_slots;
  NodeSlotMap    m_nodeSlots;
  CnctMap         m_outCncts;
  SrcMap           m_inCncts;

  //vec_nd             m_nodes;
  //veci                m_ordr;

  void          init(){}
  void            mv(GraphDB&& rval)
  {
    using namespace std;

    m_nodes     = move(rval.m_nodes); 
    m_bnds      = move(rval.m_bnds); 
    m_ids       = move(rval.m_ids);
    m_selected  = move(rval.m_selected); 
    m_slots     = move(rval.m_slots); 
    m_nodeSlots = move(rval.m_nodeSlots); 
    m_outCncts  = move(rval.m_outCncts); 
    m_inCncts   = move(rval.m_inCncts);
  }
  u64            nxt(){ return m_nxtId++; }
  auto selectedNodes() -> vec<u64> 
  {
    vec<u64> nids;                                         // nids is node ids
    TO(m_selected.size(),i)
    {
      if(m_selected[i]){
        nidxs.push_back(node(i).id);
        auto sIter = nodeSlots(i);
        auto  nidx = sIter->first;
        for(; sIter!=slotEnd() && sIter->first==nidx; ++sIter){
          sidxs.push_back( sIter->second );
        }
    }
  }

public:
  GraphDB() : m_nxtId(1) { init(); }
  GraphDB(GraphDB&& rval){ mv(std::move(rval)); }
  GraphDB& operator=(GraphDB&& rval){ mv(std::move(rval)); return *this; }

  u64        addNode(Node n, bool newId=true)
  {
    if(newId) n.id = nxt();

    auto order = m_nodes.rbegin()->first + 1;
    n.order    = order;
    m_nodes.insert({order, n});
    u64 nodeIdx = (u64)(m_nodes.size()-1);

    m_ids.insert({n.id, n.order});
    
    m_bnds.emplace_back();
    m_selected.push_back(false);

    return nodeIdx;
  }
  void       delNode(u64 nIdx)
  {
    Node&     n = this->node(nIdx);
    auto  sIter = nodeSlots(nIdx);                          // sIter is slot iterator
    auto sltCpy = sIter;
    auto sltIdx = sIter->first;
    for(; sltCpy!=slotEnd() && sltCpy->first==sltIdx; ++sltCpy){
      
    }
  }
  u64    delSelected()
  {
    using namespace std;

    // accumulate slots
    // accumulate dest slots
    // delete cncts with dest slots
    // delete slots
    // delete nodes
    u64 cnt=0;
    vec<u32> sidxs;                               // sidxs is slot indexes
    vec<u32> cidxs;                               // cidxs is connection indexes
    TO(m_selected.size(),i)
    {

      // accumulate nodes
      if(m_selected[i]){
        nidxs.push_back(node(i).id);
        auto sIter = nodeSlots(i);
        auto  nidx = sIter->first;
        for(; sIter!=slotEnd() && sIter->first==nidx; ++sIter){
          sidxs.push_back( sIter->second );
        }

        for(auto sidx : sidxs){
          if( slot(sidx).in ){
            auto destIter = m_inCncts.find(sidx);
            if(destIter != m_inCncts.end())
              cidxs.push_back(sidx);
          }else{
            auto srcIter = m_outCncts.find(sidx);
            if(srcIter != m_outCncts.end())
              cidxs.push_back(srcIter->second);
          }
        }
      }
    }

    for(auto destIdx : cidxs){
      auto delCnt = delDestCnct(destIdx);
      assert(delCnt==0 || delCnt==1);
    }

    sort(ALL(sidxs));
    for(auto sidx : sidxs){
      m_slots.erase( m_slots.begin()+sidx );
    }

    sort(ALL(nidxs));
    for(auto nidx : nidxs){
      m_nodes.erase();
    }

    //sort(ALL(nidxs));
    //for(auto nidx : nidxs){
    //  m_nodes.erase( m_nodes.begin()+nidx );
    //}

    //for(auto nidx : nidxs){
    //  m_ordr.erase(find(ALL(m_ordr), nidx));             // O(n^2)
    //}

    return cnt;
  }
  //u64    delSelected()
  //{
  //  using namespace std;
  //  
  //  // accumulate nodes
  //  // accumulate slots
  //  // accumulate dest slots
  //  // delete cncts with dest slots
  //  // delete slots
  //  // delete nodes
  //  u64 cnt=0;
  //  vec<u64> nidxs;
  //  vec<u32> sidxs;                               // sidxs is slot indexes
  //  vec<u32> cidxs;                               // cidxs is connection indexes
  //  TO(m_selected.size(),i){
  //    //if(m_selected[i]){ delNode(i); ++cnt; }
  //    if(m_selected[i]){
  //      nidxs.push_back(node(i).id);
  //      auto sIter = nodeSlots(i);
  //      auto  nidx = sIter->first;
  //      for(; sIter!=slotEnd() && sIter->first==nidx; ++sIter){
  //        sidxs.push_back( sIter->second );
  //      }
  //
  //      for(auto sidx : sidxs){
  //        if( slot(sidx).in ){
  //          auto destIter = m_inCncts.find(sidx);
  //          if(destIter != m_inCncts.end())
  //            cidxs.push_back(sidx);
  //        }else{
  //          auto srcIter = m_outCncts.find(sidx);
  //          if(srcIter != m_outCncts.end())
  //            cidxs.push_back(srcIter->second);
  //        }
  //      }
  //    }
  //  }
  //
  //  for(auto destIdx : cidxs){
  //    auto delCnt = delDestCnct(destIdx);
  //    assert(delCnt==0 || delCnt==1);
  //  }
  //
  //  sort(ALL(sidxs));
  //  for(auto sidx : sidxs){
  //    m_slots.erase( m_slots.begin()+sidx );
  //  }
  //
  //  sort(ALL(nidxs));
  //  for(auto nidx : nidxs){
  //    m_nodes.erase();
  //  }
  //
  //  //sort(ALL(nidxs));
  //  //for(auto nidx : nidxs){
  //  //  m_nodes.erase( m_nodes.begin()+nidx );
  //  //}
  //
  //  //for(auto nidx : nidxs){
  //  //  m_ordr.erase(find(ALL(m_ordr), nidx));             // O(n^2)
  //  //}
  //
  //  return cnt;
  //  
  //  //
  //  //for(auto& s : sIter){
  //
  //  //for(auto& sel : m_selected) if(sel){      
  //  //}
  //}
  auto          node(u64 id)  -> struct Node&
  {
    auto idIter = m_ids.find(id);                     // idIter is identification iterator
    if(idIter == end(m_ids)) return ERR_NODE;

    auto nIter = m_nodes.find(idIter->second);        // nIter is node iterator
    if(nIter == end(m_nodes)) return ERR_NODE;

    return nIter->second;
  }
  auto          node(u64 id) const -> struct Node const& 
  { 
    auto idIter = m_ids.find(id);                     // idIter is identification iterator
    if(idIter == end(m_ids)) return ERR_NODE;

    auto nIter = m_nodes.find(idIter->second);        // nIter is node iterator
    if(nIter == end(m_nodes)) return ERR_NODE;

    return nIter->second;
  }
  auto   orderedNode(u64 order) -> struct Node& { return m_nodes.find(order)->second; }
  bool   moveToFront(u64 id)
  {
    using namespace std;

    Node n = node(id);
    n.order;

    if(n.type==Node::NODE_ERROR) return false;
    m_nodes.erase(n.order); // todo: use a delNode here instead
    m_ids.erase(id);

    addNode(n, false);
  }
  auto      getNodes() -> NodeMap&   { return m_nodes; }
  auto         nodes() -> NodeMap&   { return m_nodes; }
  u64            nsz() const { return m_nodes.size(); }

  u64        addSlot(Slot s)
  {
    //if(s.nidx < m_nodes.size

    m_slots.push_back(s);
    u64 slotIdx = (u64)(m_slots.size() - 1);

    auto iter = m_nodeSlots.insert({s.nidx, slotIdx});
    
    return slotIdx;
  }
  bool       hasSlot(u64 sIdx)
  {
    return sIdx < m_slots.size();
  }
  auto          slot(u64 sIdx) -> Slot& 
  {
    //static Slot slot_error(0,false);  // = { {0,0}, 0, {0,0}, false, Slot::SLOT_ERROR };

    //if(sIdx<m_slots.size())
      return m_slots[sIdx];
    //else{
    //  slot_error.state = Slot::SLOT_ERROR;
    //  return slot_error;
    //}
  }
  auto     nodeSlots(u64 nIdx) -> decltype(m_nodeSlots.find(nIdx))
  {
    auto iter = m_nodeSlots.find(nIdx);
    return iter;
  }
  auto         slots() -> vec_slot& { return m_slots; }
  auto         slots() const -> vec_slot const& { return m_slots; }
  auto      getSlots() -> vec_slot& { return m_slots; }
  auto       srcSlot(u32 destSlotIdx) -> struct Slot* 
  {
    auto ci = m_inCncts.find(destSlotIdx);
    if(ci != m_inCncts.end()){
      struct Slot& s = slot(ci->second);
      return &s;
    }else 
      return nullptr;
  }
  auto     destSlots(u32 srcSlotIdx) -> decltype(m_outCncts.end())
  {
    return m_outCncts.find(srcSlotIdx);
  }
  auto       slotEnd() -> decltype(m_nodeSlots.end()) { return m_nodeSlots.end(); }
  u64            ssz() const { return m_slots.size(); }

  void       addCnct(u32 src, u32 dest)
  {
    auto srcIter = m_inCncts.find(dest);
    if(srcIter != m_inCncts.end()){
      m_outCncts.erase(src);
      m_inCncts.erase(dest);
    }

    m_outCncts.insert({src, dest});
    m_inCncts.insert({dest, src});
  }
  void    toggleCnct(u32 src, u32 dest)
  {
    if( delCnct(src,dest)==0 ){
      m_outCncts.insert({src, dest});
      m_inCncts.insert({dest, src});
    }

    //auto srcIter = m_inCncts.find(dest);
    //if(srcIter != m_inCncts.end()){
    //  auto iter = m_outCncts.find(src);
    //  for(; iter != m_outCncts.end() && iter->first==src; ){
    //    auto cpy = iter;
    //    ++iter;
    //    if(cpy->second == dest) m_outCncts.erase(cpy);
    //  }
    //  m_inCncts.erase(dest);
    //
    //}else{

    //m_outCncts.erase(src);
    //
    //if(iter != m_outCncts.end()){
    //}
    //
    //m_outCncts.erase(src);
  }
  u32        delCnct(u32 src, u32 dest)
  {
    u32 cnt=0;
    auto srcIter = m_inCncts.find(dest);
    if(srcIter != m_inCncts.end())
    {
      auto iter = m_outCncts.find(src);
      for(; iter != m_outCncts.end() && iter->first==src; ){
        auto cpy = iter;
        ++iter;
        if(cpy->second == dest){
          m_outCncts.erase(cpy);
          ++cnt;
        }
      }
      m_inCncts.erase(dest);
    }

    return cnt;
  }
  u32    delDestCnct(u32 dest)
  {
    auto iter = m_inCncts.find(dest);
    if(iter != m_inCncts.end()){
      auto src = iter->second;
      return delCnct(src, iter->first);
    }

    return 0;
  }
  auto       cnctEnd() -> decltype(m_outCncts.end())  { return m_outCncts.end(); }
  auto     cnctBegin() -> decltype(m_outCncts.begin()) { return m_outCncts.begin(); }
  auto         cncts() -> CnctMap& { return m_outCncts; }
  auto         cncts() const -> CnctMap const& { return m_outCncts; }
  u64         cnctsz() const { return m_outCncts.size(); }
  u32        delCnct(u64 sltIdx)
  {
    u32 cnt=0;
    Slot& s = slot(sltIdx);
    if(s.in){
      auto iter = m_inCncts.find(sltIdx);
      return delCnct(iter->second, iter->first);
    }else{
      auto iter = m_outCncts.find(sltIdx);
      auto  idx = iter->first;
      while(iter != m_outCncts.end() && iter->first == idx){
        m_inCncts.erase(iter->second);
        iter = m_outCncts.erase(iter);
        ++cnt;
      }
    }

    return cnt;
  }

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

  i32         order(u64 id){ return node(id).order; }
  //void        order(u64 id, i32 o)
  //{
  //  node(id).order = o;
  //}

  //i32         order(u64 idx){ return m_ordr[idx]; }
  //void        order(u64 idx, i32 o){ m_ordr[idx] = o; }

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




//n.order = m_nodes.rbegin()->order + 1;
//m_ordr.push_back( (i32)nodeIdx );

//m_nodes.push_back(n);
//u64 nodeIdx = (u64)(m_nodes.size()-1);
//m_ordr.push_back( (i32)nodeIdx );
//m_bnds.emplace_back();
//m_selected.push_back(false);
//
//return nodeIdx;

//void    moveToBack(u64 nIdx)
//{
//  using namespace std;
//
//  auto  sz = m_ordr.size();
//  auto tmp = move(m_ordr[nIdx]);
//  for(auto j=nIdx; j<sz-1; ++j)
//    m_ordr[j] = move( m_ordr[j+1] );
//
//  m_ordr[sz-1] = tmp;
//}
//auto          node(u64 nIdx)  -> struct Node& { return m_nodes[nIdx]; }
//auto          node(u64 nIdx) const -> struct Node const& { return m_nodes[nIdx]; }
//auto   orderedNode(u64 order) -> struct Node& { return m_nodes[m_ordr[order]]; }
//auto      getNodes() -> vec_nd&   { return m_nodes; }
//auto         nodes() -> vec_nd&   { return m_nodes; }

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

