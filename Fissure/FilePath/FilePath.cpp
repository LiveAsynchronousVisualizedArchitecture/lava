
#include "../../no_rt_util.h"
#include "../../tbl.hpp"
#include "../LavaFlow.hpp"

enum Slots
{  
  FILE_PATH_OUT = 0
};

static std::atomic<bool> hasRun = false;

extern "C"
{
  const char*  InTypes[]  = {"temp",               nullptr};                               // This array contains the type that each slot of the same index will accept as input.
  const char*  InNames[]  = {"temp slot",          nullptr};                               // This array contains the names of each input slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  const char* OutTypes[]  = {"ASCII",              nullptr};         // This array contains the types that are output in each slot of the same index
  const char* OutNames[]  = {"FilePath Slot Out",  nullptr};         // This array contains the names of each output slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.

  void FilePath_construct()
  {
    hasRun = false;
  }
  void FilePath_destruct()
  {
    hasRun = false;
  }

  uint64_t FilePath(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {
    using namespace std;

    if( hasRun.exchange(true) ){ 
      return 1;
    }

    tbl filePath = LavaMakeTbl(lp);
    filePath.setArrayType<char>();
    filePath.reserve(64,0);
    str      pth = "H:\\projects\\lava\\obj\\Craftsman.obj";
    TO(pth.length(),i){ 
      filePath.push( pth[i] ); 
    }
    filePath.push( '\0' ); 

    out->push( LavaTblToOut(filePath, FILE_PATH_OUT) );      // this demonstrates how to output a tbl into the first output slot

    return 1;
  }

  LavaNode LavaNodes[] =
  {
    {
      FilePath,                                      // function
      FilePath_construct,                            // constructor - this can be set to nullptr if not needed
      FilePath_destruct,                             // destructor  - this can also be set to nullptr 
      LavaNode::MSG,                                 // node_type   - this should be eighther LavaNode::MSG (will be run even without input packets) or LavaNode::FLOW (will be run only when at least one packet is available for input)
      "FilePath",                                    // name
      InTypes,                                       // in_types    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      InNames,                                       // in_names    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      OutTypes,                                      // out_types   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      OutNames,                                      // out_names   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr
      nullptr,                                       // description
      0                                              // version 
    },                                             

    LavaNodeListEnd                                  // This is a constant that has all the members of the LavaNode struct set to 0 or nullptr - it is used as a special value that ends the static list of LavaNodes. This negates the need for a separate static variable that gives the size of the list, which would be have to be kept in sync and therefore be error prone.
  };

  __declspec(dllexport) LavaNode* GetLavaNodes()
  {
    return (LavaNode*)LavaNodes;
  }
}


//
//filePath.memStart()->arrayType = tbl::TblType::I8;

//u32 i=0;
//while( LavaNxtPckt(in, &i) )
//{
//  tbl inputTbl( (void*)(in->packets[i].val.value) );
//
//  for(auto& kv : inputTbl){  // this loop demonstrates how to iterate through non-empty map elements
//  }	
//
//  // out->push( LavaTblToOut(outputTbl, SLOT_0) );      // this demonstrates how to output a tbl into the first output slot
//}
