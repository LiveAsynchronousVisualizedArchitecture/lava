
// todo: make LoadSharedLibraries into:
//       -a function to say which libraries need to be refreshed
//       a function to unload the libraries 
//       a function copy the libraries 
//       a function to load them again

#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __LAVAFLOW_DECL_HEADERGUARD_HPP__
#define __LAVAFLOW_DECL_HEADERGUARD_HPP__

//#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <regex>
#include <filesystem>
#include "no_rt_util.h"

using str = std::string;

#if defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #define NOMINMAX
  #include <Windows.h>
#endif

// start allocator declarations
static __declspec(thread) void* lava_thread_heap = nullptr;

__declspec(noinline) void   LavaHeapDestroyCallback(void*);
__declspec(noinline) void*  LavaHeapInit(size_t initialSz = 0);
__declspec(noinline) void*  LavaAlloc(size_t sz);
__declspec(noinline) int    LavaFree(void* memptr);

BOOL WINAPI DllMain(
  _In_  HINSTANCE  hinstDLL,
  _In_  DWORD      fdwReason,
  _In_  LPVOID     lpvReserved
);

template <class T>
struct ThreadAllocator
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
template <class T>
T*     ThreadAllocator<T>::allocate(const size_t n) const
{
  if(n==0) return nullptr;

  if( n > static_cast<size_t>(-1)/sizeof(T) ) {
    throw std::bad_array_new_length();
  }

  void* p = ThreadAlloc( n * sizeof(T) );
  if(!p) { throw std::bad_alloc(); }

  return static_cast<T*>(p);
}

template<class T>
void   ThreadAllocator<T>::deallocate(T*& p, size_t) const
{
  //if(n==0) return;

  ThreadFree(p);
  p = nullptr;
}
// end allocator declarations

// start allocator implementations
void    LavaHeapDestroyCallback(void* heapHnd)
{
  if(heapHnd)
    HeapDestroy(heapHnd);
  //Println("destroy called");
}
void*   LavaHeapInit(size_t initialSz)
{
  if(!lava_thread_heap) {
    lava_thread_heap = HeapCreate(HEAP_NO_SERIALIZE, initialSz, 0);
    //auto flsHnd = FlsAlloc(HeapDestroyCallback);
    //FlsSetValue(flsHnd, lava_thread_heap); // need to set the value to non-null so the callback will run
  }
  return lava_thread_heap;

  //return true;
  //}
  //else
  //return false;
}
void*   LavaAlloc(size_t sz)
{
  void* thread_heap = LavaHeapInit(sz);

  void* ret = nullptr;
  if(thread_heap)
    ret = HeapAlloc(thread_heap, HEAP_NO_SERIALIZE, sz);

  return ret;
}
int     LavaFree(void* memptr)
{
  void* thread_heap = LavaHeapInit();

  void* ret = nullptr;
  if(thread_heap && memptr) {
    auto ret = HeapFree(thread_heap, HEAP_NO_SERIALIZE, memptr);
    return ret;
  }else{ return 0; }
}

BOOL WINAPI DllMain(
  _In_ HINSTANCE hinstDLL,
  _In_ DWORD     fdwReason,
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
// end allocator implementations

// this should be filled in with other node types like scatter, gather, transform, generate, sink, blocking sink, blocking/pinned/owned msg - should a sink node always be pinned to it's own thread
enum class LavaNodeType { NONE=0, FLOW, MSG };

union       ArgType{ 
  enum { END=0, DATA_ERROR, STORE, MEMORY, SEQUENCE, ENUMERATION };
  u8 asInt;
};
struct      LavaArg
{
  u64      type :  3;
  u64     value : 61;
};
struct      LavaMsg
{
  u64        id;
  LavaArg   arg;
};
struct   LavaPacket
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
struct LavaFlowNode;

// prototype of data flow node function
//using FlowFunc = u64 (*)(u8 types[512], u64 values[512]);
extern "C" using FlowFunc = uint64_t (*)(LavaArg* in, LavaArg* out);
//using FlowFunc = uint64_t(LavaFlowNode* in, LavaFlowNode* out);

struct LavaFlowNode
{
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

// plugin loading struct
extern "C"
{
  using GetLavaFlowNodes_t  =  LavaFlowNode*(*)();

  // array of structs of the contained nodes
  //LavaFlowNode LavaFlowNodes[] =
  //{
  //  {nullptr, nullptr, nullptr, 0, 0}
  //};

  // prototype of function to return static plugin loading struct
  __declspec(dllexport) LavaFlowNode* GetLavaFlowNodes();

  //__declspec(dllexport) LavaFlowNode* GetLavaFlowNodes()
  //{
  //  return (LavaFlowNode*)LavaFlowNodes;
  //}

}

// priority queue of packets - sort by frame number, then dest node, then dest slot

// lock free hash table of in flight packets?

// shared library loading 

//using  Path      =   std::tr2::sys::path;
namespace fs = std::tr2::sys;   // todo: different compiler versions would need different filesystem paths


auto GetRefreshPaths() -> std::vector<str>
{
  using namespace std;
  using namespace  fs;

  static regex lavaRegex("lava_.*");

  std::vector<str> paths;
  path       root("../x64/Debug/");
  auto    dirIter = directory_iterator(root);
  for(auto& d : dirIter){
    auto   p = d.path();
    if(!p.has_filename()){ continue; }

    auto ext = p.extension().generic_string();
    if(ext != ".dll"){ continue; }

    str fstr = p.filename().generic_string();          // fstr is file string
    if( !regex_match(fstr, lavaRegex) ){ continue; }

    auto livepth = p;
    livepth.replace_extension(".live.dll");

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

uint64_t LoadSharedLibraries()
{
  using namespace std;
  using namespace  fs;
  
  static unordered_map<str, uint64_t> lastLoad;
  static uint64_t lastReload = 0;

  static regex lavaRegex("lava_.*");

  uint64_t  count = 0;
  path       root("../x64/Debug/");
  auto    dirIter = directory_iterator(root);
  for(auto& d : dirIter){
    auto   p = d.path();
    if(!p.has_filename()){ continue; }
    
    auto ext = p.extension().generic_string();
    if(ext != ".dll"){ continue; }

    //auto pstr = p.generic_string();                   // pstr is path string
    str fstr = p.filename().generic_string();          // fstr is file string
    if( !regex_match(fstr, lavaRegex) ){ continue; }

    //auto   ll = lastLoad[pstr];                // ll is last load

    auto livepth = p;
    livepth.replace_extension(".live.dll");

    bool doCopy = true;
    if( exists(livepth) ){
      auto liveWrite = last_write_time(livepth).time_since_epoch().count();     // liveWrite is live write time - the live shared library file's last write time 
      auto origWrite = last_write_time(p).time_since_epoch().count();     // origWrite is orginal write time - the original shared library file's last write time
      if( liveWrite > origWrite ) doCopy = false;
    }
      
    if( doCopy ){
      // shut down live file 
      // delete live file
      // copy old file to the live path
      if( copy_file(p, livepth) ){ ++count; }
    }
  }

  return count;
}

// end shared library loading

#endif








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
