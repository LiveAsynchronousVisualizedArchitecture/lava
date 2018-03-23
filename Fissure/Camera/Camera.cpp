

#include "../../no_rt_util.h"
#include "../../tbl.hpp"
#include "../LavaFlow.hpp"

//AffineSpace3f camSpace = AffineSpace3f::lookAtPoint(_camPos, _camLookAt, _camUp);
//local2world = camSpace;
//angle       = _angle;
//aspectRatio = _aspectRatio;
// Vector3f W     = xfmVector(local2world, Vector3f(-0.5f*aspectRatio,-0.5f,0.5f*rcp(tanf(deg2rad(0.5f*angle)))));
// pixel2world = AffineSpace3f(aspectRatio*local2world.l.vx,local2world.l.vy,W,local2world.p);

enum Slots
{
  CAMERA_OUT     = 0,
  CAMERA_VIZ_OUT = 1
};

extern "C"
{
  const char*   InTypes[]  = {nullptr};
  const char*   InNames[]  = {nullptr};
  const char*  OutTypes[]  = {"Camera",                            "IdxVerts",             nullptr};
  const char*  OutNames[]  = {"Camera Position and Field of View", "Camera Visualiztion",  nullptr};

  void Camera_construct(){ }
  void Camera_destruct(){ }

  uint64_t Camera(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {
    using namespace std;



    //u32 i=0;
    //while( LavaNxtPckt(in, &i) )
    //{
      //tbl inputTbl( (void*)(in->packets[i-1].val.value) );
      //
      //for(auto& kv : inputTbl){  // this loop demonstrates how to iterate through non-empty map elements
      //}	
      //
      // out->push( LavaTblToOut(outputTbl, SLOT_OUT) );      // this demonstrates how to output a tbl into the first output slot
    //}

    return 1;
  }

  LavaNode LavaNodes[] =
  {
    {
      Camera,                                      // function
      Camera_construct,                            // constructor - this can be set to nullptr if not needed
      Camera_destruct,                             // destructor  - this can also be set to nullptr 
      LavaNode::FLOW,                                // node_type   - this should be eighther LavaNode::MSG (will be run even without input packets) or LavaNode::FLOW (will be run only when at least one packet is available for input)
      "Camera",                                    // name
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

