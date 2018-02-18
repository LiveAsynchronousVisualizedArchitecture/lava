

#include "../../no_rt_util.h"
#include "../../tbl.hpp"
#include "../LavaFlow.hpp"

enum Slots
{
  FILLED_PROMPT_VALUES_SLOT = 0, 
};

extern "C"
{
  const char*  InNames[]  = {"Command Prompts and Return Types",   nullptr};     // This array contains the names of each input slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  const char*  InTypes[]  = {"InTypes",                            nullptr};     // This array contains the type that each slot of the same index will accept as input.
  const char* OutNames[]  = {"Command Prompt Values",              nullptr};     // This array contains the names of each output slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  const char* OutTypes[]  = {"KeyVals",                            nullptr};     // This array contains the types that are output in each slot of the same index

  uint64_t CmdLineInput(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {
    using namespace std;

    //TO(in->packets.size(),i) if(in->slotMask[i])
    u32 i=0;
    while( LavaNxtPckt(in, &i) )
    {
      tbl  prompts(in->packets[i].val.value);
      tbl prmptsVals = prompts;
      auto   elemCnt = prmptsVals.elems();
      auto       els = prmptsVals.elemStart();          // els is elements - a pointer to the start of the elements/key-values section of the tbl
      TO(elemCnt,e) if(!els[e].isEmpty()){
        auto       el = els[e];
        str  scanType = "%d";                     // initialize all characters to 0
        switch(el.type){
          case tbl::TblType::F32:
            scanType = "%f"; 
            break;
          default: break;
        }
        str scnStr = els[e].key + str(": ") + scanType;         // need custom type string per type
        scanf(scnStr.c_str(), &el.val);
      }

      LavaOut o;
      o.key.slot  = FILLED_PROMPT_VALUES_SLOT;
      o.val.value = (u64)prmptsVals.memStart();
      o.val.type  = LavaArgType::MEMORY;
      out->push(o);
    }

    return 1;
  }

  LavaNode LavaNodes[] =
  {
    {
      CmdLineInput,                                  // function
      nullptr,                                       // constructor
      nullptr,                                       // destructor
      LavaNode::MSG,                                 // node_type  
      "CmdLineInput",                                // name
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
