

#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __LAVAFLOW_HEADERGUARD_HPP__
#define __LAVAFLOW_HEADERGUARD_HPP__

#include <cstdint>
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>
#include <string>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <regex>
#include <filesystem>
#include "no_rt_util.h"
#include "tbl.hpp" // temp

#if defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #define NOMINMAX
  #include <Windows.h>
#endif

#define LAVA_ARG_COUNT 512

#if defined(_WIN32) // todo: actually needs to be done by compiler and not OS
  namespace fs = std::tr2::sys;                                                             // todo: different compiler versions would need different filesystem paths
#endif

// data types
struct       LavaNode;
struct     LavaParams;
struct        LavaVal;
struct        LavaOut;
union          LavaId;
class       LavaGraph;
struct        LavaMem;

using str                =  std::string;
using lava_handle        =  HMODULE;                                                      // maps handles to the LavaFlowNode pointers contained in the shared libraries
using lava_paths         =  std::vector<std::string>;
using lava_hndlNodeMap   =  std::unordered_multimap<lava_handle, LavaNode*>;              // maps handles to the LavaFlowNode pointers contained in the shared libraries
using lava_libHndls      =  std::unordered_map<LavaNode*, lava_handle>;                   // todo: need to change this depending on OS
using lava_hndlvec       =  std::vector<lava_handle>;                                     // todo: need to change this depending on OS
using lava_pathHndlMap   =  std::unordered_map<std::string, lava_handle>;                 // maps LavaFlowNode names to their OS specific handles
using lava_flowNodes     =  std::unordered_multimap<std::string, LavaNode*>;              // maps LavaFlowNode paths to their pointers
using lava_nidMap        =  std::unordered_multimap<std::string, uint64_t>;               // maps LavaFlowNode names to their ids 
using lava_flowPtrs      =  std::unordered_set<LavaNode*>;                                // LavaFlowNode pointers referenced uniquely by address instead of using an id
using lava_ptrsvec       =  std::vector<LavaNode*>;
using lava_nameNodeMap   =  std::unordered_map<std::string, LavaNode*>;                   // maps the node names to their pointers

extern "C" using            FlowFunc  =  uint64_t (*)(LavaParams*, LavaVal*, LavaOut*);   // data flow node function
extern "C" using           LavaAlloc  =  void* (*)(uint64_t);                             // custom allocation function passed in to each node call
extern "C" using  GetLavaFlowNodes_t  =  LavaNode*(*)();                                  // the signature of the function that is searched for in every shared library - this returns a LavaFlowNode* that is treated as a sort of null terminated list of the actual nodes contained in the shared library 

union     LavaArgType{ 
  enum { NONE=0, END, DATA_ERROR, STORE, MEMORY, SEQUENCE, ENUMERATION };                // todo: does this need store sequence and memory sequence?
  u8 asInt;
};
union          LavaId                                                // this Id serves as both a nodeId and LavaFlowSlot index, since a LavaFlowSlot index will alway coordinate with only one node 
{    
  static  const u64 NODE_NONE = 0xFFFFFFFFFFFF;                  // 48 bits all set
  static  const u64 SLOT_NONE = 0xFFFF;                          // 16 bits all set

  struct { 
    u64  nid : 48;                                       // id is the node id number - This is a unique number for each node that doesn't change. It can refer back to a node since it doesn't represent an ordering or an index into an array 
    u64 sidx : 16;                                       // idx is the index of the LavaFlowSlot - for a node this is 0
  };
  u64 asInt;

  LavaId() : nid(NODE_NONE), sidx(SLOT_NONE) {}
  LavaId(u64 _id, u64 _idx=SLOT_NONE) : nid(_id), sidx(_idx) {}

  bool   operator==(LavaId  r) const { return nid==r.nid && sidx==r.sidx; }
  bool    operator<(LavaId const& r)   const {
    if(nid==r.nid) return sidx < r.sidx;
    else         return nid  < r.nid;
  }
  size_t operator()(LavaId const& _id) const {
    return std::hash<u64>()(_id.nid) ^ std::hash<u64>()(_id.sidx);
  }
};
struct     LavaParams
{
  u32            inputs;
  u32           outputs;
  u64             frame;
  LavaId             id;
  LavaAlloc   mem_alloc;
  //LavaPut           put;
};
struct        LavaVal
{
  u64      type :  3;          // ArgType
  u64     value : 61;
};
struct        LavaOut
{
  // todo: change to LavaVal
  u64      type :  3;          // ArgType
  u64     value : 61;          // This will hold the address in memory - could also hold the starting block index in the database 

  union {
    struct { u64 frame; u32 slot; u32 listIdx; };
    u8 bytes[16];
  }key;
};
struct        LavaMsg
{
  u64        id;
  LavaVal    val;
};
struct     LavaPacket
{
  u64    ref_count;
  u64        frame : 63;
  u64       framed :  1;
  u64    dest_node : 48;
  u64    dest_slot : 16;
  u64     src_node : 48;
  u64     src_slot : 16;
  u64     sz_bytes;                               // the size in bytes can be used to further sort the packets so that the largets are processed first, possibly resulting in less memory usage over time
  LavaMsg      msg;

  bool operator<(LavaPacket const& r) const
  {    
    if(frame    != r.frame)    return frame    > r.frame;               // want lowest frame numbers to be done first 
    if(sz_bytes != r.sz_bytes) return sz_bytes < r.sz_bytes;            // want largest sizes to be done first
    if(framed   != r.framed)   return framed   < r.framed;              // want framed packets to be done first so that their dependencies can be resolved

    return msg.id > r.msg.id;
  }
};
struct      LavaFrame
{
  enum FRAME { ERR_FRAME = 0xFFFFFFFFFFFFFFFE, NO_FRAME = 0xFFFFFFFFFFFFFFFF };

  u64            dest = LavaId::NODE_NONE;
  u64           frame = 0;
  u64           slots = 0;
  u8             size = 0;
  LavaPacket  packets[16];

  bool getSlot(u8 sIdx)
  {
    return (slots >> sIdx) & 0x0000000000000001;
  }
  void setSlot(i8 sIdx, bool val)
  {
    //using namespace std;
    //if(val) slots |=  (u64)0x1 << max(0,sIdx-1);            // or with the proper bit set to 1
    //else    slots &= ~( (u64)(0x1) << max(0,sIdx-1) );      // and with the proper bit set to 0 and everything else set to 1

    if(val) slots |=  (u64)0x1 << sIdx;            // or with the proper bit set to 1
    else    slots &= ~( (u64)(0x1) << sIdx);       // and with the proper bit set to 0 and everything else set to 1
  }
};
struct       LavaNode
{
  enum Type { NONE=0, FLOW, MSG, NODE_ERROR=0xFFFFFFFFFFFFFFFF };                              // this should be filled in with other node types like scatter, gather, transform, generate, sink, blocking sink, blocking/pinned/owned msg - should a sink node always be pinned to it's own thread

  FlowFunc              func;
  uint64_t         node_type;
  const char*           name;
  const char**      in_names;
  const char**     out_names;
  const char**      in_types;
  const char**     out_types;
  uint64_t           version;
};
struct       LavaInst
{
  using au32  =  std::atomic<uint32_t>;
  using au64  =  std::atomic<uint64_t>;

  enum State { NORMAL=0, RUN_ERROR, LOAD_ERROR, COMPILE_ERROR };

  LavaId         id;
  LavaNode*    node;
  u32        inputs;
  u32       outputs;
  u64          time = 0;  // todo: make this atomic / make functions to add time and get the current time
  union{ State state = NORMAL; u32 stateU32; };

  void       setState(LavaInst::State s) // -> LavaInst::State
  {
    ((au32*)(&state))->store(s);
    //return this->state;
  }
  auto       getState() -> LavaInst::State
  {
    stateU32 = ((au32*)(&stateU32))->load();
    return state;
  }
  u64         addTime(u64 t)
  {
    return ((au64*)(&time))->fetch_add(t);
  }
  u64       clearTime()
  {
    return ((au64*)(&time))->exchange(0);
  }
};
struct   LavaFlowSlot
{ 
  enum State { NORMAL=0, HIGHLIGHTED, SELECTED, SLOT_ERROR };

  LavaId id; bool in=false; State state=NORMAL;

  LavaFlowSlot(){}
  LavaFlowSlot(LavaId Id, bool In=false) : id(Id), in(In), state(NORMAL) {}

  //u64 nid; bool in=false; State state=NORMAL;
  //LavaFlowSlot(u64 nId, bool In=false) : nid(nId), in(In), state(NORMAL) {}
};
struct        LavaMem
{
  using au64 = std::atomic<uint64_t>;

  void* ptr = nullptr;

  uint64_t    refCount()const{ return ((uint64_t*)ptr)[0]; }
  uint64_t   sizeBytes()const{ return ((uint64_t*)ptr)[1]; }
  uint64_t&   refCount()     { return ((uint64_t*)ptr)[0]; }
  uint64_t&  sizeBytes()     { return ((uint64_t*)ptr)[1]; }
  void*           data()     { return ((uint64_t*)ptr)+2;  }

  uint64_t      incRef()     { return ((au64*)ptr)->fetch_add( 1); }
  uint64_t      decRef()     { return ((au64*)ptr)->fetch_add(-1); }

  static LavaMem fromDataAddr(uint64_t addr)                                       // create a LavaMem struct from the address of the start of the data (after the reference count and sizeBytes at the start of the allocation)
  {
    LavaMem lm;
    lm.ptr =  (void*)(addr - sizeof(uint64_t)*2);
    return lm;
  }
};
struct    LavaCommand
{
  enum Command { ADD_NODE=0, ADD_SLOT, TGL_CNCT, DEL_NODE, DEL_CNCT };

  union Arg { 
    Arg() : id() {} 
    LavaId id; u64 val; LavaNode* ndptr; bool slotDest;
  };

  Command cmd; 
  union { Arg A; Arg dest; Arg nd; };
  union { Arg B; Arg  src; };

  //union { LavaId A; LavaId dest; LavaNode* ndptr; }; 
  //union { LavaId B; LavaId  src; };
};
// end data types

// static data segment data
#if defined(_WIN32)
  static const std::string  liveExt(".live.dll");
#endif
static __declspec(thread)       void*   lava_thread_heap = nullptr;       // thread local handle for thread local heap allocations
// end data segment data

// function declarations
BOOL WINAPI DllMain(
  _In_  HINSTANCE     hinstDLL,
  _In_  DWORD        fdwReason,
  _In_  LPVOID     lpvReserved
);

extern "C" __declspec(dllexport) LavaNode* GetLavaFlowNodes();   // prototype of function to return static plugin loading struct
// end function declarations

// allocator definitions
inline void    LavaHeapDestroyCallback(void* heapHnd)
{
  if(heapHnd)
    HeapDestroy(heapHnd);
}
inline void*   LavaHeapInit(size_t initialSz = 0)
{
  if(!lava_thread_heap) {
    lava_thread_heap = HeapCreate(HEAP_NO_SERIALIZE, initialSz, 0);
  }
  return lava_thread_heap;
}
inline void*   LavaHeapAlloc(size_t sz)
{
  void* thread_heap = LavaHeapInit(sz);

  void* ret = nullptr;
  if(thread_heap)
    ret = HeapAlloc(thread_heap, HEAP_NO_SERIALIZE, sz);

  return ret;
}
inline int     LavaHeapFree(void* memptr)
{
  void* thread_heap = LavaHeapInit();

  void* ret = nullptr;
  if(thread_heap && memptr) {
    auto ret = HeapFree(thread_heap, HEAP_NO_SERIALIZE, memptr);
    return ret;
  }else{ return 0; }
}

template <class T> struct  ThreadAllocator
{
  using value_type  =  T;

  template<class U> 
  struct rebind
  {
    using other = ThreadAllocator<U>;
  };

  ThreadAllocator() /*noexcept*/ {}   // default ctor not required by STL

                                      // A converting copy constructor:
  template<class U> __declspec(noinline) ThreadAllocator(const ThreadAllocator<U>&) /*noexcept*/ {}
  template<class U> __declspec(noinline) bool operator==(const ThreadAllocator<U>&) const /*noexcept*/
  {
    return true;
  }
  template<class U> __declspec(noinline) bool operator!=(const ThreadAllocator<U>&) const /*noexcept*/
  {
    return false;
  }
  __declspec(noinline) T*    allocate(const size_t n)     const;
  __declspec(noinline) void  deallocate(T*& p, size_t)    const;

  template<class U, class... ARGS>
  __declspec(noinline) void construct(U* p, ARGS&&... args)
  {
    ::new ((void*)p) U(std::forward<ARGS>(args)...);
  }
  template<class U>
  __declspec(noinline) void destroy(U* p)
  {
    p->~U();
  }
};
template <class T> T*      ThreadAllocator<T>::allocate(const size_t n)  const
{
  if(n==0) return nullptr;

  if( n > static_cast<size_t>(-1)/sizeof(T) ) {
    throw std::bad_array_new_length();
  }

  //void* p = ThreadAlloc( n * sizeof(T) );
  void* p = LavaHeapAlloc( n * sizeof(T) );
  if(!p) { throw std::bad_alloc(); }

  return static_cast<T*>(p);
}
template <class T> void    ThreadAllocator<T>::deallocate(T*& p, size_t) const
{
  //if(n==0) return;
  //
  //ThreadFree(p);

  LavaHeapFree(p);
  p = nullptr;
}
// end allocator definitions

using lava_memvec        =  std::vector<LavaMem, ThreadAllocator<LavaMem> >;

// Lava Helper Functions

// todo: once tbl is switched to not be an array, this might not need to be a template
// todo: allocation template parameters might mean that a template is still neccesary
// todo: A table type that has empty allocation parameters could mean an unowned type
//       | the unowned type could have a constructor that takes any tbl and makes it unowned, treating it effectivly as a reference
template<class T> LavaOut  LavaTblToOut(LavaParams const* lp, tbl<T> const& t)
{
  void* outmem  =  lp->mem_alloc(t.sizeBytes());
  memcpy(outmem, t.memStart(), t.sizeBytes());

  LavaOut o;
  o.value = (u64)outmem;
  o.type  = LavaArgType::MEMORY;

  return o;
}
// End Lava Helper Functions

class       LavaGraph
{
public:
  enum       Cmd { ADD_NODE=0, ADD_SLOT, TGL_CNCT, DEL_NODE, DEL_CNCT };
  union      Arg { LavaId id; u64 val; LavaNode* ndptr; };
  struct Command { Cmd cmd; Arg A; Arg B; };

  struct NodeInstance { uint64_t id; LavaNode* nd; };                  // a struct used for returning an instance of a node - the Nodes map of ids and LavaFlowNode pointers  

  using abool         =  std::atomic<bool>;
  using au32          =  std::atomic<uint32_t>;
  using NodeInsts     =  std::unordered_map<uint64_t, LavaInst>;       // maps an id to a LavaFlowNode struct
  using Slots         =  std::multimap<LavaId, LavaFlowSlot>;          // The key is a node id, the value is the index into the slot array.  Every node can have 0 or more slots. Slots can only have 1 and only 1 node. Slots have their node index in their struct so getting the node from the slots is easy. To get the slots that a node has, this multimap is used
  using CnctMap       =  std::unordered_map<LavaId, LavaId, LavaId>;   // maps connections from their single destination slot to their single source slot - Id is the hash function object in the third template argument
  using SrcMap        =  std::multimap<LavaId, LavaId>;                // maps connections from their single source slot to their one or more destination slots
  using vec_insts     =  std::vector<LavaInst>;                        // list of node instances - Id and pointer pairs
  using vec_nptrs     =  std::vector<LavaInst>;                        // lists used for returning from reloading functions
  using vec_cnptrs    =  std::vector<LavaNode const*>;
  using vec_ids       =  std::vector<LavaId>;
  using MsgIds        =  std::unordered_set<LavaId, LavaId>;
  using NormalizeMap  =  std::unordered_map<uint64_t, uint64_t>;
  using CmdQ          =  std::queue<LavaCommand>;
  using RetStk        =  std::stack<LavaCommand::Arg>;
  using ArgVec        =  std::vector<LavaCommand::Arg>;
  //using CmdQ          =  std::priority_queue<LavaCommand>;

private:
  mutable abool         m_useA;

  uint64_t             m_nxtId;               // nxtId is next id - a counter for every node created that only increases, giving each node a unique id
  CmdQ                  m_cmdq;
  RetStk                 m_stk;

  //NodeInsts          m_nodesA;
  //Slots              m_slotsA;
  //CnctMap            m_cnctsA;
  //SrcMap         m_destCnctsA;
  //MsgIds          m_msgNodesA;

  NodeInsts           m_nodesA;
  Slots               m_slotsA;
  CnctMap             m_cnctsA;
  SrcMap          m_destCnctsA;
  MsgIds           m_msgNodesA;

  NodeInsts           m_nodesB;
  Slots               m_slotsB;
  CnctMap             m_cnctsB;
  SrcMap          m_destCnctsB;
  MsgIds           m_msgNodesB;

  NodeInsts&            curNodes(){ return m_useA.load()?  m_nodesA     : m_nodesB;     }
  Slots&                curSlots(){ return m_useA.load()?  m_slotsA     : m_slotsB;     }
  CnctMap&              curCncts(){ if(m_useA.load()) return m_cnctsA; else return m_cnctsB; }
  SrcMap&           curDestCncts(){ return m_useA.load()?  m_destCnctsA : m_destCnctsB; }
  MsgIds&            curMsgNodes(){ return m_useA.load()?  m_msgNodesA  : m_msgNodesB;  }
  NodeInsts const&      curNodes()const{ return m_useA.load()?  m_nodesA     : m_nodesB;     }
  Slots     const&      curSlots()const{ return m_useA.load()?  m_slotsA     : m_slotsB;     }
  CnctMap   const&      curCncts()const{ return m_useA.load()?  m_cnctsA     : m_cnctsB;     }
  SrcMap    const&  curDestCncts()const{ return m_useA.load()?  m_destCnctsA : m_destCnctsB; }
  MsgIds    const&   curMsgNodes()const{ return m_useA.load()?  m_msgNodesA  : m_msgNodesB;  }

  NodeInsts&            oppNodes(){ return !m_useA.load()?  m_nodesA     : m_nodesB;     }
  Slots&                oppSlots(){ return !m_useA.load()?  m_slotsA     : m_slotsB;     }
  CnctMap&              oppCncts(){ if(!m_useA.load()) return m_cnctsA; else return m_cnctsB; }
  SrcMap&           oppDestCncts(){ return !m_useA.load()?  m_destCnctsA : m_destCnctsB; }
  MsgIds&            oppMsgNodes(){ return !m_useA.load()?  m_msgNodesA  : m_msgNodesB;  }
  NodeInsts const&      oppNodes()const{ return !m_useA.load()?  m_nodesA     : m_nodesB;     }
  Slots     const&      oppSlots()const{ return !m_useA.load()?  m_slotsA     : m_slotsB;     }
  CnctMap   const&      oppCncts()const{ return !m_useA.load()?  m_cnctsA     : m_cnctsB;     }
  SrcMap    const&  oppDestCncts()const{ return !m_useA.load()?  m_destCnctsA : m_destCnctsB; }
  MsgIds    const&   oppMsgNodes()const{ return !m_useA.load()?  m_msgNodesA  : m_msgNodesB;  }


  void            init()
  { 
    m_useA.store(true);
    m_nxtId = 1;
  }
  void              mv(LavaGraph&& rval)
  {
    using namespace std;

    m_nodesA     = move(rval.m_nodesA); 
    m_slotsA     = move(rval.m_slotsA); 
    m_cnctsA     = move(rval.m_cnctsA); 
    m_destCnctsA = move(rval.m_destCnctsA);

    m_nodesB     = move(rval.m_nodesB); 
    m_slotsB     = move(rval.m_slotsB); 
    m_cnctsB     = move(rval.m_cnctsB); 
    m_destCnctsB = move(rval.m_destCnctsB);

    //m_running   = rval.m_running;
    //m_ids       = move(rval.m_ids);
  }
  void              cp(LavaGraph const& lval)
  {
    using namespace std;

    m_nodesA     = lval.m_nodesA;
    m_slotsA     = lval.m_slotsA; 
    m_cnctsA     = lval.m_cnctsA; 
    m_destCnctsA = lval.m_destCnctsA;

    m_nodesB     = lval.m_nodesB;
    m_slotsB     = lval.m_slotsB; 
    m_cnctsB     = lval.m_cnctsB; 
    m_destCnctsB = lval.m_destCnctsB;
  }
  u64              nxt(){ return m_nxtId++; }

  auto   nodeDestSlots(vec_nptrs const& nds) -> vec_ids
  {
    using namespace std;

    vec_ids sidxs;                                            // sidxs is slot indexes
    for(auto ni : nds){                                       // ni is Node Instance np is node pointer and nds is nodes
      auto si = lower_bound(ALL(curSlots()), LavaId(ni.id.nid), [](auto a,auto b){ return a.first < b; } );          // si is slot iterator
      if(si != end(curSlots())  &&  si->first.nid == ni.id.nid){
        LavaFlowSlot& s = si->second;
        if(s.in) sidxs.push_back(si->first);
      }
    }
    return sidxs;                                        // RVO
  }
  auto       nodeSlots(vec_nptrs const& nds) -> vec_ids
  {
    using namespace std;

    vec_ids sidxs;                                            // sidxs is LavaFlowSlot indexes
    for(auto ni : nds){                                       // np is node pointer and nds is nodes
      auto si = lower_bound(ALL(curSlots()), LavaId(ni.id.nid), [](auto a,auto b){ return a.first < b; } );          // si is slot iterator
      if(si != end(curSlots())  &&  si->first.nid == ni.id.nid){
        LavaFlowSlot& s = si->second;
        sidxs.push_back(si->first);        
      }
    }
    return sidxs;                                        // RVO
  }
  auto       errorInst() -> LavaInst&
  { 
    static LavaInst ERR_INST = { LavaId(LavaNode::NODE_ERROR, LavaFlowSlot::SLOT_ERROR), nullptr, 0, 0 };
    return ERR_INST;
  }
  auto       errorInst() const -> LavaInst const&
  { 
    static LavaInst ERR_INST = { LavaId(LavaNode::NODE_ERROR, LavaFlowSlot::SLOT_ERROR), nullptr, 0, 0 };
    return ERR_INST;
  }
  
  u64          nxtSlot(u64 nid) // non-const, part of writing, and needs to use the opposite buffer
  {
    auto si = nodeSlots(nid);                   // si is slot iterator
    i64 cur = -1;
    while(si != end(oppSlots())   && 
      si->first.nid  == nid && 
      si->first.sidx <= (u64)(cur+1) ){
      //cur = si->first.idx;
      ++cur; ++si;
    }
    //return cur;

    return cur + 1;
  }
  u32       countSlots(const char** typeList)
  {
    u32 cnt=0;
    for(; typeList  &&  *typeList; ++typeList){ 
      ++cnt; 
    }
    return cnt;
  }
  LavaInst    makeInst(u64 nid, LavaNode* ln)
  {
    LavaInst li;                                       // li is Lava Instance
    li.id      = nid;
    li.node    = ln;
    li.inputs  = countSlots(ln->in_types);
    li.outputs = countSlots(ln->out_types);

    return li;
  }

public:
  LavaGraph() { init(); }
  LavaGraph(LavaGraph&& rval){ mv(std::move(rval)); }
  LavaGraph& operator=(LavaGraph&& rval){ mv(std::move(rval)); return *this; }

  auto         operator[](uint64_t nid) -> decltype(curNodes()[0])
  {
    return curNodes()[nid];
  }

  // global
  auto   normalizeIndices() -> NormalizeMap
  {
    using namespace std;

    // create a mapping of old node Ids to new ones, new ones will be their position + 1
    NormalizeMap nids;
    nids.reserve(curNodes().size());
    u64 cur = 1;
    for(auto& kv : curNodes()){
      nids[kv.first] = cur++;
    }

    // connections 
    CnctMap nxtCncts;
    for(auto kv : curCncts()){
      LavaId nxtDest = kv.first;
      LavaId nxtSrc  = kv.second;
      nxtDest.nid = nids[nxtDest.nid];
      nxtSrc.nid  = nids[nxtSrc.nid];
      nxtCncts.insert({nxtDest, nxtSrc});
    }
    curCncts() = move(nxtCncts);

    SrcMap nxtDestCncts;
    for(auto kv : curDestCncts()){
      LavaId nxtSrc   = kv.first;
      LavaId nxtDest  = kv.second;
      nxtSrc.nid   = nids[nxtSrc.nid];
      nxtDest.nid  = nids[nxtDest.nid];
      nxtDestCncts.insert({nxtSrc, nxtDest});
    }
    curDestCncts() = move(nxtDestCncts);

    // slots
    Slots nxtSlots;
    for(auto& kv : curSlots()){
      LavaId nxtId  = kv.first;
      nxtId.nid     = nids[nxtId.nid];
      LavaFlowSlot nxtS = kv.second;
      auto nxtSltId = nids[nxtS.id.nid];
      nxtS.id.nid   = nxtSltId;
      nxtS.id.sidx  = nxtS.id.sidx;
      nxtSlots.insert({nxtId, nxtS});
    }
    curSlots() = move(nxtSlots);

    // node ids 
    NodeInsts nxtNds;
    for(auto& kv : curNodes()){
      u64     nxtId = nids[kv.first];
      LavaInst inst = curNodes()[kv.first];
      nxtNds.insert({nxtId, inst});
    }
    curNodes() = move(nxtNds);

    return nids;
  }
  void      setNextNodeId(u64 nxt){ m_nxtId = nxt; }
  u64           totalTime()
  {
    u64 total = 0;
    for(auto& kv : curNodes()){ 
      total += kv.second.time;
    }
    return total;
  }
  u64           clearTime()
  {
    u64 total = 0;
    for(auto& kv : curNodes()){ 
      total += kv.second.clearTime();
    }
    return total;
  }
  void              clear()
  {
    curNodes().clear();
    curSlots().clear();
    curCncts().clear();
    curDestCncts().clear();
    curMsgNodes().clear();

    init();
    //m_ids.clear();
  }
  //u64                 put(LavaCommand::Command cmd, LavaId A, LavaId B=LavaId())
  u64                 put(LavaCommand::Command cmd, LavaCommand::Arg A, LavaCommand::Arg B = LavaCommand::Arg() )
  {
    m_cmdq.push({cmd, A, B});
    return m_cmdq.size();
  }
  auto                exec() -> ArgVec
  {
    using namespace std;
    
    ArgVec returned;
    auto sz = m_cmdq.size();
    if(sz > 0)
    {
      if(m_useA.load()){
        m_nodesB     = m_nodesA;
        m_slotsB     = m_slotsA; 
        m_cnctsB     = m_cnctsA; 
        m_destCnctsB = m_destCnctsA;
      }else{
        m_nodesA     = m_nodesB;
        m_slotsA     = m_slotsB; 
        m_cnctsA     = m_cnctsB; 
        m_destCnctsA = m_destCnctsB;
      }

      while(m_cmdq.size() > 0)
      {
        auto lc = m_cmdq.front();                 // lc is LavaCommand
        m_cmdq.pop();
        switch(lc.cmd)
        {
          case LavaCommand::TGL_CNCT:{
            this->toggleCnct(lc.src.id, lc.dest.id);
          }break;

          case LavaCommand::DEL_CNCT:{
            this->delCnct(lc.A.id);
          }break;

          case LavaCommand::ADD_NODE:{
            u64 nid = this->addNode(lc.A.ndptr, lc.B.val);
            LavaCommand::Arg ret;
            ret.id.nid = nid;
            m_stk.push(ret);
            returned.push_back(ret);
          }break;

          case LavaCommand::DEL_NODE:{
            this->delNode(lc.A.id.nid);
          }break;

          case LavaCommand::ADD_SLOT:{
            LavaFlowSlot s(m_stk.top().id.nid, lc.A.slotDest);
            m_stk.pop();
            LavaId sid = this->addSlot(s); // lc.A.id.sidx);
            LavaCommand::Arg ret;
            ret.id = sid;
            m_stk.push(ret);
            returned.push_back(ret);
          }break;

          default: break;
        };
      }

      while(m_stk.size()>0){ m_stk.pop(); }
      
      m_useA.store( !m_useA.load() );                                             // this should be the only place where it is flipped, so the store is all that matters
    }

    return returned;
  }

  // opposite buffer changes
  uint64_t    addNode(LavaNode* ln, uint64_t nid)
  {
    //u64 id = nxt();
    if(ln->node_type == LavaNode::MSG)
      oppMsgNodes().insert(nid);

    LavaInst li = makeInst(nid, ln);
    return oppNodes().insert({nid, li}).first->first;                             // returns a pair that contains the key-value pair
  }
  uint64_t    addNode(LavaNode* ln, bool newId=true)
  {
    u64 id = nxt();
    if(ln->node_type == LavaNode::MSG)
      oppMsgNodes().insert(id);

    LavaInst li = makeInst(id, ln);
    return oppNodes().insert({id, li}).first->first;                             // returns a pair that contains the key-value pair
  }
  bool        delNode(uint64_t nid)
  {
    // erase any connects that go with the slots here
    auto     si = this->nodeSlots(nid);                       // si is slot iterator
    auto siCnct = si; 
    for(; siCnct!=end(oppSlots())  &&  siCnct->first.nid==nid; ++siCnct ){
      if(siCnct->second.in) this->delDestCnct(nid);
      else                  this->delSrcCncts(nid);
    }

    auto slcnt = oppSlots().erase(nid);                         // slcnt is slot count
    auto   cnt = oppNodes().erase(nid);
    oppMsgNodes().erase(nid);

    return (cnt+slcnt) > 0;                                  // return true if 1 or more elements were erased, return false if no elements were erasedm
  }
  LavaId      addSlot(LavaFlowSlot  s, u64 sidx=0)
  {
    LavaId id(s.id.nid, sidx);
    id.sidx = sidx? sidx  :  nxtSlot(s.id.nid);
    s.id    = id;
    oppSlots().insert({id, s});

    return id;
  }
  u32         delCnct(LavaId  src, LavaId  dest)
  {
    u32      cnt = 0;
    auto srcIter = oppCncts().find(dest);
    if(srcIter != oppCncts().end())
    {
      auto iter = oppDestCncts().find(src);
      for(; iter != oppDestCncts().end() && iter->first == src; ){
        auto cpy = iter++;          // ++iter;
        if(cpy->second == dest){
          oppDestCncts().erase(cpy);
          ++cnt;
        }
      }
      oppCncts().erase(dest);
    }

    return cnt;
  }
  void     toggleCnct(LavaId  src, LavaId  dest)
  {
    u32    delcnt = 0;
    auto       di = oppCncts().find(dest);
    bool matchSrc = false;
    if( di != end(oppCncts()) ){
      matchSrc = src == di->second;
      delcnt   = delCnct(di->second,dest);
    }

    if( !(matchSrc && delcnt>0) ){
      oppCncts().insert({dest, src});
      oppDestCncts().insert({src, dest});
    }
  }


  // nodes
  auto           node(u64 id)  -> LavaInst&
  {
    auto nIter = curNodes().find(id);                                          // nIter is node iterator
    if(nIter == end(curNodes())) return errorInst();

    //LavaInst ret;
    //ret.id   = nIter->first;
    //ret.node = nIter->second;
    //return ret;

    return nIter->second;
  }
  auto           node(u64 id) const -> LavaInst
  {
    auto nIter = curNodes().find(id);                                          // nIter is node iterator
    if(nIter == end(curNodes())) return errorInst();

    return nIter->second;
  }
  auto          nodes() const -> vec_insts
  {
    vec_insts nds;
    nds.reserve(curNodes().size());
    for(auto& on : curNodes()){                                  // on is order and node - order is on.first    LavaInst is on.second
      nds.push_back(on.second);

      //NodeInstance inst;
      //inst.id = on.first;
      //inst.nd = on.second;
      //nds.push_back(inst);
    }
    return nds;
  }
  auto          nodes() -> vec_insts
  {
    vec_insts nds;
    nds.reserve(curNodes().size());
    for(auto& on : curNodes()){                                  // on is order and node - order is on.first    node is on.second
      nds.push_back(on.second);

      //NodeInstance inst;
      //inst.id = on.first;
      //inst.nd = on.second;
      //nds.push_back(inst);
    }
    return nds;
  }
  u64           maxId()
  {
    using namespace std;

    u64 mx = 0;
    for(auto kv : curNodes()){ mx = max(mx, kv.first); }

    return mx;

    //for(auto kv : m_ids){ mx = max(mx, kv.first); }
  }
  u64             nsz() const { return curNodes().size(); }
  auto       msgNodes() const -> MsgIds const&
  {
    return curMsgNodes();
  }
  auto       nodeFunc(uint64_t nid) const -> FlowFunc
  {
    auto ni = curNodes().find(nid);                            // ni is node iterator
    if(ni == end(curNodes()))
      return ni->second.node->func;
      //return ni->second->func;
    else
      return nullptr;
  }
  void       setState(u64 nid, LavaInst::State state) // -> LavaInst::State
  {
    //return this->node(nid).setState(nid, state);
    LavaInst& li = this->node(nid);
    li.setState(state);
  }
  auto       getState(u64 nid) -> LavaInst::State
  {
    //return this->node(nid).getState();
    LavaInst& li = this->node(nid);
    return li.getState();
  }

  // slots
  auto           slot(LavaId   id) -> LavaFlowSlot*
  {
    auto si = curSlots().find(id);
    if(si == curSlots().end()) 
      return nullptr;

    return &si->second;
  }
  auto      nodeSlots(u64     nid) const -> decltype(curSlots().cbegin())
  {
    return lower_bound(ALL(curSlots()), nid, [](auto a,auto b){ return a.first.nid < b; } );
  }
  auto      nodeSlots(u64     nid) -> decltype(oppSlots().begin())
  {
    return lower_bound(ALL(oppSlots()), nid, [](auto a,auto b){ return a.first.nid < b; } );
  }
  auto          slots() -> Slots& { return curSlots(); }
  auto          slots() const -> Slots const& { return curSlots();  }
  auto       getSlots() -> Slots& { return curSlots(); }
  auto        srcSlot(LavaId destId) -> LavaFlowSlot* 
  {
    auto ci = curCncts().find(destId);
    if(ci != curCncts().end()){
      LavaFlowSlot* s = slot(ci->second);
      return s;
    }else 
      return nullptr;
  }
  auto      destSlots(LavaId  srcId) -> decltype(curDestCncts().find(srcId))
  {
    return curDestCncts().find(srcId);
  }
  auto        slotEnd() -> decltype(curSlots().end()) { return curSlots().end(); }
  u64             ssz() const { return curSlots().size(); }

  // connections
  u32     delSrcCncts(LavaId  src)
  {
    u32  cnt = 0;
    auto  di = curDestCncts().find(src);
    for(; di!=curDestCncts().end() && di->first==src; ++cnt, ++di){   // ++di,
      curCncts().erase(di->second);
    }
    curDestCncts().erase(src);

    return cnt;

    //di = m_destCncts.find(src);
  }
  auto      destCncts(LavaId  src) -> decltype(curDestCncts().begin()) // C++14 -> decltype(m_slots.find(Id(nid)))
  {
    return lower_bound(ALL(curDestCncts()), LavaId(src), [](auto a,auto b){ return a.first < b; } );
  }
  u64   destCnctCount(LavaId  src)
  {
    return curDestCncts().count(src);
  }
  bool    delDestCnct(LavaId dest)
  {
    auto iter = curCncts().find(dest);
    if(iter==curCncts().end()) return false;

    auto src = iter->second;
    curCncts().erase(dest);

    auto srcIter = curDestCncts().find(src);
    for(; srcIter!=end(curDestCncts()) && srcIter->first==src && srcIter->second==dest; ){
      auto cpy = srcIter++;
      curDestCncts().erase(cpy);
    }

    return true;
  }
  auto    destCnctEnd() -> decltype(curDestCncts().end()) { return curDestCncts().end(); }
  auto        cnctEnd() -> decltype(curCncts().end())     { return curCncts().end(); }
  auto      cnctBegin() -> decltype(curCncts().begin())   { return curCncts().begin(); }
  auto          cncts() -> CnctMap& { return curCncts(); }
  auto          cncts() const -> CnctMap const& { return curCncts(); }
  u64          cnctsz() const { return curCncts().size(); }
  u32         delCnct(LavaId id)
  {
    u32 cnt = 0;
    LavaFlowSlot* s = this->slot(id);
    if(!s) return 0;

    if(s->in){
      auto iter = curCncts().find(id);
      return delCnct(iter->second, iter->first);
    }else{
      auto iter = curDestCncts().find(id);
      auto  idx = iter->first;
      while(iter != curDestCncts().end() && iter->first == idx){
        curCncts().erase(iter->second);
        iter = curDestCncts().erase(iter);
        ++cnt;
      }
    }

    return cnt;
  }
  auto    srcCnctsMap() -> SrcMap&
  {
    return curDestCncts();
  }
};
struct       LavaFlow
{
public:
  using au64            =  std::atomic<uint64_t>;
  using PacketQueue     =  std::priority_queue<LavaPacket>;
  //using FrameQueue      =  std::priority_queue<LavaFrame>;
  using FrameQueue      =  std::vector<LavaFrame>;
  using MsgNodeVec      =  std::vector<uint64_t>;
  using Mutex           =  std::mutex;
  using PacketCallback  =  void (*)(LavaPacket pkt);

  enum FlowErr { NONE=0, RUN_ERR=0xFFFFFFFFFFFFFFFF };

  lava_pathHndlMap         libs;     // libs is libraries - this maps the live path of the shared libary with the OS specific handle that the OS loading function returns
  lava_nidMap              nids;     // nids is node ids  - this maps the name of the node to all of the graph node ids that use it
  lava_flowNodes           flow;
  lava_flowPtrs            ptrs;     // ptrs are the LavaFlowNode pointers - each one needs a unique id so they can be referenced elsewhere 
  lava_hndlNodeMap       ndptrs;     // ndptrs is node pointers - a map from each handle to the one (zero?) or more LavaFlowNode pointers the shared lib contains
  lava_nameNodeMap    nameToPtr;     // maps node names to their pointers 

  mutable bool         m_running = false;            // todo: make this atomic
  mutable u64         m_curMsgId = 0;                // todo: make this atomic
  mutable u64            m_frame = 0;                // todo: make this atomic
  mutable LavaId         m_curId = LavaNode::NONE;   // todo: make this atomic - won't be used as a single variable anyway
  mutable u64      m_threadCount = 0;                // todo: make this atomic
  mutable u32            version = 0;                // todo: make this atomic
  mutable Mutex           m_qLck;
  mutable Mutex      m_frameQLck;
  mutable PacketCallback  packetCallback;            // todo: make this an atomic version of the function pointer
  mutable PacketQueue          q;
  mutable FrameQueue      frameQ;

  MsgNodeVec        m_msgNodesA;
  LavaGraph              graph;

  u64      incThreadCount()
  {
    return std::atomic_fetch_add( (au64*)&m_threadCount,  1);
  }
  u64      decThreadCount()
  {
    return std::atomic_fetch_add( (au64*)&m_threadCount, -1);
  }
  uint64_t       nxtMsgId()
  {
    return ++m_curMsgId;
  }
  bool          nxtPacket(LavaPacket* outPkt)
  {
    using namespace std;
    
    // need to somehow find a full set of packets for the current frame - how to keep one frame from outrunning another? 
    // does each node's slot need its own queue? should packets be organized differently? one queue per frame? what determines a frame? one pass through all the message nodes?
    bool packetWritten = false;

    m_qLck.lock();             // lock mutex
      if(q.size() > 0){
        *outPkt = q.top();
        q.pop();
        packetWritten = true;
        m_curId = outPkt->dest_node;
      }
    m_qLck.unlock();           // unlock mutex

    return packetWritten;

    //lock_guard<Mutex> qLck(m_qLck);
    //LavaPacket pckt = q.top();
  }
  void          putPacket(LavaPacket pkt)
  {
    m_qLck.lock();              // mutex lock
      q.push(pkt);              // todo: use a mutex here initially
    m_qLck.unlock();            // mutex unlock
  }

  // query 
  auto     getNxtPacketId() -> LavaId
  {
    using namespace std;
    
    LavaId ret;
    lock_guard<Mutex>  qLck(m_qLck);
      if(q.size() > 0){
        ret.nid  = q.top().dest_node;
        ret.sidx = q.top().dest_slot;
      }else{
        ret.nid  = LavaId::NODE_NONE;
        ret.sidx = LavaId::SLOT_NONE;
      }
      return ret;
    // implicit unlock
  }

  // execution
  void              start(){ m_running =  true; }
  void               stop()
  {
    m_running = false;                                 // this will make the 'running' boolean variable false, which will make the the while(running) loop stop, and the threads will end
    //for(auto& t : fd.flowThreads){
    //  t.join();
    //}
  }
  //void          pauseLoop(){}
};

#if defined(__LAVAFLOW_IMPL__)

// function implementations
BOOL WINAPI DllMain(
  _In_ HINSTANCE    hinstDLL,
  _In_ DWORD       fdwReason,
  _In_ LPVOID    lpvReserved
)
{
  //Println("dllmain called: ", fdwReason);
  switch(fdwReason)
  {
    //case DLL_PROCESS_ATTACH:
    //case DLL_THREAD_ATTACH:
    //  lava_thread_heap = nullptr;
    //  break;
  case DLL_PROCESS_DETACH:
  case DLL_THREAD_DETACH:
    //Println("Heap Destroy");
    LavaHeapDestroyCallback(lava_thread_heap);
    lava_thread_heap = nullptr;
    break;
  default:
    ;
  }
  return true;
}

namespace {

#include "str_util.hpp"
void                printdb(simdb const& db)
{
  using namespace std;
  
  Println("size: ", db.size());

  std::vector<i8> memv(db.memsize(), 0);
  memcpy( (void*)memv.data(), db.mem(), db.memsize() );

  Println("\n");

  u64 blksz = db.blockSize();
  TO(memv.size(),i){ 
    if(i % blksz == 0){
      putc('|', stdout);
    }
    putc(memv[i] ,stdout);
  }
}

void           PrintLavaMem(LavaMem lm)
{
  printf("\n addr: %llu  data addr: %llu  ref count: %llu   size bytes: %llu \n", 
    (u64)(lm.ptr), (u64)(lm.data()), (u64)lm.refCount(), (u64)lm.sizeBytes() );
}

auto       GetSharedLibPath() -> std::wstring
{
  using namespace std;
  
  #ifdef _WIN32
    HMODULE hModule = GetModuleHandleW(NULL);
    WCHAR path[MAX_PATH];
    GetModuleFileNameW(hModule, path, MAX_PATH);

    //return string(TEXT(path));
    return path;
  #endif
}
auto        GetRefreshPaths() -> lava_paths
{
  using namespace std;
  using namespace  fs;

  static const regex lavaRegex("lava_.*");
  static const regex extRegex(".*\\.live\\.dll");

  auto libPath = path( GetSharedLibPath() );
  libPath.remove_filename();
  path    root = libPath;

  vector<str> paths;
  auto    dirIter = directory_iterator(root);
  for(auto& d : dirIter){
    auto   p = d.path();
    if(!p.has_filename()){ continue; }

    auto ext = p.extension().generic_string();                     // ext is extension
    if(ext!=".dll"){ continue; }

    str fstr = p.filename().generic_string();                      // fstr is file string
    if(  regex_match(fstr, extRegex) ){ continue; }
    if( !regex_match(fstr,lavaRegex) ){ continue; }

    auto livepth = p;
    livepth.replace_extension( liveExt );

    bool refresh = true;
    if( exists(livepth) ){
      auto liveWrite = last_write_time(livepth).time_since_epoch().count();         // liveWrite is live write time - the live shared library file's last write time 
      auto origWrite = last_write_time(p).time_since_epoch().count();               // origWrite is orginal write time - the original shared library file's last write time
      if( liveWrite > origWrite ) refresh = false;
    }

    if(refresh) paths.push_back( p.generic_string() );
  }

  return paths;
}
uint64_t    CopyPathsToLive(lava_paths       const& paths)
{
  using namespace std;
  using namespace  fs;

  uint64_t count = 0;
  for(auto const& p : paths){
    path livepth(p);
    livepth.replace_extension(".live.dll");
    
    bool doCopy = false;
    if( exists(livepth) ){
      doCopy = remove(livepth);
    }else{ doCopy = true; }
    
    if(doCopy){ 
      copy_file(p, livepth);
      ++count;
    }
  }

  return count;
}
uint64_t        RemovePaths(lava_paths       const& paths)
{
  using namespace std;
  using namespace  fs;

  uint64_t count = 0;
  for(auto const& p : paths){
    if( remove(path(p)) ){ ++count; }
  }

  return count;
}
auto               LoadLibs(lava_paths       const& paths) -> lava_hndlvec
{
  lava_hndlvec hndls(paths.size(), 0);

  TO(paths.size(), i){
    HMODULE lib = LoadLibrary(TEXT(paths[i].c_str()));
    hndls[i] = lib;
  }

  return hndls;
}
uint64_t           FreeLibs(lava_hndlvec     const& hndls)
{
  uint64_t count = 0;
  for(auto const& h : hndls){
    if( FreeLibrary(h) ){ ++count; }
  }
  return count;
}
auto           GetLivePaths(lava_paths       const& paths) -> lava_paths 
{
  using namespace std;
  using namespace  fs;

  lava_paths ret;
  for(auto const& p : paths){
    ret.emplace_back( path(p).replace_extension(liveExt).generic_string() );
  }

  return ret;
}
auto         GetLiveHandles(lava_pathHndlMap const& hndls, lava_paths const& paths) -> lava_hndlvec
{
  lava_hndlvec ret;
  for(auto const& p : paths){
    auto hi = hndls.find(p);         // hi is handle iterator
    if(hi != end(hndls)) ret.push_back(hi->second);
  }

  return ret;
}
auto       GetFlowNodeLists(lava_hndlvec     const& hndls) -> lava_ptrsvec
{
  lava_ptrsvec ret(hndls.size(), nullptr);
  TO(hndls.size(),i){
    auto h = hndls[i];
    if(h){
      auto  GetLavaFlowNodes = (GetLavaFlowNodes_t)GetProcAddress(h, TEXT("GetLavaNodes") );
      LavaNode* nodeList = GetLavaFlowNodes();
      ret[i] = nodeList;
    }
  }

  return ret;
}

void*             LavaAlloc(uint64_t sizeBytes)
{
  //uint64_t* mem = (uint64_t*)malloc(sizeBytes + sizeof(uint64_t)*2);
  uint64_t* mem = (uint64_t*)LavaHeapAlloc(sizeBytes + sizeof(uint64_t)*2);
  mem[0]  =  0;              // reference count
  mem[1]  =  sizeBytes;      // number of bytes of main allocation

  return (void*)(mem + 2);  // sizeof(uint64_t)*2);
}
void               LavaFree(uint64_t addr)
{
  void* p = (void*)(addr - sizeof(uint64_t)*2);
  //free(p);
  LavaHeapFree(p);
}

uint64_t      exceptWrapper(FlowFunc f, LavaFlow& lf, LavaParams* lp, LavaVal* inArgs, LavaOut* outArgs)
{
  uint64_t        ret = LavaFlow::NONE;
  uint64_t  winExcept = 0;
  __try{
    f(lp, inArgs, outArgs);
  }__except( (winExcept=GetExceptionCode()) || EXCEPTION_EXECUTE_HANDLER ){
    ret = LavaFlow::RUN_ERR;
    //lf.graph.setState(lp->id.nid, LavaInst::RUN_ERROR);

    printf("\n windows exception code: %llu \n", winExcept);
  }

  return ret;
}
uint64_t                runFunc(LavaFlow&   lf, lava_memvec& ownedMem, uint64_t nid, LavaParams* lp, LavaVal* inArgs,  LavaOut* outArgs) noexcept   // runs the function in the node given by the node id, puts its output into packets and ultimatly puts those packets into the packet queue
{
  using namespace std;
  using namespace std::chrono;

  LavaInst&  li = lf.graph[nid];
  FlowFunc func = li.node->func; //lf.graph[nid].node->func;
  if(func)
  {
    LavaParams lp;
    SECTION(create arguments and call function)
    {
      lp.inputs      =     1;
      lp.outputs     =   512;
      lp.frame       =   lf.m_frame;
      lp.id          =   LavaId(nid);
      lp.mem_alloc   =   LavaAlloc;             //lp.mem_alloc   =   malloc;  // LavaHeapAlloc;

      auto  stTime = high_resolution_clock::now();
        uint64_t ret = exceptWrapper(func, lf, &lp, inArgs, outArgs);
        if(ret != LavaFlow::NONE){ return ret; }
      auto endTime = high_resolution_clock::now();
      duration<u64,nano> diff = (endTime - stTime);
      li.addTime( diff.count() );   //li.time += diff.count();
    }
    SECTION(create packets and put them into packet queue)
    {
      TO(lp.outputs,i)
      {
        if(outArgs[i].type == LavaArgType::NONE){ continue; }

        // create new value for the new packet
        LavaVal val;
        val.type  = outArgs[i].type;
        val.value = outArgs[i].value;
        auto sidx = outArgs[i].key.slot;

        LavaMem mem = LavaMem::fromDataAddr(outArgs[i].value);
        PrintLavaMem(mem);
        auto szBytes = mem.sizeBytes();

        // create new packet 
        LavaPacket basePkt;
        basePkt.frame       =   lf.m_frame;            // increment the frame on every major loop through both data and message nodes - how to know when a full cycle has passed? maybe purely by message nodes - only increment frame if data is created through a message node cycle
        basePkt.framed      =   false;                 // would this go on the socket?
        basePkt.src_node    =   nid;
        basePkt.src_slot    =   sidx;
        basePkt.msg.id      =   lf.nxtMsgId();
        basePkt.msg.val     =   val;

        // route the packet using the graph - the packet may be copied multiple times and go to multiple destination slots
        LavaId   src  =  { nid, sidx };
        auto      di  =  lf.graph.destCncts(src);                            // di is destination iterator
        auto   diCnt  =  di;                                                 // diCnt is destination iterator counter - used to count the number of destination slots this packet will be copied to so that the reference count can be set correctly
        auto    diEn  =  lf.graph.destCnctEnd();

        for(; di!=diEn && di->first==src; ++di)
        {                                                                    // loop through the 1 or more destination slots connected to this source
          LavaId   pktId = di->second;
          LavaPacket pkt = basePkt;                                                    // pkt is packet
          pkt.dest_node  = pktId.nid;
          pkt.dest_slot  = pktId.sidx;

          mem.incRef();

          lf.putPacket(pkt);

          //lf.m_qLck.lock();              // mutex lock
          //  lf.q.push(pkt);              // todo: use a mutex here initially
          //lf.m_qLck.unlock();            // mutex unlock
        }

        ownedMem.push_back(mem);

        lf.packetCallback(basePkt);

        //PutMem(LavaId(basePkt.src_node, basePkt.src_slot) , mem);
        //auto keys = vizdb.getKeyStrs();
        //printdb(vizdb);
      }
    } // SECTION(create packets and put them into packet queue)

    return true;
  }
  return false;
}

}

void               LavaInit()
{
  //new (&db)     simdb("lava_db", 128, 2<<4);
  //new (&vizdb)  simdb("viz_db",  128, 2<<4);
}
bool        RefreshFlowLibs(LavaFlow& inout_flow)
{
  bool     newlibs  =  false;
  auto       paths  =  GetRefreshPaths();
  if(paths.size() > 0){
    newlibs = true;
    ++inout_flow.version;
  }

  auto   livePaths  =  GetLivePaths(paths);

  // coordinate live paths to handles
  auto liveHandles  =  GetLiveHandles(inout_flow.libs, livePaths);

  // free the handles
  auto   freeCount  =  FreeLibs(liveHandles); 

  // delete the now unloaded live shared library files
  auto    delCount  =  RemovePaths(livePaths);

  // copy the refresh paths' files
  auto   copyCount  =  CopyPathsToLive(paths); 

  // load the handles
  auto loadedHndls  =  LoadLibs(livePaths);

  // put loaded handles into LavaFlow struct
  TO(livePaths.size(), i){
    auto h = loadedHndls[i];
    if(h){
      inout_flow.libs[livePaths[i]] = h;
    }
  }

  // extract the flow node lists from the handles
  auto flowNdLists = GetFlowNodeLists(loadedHndls);

  // extract the flow nodes from the lists and put them into the multi-map
  TO(livePaths.size(),i)
  {
    LavaNode* ndList = flowNdLists[i];
    if(ndList){
      auto const& p = livePaths[i]; 
      inout_flow.flow.erase(p);                              // delete the current node list for the livePath
      for(; ndList->func!=nullptr; ++ndList){                // insert each of the LavaFlowNodes in the ndList into the multi-map
        inout_flow.nameToPtr.erase(ndList->name);
        inout_flow.nameToPtr.insert( {ndList->name, ndList} );
        inout_flow.flow.insert( {p, ndList} );
      }
    }
  }

  return newlibs;
}
void               LavaLoop(LavaFlow& lf) noexcept
{
  using namespace std;
  const LavaOut defOut = { LavaArgType::NONE, 0, 0, 0, 0 };

  lava_memvec ownedMem;

  lf.incThreadCount();

  LavaVal    inArgs[LAVA_ARG_COUNT];              // these will end up on the per-thread stack when the thread enters this function, which is what we want - thread specific memory for the function call
  LavaOut   outArgs[LAVA_ARG_COUNT];              // if the arguments are going to 
  memset(inArgs,  0, sizeof(inArgs)  );
  TO(LAVA_ARG_COUNT,i){ outArgs[i] = defOut; }    // memset(outArgs, 0, sizeof(outArgs) );

  LavaHeapInit();

  while(lf.m_running)
  {
    SECTION(loop through message nodes)
    {
      printf("\n lava heap: %llu \n", (u64)lava_thread_heap);

      auto const& mnds = lf.graph.msgNodes();
      for(auto id : mnds)
      {
        lf.m_curId = id;

        LavaParams lp;
        uint64_t err = runFunc(lf, ownedMem, id.nid, &lp, inArgs, outArgs); 
        switch(err)
        {
        case LavaFlow::RUN_ERR:{
          lf.graph.setState(id.nid, LavaInst::RUN_ERROR);
          //lf.putPacket(pckt);               // if there was an error, put the packet back into the queue
        }break;

        case LavaFlow::NONE:
        default: 
          break;
        }

      }
      ++lf.m_frame; // todo: rethink this and make sure it will work 
    } // SECTION(loop through message nodes)
    SECTION(take a packet from the packet queue and put it into a slot in the frame queue)
    {
      LavaPacket pckt;
      bool ok = lf.nxtPacket(&pckt);
      if(ok){
        lf.m_frameQLck.lock();             // lock mutex
        
        // loop through the current frames looking for one with the same frame number and destination slot id
        TO(lf.frameQ.size(), i){
          if(lf.frameQ[i].frame != pckt.frame) continue;
          if(lf.frameQ[i].dest != pckt.dest_node) continue;     // todo: unify dest_node and dest_id etc. as one LavaId LavaPacket
          //if(lf.frameQ[i].dest.sidx != pckt.dest_slot) continue;

          //lf.frameQ[i].
        }

        lf.m_frameQLck.unlock();           // unlock mutex
      }
    }
    SECTION(loop through data packets)
    {
      LavaPacket pckt;
      while( lf.nxtPacket(&pckt) )                                        // todo: will need to figure out how to get a full frame of packets here instead of simply taking on packet at a time
      {
        lf.m_curId.nid  = pckt.dest_node;
        lf.m_curId.sidx = pckt.dest_slot;

        inArgs[pckt.dest_slot] = pckt.msg.val;
        LavaParams lp;                                                    // get the function from the dest node and put the packets into the dest LavaVal input array 
        lp.inputs = 1;
        
        uint64_t err = runFunc(lf, ownedMem, pckt.dest_node, &lp, inArgs, outArgs); 
        switch(err)
        {
          case LavaFlow::RUN_ERR:{
            lf.graph.setState(pckt.dest_node, LavaInst::RUN_ERROR);
            lf.putPacket(pckt);               // if there was an error, put the packet back into the queue
          }break;

          case LavaFlow::NONE:
          default: 
            break;
        }

        LavaMem lm = LavaMem::fromDataAddr(inArgs[pckt.dest_slot].value);
        PrintLavaMem(lm);
        lm.decRef();
      }
    } // SECTION(loop through data packets)
    SECTION(partition owned allocations and free those with their reference count at 0)
    {
      for(auto const& lm : ownedMem){
        PrintLavaMem(lm);
        //printf("\n reference count: %llu \n", (u64)lm.refCount() );
      }

      auto zeroRef  = partition(ALL(ownedMem), [](auto a){return a.refCount() > 0;} );                          // partition the memory with zero references to the end / right of the vector so they can be deleted by just cutting down the size of the vector
      auto freeIter = zeroRef;
      for(; freeIter != end(ownedMem); ++freeIter){                                                              // loop through the memory with zero references and free them
        LavaFree( (uint64_t)freeIter->data() );
      }

      ownedMem.erase(zeroRef, end(ownedMem));                                                                  // erase the now freed memory
    }
  } // while(m_running)

  SECTION(loop through allocations and wait for their ref counts to be zero before exiting the loop)
  {
    // will the allocations need to be freed here like the normal deallocation loop?
  }

  lf.decThreadCount();
}
// end function implementations


// priority queue of packets - sort by frame number, then dest node, then dest slot
//
// lock free hash table of in flight packets?

#endif // endif for implementation

#endif



















// C++14 -> decltype(m_slots.find(Id(nid)))
//return lower_bound(ALL(m_slots), LavaId(nid), [](auto a,auto b){ return a.first < b; } );

//if(delcnt==0 || !matchSrc ){
//delCnct(src,dest)==0 

//vec_ids sidxs;                                            // sidxs is slot indexes
//for(auto np : nds){                                       // np is node pointer and nds is nodes
//  auto si = lower_bound(ALL(m_slots), LavaId(np->id), [](auto a,auto b){ return a.first < b; } );          // si is slot iterator
//  if(si != end(m_slots)  &&  si->first.nid == np->id){
//    LavaFlowSlot& s = si->second;
//    if(s.in) sidxs.push_back(si->first);
//  }
//}
//return sidxs;                                        // RVO

//case LavaCommand::ADD_SLOT:{
//  this->addSlot( , lc.A.sidx);
//}break;

//unordered_map<u64,u64> nids;
//
//unordered_map<u64,u64> ordrs;
//ordrs.reserve(m_nodes.size());
//u64 curOrdr = 1;
//for(auto& kv : m_nodes){
//  ordrs[kv.first] = curOrdr++;
//}

//LavaNode* nd = m_nodes[kv.first].node;
//
//u64 nxtOrdr = ordrs[kv.second];

//NodeIdMap nxtIds;
//for(auto& kv : m_ids){
//  u64   nxtId = nids[kv.first];
//  u64 nxtOrdr = ordrs[kv.second];
//  nxtIds.insert({nxtId, nxtOrdr});
//}
//m_ids = move(nxtIds);

// and finally nodes
//NodeMap nxtOrdrs;
//for(auto& kv : m_nodes){
//  LavaFlowNode nxtNd  = kv.second;
//  nxtNd.id    = nids[nxtNd.id];
//  //u64 nxtOrdr = ordrs[kv.first];
//  //nxtOrdrs.insert({nxtOrdr, nxtNd});
//}
//m_nodes = move(nxtOrdrs);

//#include "simdb.hpp"
//
//static simdb      db;
//static simdb   vizdb;

//bool                 PutMem(LavaId id, LavaMem lm)
//{
//  char label[256];
//  sprintf(label, "%d:%d", (u32)id.nid, (u32)id.sidx);
//
//  uint32_t stblk=0;
//  bool ok = db.put(label, lm.data(), (u32)lm.sizeBytes(), &stblk);
//
//  return ok;
//  //str label = str(id.nid) + ":" + str(id.sidx);
//}

//__try{
//printf("\n try \n");
//
//}__except( (winExcept=GetExceptionCode())  ){
//}__except(EXCEPTION_EXECUTE_HANDLER){
//printf("windows exception code: %llu", winExcept);
//}

//printf("\n try \n");
//}__except(EXCEPTION_EXECUTE_HANDLER)

//uint64_t ret   =   func(&lp, inArgs, outArgs);
//
//SECTION(loop through inputs and decrement their references now that the function is done)
//{ //}

//
//mutable u64    packetCallback;                  // todo: make this an atomic version of the function pointer

//LavaInst li = this->node(nid).setState
//stateU32 = ((au32*)(&stateU32))->exchange(state);
//return state;
//
//stateU32 = ((au32*)(&stateU32))->load();
//return state;

//LavaMem lm;
//lm.ptr = (void*)(inArgs[pckt.dest_slot].value - 16);
//printf("\n reference count: %llu \n", (u64)lm.refCount() );
//LavaFree((u64)lm.data());

//inArgs[0].type  = LavaArgType::MEMORY;
//TO(lp.inputs,i){
//}

//
//vec_ids         m_msgNodes;

//using NodeInsts     =  std::unordered_map<uint64_t, LavaNode*>;      // maps an id to a LavaFlowNode struct
//using vec_insts     =  std::vector<NodeInstance>;                    // list of node instances - Id and pointer pairs
//using vec_nptrs     =  std::vector<LavaNode*>;                       // lists used for returning from reloading functions

//auto       msgNodes() const -> vec_ids const&
//{
//  return m_msgNodes;
//}

//LavaMem mem;
//mem.ptr = (void*)outArgs[i].value;

//u64   refCnt  =  0;                                                  // todo: make this part of the allocation
//for(; diCnt!=diEn && diCnt->first==src; ++diCnt){ ++refCnt; } 
//basePkt.ref_count = refCnt;                                        // reference count will ultimatly need to be handled very differently, not on a packet basis, since the packets are copied - it should probably be on the value somehow - maybe the allocator passed should allocate an extra 8 bytes for the reference count and that should be treated atomically 
//
//tbl<u8> pth( (void*)outArgs[0].value, false);
//pth.owned(false);
//
//auto szBytes = pth.sizeBytes();
//
//auto szBytes = ((uint64_t*)outArgs[i].value)[-1];
//
//db.put(outArgs[i].key.bytes, sizeof(LavaOut::key), (void*)outArgs[i].value, (u32)szBytes);
//
//LavaFree( outArgs[i].value );

// run the function 
// use the LavaParams output struct to make the LavaOut list packets  
// route the packets with the graph (set the dest node and slot)  
// put them into the packet queue

//tbl<u8> pth( (void*)outArgs[0].value );
//printf("\n out string %s \n", (char*)pth.data() );

//auto i = mem.size() - 1;
//while( mem[i].refCount() < 1 ){ --i; }
//
//sort(ALL(mem), [](auto a,auto b){return a.refCount() > b.refCount();} );
//auto sz = mem.size();
//TO(sz,i){
//  if( mem[i].refCount() < 1 ){
//  }
//}

//
//union   LavaGraph::Id;

//auto       errorInst() -> NodeInstance&
//{ // default contrutor sets the type to Node::NODE_ERROR
//  static NodeInstance ERR_INST;
//  return ERR_INST;
//}
//auto       errorInst() const -> NodeInstance const&
//{ // default contrutor sets the type to Node::NODE_ERROR
//  static NodeInstance ERR_INST;
//  return ERR_INST;
//}

//LavaInst ret;
//ret.id = nIter->first;
//ret.ln = nIter->second;
//return ret;

//auto           node(u64 id)  -> NodeInstance
//{
//  auto nIter = m_nodes.find(id);                                          // nIter is node iterator
//  //if(nIter == end(m_nodes)) return errorNode();
//  if(nIter == end(m_nodes)) return errorInst();
//
//  NodeInstance ret;
//  ret.id = nIter->first;
//  ret.nd = nIter->second;
//  return ret;
//
//  //return nIter->second;
//}
//auto           node(u64 id) const -> NodeInstance
//{
//  auto nIter = m_nodes.find(id);                                          // nIter is node iterator
//                                                                          //if(nIter == end(m_nodes)) return errorNode();
//  if(nIter == end(m_nodes)) return errorInst();
//
//  NodeInstance ret;
//  ret.id = nIter->first;
//  ret.nd = nIter->second;
//  return ret;
//
//  //return nIter->second;
//}

//
//this_thread::sleep_for( chrono::milliseconds(1000) );

//auto  FlattenNodeLists(lava_ptrsvec const& flowNdLists, lava_paths const& livePaths)
//{
//  TO(livePaths.size(),i)
//  {
//    LavaNode* ndList = flowNdLists[i];
//    if(ndList){
//      auto const& p = livePaths[i]; 
//      fd.flow.flow.erase(p);                              // delete the current node list for the livePath
//      for(; ndList->func!=nullptr; ++ndList){             // insert each of the LavaFlowNodes in the ndList into the multi-map
//        fd.flow.nameToPtr.erase(ndList->name);
//        fd.flow.nameToPtr.insert( {ndList->name, ndList} );
//        fd.flow.flow.insert( {p, ndList} );
//      }
//    }
//  }
//}

//auto const& p = livePaths[i]; 
//fd.flow.flow.erase(p);                              // delete the current node list for the livePath
//for(; ndList->func!=nullptr; ++ndList){             // insert each of the LavaFlowNodes in the ndList into the multi-map
//  fd.flow.nameToPtr.erase(ndList->name);
//  fd.flow.nameToPtr.insert( {ndList->name, ndList} );
//  fd.flow.flow.insert( {p, ndList} );
//}
//
//// delete interface buttons from the nanogui window
//fd.ui.ndBtns.clear();
//
//// redo interface node buttons
//for(auto& kv : fd.flow.flow){
//  LavaNode*     fn = kv.second;                               // fn is flow node
//  auto       ndBtn = new Button(fd.ui.keyWin, fn->name);
//  ndBtn->setCallback([fn](){ 
//    node_add(fn->name, Node(fn->name, (Node::Type)((u64)fn->node_type), {100,100}) );
//  });
//}
//fd.ui.screen.performLayout();
//
//TO(paths.size(),i) printf("\n %llu : %s \n", i, paths[i].c_str() );

//return m_nodes.insert({ln->id, ln}).first->first;                         // returns a pair that contains the key-value pair
//if(newId) ln->id = nxt();
//
//if(ln->node_type == LavaNode::MSG)
//  m_msgNodes.push_back(ln->id);
//
//LavaInst li = makeInst(ln->id, ln);
//return m_nodes.insert({ln->id, li}).first->first;                       // returns a pair that contains the key-value pair
//
////return m_nodes.insert({ln->id, ln}).first->first;                     // returns a pair that contains the key-value pair

//uint64_t                id;
//
//uint16_t            inputs;       // cache after counting inputs 
//uint16_t           outputs;       // cache after counting output
