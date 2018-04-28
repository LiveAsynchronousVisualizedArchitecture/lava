

#include "../../no_rt_util.h"
#include "../../tbl.hpp"
#include "../LavaFlow.hpp"

enum Slots
{
  PROMPTS = 0
};

static std::atomic<bool> hasRun = false;

extern "C"
{
  const char* OutTypes[] = {"Prompts", nullptr};                        // This array contains the types that are output in each slot of the same index
  const char* OutNames[] = {"Prompts for values and their types, given by the key-values of the tbl",  nullptr};             // This array contains the names of each output slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.

  void CreatePrompts_construct()
  {
    hasRun = false;
  }
  void CreatePrompts_destruct()
  {
    hasRun = false;
  }

  uint64_t CreatePrompts(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {
    using namespace std;

    if(hasRun){ return 1; }
    //hasRun = true;

    //tbl prompts = LavaMakeTbl(lp);
    //prompts("Expected Value") = 0.f;
    //prompts("Variance")       = 0.f;
    //prompts("Vertex Count")   = (u64)0ul;
    //
    //out->push( LavaTblToOut(prompts,PROMPTS) );

    tbl newtbl = LavaMakeTbl(lp);
    newtbl("roughness") = 0.1f;

    out->push( LavaTblToOut(newtbl,PROMPTS) );

    return 1;
  }

  LavaNode LavaNodes[] =
  {
    {
      CreatePrompts,                                 // function
      CreatePrompts_construct,                       // constructor
      CreatePrompts_destruct,                        // destructor
      LavaNode::MSG,                                 // node_type  
      "CreatePrompts",                               // name
      nullptr,                                       // in_types 
      nullptr,                                       // in_names
      OutTypes,                                      // out_types 
      OutNames,                                      // out_names
      "Outputs a tbl whose keys are to be used to prompt at stdin, with the input type being the value type",   // description
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
//auto f = prompts.memStart();

//const char*  InNames[]  = {"Template Slot In",   nullptr};           // This array contains the names of each input slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
//const char*  InTypes[]  = {"IdxVerts",           nullptr};           // This array contains the type that each slot of the same index will accept as input.
