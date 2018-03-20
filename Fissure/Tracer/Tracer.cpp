

#include "rtcore.h"

#include "../../no_rt_util.h"
#include "../../tbl.hpp"
#include "../LavaFlow.hpp"

enum Slots
{
  // This is an example enumeration that is meant to be helpful, though is not strictly neccesary. Referencing slots by a name will generally be less error prone than using their index and remembering what each index is for
  SLOT_IN  = 0,        
  SLOT_OUT = 0
};

RTCDevice g_device;

extern "C"
{
  const char* description = "Takes in geometry that will be sorted into an acceleration structure and ultimatly used to trace rays";                                     // description

  const char*  InTypes[]  = {"IdxVerts",               nullptr};          // This array contains the type that each slot of the same index will accept as input.
  const char*  InNames[]  = {"Input Scene Geometry",   nullptr};          // This array contains the names of each input slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  const char* OutTypes[]  = {"IdxVerts",               nullptr};          // This array contains the types that are output in each slot of the same index
  const char* OutNames[]  = {"Traced Rays",            nullptr};          // This array contains the names of each output slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.

  void Tracer_construct()
  {
    /* create new Embree device */
    g_device = rtcNewDevice("");
    //error_handler(nullptr,rtcGetDeviceError(g_device));

    /* set error handler */
    //rtcSetDeviceErrorFunction(g_device,error_handler,nullptr);

    /* create scene */
    //g_scene = rtcNewScene(g_device);

    /* add cube */
    //addCube(g_scene);

    /* add ground plane */
    //addGroundPlane(g_scene);

    /* commit changes to scene */
    //rtcCommitScene (g_scene);

    /* set start render mode */
    //renderTile = renderTileStandard;
    //key_pressed_handler = device_key_pressed_default;
  }
  void Tracer_destruct()
  {
    rtcReleaseDevice(g_device);
  }

  uint64_t Tracer(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {
    using namespace std;

    //SECTION(input packet loop)
    //{
    //  u32 i=0;
    //  while( LavaNxtPckt(in, &i) )
    //  {
    //    tbl inputTbl( (void*)(in->packets[i-1].val.value) );
    //
    //    for(auto& kv : inputTbl){  // this loop demonstrates how to iterate through non-empty map elements
    //    }	
    //
    //    // out->push( LavaTblToOut(outputTbl, SLOT_OUT) );      // this demonstrates how to output a tbl into the first output slot
    //  }
    //}

    return 1;
  }

  LavaNode LavaNodes[] =
  {
    {
      Tracer,                                       // function
      Tracer_construct,                            // constructor - this can be set to nullptr if not needed
      Tracer_destruct,                             // destructor  - this can also be set to nullptr 
      LavaNode::MSG,                               // node_type   - this should be eighther LavaNode::MSG (will be run even without input packets) or LavaNode::FLOW (will be run only when at least one packet is available for input)
      "Tracer",                                    // name
      InTypes,                                     // in_types    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      InNames,                                     // in_names    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      OutTypes,                                    // out_types   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      OutNames,                                    // out_names   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr
      description,                                 // description
      0                                            // version 
    },                                             

    LavaNodeListEnd                                  // This is a constant that has all the members of the LavaNode struct set to 0 or nullptr - it is used as a special value that ends the static list of LavaNodes. This negates the need for a separate static variable that gives the size of the list, which would be have to be kept in sync and therefore be error prone.
  };

  __declspec(dllexport) LavaNode* GetLavaNodes()
  {
    return (LavaNode*)LavaNodes;
  }
}

