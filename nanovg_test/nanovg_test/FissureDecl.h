
#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __FISSURE_DECL_HEADERGUARD_HPP__
#define __FISSURE_DECL_HEADERGUARD_HPP__

#include <set>
#include <nanogui/nanogui.h>
#include "../no_rt_util.h"
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
    return max(0.f,xmx-xmn) + max(0.f,ymx-ymn);
  }
};
using  vec_bnd     =    vec<Bnd>;

struct    Node
{
  enum Type { MSG=0, FLOW=1, NODE_ERROR };

  LavaFlowNode* lfn;                              // lfn is Lava Flow Node
  u64      id = 0;
  u64   order = 0;
  v2        P = {0,0};
  bool    sel = false;                            // sel is selected
  Bnd       b = {0, 0, 128.f, 48.f};
  str     txt = "";
  Type   type = NODE_ERROR;

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

    id    = move(r.id);
    order = move(r.order);
    P     = move(r.P);
    type  = move(r.type);
    sel   = move(r.sel);
    b     = move(r.b);
    txt   = move(r.txt);
  }

  Node(){}
  Node(str _txt, Type _type=FLOW, v2 _P=v2(0,0) ) : txt(_txt), P(_P), type(_type)
  {
    if(type==MSG) b.ymx = b.xmx;
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

class  GraphDB
{
public:
  using Id = LavaId;

  using NodeMap      = std::map<u64, Node>;                    // maps an order to a Node struct
  using NodeIdMap    = std::unordered_map<u64, u64>;           // maps a node id to its order, which can be used to find the node in the NodeMap
  using Slots        = std::multimap<Id, Slot>;                // The key is a node id, the value is the index into the slot array.  Every node can have 0 or more slots. Slots can only have 1 and only 1 node. Slots have their node index in their struct so getting the node from the slots is easy. To get the slots that a node has, this multimap is used
  using CnctMap      = std::unordered_map<Id, Id, Id>;         // maps connections from their single destination slot to their single source slot - Id is the hash function object in the third template argument
  using SrcMap       = std::multimap<Id, Id>;                  // maps connections from their single source slot to their one or more destination slots
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
    m_cncts     = move(rval.m_cncts); 
    m_destCncts = move(rval.m_destCncts);
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
    
    vec_ids sidxs;                                            // sidxs is slot indexes
    for(auto np : nds){                                       // np is node pointer and nds is nodes
      auto si = lower_bound(ALL(m_slots), Id(np->id), [](auto a,auto b){ return a.first < b; } );          // si is slot iterator
      if(si != end(m_slots)  &&  si->first.nid == np->id){
        Slot& s = si->second;
        if(s.in) sidxs.push_back(si->first);
      }
    }
    return sidxs;                                        // RVO
  }
  auto      nodeSlots(vec_nptrs const& nds) -> vec_ids
  {
    using namespace std;

    vec_ids sidxs;                                            // sidxs is slot indexes
    for(auto np : nds){                                       // np is node pointer and nds is nodes
      auto si = lower_bound(ALL(m_slots), Id(np->id), [](auto a,auto b){ return a.first < b; } );          // si is slot iterator
      if(si != end(m_slots)  &&  si->first.nid == np->id){
        Slot& s = si->second;
        sidxs.push_back(si->first);        
      }
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
  u64         nxtSlot(u64 nid)
  {
    auto si = nodeSlots(nid);                   // si is slot iterator
    u64 cur = 1;
    while(si != end(m_slots)   && 
          si->first.nid  == nid && 
          si->first.sidx <= cur+1 ){
      cur = si->first.sidx;
      ++si;
    }
    
    return cur + 1;
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
    unordered_map<u64,u64> nids;
    nids.reserve(m_ids.size());
    u64 cur = 1;
    for(auto& kv : m_ids){
      nids[kv.first] = cur++;
    }

    unordered_map<u64,u64> ordrs;
    ordrs.reserve(m_nodes.size());
    u64 curOrdr = 1;
    for(auto& kv : m_nodes){
      ordrs[kv.first] = curOrdr++;
    }

    // connections 
    CnctMap nxtCncts;
    for(auto kv : m_cncts){
      Id nxtDest = kv.first;
      Id nxtSrc  = kv.second;
      nxtDest.nid = nids[nxtDest.nid];
      nxtSrc.nid  = nids[nxtSrc.nid];
      nxtCncts.insert({nxtDest, nxtSrc});
    }
    m_cncts = move(nxtCncts);

    SrcMap nxtDestCncts;
    for(auto kv : m_destCncts){
      Id nxtSrc   = kv.first;
      Id nxtDest  = kv.second;
      nxtSrc.nid   = nids[nxtSrc.nid];
      nxtDest.nid  = nids[nxtDest.nid];
      nxtDestCncts.insert({nxtSrc, nxtDest});
    }
    m_destCncts = move(nxtDestCncts);

    // slots
    Slots nxtSlots;
    for(auto& kv : m_slots){
      Id nxtId  = kv.first;
      nxtId.nid  = nids[nxtId.nid];
      Slot nxtS = kv.second;
      nxtS.nid  = nids[nxtS.nid];
      nxtSlots.insert({nxtId, nxtS});
    }
    m_slots = move(nxtSlots);

    // node ids 
    NodeIdMap nxtIds;
    for(auto& kv : m_ids){
      u64   nxtId = nids[kv.first];
      u64 nxtOrdr = ordrs[kv.second];
      nxtIds.insert({nxtId, nxtOrdr});
    }
    m_ids = move(nxtIds);

    // and finally nodes
    NodeMap nxtOrdrs;
    for(auto& kv : m_nodes){
      Node nxtNd  = kv.second;
      nxtNd.id    = nids[nxtNd.id];
      u64 nxtOrdr = ordrs[kv.first];
      nxtOrdrs.insert({nxtOrdr, nxtNd});
    }
    m_nodes = move(nxtOrdrs);
  }
  void             clear()
  {
    m_nodes.clear();
    m_ids.clear();
    m_slots.clear();
    m_cncts.clear();
    m_destCncts.clear();
  }
  void     setNextNodeId(u64 nxt){ m_nxtId = nxt; }

  // selection
  u64        delSelected()
  {
    using namespace std;

    u64    cnt = 0;
    auto   nds = selectedNodes();      // accumulate nodes
    auto sidxs = nodeSlots(nds);   // accumulate dest slots  // accumulate slots

    // delete cncts with dest slots
    for(auto sidx : sidxs){ 
      auto s = slot(sidx);
      if(s->in)
        delDestCnct(sidx);  //){ ++cnt; }
      else
        delSrcCncts(sidx);
    }

    // delete slots
    for(auto sidx : sidxs){ m_slots.erase(sidx); }

    // delete nodes
    for(auto n : nds){
      m_ids.erase(n->id);
      m_nodes.erase(n->order);
    }

    return cnt;
  }
  void         clearSels()
  {
    for(auto& on : m_nodes) on.second.sel = false;
    for(auto& kv : m_slots) kv.second.state = Slot::NORMAL;
  }
  void     clearSlotSels()
  {
    for(auto& kv : m_slots) kv.second.state = Slot::NORMAL;
  }

  // nodes
  auto       addNode(Node n, bool newId=true) -> Node&
  {
    if(newId) n.id = nxt();

    n.order    = nxtOrder();
    Node& nref = m_nodes.insert({n.order, n}).first->second;

    m_ids.insert({n.id, n.order});
    
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
  u64          maxId()
  {
    using namespace std;
    
    u64 mx = 0;
    for(auto kv : m_ids){ mx = max(mx, kv.first); }

    return mx;
  }
  u64            nsz() const { return m_nodes.size(); }

  // slots
  Id         addSlot(Slot  s, u64 idx=0)
  {
    Id id(s.nid, idx);
    id.sidx = idx? idx  :  nxtSlot(s.nid);
    m_slots.insert({id, s});

    return id;
  }
  auto          slot(Id   id) -> Slot*
  {
    auto si = m_slots.find(id);
    if(si == m_slots.end()) 
      return nullptr;
  
    return &si->second;
  }
  auto     nodeSlots(u64 nid) -> decltype(m_slots.begin()) // C++14 -> decltype(m_slots.find(Id(nid)))
  {
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
    u32  cnt = 0;
    auto  di = m_destCncts.find(src);
    for(; di!=m_destCncts.end() && di->first==src; ++cnt, ++di){   // ++di,
      m_cncts.erase(di->second);
    }
    m_destCncts.erase(src);

    return cnt;

    //di = m_destCncts.find(src);
  }
  auto     destCncts(Id  src) -> decltype(m_destCncts.begin()) // C++14 -> decltype(m_slots.find(Id(nid)))
  {
    return lower_bound(ALL(m_destCncts), Id(src), [](auto a,auto b){ return a.first < b; } );
  }
  u64  destCnctCount(Id  src)
  {
    return m_destCncts.count(src);
  }
  bool   delDestCnct(Id dest)
  {
    auto iter = m_cncts.find(dest);
    if(iter==m_cncts.end()) return false;

    auto src = iter->second;
    m_cncts.erase(dest);

    auto srcIter = m_destCncts.find(src);
    for(; srcIter!=end(m_destCncts) && srcIter->first==src && srcIter->second==dest; ){
      auto cpy = srcIter++;
      m_destCncts.erase(cpy);
    }

    return true;
  }
  u32        delCnct(Id  src, Id  dest)
  {
    u32      cnt = 0;
    auto srcIter = m_cncts.find(dest);
    if(srcIter != m_cncts.end())
    {
      auto iter = m_destCncts.find(src);
      for(; iter != m_destCncts.end() && iter->first == src; ){
        auto cpy = iter++;          // ++iter;
        if(cpy->second == dest){
          m_destCncts.erase(cpy);
          ++cnt;
        }
      }
      m_cncts.erase(dest);
    }

    return cnt;
  }
  void    toggleCnct(Id  src, Id  dest)
  {
    u32    delcnt = 0;
    auto       di = m_cncts.find(dest);
    bool matchSrc = false;
    if( di != end(m_cncts) ){
      matchSrc = src == di->second;
      delcnt   = delCnct(di->second,dest);
    }

    //if(delcnt==0 || !matchSrc ){
    if( !(matchSrc && delcnt>0) ){
      m_cncts.insert({dest, src});
      m_destCncts.insert({src, dest});
    }
    //delCnct(src,dest)==0 
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
  auto   srcCnctsMap() -> SrcMap&   //decltype(m_destCncts)
  {
    return m_destCncts;
  }
};


struct FisData
{
  struct IdOrder {
    u64      id = 0;
    u64   order = 0;
    bool operator<(IdOrder l) const { return order < l.order; }
  };

  using         Id  =  LavaId;
  using    NodeMap  =  std::unordered_map<uint64_t, Node>;
  using  NodeOrder  =  std::set<IdOrder>;
  using      Slots  =  std::multimap<LavaId, Slot>;            // The key is a node id, the value is the index into the slot array.  Every node can have 0 or more slots. Slots can only have 1 and only 1 node. Slots have their node index in their struct so getting the node from the slots is easy. To get the slots that a node has, this multimap is used

  GLFWwindow*         win = nullptr;                            // Platform 
  LavaGraph         lgrph;
  LavaFlow             lf;
  //GraphDB            grph;

  struct Graph
  {
    NodeMap       nds;       // nds  is nodes
    NodeOrder    ordr;       // ordr is orders 
    Slots       slots;       // 

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
    Screen         screen;
    Window*        keyWin = nullptr;
    Window*         dbWin = nullptr;
    BoxLayout*     keyLay = nullptr;
    vec_btn        ndBtns;

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
    NVGcolor    msgnd_color  =  nvgRGBAf(.2f, .3f, .375f, 1.f);
    NVGcolor   msgnd_selclr  =  nvgRGBAf(.5f,.4f,.1f, .75f);
    NVGcolor   msgnd_gradst  =  nvgRGBAf(.3f, .3f, .3f, .5f);
    NVGcolor   msgnd_graden  =  nvgRGBAf(.15f, .15f, .15f, .45f); 

    f32           nd_border  =   3.5f;
    f32            slot_rad  =   15.f;
    f32         slot_border  =   3.5f;
  } ui;
  struct
  {
    v2                prevPntr;
    bool                  rtDn = false;    // right mouse button down
    bool                 lftDn = false;    // left mouse button down
    bool              prevRtDn = false;    // right mouse button down
    bool             prevLftDn = false;    // left mouse button down

    v2                    drgP;
    v2                 drgofst;
    bool                drgbox = false;
    bool                 drgNd = false;
  } mouse;
  struct 
  {
    i64        pri = -1;
    i64        sec = -1;
    Id   slotInSel;
    Id  slotOutSel;
  } sel;
};

#endif












//
//using         Id  =  LavaGraph::Id;

//using Id = LavaGraph::Id;
//
//union Id                                                // this Id serves as both a nodeId and slot index, since a slot index will alway coordinate with only one node 
//{    
//  struct { 
//    u64 id  : 48;                                       // id is the node id number - This is a unique number for each node that doesn't change. It can refer back to a node since it doesn't represent an ordering or an index into an array 
//    u64 idx : 16;                                       // idx is the index of the slot - for a node this is 0
//  };
//  u64 asInt;
//
//  Id() : id(0), idx(0) {}
//  Id(u64 _id, u64 _idx=0) : id(_id), idx(_idx) {}
//
//  bool   operator==(Id  r) const { return id==r.id && idx==r.idx; }
//  bool    operator<(Id const& r) const {
//    if(id==r.id) return idx < r.idx;
//    else         return id  < r.id;
//  }
//  size_t operator()(Id const& _id) const {
//    return std::hash<u64>()(_id.id) ^ std::hash<u64>()(_id.idx);
//  }
//};

//struct    Node
//{
//  enum Type { MSG=0, FLOW=1, NODE_ERROR };
//
//  u64      id = 0;
//  u64   order = 0;
//  v2        P = {0,0};
//  Type   type = NODE_ERROR;
//  bool    sel = false;                            // sel is selected
//  Bnd       b = {0, 0, 128.f, 48.f};
//  str     txt = ""; 
//
//  void cp(Node const& l)
//  {
//    id    = l.id;
//    order = l.order;
//    P     = l.P;
//    type  = l.type;
//    sel   = l.sel;
//    b     = l.b;
//    txt   = l.txt;
//  }
//  void mv(Node&&      r)
//  {
//    using namespace std;
//    
//    id    = move(r.id);
//    order = move(r.order);
//    P     = move(r.P);
//    type  = move(r.type);
//    sel   = move(r.sel);
//    b     = move(r.b);
//    txt   = move(r.txt);
//  }
//
//  Node(){}
//  Node(str _txt, Type _type=FLOW, v2 _P=v2(0,0) ) : txt(_txt), P(_P), type(_type) {}
//  Node(Node const& l){ cp(l); }
//  Node(Node&& r){ mv(std::move(r)); }
//
//  bool operator<(Node const& l){ return l.order; }
//};

//void       addCnct(Id src, Id dest)
//{
//  auto ci = m_cncts.find(dest);
//  if(ci != m_cncts.end()){
//    m_cncts.erase(dest);
//    m_destCncts.erase(src);
//  }
//
//  m_cncts.insert({dest, src});
//  m_destCncts.insert({src, dest});
//}

//const v2      NODE_SZ       = { 256.f, 64.f };
//const v2      NODE_HALF_SZ  = NODE_SZ/2.f;

//const auto    NODE_CLR      = nvgRGBf(.2f, .3f, .375f);
//const float   BORDER        = 3.5f;
//const f32     IORAD         = 15.f;

//
//u64  = si->first.id;

//vecui sidxs;                                            // sidxs is slot indexes
//
//auto sidx = si->second;
//if(slot(sidx).in) sidxs.push_back(sidx);
//
//auto si = m_slots.find(np->id);                   // si is slot iterator
//if(si != end(m_slots)){
//  auto sidx = si->second;
//  if(slot(sidx).in) sidxs.push_back(sidx);
//}

//vecui sidxs;                                            // sidxs is slot indexes
//
//if(s.in) sidxs.push_back(si->first);
//
//auto sidx = si->second;
//if(slot(sidx).in) sidxs.push_back(sidx);
//
//auto si = m_slots.find(np->id);                   // si is slot iterator
//if(si != end(m_slots)){
//  auto sidx = si->second;
//  if(slot(sidx).in) sidxs.push_back(sidx);
//}

//m_slots.clear();
//m_selected.clear();

//auto sidxs = nodeDestSlots(nds);   // accumulate dest slots  // accumulate slots
//if( delDestCnct(sidx) ){ ++cnt; }

//m_slots.erase(ALL(sidxs));
//for(auto sidx : sidxs){ m_slots.erase( m_slots.begin() + sidx ); }

//u64 nodeIdx = (u64)(m_nodes.size()-1);
//
//m_selected.push_back(false);

//delCnct(src, dest);
//
//auto iter = m_destCncts.find(dest);
//if(iter == m_destCncts.end()) return false;
//
//auto src = iter->second;
//delCnct(src, iter->first);

//
//auto iter = m_destCncts.find(id);

//m_slots     = move(rval.m_slots); 
//m_selected  = move(rval.m_selected); 
//m_bnds      = move(rval.m_bnds); 

//
//m_slots.insert({id, s});                   // todo: make this find the last slot idx and make it sequential

//auto iter = m_slots.find(id);
//return iter;

//vec<u64> nids;
//nids.reserve(m_nodes.size());
//for(auto& kv : m_nodes) nids.push_back(kv.first);

//
//u64   nxtId = nids[kv.first];

//u64 nxtId = nids[kv.first];
//nxtNodes.insert({nxtId, kv.second});
//kv.second.id = nxt;
//nxtIds.insert({nxt, kv.first});

//CnctMap nxtCncts;
//TO(nids.size(),i){
//  u64 nid = nids[i];
//  auto ci = m_cncts.find(Id(nid));
//  for(; ci != m_cncts.end() && ci->first.id==nid; ++ci){
//    Id nxtId = ci->first;
//    //nxtId.idx = 
//    //nxtId.idx = ci->first.idx;
//    nxtCncts.insert({nxtId, move(ci->second)});
//  }
//}
//m_cncts = move(nxtCncts);

//SrcMap nxtDestCncts;
//TO(nids.size(),i){
//  u64 nid = nids[i];
//  auto ci = m_destCncts.find(Id(nid));
//  for(; ci != m_destCncts.end() && ci->first.id==nid; ++ci){
//    Id nxtId(i+1);
//    nxtId.idx = ci->first.idx;
//    nxtDestCncts.insert({nxtId, move(ci->second)});
//  }
//}
//m_destCncts = move(nxtDestCncts);

//TO(nids.size(),i)
//{
//  u64 nid = nids[i];
//  auto si = nodeSlots(nid);
//  for(; si != m_slots.end() && si->first.id==nid; ++si){
//    Id nxtId(i+1);
//    nxtId.idx = si->first.idx;
//    nxtSlots.insert({nxtId, move(si->second)});
//  }
//}
//m_slots = move(nxtSlots);

//Slots nxtSlots;
//TO(nids.size(),i)
//{
//  u64 nid = nids[i];
//  auto si = nodeSlots(nid);
//  for(; si != m_slots.end() && si->first.id==nid; ++si){
//    Id nxtId(i+1);
//    nxtId.idx = si->first.idx;
//    nxtSlots.insert({nxtId, move(si->second)});
//  }
//}
//m_slots = move(nxtSlots);

//NodeMap nxtNodes;
//TO(nids.size(),i){
//  u64 nid = nids[i];
//  auto ni = m_nodes.find(nid);
//  for(; ni != m_nodes.end() && ni->first==nid; ++ni){
//    u64 nxtId = i+1;
//    nxtNodes.insert({nxtId, move(ni->second)});
//  }
//}
//m_nodes = move(nxtNodes);

//auto si = m_slots.find(Id(nid));
//for(; si != m_slots.end() && si->first.id==nid; ++si){
//auto  s = slot(nid);

//for(auto& kv : m_cncts){
//  kv.first.id = 
//}
//
//
//binary_search(ALL(nids), val);
//
//for(auto nid : nids){
//m_cncts.erase(ci);
