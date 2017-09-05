

#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __LAVAFLOW_HEADERGUARD_HPP__
#define __LAVAFLOW_HEADERGUARD_HPP__

#include <cstdint>
#include <string>
#include <unordered_map>
#include <regex>
#include <filesystem>
#include <unordered_set>
#include <map>
#include "no_rt_util.h"

#if defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #define NOMINMAX
  #include <Windows.h>
#endif

// data types
struct   LavaFlowNode;
struct        LavaArg;

namespace fs = std::tr2::sys;                                     // todo: different compiler versions would need different filesystem paths

using str             =  std::string;
using lava_paths      =  std::vector<std::string>;
using lava_libHndls   =  std::unordered_map<LavaFlowNode*, HMODULE>;                    // todo: need to change this depending on OS
using lava_hndlvec    =  std::vector<HMODULE>;                                          // todo: need to change this depending on OS
using lava_hndlMap    =  std::unordered_map<std::string, HMODULE>;                      // maps LavaFlowNode names to their OS specific handles
using lava_flowNodes  =  std::unordered_multimap<std::string, LavaFlowNode*>;           // maps LavaFlowNode names to their pointers
using lava_nidMap     =  std::unordered_multimap<std::string, uint64_t>;                // maps LavaFlowNode names to their ids 
using lava_flowPtrs   =  std::unordered_set<LavaFlowNode*>;                             // LavaFlowNode pointers referenced uniquely by address instead of using an id
using lava_ptrsvec    =  std::vector<LavaFlowNode*>;

//using lava_libHndls   =  std::vector<HMODULE>;                                        // todo: need to change this depending on OS
//using lava_flowPtrs   =  std::unordered_map<uint64_t, LavaFlowNode*>;                 // maps ids to LavaFlowNode pointers
//using lava_flowPtrs   =  std::vector<LavaFlowNode*>;

extern "C" using           FlowFunc  =  uint64_t (*)(LavaArg* in, LavaArg* out);        // data flow node function
extern "C" using GetLavaFlowNodes_t  =  LavaFlowNode*(*)();                             // the signature of the function that is searched for in every shared library - this returns a LavaFlowNode* that is treated as a sort of null terminated list of the actual nodes contained in the shared library 

//enum class LavaNodeType { NONE=0, FLOW, MSG, NODE_ERROR };                            // this should be filled in with other node types like scatter, gather, transform, generate, sink, blocking sink, blocking/pinned/owned msg - should a sink node always be pinned to it's own thread

union         ArgType{ 
  enum { END=0, DATA_ERROR, STORE, MEMORY, SEQUENCE, ENUMERATION };
  u8 asInt;
};
struct        LavaArg
{
  u64      type :  3;
  u64     value : 61;
};
struct        LavaMsg
{
  u64        id;
  LavaArg   arg;
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
  LavaMsg      msg;
};
struct   LavaFlowNode
{
  enum { NONE=0, FLOW, MSG, NODE_ERROR };                              // this should be filled in with other node types like scatter, gather, transform, generate, sink, blocking sink, blocking/pinned/owned msg - should a sink node always be pinned to it's own thread

  FlowFunc              func;
  uint64_t         node_type;
  uint16_t            inputs;
  uint16_t           outputs;
  const char*           name;
  const char**      in_types;
  const char**     out_types;
  uint64_t           version;
  uint64_t                id;
};
struct   LavaFlowSlot { 
  enum State { NORMAL=0, HIGHLIGHTED, SELECTED, SLOT_ERROR };

  u64 nid; bool in=false; State state=NORMAL;

  LavaFlowSlot(u64 nId, bool In=false) : nid(nId), in(In), state(NORMAL) {}
};
struct       LavaFlow
{
  lava_hndlMap       libs;    // libs is libraries - this maps the live path of the shared libary with the OS specific handle that the OS loading function returns
  lava_nidMap        nids;    // nids is node ids  - this maps the name of the node to all of the graph node ids that use it
  lava_flowNodes     flow;
  lava_flowPtrs      ptrs;    // ptrs are the LavaFlowNode pointers - each one needs a unique id so they can be referenced elsewhere 
};
// end data types

// static data segment data
static const              std::string   liveExt(".live.dll");             // todo: change depending on OS
static __declspec(thread)       void*   lava_thread_heap = nullptr;       // thread local handle for thread local heap allocations
// end data segment data

// function declarations
BOOL WINAPI DllMain(
  _In_  HINSTANCE     hinstDLL,
  _In_  DWORD        fdwReason,
  _In_  LPVOID     lpvReserved
);

extern "C" __declspec(dllexport) LavaFlowNode* GetLavaFlowNodes();   // prototype of function to return static plugin loading struct
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
inline void*   LavaAlloc(size_t sz)
{
  void* thread_heap = LavaHeapInit(sz);

  void* ret = nullptr;
  if(thread_heap)
    ret = HeapAlloc(thread_heap, HEAP_NO_SERIALIZE, sz);

  return ret;
}
inline int     LavaFree(void* memptr)
{
  void* thread_heap = LavaHeapInit();

  void* ret = nullptr;
  if(thread_heap && memptr) {
    auto ret = HeapFree(thread_heap, HEAP_NO_SERIALIZE, memptr);
    return ret;
  }else{ return 0; }
}

template <class T> struct ThreadAllocator
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
template <class T> T*     ThreadAllocator<T>::allocate(const size_t n) const
{
  if(n==0) return nullptr;

  if( n > static_cast<size_t>(-1)/sizeof(T) ) {
    throw std::bad_array_new_length();
  }

  void* p = ThreadAlloc( n * sizeof(T) );
  if(!p) { throw std::bad_alloc(); }

  return static_cast<T*>(p);
}
template <class T> void   ThreadAllocator<T>::deallocate(T*& p, size_t) const
{
  //if(n==0) return;

  ThreadFree(p);
  p = nullptr;
}
// end allocator definitions

class  LavaGraph
{
public:
  union Id                                                // this Id serves as both a nodeId and LavaFlowSlot index, since a LavaFlowSlot index will alway coordinate with only one node 
  {    
    struct { 
      u64 id  : 48;                                       // id is the node id number - This is a unique number for each node that doesn't change. It can refer back to a node since it doesn't represent an ordering or an index into an array 
      u64 idx : 16;                                       // idx is the index of the LavaFlowSlot - for a node this is 0
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

  using Node         = std::unordered_map<u64, LavaFlowNode>;        // maps an id to a LavaFlowNode struct
  using Slots        = std::multimap<Id, LavaFlowSlot>;              // The key is a node id, the value is the index into the slot array.  Every node can have 0 or more slots. Slots can only have 1 and only 1 node. Slots have their node index in their struct so getting the node from the slots is easy. To get the slots that a node has, this multimap is used
  using CnctMap      = std::unordered_map<Id, Id, Id>;               // maps connections from their single destination slot to their single source slot - Id is the hash function object in the third template argument
  using SrcMap       = std::multimap<Id, Id>;                        // maps connections from their single source slot to their one or more destination slots
  using vec_nptrs    = std::vector<LavaFlowNode*>;
  using vec_cnptrs   = std::vector<LavaFlowNode const*>;
  using vec_ids      = std::vector<Id>;

private:
  u64                m_nxtId;               // nxtId is next id - a counter for every node created that only increases, giving each node a unique id
  Node               m_nodes;
  Slots              m_slots;
  CnctMap            m_cncts;
  SrcMap         m_destCncts;

  void           init(){}
  void             mv(LavaGraph&& rval)
  {
    using namespace std;

    m_nodes     = move(rval.m_nodes); 
    m_slots     = move(rval.m_slots); 
    m_cncts     = move(rval.m_cncts); 
    m_destCncts = move(rval.m_destCncts);

    //m_ids       = move(rval.m_ids);
  }
  u64             nxt(){ return m_nxtId++; }

  auto  nodeDestSlots(vec_nptrs const& nds) -> vec_ids
  {
    using namespace std;

    vec_ids sidxs;                                            // sidxs is slot indexes
    for(auto np : nds){                                       // np is node pointer and nds is nodes
      auto si = lower_bound(ALL(m_slots), Id(np->id), [](auto a,auto b){ return a.first < b; } );          // si is slot iterator
      if(si != end(m_slots)  &&  si->first.id == np->id){
        LavaFlowSlot& s = si->second;
        if(s.in) sidxs.push_back(si->first);
      }
    }
    return sidxs;                                        // RVO
  }
  auto      nodeSlots(vec_nptrs const& nds) -> vec_ids
  {
    using namespace std;

    vec_ids sidxs;                                            // sidxs is LavaFlowSlot indexes
    for(auto np : nds){                                     // np is node pointer and nds is nodes
      auto si = lower_bound(ALL(m_slots), Id(np->id), [](auto a,auto b){ return a.first < b; } );          // si is slot iterator
      if(si != end(m_slots)  &&  si->first.id == np->id){
        LavaFlowSlot& s = si->second;
        sidxs.push_back(si->first);        
      }
    }
    return sidxs;                                        // RVO
  }
  auto      errorNode() -> LavaFlowNode&
  { // default contrutor sets the type to Node::NODE_ERROR
    static LavaFlowNode ERR_NODE;
    return ERR_NODE; 
  }
  auto      errorNode() const -> LavaFlowNode const&
  { // default contrutor sets the type to Node::NODE_ERROR
    static LavaFlowNode ERR_NODE;
    return ERR_NODE; 
  }
  u64         nxtSlot(u64 nid)
  {
    auto si = nodeSlots(nid);                   // si is slot iterator
    u64 cur = 1;
    while(si != end(m_slots)   && 
      si->first.id  == nid && 
      si->first.idx <= cur+1 ){
      cur = si->first.idx;
      ++si;
    }

    return cur + 1;
  }

public:
  LavaGraph() : m_nxtId(1) { init(); }
  LavaGraph(LavaGraph&& rval){ mv(std::move(rval)); }
  LavaGraph& operator=(LavaGraph&& rval){ mv(std::move(rval)); return *this; }

  // global
  void  normalizeIndices()
  {
    using namespace std;

    // create a mapping of old node Ids to new ones, new ones will be their position + 1
    unordered_map<u64,u64> nids;
    nids.reserve(m_nodes.size());
    u64 cur = 1;
    for(auto& kv : m_nodes){
      nids[kv.first] = cur++;
    }

    //unordered_map<u64,u64> ordrs;
    //ordrs.reserve(m_nodes.size());
    //u64 curOrdr = 1;
    //for(auto& kv : m_nodes){
    //  ordrs[kv.first] = curOrdr++;
    //}

    // connections 
    CnctMap nxtCncts;
    for(auto kv : m_cncts){
      Id nxtDest = kv.first;
      Id nxtSrc  = kv.second;
      nxtDest.id = nids[nxtDest.id];
      nxtSrc.id  = nids[nxtSrc.id];
      nxtCncts.insert({nxtDest, nxtSrc});
    }
    m_cncts = move(nxtCncts);

    SrcMap nxtDestCncts;
    for(auto kv : m_destCncts){
      Id nxtSrc   = kv.first;
      Id nxtDest  = kv.second;
      nxtSrc.id   = nids[nxtSrc.id];
      nxtDest.id  = nids[nxtDest.id];
      nxtDestCncts.insert({nxtSrc, nxtDest});
    }
    m_destCncts = move(nxtDestCncts);

    // slots
    Slots nxtSlots;
    for(auto& kv : m_slots){
      Id nxtId  = kv.first;
      nxtId.id  = nids[nxtId.id];
      LavaFlowSlot nxtS = kv.second;
      nxtS.nid  = nids[nxtS.nid];
      nxtSlots.insert({nxtId, nxtS});
    }
    m_slots = move(nxtSlots);

    // node ids 
    Node nxtNds;
    for(auto& kv : m_nodes){
      u64   nxtId = nids[kv.first];
      //u64 nxtOrdr = ordrs[kv.second];
      LavaFlowNode nd = m_nodes[kv.first];
      nxtNds.insert({nxtId, nd});
    }
    m_nodes = move(nxtNds);

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
  }
  void             clear()
  {
    m_nodes.clear();
    m_slots.clear();
    m_cncts.clear();
    m_destCncts.clear();

    //m_ids.clear();
  }
  void     setNextNodeId(u64 nxt){ m_nxtId = nxt; }

  // selection
  uint64_t   addNode(LavaFlowNode n, bool newId=true)
  {
    if(newId) n.id = nxt();
    return m_nodes.insert({n.id, n}).first->first;                          // returns a pair that contains the key-value pair
  }
  auto          node(u64 id)  -> struct LavaFlowNode&
  {
    auto nIter = m_nodes.find(id);                         // nIter is node iterator
    if(nIter == end(m_nodes)) return errorNode();

    return nIter->second;
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
  u64          maxId()
  {
    using namespace std;

    u64 mx = 0;
    for(auto kv : m_nodes){ mx = max(mx, kv.first); }

    return mx;

    //for(auto kv : m_ids){ mx = max(mx, kv.first); }
  }
  u64            nsz() const { return m_nodes.size(); }

  // slots
  Id         addSlot(LavaFlowSlot  s, u64 idx=0)
  {
    Id id(s.nid, idx);
    id.idx = idx? idx  :  nxtSlot(s.nid);
    m_slots.insert({id, s});

    return id;
  }
  auto          slot(Id   id) -> LavaFlowSlot*
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
  auto       srcSlot(Id destId) -> LavaFlowSlot* 
  {
    auto ci = m_cncts.find(destId);
    if(ci != m_cncts.end()){
      LavaFlowSlot* s = slot(ci->second);
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
    LavaFlowSlot* s = this->slot(id);
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
  auto   srcCnctsMap() -> SrcMap&
  {
    return m_destCncts;
  }
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
uint64_t    CopyPathsToLive(lava_paths    const& paths)
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
uint64_t        RemovePaths(lava_paths    const& paths)
{
  using namespace std;
  using namespace  fs;

  uint64_t count = 0;
  for(auto const& p : paths){
    if( remove(path(p)) ){ ++count; }
  }

  return count;
}
auto               LoadLibs(lava_paths    const& paths) -> lava_hndlvec
{
  //lava_libHndls hndls(paths.size(), 0);
  lava_hndlvec hndls(paths.size(), 0);

  TO(paths.size(), i){
    HMODULE lib = LoadLibrary(TEXT(paths[i].c_str()));
    hndls[i] = lib;
  }

  return hndls;

  //for(auto const& p : paths){
  //hndls.push_back(lib);
}
uint64_t           FreeLibs(lava_hndlvec const& hndls)
{
  uint64_t count = 0;
  for(auto const& h : hndls){
    if( FreeLibrary(h) ){ ++count; }
  }
  return count;
}
auto           GetLivePaths(lava_paths    const& paths) -> lava_paths 
{
  using namespace std;
  using namespace  fs;

  lava_paths ret;
  for(auto const& p : paths){
    ret.emplace_back( path(p).replace_extension(liveExt).generic_string() );
  }

  return ret;
}
auto         GetLiveHandles(lava_hndlMap  const& hndls, lava_paths const& paths) -> lava_hndlvec
{
  lava_hndlvec ret;
  for(auto const& p : paths){
    auto hi = hndls.find(p);         // hi is handle iterator
    if(hi != end(hndls)) ret.push_back(hi->second);
  }

  return ret;
}
auto       GetFlowNodeLists(lava_hndlvec  const& hndls) -> lava_ptrsvec
{
  //lava_flowPtrs ret; 
  //ret.reserve(hndls.size()); // nullptr);
  lava_ptrsvec ret; 
  ret.reserve(hndls.size()); // nullptr);
  TO(hndls.size(),i){
    auto h = hndls[i];
    if(h){
      auto  GetLavaFlowNodes = (GetLavaFlowNodes_t)GetProcAddress(h, TEXT("GetLavaFlowNodes") );
      LavaFlowNode* nodeList = GetLavaFlowNodes();
      //ret[i] = nodeList;

    }
  }

  return ret;
}
// end function implementations

// priority queue of packets - sort by frame number, then dest node, then dest slot

// lock free hash table of in flight packets?

#endif // endif for implementation

//__declspec(noinline) void   LavaHeapDestroyCallback(void*);
//__declspec(noinline) void*             LavaHeapInit(size_t initialSz = 0);
//__declspec(noinline) void*                LavaAlloc(size_t sz);
//__declspec(noinline) int                   LavaFree(void* memptr);

#endif












//using NodeMap      = std::map<u64, LavaFlowNode>;         // maps an id to a LavaFlowNode struct
//using NodeIdMap    = std::unordered_map<u64, u64>;        // maps a node id to its order, which can be used to find the node in the NodeMap

//auto          node(u64 id)  -> struct LavaFlowNode&
//{
//  auto idIter = m_ids.find(id);                     // idIter is identification iterator
//  if(idIter == end(m_ids)) return errorNode();
//
//  auto nIter = m_nodes.find(idIter->second);        // nIter is node iterator
//  if(nIter == end(m_nodes)) return errorNode();
//
//  return nIter->second;
//}
//auto          node(u64 id) const -> struct LavaFlowNode const& 
//{ 
//  auto idIter = m_ids.find(id);                     // idIter is identification iterator
//  if(idIter == end(m_ids)) return errorNode();      // ERR_NODE;
//
//  auto nIter = m_nodes.find(idIter->second);        // nIter is node iterator
//  if(nIter == end(m_nodes)) return errorNode();     // ERR_NODE;
//
//  return nIter->second;
//}

//u64        nxtOrder()
//{
//  u64 order = 1;
//  if(m_nodes.size() > 0)
//    order = m_nodes.rbegin()->first + 1;
//
//  return order;
//}

//using NodeMap      = std::map<u64, LavaFlowNode>;         // maps an order to a LavaFlowNode struct
//
//NodeIdMap            m_ids;

//auto  selectedNodes() -> vec_nptrs
//{
//  //vec<LavaFlowNode*> nds;                                     // nids is node ids
//  vec_nptrs nds;                                     // nids is node ids
//  for(auto& on : m_nodes){                            // on is order and node - order is on.first    node is on.second
//    if(on.second.sel) nds.push_back(&on.second);
//  }
//
//  return nds;                                         // counting on RVO (return value optimization) here
//}

//u64        delSelected()
//{
//  using namespace std;
//
//  u64    cnt = 0;
//  auto   nds = selectedNodes();      // accumulate nodes
//  auto sidxs = nodeSlots(nds);   // accumulate dest slots  // accumulate slots
//
//                                 // delete cncts with dest slots
//  for(auto sidx : sidxs){ 
//    auto s = slot(sidx);
//    if(s->in)
//      delDestCnct(sidx);  //){ ++cnt; }
//    else
//      delSrcCncts(sidx);
//  }
//
//  // delete slots
//  for(auto sidx : sidxs){ m_slots.erase(sidx); }
//
//  // delete nodes
//  for(auto n : nds){
//    m_ids.erase(n->id);
//    m_nodes.erase(n->order);
//  }
//
//  return cnt;
//}
//void         clearSels()
//{
//  for(auto& on : m_nodes) on.second.sel = false;
//  for(auto& kv : m_slots) kv.second.state = LavaFlowSlot::NORMAL;
//}
//void     clearSlotSels()
//{
//  for(auto& kv : m_slots) kv.second.state = LavaFlowSlot::NORMAL;
//}
//
// nodes
//auto       addNode(LavaFlowNode n, bool newId=true) -> LavaFlowNode&
//if(newId) n.id = nxt();
//
//n.order    = nxtOrder();
//LavaFlowNode& nref = m_nodes.insert({n.order, n}).first->second;
//
//m_ids.insert({n.id, n.order});
//
//return nref;
//}
//auto   moveToFront(u64 id) -> LavaFlowNode&
//{
//  using namespace std;
//
//  LavaFlowNode  n = node(id);
//  auto prevOrder = n.order;   // addNode will get the next order number
//                              //n.order = nxtOrder();
//
//  if(n.node_type==LavaFlowNode::NODE_ERROR) return errorNode();
//  m_nodes.erase(prevOrder); // todo: use a delNode here instead
//  m_ids.erase(id);
//
//  return addNode(n, false);
//}
//u64          order(u64 id){ return node(id).order; }
//auto           bnd(u64 id) -> Bnd& { return node(id).b; }

//
//Println("destroy called");

//auto flsHnd = FlsAlloc(HeapDestroyCallback);
//FlsSetValue(flsHnd, lava_thread_heap); // need to set the value to non-null so the callback will run
//
//return true;
//}
//else
//return false;

//static const string liveExt(".live.dll");
//
//path    root = libPath.root_directory();
//path    root = libPath.root_path();        //root( libPath );
//path       root("../x64/Debug/");
//path       root("../x64/Release/");
//
//livepth.replace_extension(".live.dll");

// std::unordered_map<std::string, HMODULE>;
//using lava_nodeMap   =  std::unordered_map<std::string, HMODULE>;

// array of structs of the contained nodes
//LavaFlowNode LavaFlowNodes[] =
//{
//  {nullptr, nullptr, nullptr, 0, 0}
//};

//__declspec(dllexport) LavaFlowNode* GetLavaFlowNodes()
//{
//  return (LavaFlowNode*)LavaFlowNodes;
//}

//
//using  Path      =   std::tr2::sys::path;

//uint64_t  LoadSharedLibraries()
//{
//  using namespace std;
//  using namespace  fs;
//  
//  static unordered_map<str, uint64_t> lastLoad;
//  static uint64_t lastReload = 0;
//
//  static regex lavaRegex("lava_.*");
//
//  uint64_t  count = 0;
//  path       root("../x64/Debug/");
//  auto    dirIter = directory_iterator(root);
//  for(auto& d : dirIter){
//    auto   p = d.path();
//    if(!p.has_filename()){ continue; }
//    
//    auto ext = p.extension().generic_string();
//    if(ext != ".dll"){ continue; }
//
//    //auto pstr = p.generic_string();                   // pstr is path string
//    str fstr = p.filename().generic_string();          // fstr is file string
//    if( !regex_match(fstr, lavaRegex) ){ continue; }
//
//    //auto   ll = lastLoad[pstr];                // ll is last load
//
//    auto livepth = p;
//    livepth.replace_extension(".live.dll");
//
//    bool doCopy = true;
//    if( exists(livepth) ){
//      auto liveWrite = last_write_time(livepth).time_since_epoch().count();     // liveWrite is live write time - the live shared library file's last write time 
//      auto origWrite = last_write_time(p).time_since_epoch().count();     // origWrite is orginal write time - the original shared library file's last write time
//      if( liveWrite > origWrite ) doCopy = false;
//    }
//      
//    if( doCopy ){
//      // shut down live file 
//      // delete live file
//      // copy old file to the live path
//      if( copy_file(p, livepth) ){ ++count; }
//    }
//  }
//
//  return count;
//}

//using FlowFunc = u64 (*)(u8 types[512], u64 values[512]);
//using FlowFunc = uint64_t(LavaFlowNode* in, LavaFlowNode* out);

//File f;
//exists();
//
//path p;
//p.replace_extension();
//remove();
//  
//m_livePath.replace_extension(".live.dll");

//struct LavaNodeType
//{
//  enum { FLOW=0, MSG }; // this should be filled in with other node types like scatter, gather, transform, generate, sink, blocking sink, blocking/pinned/owned msg - should a sink node always be pinned to it's own thread?
//};

//
//  //{nullptr, nullptr, nullptr, nullptr, 0, 0}

//
//u64 src_slot : 16;

//struct Transform;
//using  DoTransform      =  void*(*)(void* in); // void** out);
//using  GetTransforms_t  =  Transform*(*)();

//{"IndexedVerts", "IndexedVerts", "ImgFilter", 
//IndexedVerts_IndexedVerts, 0, 0},

//{"IndexedVerts", "IndexedVerts", "PcaProject", 
//IndexedVerts_PCA, 0, 0},
