

#include "../shared/vec.hpp"
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

const f32     fovAngle  =  65.f;
const f32  asepctRatio  =   1.f;
const f32    center[3]  =  {0, 0,  5.f};
const f32    camDir[3]  =  {0, 0, -1.f};

template<typename T> __forceinline T  deg2rad(const T& x){ return x * T(1.74532925199432957692e-2f); }
f32 rcp(f32 x){ return 1.f / x; }                         // does rcp stand for recipricol? 

extern "C"
{
  const char*   InTypes[]  = {"Dummy", nullptr};
  const char*   InNames[]  = {"Dummy Slot", nullptr};
  const char*  OutTypes[]  = {"Camera",                            "IdxVerts",             nullptr};
  const char*  OutNames[]  = {"Camera Position and Field of View", "Camera Visualiztion",  nullptr};

  void Camera_construct(){ }
  void Camera_destruct(){ }

  uint64_t Camera(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {
    using namespace std;

    //f32 fovOfst = .5f * rcp(tanf(deg2rad( .5f * fovAngle )));
    f32 fovOfst = tanf(deg2rad( fovAngle ));

    tbl      iv = LavaMakeTbl(lp);
    SECTION(make idx verts to visualize camera position and frustrum)
    {
      tbl   px = LavaMakeTbl(lp);
      tbl   py = LavaMakeTbl(lp);
      tbl   pz = LavaMakeTbl(lp);
      tbl  ind = LavaMakeTbl(lp);

      px.setArrayType<f32>();
      py.setArrayType<f32>();
      pz.setArrayType<f32>();
      ind.setArrayType<u32>();
      iv.setArrayType<i8>();

      px.push(center[0]);
      py.push(center[1]);
      pz.push(center[2]);

      px.push(center[0] - fovOfst);
      px.push(center[0] + fovOfst);
      px.push(center[0] + fovOfst);
      px.push(center[0] - fovOfst);

      py.push(center[1] - fovOfst);
      py.push(center[1] - fovOfst);
      py.push(center[1] + fovOfst);
      py.push(center[1] + fovOfst);

      pz.push(center[2] + 1.f);
      pz.push(center[2] + 1.f);
      pz.push(center[2] + 1.f);
      pz.push(center[2] + 1.f);

      TO(4,i){
        ind.push( 0u );
        ind.push( (u32)(i+1) );
      }
      TO(3,i){
        ind.push( (u32)(i+1) );
        ind.push( (u32)(i+2) );
      }
      ind.push( 4u );
      ind.push( 1u );

      iv("positions x")  = &px;
      iv("positions y")  = &py;
      iv("positions z")  = &pz;
      iv("indices")      = &ind;
      iv("mode")         = 1;            // 0 should be points, 1 should be lines
      iv("type")         = tbl::StrToInt("IdxVerts");
      iv.flatten();
    }
    
    out->push( LavaTblToOut(iv, CAMERA_VIZ_OUT) );      // this demonstrates how to output a tbl into the first output slot

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








//__forceinline float rcp  ( const float x )
//{
//  const __m128 a = _mm_set_ss(x);
//
//#if defined(__AVX512VL__)
//  const __m128 r = _mm_rcp14_ss(_mm_set_ss(0.0f),a);
//#else
//  const __m128 r = _mm_rcp_ss(a);
//#endif
//
//#if defined(__AVX2__)
//  return _mm_cvtss_f32(_mm_mul_ss(r,_mm_fnmadd_ss(r, a, _mm_set_ss(2.0f))));
//#else
//  return _mm_cvtss_f32(_mm_mul_ss(r,_mm_sub_ss(_mm_set_ss(2.0f), _mm_mul_ss(r, a))));
//#endif
//}

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
