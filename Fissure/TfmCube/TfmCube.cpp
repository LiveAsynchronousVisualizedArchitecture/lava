
#include "../../no_rt_util.h"
#include "../../tbl.hpp"
#include "../LavaFlow.hpp"

struct vert { f32 p[3],n[3],c[4],tx[2]; };
//using IvTbl = tbl<vert>;
using IvTbl = tbl;

enum Slots
{
  // This is an example enumeration that is meant to be helpful, though is not strictly neccesary. Referencing slots by a name will generally be less error prone than using their index and remembering what each index is for
  MODEL_OUT = 0,
  //SLOT_1 = 1
  //SLOT_2 = 2,
  //SLOT_3 = 3
};

extern "C"
{
  const char*  InNames[] = {"Input Model",         nullptr};            // This array contains the names of each input slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  const char*  InTypes[] = {"IdxVerts",            nullptr};            // This array contains the type that each slot of the same index will accept as input.
  const char* OutNames[] = {"Output Model",        nullptr};            // This array contains the names of each output slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  const char* OutTypes[] = {"IdxVerts",            nullptr};            // This array contains the types that are output in each slot of the same index
  
  uint64_t TfmCube(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {
    using namespace std;

    TO(in->packets.size(),i) if(in->slotMask[i])
    {
      LavaPacket const& pkt = in->packets[i];
      //void*               p = (void*)pkt.msg.val.value;
      void*               p = (void*)pkt.val.value;
      IvTbl inCube(p, false, false);
      auto  bytes = inCube.sizeBytes();
      void*    tp = lp->mem_alloc(bytes);
      memcpy(tp, p, bytes);
      IvTbl oCube(tp, false, false);
      TO(oCube.size(),i){
        oCube[i].p[0] += 1.f;
        oCube[i].p[1] += 1.f;
        
        oCube[i].c[0] = 0;
        oCube[i].c[1] = 0;
      }

      out->push({MODEL_OUT, (u64)oCube.memStart()});                               // Creates a LavaOut struct
    }

    return 1;
  }

  LavaNode LavaNodes[] =
  {
    {
      TfmCube,                                       // function
      nullptr,                                       // constructor
      nullptr,                                       // destructor
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









//
//out->push({MODEL_OUT, pkt.msg.val.value, LavaArgType::PASSTHRU });

//IvTbl  oCube(p, false, false);
//tbl<vert>::make_borrowed(lp->mem_alloc, oCube.

//out->push(LavaOut(0));
//out->push({0});

//LavaOut lo;
//lo.type  = LavaArgType::MEMORY;
//lo.value = (u64)nullptr;
//out->push(lo);

//const char*  InNames[]  = {"Template Slot In",   nullptr};            // This array contains the names of each input slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
//const char* OutNames[]  = {"Template Slot Out",  nullptr};            // This array contains the names of each output slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.

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
