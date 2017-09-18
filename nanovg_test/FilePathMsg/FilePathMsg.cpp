

#include <thread>
#include "../no_rt_util.h"
#include "../LavaFlow.hpp"
#include "../tbl.hpp"

extern "C"
{
  const char* OutNames[]  = {"File Path", nullptr};
  const char* OutTypes[]  = {"tbl_str",   nullptr};

  uint64_t FilePathMsg(LavaParams* inout_lp, LavaVal* in, LavaOut* out)
  {
    using namespace std;

    // tbl_str filePathStr = "C:\\projects\\lava\\";
    // auto dbid = simdb.put(nodeId, filePathStr);
    // LavaArg out[0] = dbid;
    //
    //this_thread::sleep_for( chrono::milliseconds(1000) );

    printf("\n FilePathMsg run \n");

    printf("\n lava heap: %llu \n", (u64)lava_thread_heap);
    LavaHeapInit();
    printf("\n lava heap: %llu \n", (u64)lava_thread_heap);

    tbl<u8> filePath(256);
    memset(filePath.data(), 0, 256);
    const char* pthstr = "H:\\projects\\lava\\lava\\tst.txt";
    auto        pthlen = strlen(pthstr);
    memcpy(filePath.data(), pthstr, pthlen+1);

    printf("\n pthstr: %s \n  pthlen: %d  \n filePath.data(): %s \n\n", pthstr, (int)pthlen, filePath.data());
    
    //void* outmem = malloc(filePath.sizeBytes());
    void* outmem = inout_lp->mem_alloc(filePath.sizeBytes());
    memcpy(outmem, filePath.memStart(), filePath.sizeBytes());
    
    ((tbl<u8>)outmem).owned(false);
    
    inout_lp->outputs = 1;
    out[0].value = (u64)outmem;
    out[0].type  = LavaArgType::MEMORY;

    this_thread::sleep_for( chrono::milliseconds(500) );

    return 1;
  }

  LavaNode LavaNodes[] =
  {
    {
      FilePathMsg,                                   // function
      (uint64_t)LavaNode::MSG,                       // node_type  
      "FilePathMsg",                                 // name
      nullptr,                                       // in_names
      OutNames,                                      // out_names
      nullptr,                                       // in_types 
      OutTypes,                                      // out_types 
      0,                                             // version 
      0                                              // id
    },                                             

    {nullptr, (uint64_t)LavaNode::NONE, nullptr, nullptr, nullptr, nullptr, nullptr, 0, 0}
  };

  __declspec(dllexport) LavaNode* GetLavaNodes()
  {
    return (LavaNode*)LavaNodes;
  }
}








//const char* FilePathMsgTypes[] = {"tbl_str"};
//  
//const char* InNames[]   = {"File Path", nullptr};
//const char* InTypes[]   = {"tbl_str",   nullptr};

//LavaNode LavaNodes[] =
//{
//  {FilePathMsg, (uint64_t)LavaNode::MSG,         // function, node_type  
//  0, 1, "FilePathMsg",                           // inputs, outputs, name                                   
//  nullptr, FilePathMsgTypes,                     // in_types, out_types 
//  0, 0},                                         // version, id
//
//  {nullptr, (uint64_t)LavaNode::NONE, 0,0, nullptr, nullptr,nullptr, 0, 0}
//};

//((tbl<u8>*)out[0].value)->owned(false);
//
//free( (void*)out[0].value );
//
//this_thread::sleep_for( chrono::milliseconds(3000) );
//
//free( (void*)out[0].value );
