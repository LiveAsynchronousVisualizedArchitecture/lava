

#include "../no_rt_util.h"
#include "../LavaFlow.hpp"

extern "C"
{
  const char* FilePathMsgTypes[] = {"tbl_str"};

  uint64_t FilePathMsg(LavaArg* in, LavaArg* out)
  {
    // tbl_str filePathStr = "C:\\projects\\lava\\";
    // auto dbid = simdb.put(nodeId, filePathStr);
    // LavaArg out[0] = dbid;
    printf("\nFilePathMsg run\n");
    return 1;
  }

  LavaFlowNode lavaFlowNodes[] =
  {
    {FilePathMsg, (uint64_t)LavaFlowNode::MSG,     // function, node_type  
    0, 1, "FilePathMsg",                           // inputs, outputs, name                                   
    nullptr, FilePathMsgTypes,                     // in_types, out_types 
    0, 0},                                         // version, id

    {nullptr, (uint64_t)LavaFlowNode::NONE, 0,0, nullptr, nullptr,nullptr, 0, 0}
  };

  __declspec(dllexport) LavaFlowNode* GetLavaFlowNodes()
  {
    return (LavaFlowNode*)lavaFlowNodes;
  }
}


