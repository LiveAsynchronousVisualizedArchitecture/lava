

#ifdef _MSC_VER
#pragma once
#endif

#ifndef __LAVAFLOW_DECL_HEADERGUARD_HPP__
#define __LAVAFLOW_DECL_HEADERGUARD_HPP__

#include "../no_rt_util.h"

union   ArgType{ 
  enum { STORE=0, MEMORY, DATA_ERROR, SEQUENCE, ENUMERATION };
  u8 asInt;
};
struct      Arg
{
  u64      type :  8;
  u64  src_node : 48;
  u64     value;

  //u64 src_slot : 16;
};
struct      Msg
{
  u64 id;

  Arg arg;
};
struct   Packet
{
  u64    ref_count;
  u64        frame : 63;
  u64       framed :  1;
  u64    dest_node : 48;
  u64    dest_slot : 16;
  Msg          msg;
};

// prototype of data flow node function
using FlowFunc = u64 (*)(u8 types[512], u64 values[512]);

struct LavaFlowNode
{
  const char*        in_type;
  const char*       out_type;
  const char*           name;
  FlowFunc              func;
  size_t             version;
  size_t                  id;
};

// plugin loading struct
extern "C"
{
  using GetLavaFlowNode  =  LavaFlowNode*(*)();

  // array of structs of the contained nodes
  LavaFlowNode LavaFlowNodes[] =
  {
    {nullptr, nullptr, nullptr, nullptr, 0, 0}
  };

  // prototype of function to return static plugin loading struct
  __declspec(dllexport) LavaFlowNode* GetLavaFlowNodes()
  {
    return (LavaFlowNode*)LavaFlowNodes;
  }
}

// priority queue of packets - sort by frame number, then dest node, then dest slot

// lock free hash table of in flight packets?

#endif






//struct Transform;
//using  DoTransform      =  void*(*)(void* in); // void** out);
//using  GetTransforms_t  =  Transform*(*)();

//{"IndexedVerts", "IndexedVerts", "ImgFilter", 
//IndexedVerts_IndexedVerts, 0, 0},

//{"IndexedVerts", "IndexedVerts", "PcaProject", 
//IndexedVerts_PCA, 0, 0},
