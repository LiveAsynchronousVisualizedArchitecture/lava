

//#include "../../libcpp.hpp"

//extern "C" void __cdecl __std_terminate(){}
//#include <intrin.h>
//#include "rdseed.c"
//#include "rdrand.c"

#include <random>
#include <limits>
//#include "drng.h"
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
  IN_CAMERA_PARAMS  = 0,

  RAYS_OUT          = 0,
  CAMERA_VIZ_OUT    = 1
};

namespace RNG
{
  using rng_t  = ::std::mt19937;
  using urng_t = ::std::unique_ptr<rng_t>;

  //rng_t   gen(0);
  //rng_t*  m_genPtr = &gen;

  rng_t   gen;
  rng_t*  m_genPtr;
}

//float     randomf(float lo, float hi)
//{
//  ::std::uniform_real_distribution<float> dis(lo, hi);
//  return dis(*RNG::m_genPtr);
//}
float     randomf()
{
  ::std::uniform_real_distribution<float> dis(0.f, 1.f);
  return dis(*RNG::m_genPtr);
}

//float     randomf()
//{
//  using namespace std;
//
//  u64 x64 = 0;
//  int err = rdrand_64(&x64, 1);
//
//  u32   x = x64 >> 32;
//
//  f64 ret64 = (f64)x / (f64)(numeric_limits<u32>::max());
//
//  return (f32)ret64;
//}

//float     randomf()
//{
//  return 0.5f;
//}

template<typename T> __forceinline T  deg2rad(const T& x){ return x * T(1.74532925199432957692e-2f); }
f32 rcp(f32 x){ return 1.f / x; }                         // does rcp stand for recipricol? 

f32 lerp(f32 a, f32 b, f32 n){ return (1.f-n)*a  +  n*b; }
v2f lerp(v2f a, v2f b, f32 n)
{
  v2f ret;
  TO(2,i){
    ret[0] = lerp(a[0], b[0], n);
  }
  return ret;
}

tbl raysToIdxVerts(LavaParams const* lp, tbl const& rays)
{
  using namespace std;
  
  tbl   ox = rays("origin x");
  tbl   oy = rays("origin y");
  tbl   oz = rays("origin z");
  tbl   dx = rays("direction x");
  tbl   dy = rays("direction y");
  tbl   dz = rays("direction z");

  tbl   iv, px, py, pz, cr, cg, ca, ind;
  //iv = px = py = pz = cr = cg = ca = ind = LavaLocalTbl(lp);
  SECTION(create tables for indexed verts)
  {
    iv  = LavaMakeTbl(lp, 0, (u8)0);
    px  = LavaMakeTbl(lp, 0, 0.f);
    py  = LavaMakeTbl(lp, 0, 0.f);
    pz  = LavaMakeTbl(lp, 0, 0.f);
    cr  = LavaMakeTbl(lp, 0, 0.f);
    cg  = LavaMakeTbl(lp, 0, 0.f);
    ca  = LavaMakeTbl(lp, 0, 0.f);
    ind = LavaMakeTbl(lp, 0,  0u);
  }

  TO(ox.size(),i){
    px.push<f32>( (f32)ox[i] );
    py.push<f32>( (f32)oy[i] );
    pz.push<f32>( (f32)oz[i] );

    px.push<f32>( (f32)ox[i] + (f32)dx[i] );
    py.push<f32>( (f32)oy[i] + (f32)dy[i] );
    pz.push<f32>( (f32)oz[i] + (f32)dz[i] );

    cr.push(0.5f);
    cr.push(1.0f);
    cg.push(0.0f);
    cg.push(1.0f);
    ca.push(0.1f);
    ca.push(0.1f);

    ind.push( (u32)(i*2 + 0) );
    ind.push( (u32)(i*2 + 1) );
  }

  iv("positions x")  = &px;
  iv("positions y")  = &py;
  iv("positions z")  = &pz;
  iv("colors red")   = &cr;
  iv("colors green") = &cg;
  iv("colors alpha") = &ca;
  iv("indices")      = &ind;
  iv("mode")         = (u32)1;            // 0 should be points, 1 should be lines
  iv("type")         = tbl::StrToInt("IdxVerts");
  iv.flatten();

  return move(iv);
}

//const f32     fovAngle  =  40.f;
//const f32  asepctRatio  =   1.f;
//const u64       rayCnt  =  1000;
//const f32    origin[3]  =  {1.f, 2.f,   5.f};
//const f32    camDir[3]  =  {0,     0,  -1.f};

static f32          INF;  //=  std::numeric_limits<f32>::infinity();

static f32     fovAngle  =  40.f;
static f32  aspectRatio  =   1.f;
static u64       rayCnt  =  1000;
static f32    origin[3]  =  {1.f, 2.f,   5.f};
static f32    camDir[3]  =  {0,     0,  -1.f};

static std::atomic<bool> hasRun;

extern "C"
{
  const char*   InTypes[]  = {
                              "CamParam", 
                              nullptr};
  const char*   InNames[]  = {
                              "Camera and Ray Parameters", 
                              nullptr};
  const char*  OutTypes[]  = {
                              "Rays",                       
                              "IdxVerts",
                              nullptr};
  const char*  OutNames[]  = {
                              "Chunk of rays to be traced", 
                              "Camera Visualiztion",  
                              nullptr};

  void Camera_construct()
  {
    new (&RNG::gen) RNG::rng_t(0);
    RNG::m_genPtr = &RNG::gen;

    INF = std::numeric_limits<f32>::infinity();
    hasRun = false;
  }
  void Camera_destruct(){  hasRun = false; }

  uint64_t Camera(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {
    using namespace std;

    //if( hasRun.exchange(true) ){ return 1; }

    SECTION(create parameters)
    {
      tbl camParams = LavaTblFromPckt(lp, in, IN_CAMERA_PARAMS);
      fovAngle      = camParams("fov");
      aspectRatio   = camParams("aspect ratio");
      rayCnt        = camParams("ray count");
      origin[0]     = camParams("origin x");
      origin[1]     = camParams("origin y");
      origin[2]     = camParams("origin z");
      camDir[0]     = camParams("dir x");
      camDir[1]     = camParams("dir y");
      camDir[2]     = camParams("dir z");
    }
    f32 fovOfst = tanf(deg2rad( 0.5f * fovAngle ));

    //SECTION(create parameters)
    //{
    //  tbl camParams = LavaMakeTbl(lp, 0, (i8)0);
    //  camParams("fov")          =  40.f;
    //  camParams("aspect ratio") =  1.f;
    //  camParams("ray count")    =  (u64)100;
    //  camParams("origin x")     =  1.f;
    //  camParams("origin y")     =  2.f;
    //  camParams("origin z")     =  5.f;
    //  camParams("dir x")        =  0.f;
    //  camParams("dir y")        =  0.f;
    //  camParams("dir z")        = -1.f;
    //  camParams.flatten();
    //  out->push( LavaTblToOut(camParams,CAMERA_PARAMS_OUT) );
    //}

    SECTION(create a tbl of the rays from the camera)
    {
      tbl  rays = LavaMakeTbl(lp);
      tbl    ox = LavaMakeTbl(lp);
      tbl    oy = LavaMakeTbl(lp);
      tbl    oz = LavaMakeTbl(lp);
      tbl    dx = LavaMakeTbl(lp);
      tbl    dy = LavaMakeTbl(lp);
      tbl    dz = LavaMakeTbl(lp);
      tbl  tfar = LavaMakeTbl(lp, rayCnt, INF);
      //tbl tnear = LavaMakeTbl(lp);  // can default to 0
      //tbl  tfar = LavaMakeTbl(lp);  // can default to infinity
      //tbl  time = LavaMakeTbl(lp);  // can default to 0

      ox.setArrayType<f32>();
      oy.setArrayType<f32>();
      oz.setArrayType<f32>();
      dx.setArrayType<f32>();
      dy.setArrayType<f32>();
      dz.setArrayType<f32>();
      rays.setArrayType<i8>();

      v2f mnWin = {-fovOfst, -fovOfst};
      v2f mxWin = { fovOfst,  fovOfst};

      TO(rayCnt,i){
        ox.push<f32>( origin[0] );
        oy.push<f32>( origin[1] );
        oz.push<f32>( origin[2] );

        //f32   x = lerp(mnWin.x, mxWin.x, randomf(0, 1.f) );
        //f32   y = lerp(mnWin.y, mxWin.y, randomf(0, 1.f) );
        f32   x = lerp(mnWin.x, mxWin.x, randomf() );
        f32   y = lerp(mnWin.y, mxWin.y, randomf() );
        v3f dir = {x,y, 1.f };
        dir     = norm(dir);
        dx.push<f32>(  dir.x );
        dy.push<f32>(  dir.y );
        dz.push<f32>( -dir.z );
      }

      rays("origin x")    = &ox;
      rays("origin y")    = &oy;
      rays("origin z")    = &oz;
      rays("direction x") = &dx;
      rays("direction y") = &dy;
      rays("direction z") = &dz;
      rays("tfar")        = &tfar;
      rays.flatten();

      tbl rayIV = raysToIdxVerts(lp, rays);
      out->push( LavaTblToOut(rayIV,CAMERA_VIZ_OUT) );

      out->push( LavaTblToOut(rays,RAYS_OUT) );
    }

    //SECTION(make idx verts to visualize camera position and frustrum)
    //{
    //  tbl   iv = LavaMakeTbl(lp);
    //  tbl   px = LavaMakeTbl(lp);
    //  tbl   py = LavaMakeTbl(lp);
    //  tbl   pz = LavaMakeTbl(lp);
    //  tbl  ind = LavaMakeTbl(lp);
    //
    //  px.setArrayType<f32>();
    //  py.setArrayType<f32>();
    //  pz.setArrayType<f32>();
    //  ind.setArrayType<u32>();
    //  iv.setArrayType<i8>();
    //
    //  px.push(origin[0]);
    //  py.push(origin[1]);
    //  pz.push(origin[2]);
    //
    //  px.push(origin[0] - fovOfst);
    //  px.push(origin[0] + fovOfst);
    //  px.push(origin[0] + fovOfst);
    //  px.push(origin[0] - fovOfst);
    //
    //  py.push(origin[1] - fovOfst);
    //  py.push(origin[1] - fovOfst);
    //  py.push(origin[1] + fovOfst);
    //  py.push(origin[1] + fovOfst);
    //
    //  pz.push(origin[2] + 1.f);
    //  pz.push(origin[2] + 1.f);
    //  pz.push(origin[2] + 1.f);
    //  pz.push(origin[2] + 1.f);
    //
    //  TO(4,i){
    //    ind.push( 0u );
    //    ind.push( (u32)(i+1) );
    //  }
    //  TO(3,i){
    //    ind.push( (u32)(i+1) );
    //    ind.push( (u32)(i+2) );
    //  }
    //  ind.push( 4u );
    //  ind.push( 1u );
    //
    //  iv("positions x")  = &px;
    //  iv("positions y")  = &py;
    //  iv("positions z")  = &pz;
    //  iv("indices")      = &ind;
    //  iv("mode")         = 1;            // 0 should be points, 1 should be lines
    //  iv("type")         = tbl::StrToInt("IdxVerts");
    //  iv.flatten();
    //
    // // out->push( LavaTblToOut(iv,CAMERA_VIZ_OUT) );      // this demonstrates how to output a tbl into the first output slot
    //}
    
    return 1;
  }

  LavaNode LavaNodes[] =
  {
    {
      Camera,                                      // function
      Camera_construct,                            // constructor - this can be set to nullptr if not needed
      Camera_destruct,                             // destructor  - this can also be set to nullptr 
      LavaNode::FLOW,                              // node_type   - this should be eighther LavaNode::MSG (will be run even without input packets) or LavaNode::FLOW (will be run only when at least one packet is available for input)
      "Camera",                                    // name
      InTypes,                                     // in_types    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      InNames,                                     // in_names    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      OutTypes,                                    // out_types   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      OutNames,                                    // out_names   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr
      nullptr,                                     // description
      0                                            // version 
    },                                             

    LavaNodeListEnd                                  // This is a constant that has all the members of the LavaNode struct set to 0 or nullptr - it is used as a special value that ends the static list of LavaNodes. This negates the need for a separate static variable that gives the size of the list, which would be have to be kept in sync and therefore be error prone.
  };

  __declspec(dllexport) LavaNode* GetLavaNodes()
  {
    return (LavaNode*)LavaNodes;
  }
}






//"CamParam", 
//"Parameters that Drive Camera and Ray Generation", 

//tbl   iv = LavaMakeTbl(lp);
//tbl   px = LavaMakeTbl(lp);
//tbl   py = LavaMakeTbl(lp);
//tbl   pz = LavaMakeTbl(lp);
//tbl   cr = LavaMakeTbl(lp);
//tbl   cg = LavaMakeTbl(lp);
//tbl   ca = LavaMakeTbl(lp);
//tbl  ind = LavaMakeTbl(lp);
//
//iv.setArrayType<i8>();
//px.setArrayType<f32>();
//py.setArrayType<f32>();
//pz.setArrayType<f32>();
//cr.setArrayType<f32>();
//cg.setArrayType<f32>();
//ca.setArrayType<f32>();
//ind.setArrayType<u32>();

//const char*   InTypes[]  = {"Dummy",       nullptr};
//const char*   InNames[]  = {"Dummy Slot",  nullptr};

//
//f32 fovOfst = .5f * rcp(tanf(deg2rad( .5f * fovAngle )));

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
