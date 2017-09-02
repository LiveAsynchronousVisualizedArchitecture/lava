

#ifdef _MSC_VER
#pragma once
#endif

#ifndef __LAVAFLOW_DECL_HEADERGUARD_HPP__
#define __LAVAFLOW_DECL_HEADERGUARD_HPP__

#include <cstdint>
#include "no_rt_util.h"

//struct LavaNodeType
//{
//  enum { FLOW=0, MSG }; // this should be filled in with other node types like scatter, gather, transform, generate, sink, blocking sink, blocking/pinned/owned msg - should a sink node always be pinned to it's own thread?
//};

// this should be filled in with other node types like scatter, gather, transform, generate, sink, blocking sink, blocking/pinned/owned msg - should a sink node always be pinned to it's own thread
enum class LavaNodeType { NONE=0, FLOW, MSG };

union   ArgType{ 
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

#endif






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
