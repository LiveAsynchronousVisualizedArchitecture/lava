

#include <thread>
#include "../no_rt_util.h"
#include "../LavaFlow.hpp"
#include "../tbl.hpp"

//using millisec = std::chrono::duration<float, std::milli>;

extern "C"
{
  const char* FilePathMsgTypes[] = {"tbl_str"};

  uint64_t FilePathMsg(LavaParams* inout_lp, LavaIn* in, LavaOut* out)
  {
    using namespace std;

    // tbl_str filePathStr = "C:\\projects\\lava\\";
    // auto dbid = simdb.put(nodeId, filePathStr);
    // LavaArg out[0] = dbid;
    printf("\n FilePathMsg run \n");


    printf("\n lava heap: %llu \n", (u64)lava_thread_heap);
    LavaHeapInit();
    printf("\n lava heap: %llu \n", (u64)lava_thread_heap);


    //this_thread::sleep_for( chrono::milliseconds(1000) );

    tbl<u8> filePath(256);
    memset(filePath.data(), 0, 256);
    const char* pthstr = "H:\\projects\\lava\\lava\\tst.txt";
    auto        pthlen = strlen(pthstr);
    memcpy(filePath.data(), pthstr, pthlen+1);

    printf("\n pthstr: %s \n  pthlen: %d  \n filePath.data(): %s \n\n", pthstr, (int)pthlen, filePath.data());
    
    void* outmem = malloc(filePath.sizeBytes());
    memcpy(outmem, filePath.memStart(), filePath.sizeBytes());
    
    ((tbl<u8>)outmem).owned(false);
    
    out[0].value = (u64)outmem;
    out[0].type  = ArgType::MEMORY;

    //((tbl<u8>*)out[0].value)->owned(false);
    //
    //free( (void*)out[0].value );
    //
    //this_thread::sleep_for( chrono::milliseconds(3000) );

    free( (void*)out[0].value );

    return 1;
  }

  LavaNode LavaNodes[] =
  {
    {FilePathMsg, (uint64_t)LavaNode::MSG,         // function, node_type  
    0, 1, "FilePathMsg",                           // inputs, outputs, name                                   
    nullptr, FilePathMsgTypes,                     // in_types, out_types 
    0, 0},                                         // version, id

    {nullptr, (uint64_t)LavaNode::NONE, 0,0, nullptr, nullptr,nullptr, 0, 0}
  };

  __declspec(dllexport) LavaNode* GetLavaNodes()
  {
    return (LavaNode*)LavaNodes;
  }
}


