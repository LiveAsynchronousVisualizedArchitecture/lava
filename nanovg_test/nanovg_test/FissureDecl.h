
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
using str  =  std::string;

using e2i  =  Eigen::Vector2i;
using e4f  =  Eigen::Vector4f;

using     veci     =    vec<i32>;
using    vecui     =    vec<u32>;
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
  Type   type = NODE_ERROR;
  bool    sel = false;                            // sel is selected
  Bnd       b;
  str     txt = ""; 

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
  void mv(Node&& r)
  {
    using namespace std;
    
    id    = move(r.id);
    order = move(r.order);
    P     = move(r.P);
    type  = move(r.type);
    sel   = move(r.sel);
    b     = move(r.b);
    txt   = move(r.txt);
  }

  Node(){}
  Node(str _txt, Type _type=FLOW, v2 _P=v2(0,0) ) : txt(_txt), P(_P), type(_type) {}
  Node(Node const& l){ cp(l); }
  Node(Node&& r){ mv(std::move(r)); }

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

  v2 P; u64 nid; v2 N; bool in=false; State state=NORMAL;

  Slot(u64 nId, bool In=false) : nid(nId), in(In), P(0,0), N(0,1), state(NORMAL) {}
};
using vec_slot     =    vec<Slot>;

class  GraphDB
{
public:
  union Id                                                 // this Id serves as both a nodeId and slot index, since a slot index will alway coordinate with only one node 
  {    
    struct { 
      u64 id  : 48;                                       // id is the node id number - This is a unique number for each node that doesn't change. It can refer back to a node since it doesn't represent an ordering or an index into an array 
      u64 idx : 16;                                       // idx is the index of the slot - for a node this is 0
    };
    u64 asInt;

    Id() : id(0), idx(0) {}
    Id(u64 _id, u64 _idx=0) : id(_id), idx(_idx) {}

    bool   operator==(Id  r) const { return id==r.id && idx==r.idx; }
    bool    operator<(Id const& r) const {
      if(id==r.id) return idx < r.idx;
      else         return id  < r.id;
    }
    size_t operator()(Id const& _id) const {
      return std::hash<u64>()(_id.id) ^ std::hash<u64>()(_id.idx);
    }
  };

  using NodeMap      = std::map<u64, Node>;             // maps an order to a Node struct
  using NodeIdMap    = std::unordered_map<u64, u64>;    // maps a node id to its order, which can be used to find the node in the NodeMap
  using Slots        = std::multimap<Id, Slot>;         // The key is a node id, the value is the index into the slot array.  Every node can have 0 or more slots. Slots can only have 1 and only 1 node. Slots have their node index in their struct so getting the node from the slots is easy. To get the slots that a node has, this multimap is used
  using CnctMap      = std::unordered_map<Id, Id, Id>;  // Id is the hash function object in the third template argument - maps connections from their single destination slot to their single source slot 
  using SrcMap       = std::multimap<Id, Id>;           // maps connections from their single source slot to their one or more destination slots
  using vec_nptrs    = std::vector<Node*>;
  using vec_cnptrs   = std::vector<Node const*>;
  using vec_ids      = std::vector<Id>;

private:
  u64                m_nxtId;               // nxtId is next id - a counter for every node created that only increases, giving each node a unique id
  NodeMap            m_nodes;
  NodeIdMap            m_ids;
  Slots              m_slots;
  CnctMap            m_cncts;
  SrcMap         m_destCncts;

  void           init(){}
  void             mv(GraphDB&& rval)
  {
    using namespace std;

    m_nodes     = move(rval.m_nodes); 
    m_ids       = move(rval.m_ids);
    m_slots     = move(rval.m_slots); 
    m_cncts  = move(rval.m_cncts); 
    m_destCncts   = move(rval.m_destCncts);

    //m_slots     = move(rval.m_slots); 
    //m_selected  = move(rval.m_selected); 
    //m_bnds      = move(rval.m_bnds); 
  }
  u64             nxt(){ return m_nxtId++; }
  u64        nxtOrder()
  {
    u64 order = 1;
    if(m_nodes.size() > 0)
      order = m_nodes.rbegin()->first + 1;

    return order;
  }
  auto  selectedNodes() -> vec_nptrs
  {
    vec<Node*> nds;                                     // nids is node ids
    for(auto& on : m_nodes){                            // on is order and node - order is on.first    node is on.second
      if(on.second.sel) nds.push_back(&on.second);
    }

    return nds;                                         // counting on RVO (return value optimization) here
  }
  auto  nodeDestSlots(vec_nptrs const& nds) -> vec_ids
  {
    using namespace std;
    
    //vecui sidxs;                                            // sidxs is slot indexes
    vec_ids sidxs;                                            // sidxs is slot indexes
    for(auto np : nds){                                       // np is node pointer and nds is nodes
      auto si = lower_bound(ALL(m_slots), Id(np->id), [](auto a,auto b){ return a.first < b; } );          // si is slot iterator
      if(si != end(m_slots)  &&  si->first.id == np->id){
        Slot& s = si->second;
        if(s.in) sidxs.push_back(si->first);

        //auto sidx = si->second;
        //if(slot(sidx).in) sidxs.push_back(sidx);
      }

      //auto si = m_slots.find(np->id);                   // si is slot iterator
      //if(si != end(m_slots)){
      //  auto sidx = si->second;
      //  if(slot(sidx).in) sidxs.push_back(sidx);
      //}
    }
    return sidxs;                                        // RVO
  }
  auto  nodeSlots(vec_nptrs const& nds) -> vec_ids
  {
    using namespace std;

    //vecui sidxs;                                            // sidxs is slot indexes
    vec_ids sidxs;                                            // sidxs is slot indexes
    for(auto np : nds){                                     // np is node pointer and nds is nodes
      auto si = lower_bound(ALL(m_slots), Id(np->id), [](auto a,auto b){ return a.first < b; } );          // si is slot iterator
      if(si != end(m_slots)  &&  si->first.id == np->id){
        Slot& s = si->second;
        sidxs.push_back(si->first);
        
        //if(s.in) sidxs.push_back(si->first);
        //
        //auto sidx = si->second;
        //if(slot(sidx).in) sidxs.push_back(sidx);
      }

      //auto si = m_slots.find(np->id);                   // si is slot iterator
      //if(si != end(m_slots)){
      //  auto sidx = si->second;
      //  if(slot(sidx).in) sidxs.push_back(sidx);
      //}
    }
    return sidxs;                                        // RVO
  }
  auto      errorNode() -> Node&
  { // default contrutor sets the type to Node::NODE_ERROR
    static Node ERR_NODE;
    return ERR_NODE; 
  }
  auto      errorNode() const -> Node const&
  { // default contrutor sets the type to Node::NODE_ERROR
    static Node ERR_NODE;
    return ERR_NODE; 
  }

public:
  GraphDB() : m_nxtId(1) { init(); }
  GraphDB(GraphDB&& rval){ mv(std::move(rval)); }
  GraphDB& operator=(GraphDB&& rval){ mv(std::move(rval)); return *this; }

  // global
  void  normalizeIndices()
  {
    using namespace std;
    
    // create a mapping of old node Ids to new ones, new ones will be their position + 1
    vec<u64> nids;
    nids.reserve(m_nodes.size());
    for(auto& kv : m_nodes) nids.push_back(kv.first);

    // connections 
    CnctMap nxtCncts;
    TO(nids.size(),i){
      u64 nid = nids[i];
      auto ci = m_cncts.find(Id(nid));
      for(; ci != m_cncts.end() && ci->first.id==nid; ++ci){
        Id nxtId(i+1);
        nxtId.idx = ci->first.idx;
        nxtCncts.insert({nxtId, move(ci->second)});
      }
    }
    m_cncts = move(nxtCncts);

    SrcMap nxtDestCncts;
    TO(nids.size(),i){
      u64 nid = nids[i];
      auto ci = m_destCncts.find(Id(nid));
      for(; ci != m_destCncts.end() && ci->first.id==nid; ++ci){
        Id nxtId(i+1);
        nxtId.idx = ci->first.idx;
        nxtDestCncts.insert({nxtId, move(ci->second)});
      }
    }
    m_destCncts = move(nxtDestCncts);

    // slots
    //auto si = m_slots.find(Id(nid));
    //for(; si != m_slots.end() && si->first.id==nid; ++si){
    //auto  s = slot(nid);
    Slots nxtSlots;
    TO(nids.size(),i)
    {
      u64 nid = nids[i];
      auto si = nodeSlots(nid);
      for(; si != m_slots.end() && si->first.id==nid; ++si){
        Id nxtId(i+1);
        nxtId.idx = si->first.idx;
        nxtSlots.insert({nxtId, move(si->second)});
      }
    }
    m_slots = move(nxtSlots);

    // and finally nodes
    NodeMap nxtNodes;
    TO(nids.size(),i){
      u64 nid = nids[i];
      auto ni = m_nodes.find(nid);
      for(; ni != m_nodes.end() && ni->first==nid; ++ni){
        u64 nxtId = i+1;
        nxtNodes.insert({nxtId, move(ni->second)});
      }
    }
    m_nodes = move(nxtNodes);

    //for(auto& kv : m_cncts){
    //  kv.first.id = 
    //}
    //
    //
    //binary_search(ALL(nids), val);
    //
    //for(auto nid : nids){
    //m_cncts.erase(ci);
  }
  void             clear()
  {
    m_nodes.clear();
    m_ids.clear();
    m_slots.clear();
    m_cncts.clear();
    m_destCncts.clear();

    //m_slots.clear();
    //m_selected.clear();
  }

  // selection
  u64        delSelected()
  {
    using namespace std;

    u64    cnt = 0;
    auto   nds = selectedNodes();      // accumulate nodes
    //auto sidxs = nodeDestSlots(nds);   // accumulate dest slots  // accumulate slots
    auto sidxs = nodeSlots(nds);   // accumulate dest slots  // accumulate slots

                                       // delete cncts with dest slots
    for(auto sidx : sidxs){ 
      //if( delDestCnct(sidx) ){ ++cnt; }
      auto s = slot(sidx);
      if(s->in) delDestCnct(sidx);  //){ ++cnt; }
      else      delSrcCncts(sidx);
    }

    // delete slots
    for(auto sidx : sidxs){ m_slots.erase(sidx); }

    // delete nodes
    for(auto n : nds){
      m_ids.erase(n->id);
      m_nodes.erase(n->order);
    }

    return cnt;

    //m_slots.erase(ALL(sidxs));
    //for(auto sidx : sidxs){ m_slots.erase( m_slots.begin() + sidx ); }
  }
  void         clearSels()
  {
    for(auto& on : m_nodes) on.second.sel = false;
    for(auto& kv : m_slots) kv.second.state = Slot::NORMAL;
    //for(auto& slt : m_slots) slt.state = Slot::NORMAL;
  }

  // nodes
  auto       addNode(Node n, bool newId=true) -> Node&
  {
    if(newId) n.id = nxt();

    n.order    = nxtOrder();
    Node& nref = m_nodes.insert({n.order, n}).first->second;
    //u64 nodeIdx = (u64)(m_nodes.size()-1);

    m_ids.insert({n.id, n.order});
    
    //m_selected.push_back(false);

    return nref;
  }
  auto          node(u64 id)  -> struct Node&
  {
    auto idIter = m_ids.find(id);                     // idIter is identification iterator
    if(idIter == end(m_ids)) return errorNode();

    auto nIter = m_nodes.find(idIter->second);        // nIter is node iterator
    if(nIter == end(m_nodes)) return errorNode();

    return nIter->second;
  }
  auto          node(u64 id) const -> struct Node const& 
  { 
    auto idIter = m_ids.find(id);                     // idIter is identification iterator
    if(idIter == end(m_ids)) return errorNode();      // ERR_NODE;

    auto nIter = m_nodes.find(idIter->second);        // nIter is node iterator
    if(nIter == end(m_nodes)) return errorNode();     // ERR_NODE;

    return nIter->second;
  }
  auto   moveToFront(u64 id) -> Node&
  {
    using namespace std;

    Node  n = node(id);
    auto prevOrder = n.order;   // addNode will get the next order number
    //n.order = nxtOrder();

    if(n.type==Node::NODE_ERROR) return errorNode();
    m_nodes.erase(prevOrder); // todo: use a delNode here instead
    m_ids.erase(id);

    return addNode(n, false);
  }
  auto         nodes() -> vec_nptrs
  {
    vec_nptrs nds;
    nds.reserve(m_nodes.size());
    for(auto& on : m_nodes){                            // on is order and node - order is on.first    node is on.second
      nds.push_back(&on.second);
    }

    return nds;
  }
  auto         nodes() const -> vec_cnptrs  // these two versions could be done with a single template
  {
    vec_cnptrs nds;
    nds.reserve(m_nodes.size());
    for(auto& on : m_nodes){                            // on is order and node - order is on.first    node is on.second
      nds.push_back(&on.second);
    }

    return nds;
  }
  u64          order(u64 id){ return node(id).order; }
  auto           bnd(u64 id) -> Bnd& { return node(id).b; }
  u64            nsz() const { return m_nodes.size(); }

  // slots
  u64        addSlot(Slot  s)
  {
    //if(s.nidx < m_nodes.size

    //m_slots.push_back(s);
    //{s.nid,1}
    Id id(s.nid);
    id.idx = 1;
    m_slots.insert({id, s}); // todo: make this find the last slot idx and make it sequential

    //u64 slotIdx = (u64)(m_slots.size() - 1);
    //auto iter = m_slots.insert({s.nid, slotIdx});
    //return slotIdx;

    return 0;
  }
  auto          slot(Id   id) -> Slot*
  {
    auto si = m_slots.find(id);
    if(si == m_slots.end()) return nullptr;   // errorSlot();
  
    return &si->second;
    //return m_slots[sIdx];
  }
  auto     nodeSlots(u64 nid) -> decltype(m_slots.begin()) // C++14 -> decltype(m_slots.find(Id(nid)))
  {
    //auto iter = m_slots.find(id);
    //return iter;

    return lower_bound(ALL(m_slots), Id(nid), [](auto a,auto b){ return a.first < b; } );
  }
  auto         slots() -> Slots& { return m_slots; }
  auto         slots() const -> Slots const& { return m_slots; }
  auto      getSlots() -> Slots& { return m_slots; }
  auto       srcSlot(Id destId) -> Slot* 
  {
    auto ci = m_cncts.find(destId);
    if(ci != m_cncts.end()){
      Slot* s = slot(ci->second);
      return s;
    }else 
      return nullptr;
  }
  auto     destSlots(Id  srcId) -> decltype(m_destCncts.find(srcId))
  {
    return m_destCncts.find(srcId);
  }
  auto       slotEnd() -> decltype(m_slots.end()) { return m_slots.end(); }
  u64            ssz() const { return m_slots.size(); }

  // connections
  u32    delSrcCncts(Id  src)
  {
    u32      cnt = 0;
    auto destIter = m_destCncts.find(src);
    if(destIter != m_destCncts.end())
    {
      auto iter = m_cncts.find(destIter->second);
      //auto dest = iter->second;
      for(; iter!=m_cncts.end() && iter->first==src; ++cnt){
        auto cpy = iter++; //++iter;
        m_cncts.erase(cpy);
        //++cnt;
        //if(cpy->second == dest){
        //}
      }
      m_destCncts.erase(src);
    }

    return cnt;
  }

  auto     destCncts(Id src) -> decltype(m_destCncts.begin()) // C++14 -> decltype(m_slots.find(Id(nid)))
  {
    return lower_bound(ALL(m_destCncts), Id(src), [](auto a,auto b){ return a.first < b; } );
  }
  u32        delCnct(Id  src, Id  dest)
  {
    u32      cnt = 0;
    auto srcIter = m_destCncts.find(dest);
    if(srcIter != m_destCncts.end())
    {
      auto iter = m_cncts.find(src);
      for(; iter != m_cncts.end() && iter->first == src; ){
        auto cpy = iter;
        ++iter;
        if(cpy->second == dest){
          m_cncts.erase(cpy);
          ++cnt;
        }
      }
      m_destCncts.erase(dest);
    }

    return cnt;
  }
  bool   delDestCnct(Id dest)
  {
    auto iter = m_destCncts.find(dest);
    if(iter == m_destCncts.end()) return false;

    auto src = iter->second;
    delCnct(src, iter->first);

    return true;
  }
  void       addCnct(Id src, Id dest)
  {
    auto destIter = m_destCncts.find(src);
    if(destIter != m_destCncts.end()){
      m_cncts.erase(dest);
      m_destCncts.erase(src);
    }

    m_cncts.insert({dest, src});
    m_destCncts.insert({src, dest});
  }
  void    toggleCnct(Id src, Id dest)
  {
    if( delCnct(src,dest)==0 ){
      m_cncts.insert({dest, src});
      m_destCncts.insert({src, dest});
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
  auto   destCnctEnd() -> decltype(m_destCncts.end())  { return m_destCncts.end(); }
  auto       cnctEnd() -> decltype(m_cncts.end())  { return m_cncts.end(); }
  auto     cnctBegin() -> decltype(m_cncts.begin()) { return m_cncts.begin(); }
  auto         cncts() -> CnctMap& { return m_cncts; }
  auto         cncts() const -> CnctMap const& { return m_cncts; }
  u64         cnctsz() const { return m_cncts.size(); }
  u32        delCnct(Id id)
  {
    u32 cnt = 0;
    Slot* s = slot(id);
    if(!s) return 0;

    if(s->in){
      //auto iter = m_destCncts.find(id);
      auto iter = m_cncts.find(id);
      return delCnct(iter->second, iter->first);
    }else{
      auto iter = m_destCncts.find(id);
      auto  idx = iter->first;
      while(iter != m_destCncts.end() && iter->first == idx){
        m_cncts.erase(iter->second);
        iter = m_destCncts.erase(iter);
        ++cnt;
      }
    }

    return cnt;
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











//auto ci = m_inCncts.find(destId);
//if(ci != m_inCncts.end()){
//  Slot* s = slot(ci->second);
//  return s;
//}else 
//  return nullptr;

//using CnctMap      = std::multimap<u32, u32>;            // maps connections from their single source slot to their one or more destination slots
//using SrcMap       = std::unordered_map<u32, u32>;       // maps connections from their single destination slot to their single source slot 
//using NodeSlotMap  = std::multimap<Id, u64>;             // The key is a node id, the value is the index into the slot array.  Every node can have 0 or more slots. Slots can only have 1 and only 1 node. Slots have their node index in their struct so getting the node from the slots is easy. To get the slots that a node has, this multimap is used
//using SlotIdxs     = std::set<Id>;
//using SlotMap
//using NodeSlotMap  = std::multimap<u64, u64>;            // The key is a node id, the value is the index into the slot array.  Every node can have 0 or more slots. Slots can only have 1 and only 1 node. Slots have their node index in their struct so getting the node from the slots is easy. To get the slots that a node has, this multimap is used.
//
//vec<bool>       m_selected;             // bitfield for selected nodes
//vec_slot           m_slots;
//SlotIdxs           m_slots;
//NodeSlotMap    m_nodeSlots;

//bool       hasSlot(u64 sIdx)
//{
//  return sIdx < m_slots.size();
//}
//auto          slot(u64 sIdx) -> Slot& 
//{
//  return m_slots[sIdx];
//}
//bool   delDestCnct(u32 dest)
//{
//  auto iter = m_inCncts.find(dest);
//  if(iter == m_inCncts.end()) return false;
//
//  auto src = iter->second;
//  delCnct(src, iter->first);
//
//  return true;
//}
//
//u64         selsz() const { return m_selected.size(); }

//static Id nxtId()
//{
//  static u64 curId = 1;
//
//  Id ret;
//  ret.id  = curId++;
//  ret.idx = 0;
//
//  return ret;
//}

//static Slot slot_error(0,false);  // = { {0,0}, 0, {0,0}, false, Slot::SLOT_ERROR };
//
//if(sIdx<m_slots.size())
//
//else{
//  slot_error.state = Slot::SLOT_ERROR;
//  return slot_error;
//}

//void       delNode(u64 id)
//{
//  Node&     n = this->node(id);
//  auto  sIter = nodeSlots(id);                          // sIter is slot iterator
//  auto sltCpy = sIter;
//  auto sltIdx = sIter->first;
//  for(; sltCpy!=slotEnd() && sltCpy->first==sltIdx; ++sltCpy){
//    
//  }
//}
//
//auto   orderedNode(u64 order) -> struct Node& { return m_nodes.find(order)->second; }

//using vecui
//using NodeSlotMap  = std::multimap<u64, u64>;          // The key is the index into the node array, the value is the index into the slot array.  Every node can have 0 or more slots. Slots can only have 1 and only 1 node. Slots have their node index in their struct so getting the node from the slots is easy. To get the slots that a node has, this multimap is used.
//using NodeSet      = std::set<Node>;
//using SrcMap      = std::multimap<u32, u32>;           // todo: change this to a single map
//
//static Node ERR_NODE;

//vec_bnd             m_bnds;
//vec_nd             m_nodes;
//veci                m_ordr;

//bool          sel(u64 idx){ return m_selected[idx]; }
//void          sel(u64 idx, bool s){ m_selected[idx] = s; }
//auto         sels() -> vec<bool>& { return m_selected; }
//
//for(auto& sel : m_selected) sel = false;

//u64 order = 1;
//if(m_nodes.size()!=0){ order = m_nodes.rbegin()->first + 1; }
//
//m_bnds.emplace_back();
//
//m_bnds.clear();
//
//auto      getNodes() -> NodeMap&   { return m_nodes; }
//auto         nodes() -> NodeMap&   { return m_nodes; }
//auto       bounds() -> vec_bnd& { return m_bnds; }

//bnd       B;
//LavaNode ln;

// accumulate connections from destination slots
//vec<u32> sidxs;                               // sidxs is slot indexes
//vec<u32> cidxs;                               // cidxs is connection indexes
//auto iter = find(ALL(m_slots), sidx);

//for(auto kv : m_inCncts){
//  auto ds = m_inCncts.find(sidx);  // ds is destination / source iterator
//  if(ds != end(m_inCncts)){
//    ds->second;
//  }
//}

//void        order(u64 id, i32 o)
//{
//  node(id).order = o;
//}

//i32         order(u64 idx){ return m_ordr[idx]; }
//void        order(u64 idx, i32 o){ m_ordr[idx] = o; }

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

