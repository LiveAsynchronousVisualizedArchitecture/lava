

#include <atomic>
#include <stdlib.h>
#include "../../no_rt_util.h"
#include "../../tbl.hpp"
#include "../LavaFlow.hpp"

enum Slots
{
  IN_CACHE_SET = 0,        
  OUT_CACHE    = 0
};

static std::atomic<void*> cachePtr;

bool setCachePtr(void* p)
{
  auto prev = cachePtr.exchange(nullptr);
  if(prev){
    free(prev);
    return true;
  }
  return false;
}

extern "C"
{
  const char*  InTypes[]  = {"",              nullptr};
  const char*  InNames[]  = {"Cache Value",   nullptr};
  const char* OutTypes[]  = {"",              nullptr};
  const char* OutNames[]  = {"Cache Output",  nullptr};

  void Cache_construct()
  {
    cachePtr.store(nullptr);
  }
  void Cache_destruct()
  {
    setCachePtr(nullptr);
  }

  uint64_t Cache(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {
    using namespace std;

    SECTION(if there is an input packet, use it to set the cache)
    {
      if(in->slotMask[IN_CACHE_SET]){
        u64         sz = in->packets[IN_CACHE_SET].sz_bytes;
        void*    inPtr = (void*)in->packets[IN_CACHE_SET].val.value;
        void* allocPtr = malloc(sz);
        if(allocPtr){
          memcpy(allocPtr, inPtr, sz);
          setCachePtr(allocPtr);
        }
      }
    }
    SECTION(if the cache has been set at any point, output a packet)
    {
      void* outPtr = cachePtr.load();
      if(outPtr != nullptr){
        LavaOut o;
        o.val.value = (u64)outPtr;
        o.val.type  = LavaArgType::MEMORY;
        o.key.slot  = OUT_CACHE;
        out->push(o);
      }
    }

    return 1;
  }

  LavaNode LavaNodes[] =
  {
    {
      Cache,                                      // function
      Cache_construct,                            // constructor - this can be set to nullptr if not needed
      Cache_destruct,                             // destructor  - this can also be set to nullptr 
      LavaNode::CACHE,                            // node_type   - this should be eighther LavaNode::MSG (will be run even without input packets) or LavaNode::FLOW (will be run only when at least one packet is available for input)
      "Cache",                                    // name
      InTypes,                                    // in_types    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      InNames,                                    // in_names    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      OutTypes,                                   // out_types   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      OutNames,                                   // out_names   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr
      nullptr,                                    // description
      0                                           // version 
    },                                             

    LavaNodeListEnd                                  // This is a constant that has all the members of the LavaNode struct set to 0 or nullptr - it is used as a special value that ends the static list of LavaNodes. This negates the need for a separate static variable that gives the size of the list, which would be have to be kept in sync and therefore be error prone.
  };

  __declspec(dllexport) LavaNode* GetLavaNodes()
  {
    return (LavaNode*)LavaNodes;
  }
}







//
//out->push( LavaTblToOut(outputTbl, OUT_CACHE) );

//u32 i=0;
//while( LavaNxtPckt(in, &i) )
//{
//  tbl inputTbl( (void*)(in->packets[i-1].val.value) );
//  for(auto& kv : inputTbl){  // this loop demonstrates how to iterate through non-empty map elements
//  }	
//}

