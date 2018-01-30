
#include "../../no_rt_util.h"
#include "../../tbl.hpp"
#include "../LavaFlow.hpp"

enum Slots
{
  // This is an example enumeration that is meant to be helpful, though is not strictly neccesary. Referencing slots by a name will generally be less error prone than using their index and remembering what each index is for
  
  SLOT_0 = 0,        
  SLOT_1 = 1
  //SLOT_2 = 2,
  //SLOT_3 = 3
};

extern "C"
{
  const char*  InNames[] = {"Input Model",         nullptr};            // This array contains the names of each input slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  const char*  InTypes[] = {"IdxVerts",            nullptr};            // This array contains the type that each slot of the same index will accept as input.
  const char* OutNames[] = {"Output Model",        nullptr};            // This array contains the names of each output slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  const char* OutTypes[] = {"IdxVerts",            nullptr};            // This array contains the types that are output in each slot of the same index
  
  //const char*  InNames[]  = {"Template Slot In",   nullptr};            // This array contains the names of each input slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  //const char* OutNames[]  = {"Template Slot Out",  nullptr};            // This array contains the names of each output slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.

  uint64_t TfmCube(LavaParams* inout_lp, LavaFrame* in, lava_threadQ* out) noexcept
  {
    using namespace std;

    TO(in->packets.size(),i) if(in->slotMask[i]){
      LavaPacket const& pkt = in->packets[i];
    }

    //LavaOut lo;
    //lo.type  = LavaArgType::MEMORY;
    //lo.value = (u64)nullptr;
    //out->push(lo);

    out->push(LavaOut(0));
    out->push({0});

    return 1;
  }

  LavaNode LavaNodes[] =
  {
    {
      TfmCube,                                       // function
      LavaNode::FLOW,                                // node_type  
      "TfmCube",                                     // name
      InNames,                                       // in_names
      OutNames,                                      // out_names
      InTypes,                                       // in_types 
      OutTypes,                                      // out_types 
      0                                              // version 
    },                                             

    LavaNodeListEnd                                  // This is a constant that has all the members of the LavaNode struct set to 0 or nullptr - it is used as a special value that ends the static list of LavaNodes. This negates the need for a separate static variable that gives the size of the list, which would be have to be kept in sync and therefore be error prone.
  };

  __declspec(dllexport) LavaNode* GetLavaNodes()
  {
    return (LavaNode*)LavaNodes;
  }
}









//in[i].packets
//
//TO(inout_lp->inputs,i)
//{
//  LavaFrame const& frame = in[i];
//  TO(frame.packets.size(),j)
//  {
//    if(frame.slotMask[j]){
//      LavaPacket const& pkt = frame.packets[j];
//    }
//  }
//}
