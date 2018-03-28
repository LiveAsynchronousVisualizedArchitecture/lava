
//#include <numeric>  // iota

#include <random>
#include "rtcore.h"
#include "../../no_rt_util.h"
#include "../../tbl.hpp"
#include "../LavaFlow.hpp"
#include "../shared/vec.hpp"

enum Slots
{
  IN_GEOMETRY       = 0,
  IN_RAYS           = 1,

  OUT_RAY_VISUALIZATION  = 0,
  OUT_RAY_HITS           = 1
};

struct Triangle { int v0, v1, v2; };

using vec_ray = std::vector<RTCRayHit>;
using    vecf = std::vector<f32>;
using vec_u32 = std::vector<u32>;

RTCDevice g_device = nullptr;
RTCScene   g_scene = nullptr;
RTCBounds    g_bnd;

namespace RNG
{
  using rng_t  = ::std::mt19937;
  using urng_t = ::std::unique_ptr<rng_t>;

  rng_t   gen(0);
  rng_t*  m_genPtr = &gen;
}
float     randomf(float lo, float hi)
{
  ::std::uniform_real_distribution<float> dis(lo, hi);
  return dis(*RNG::m_genPtr);
}

//tbl   raysToIdxVerts(LavaParams const* lp, RTCRayHitNp const& rh, u32 rayCnt)
//{
//  using namespace std;
//
//  tbl  px = LavaMakeTbl(lp);
//  tbl  py = LavaMakeTbl(lp);
//  tbl  pz = LavaMakeTbl(lp);
//  tbl  cr = LavaMakeTbl(lp);
//  tbl  cg = LavaMakeTbl(lp);
//  tbl  ca = LavaMakeTbl(lp);
//  tbl ind = LavaMakeTbl(lp);
//  tbl  iv = LavaMakeTbl(lp);
//
//  px.setArrayType<f32>();
//  py.setArrayType<f32>();
//  pz.setArrayType<f32>();
//  cr.setArrayType<f32>();
//  cg.setArrayType<f32>();
//  ca.setArrayType<f32>();
//  ind.setArrayType<u32>();
//  iv.setArrayType<i8>();
//
//  TO(rayCnt,i){
//    px.push( rh.ray.org_x[i]  );
//    py.push( rh.ray.org_y[i]  );
//    pz.push( rh.ray.org_z[i]  );
//
//    px.push( rh.ray.org_x[i] + (rh.ray.dir_x[i] * rh.ray.tfar[i]) );
//    py.push( rh.ray.org_y[i] + (rh.ray.dir_y[i] * rh.ray.tfar[i]) );
//    pz.push( rh.ray.org_z[i] + (rh.ray.dir_z[i] * rh.ray.tfar[i]) );
//
//    cr.push(0.5f);
//    cr.push(1.0f);
//    cg.push(0.0f);
//    cg.push(1.0f);
//    ca.push(0.1f);
//    ca.push(0.2f);
//
//    ind.push( (u32)(i*2+0) );
//    ind.push( (u32)(i*2+1) );
//  }
//
//  iv("positions x")  = &px;
//  iv("positions y")  = &py;
//  iv("positions z")  = &pz;
//  iv("colors red")   = &cr;
//  iv("colors green") = &cg;
//  //iv("colors blue")  = &pz;
//  iv("colors alpha") = &ca;
//  iv("indices")      = &ind;
//  iv("mode")         = 1;            // 0 should be points, 1 should be lines
//  iv("type")         = tbl::StrToInt("IdxVerts");
//  iv.flatten();
//
//  return move(iv);
//}

tbl   raysToIdxVerts(LavaParams const* lp, RTCRayHitNp const& rh, u32 rayCnt)
{
  using namespace std;

  tbl  px = LavaMakeTbl(lp);
  tbl  py = LavaMakeTbl(lp);
  tbl  pz = LavaMakeTbl(lp);
  tbl  cr = LavaMakeTbl(lp);
  tbl  cg = LavaMakeTbl(lp);
  tbl  ca = LavaMakeTbl(lp);
  tbl ind = LavaMakeTbl(lp);
  tbl  iv = LavaMakeTbl(lp);

  px.setArrayType<f32>();
  py.setArrayType<f32>();
  pz.setArrayType<f32>();
  cr.setArrayType<f32>();
  cg.setArrayType<f32>();
  ca.setArrayType<f32>();
  ind.setArrayType<u32>();
  iv.setArrayType<i8>();

  TO(rayCnt,i){
    px.push( rh.ray.org_x[i]  );
    py.push( rh.ray.org_y[i]  );
    pz.push( rh.ray.org_z[i]  );

    px.push( rh.ray.org_x[i] + (rh.ray.dir_x[i]*rh.ray.tfar[i]) );
    py.push( rh.ray.org_y[i] + (rh.ray.dir_y[i]*rh.ray.tfar[i]) );
    pz.push( rh.ray.org_z[i] + (rh.ray.dir_z[i]*rh.ray.tfar[i]) );

    cr.push(0.5f);
    cr.push(1.0f);
    cg.push(0.0f);
    cg.push(1.0f);
    ca.push(0.2f);
    ca.push(0.1f);

    ind.push( (u32)(i*2+0) );
    ind.push( (u32)(i*2+1) );
  }

  //TO(rayCnt,i)     // now do the ray hits
  //{  
  //  // ray hit point
  //  v3 hitP;
  //  hitP.x = rh.ray.org_x[i] + (rh.ray.dir_x[i]*rh.ray.tfar[i]);
  //  hitP.y = rh.ray.org_y[i] + (rh.ray.dir_y[i]*rh.ray.tfar[i]);
  //  hitP.z = rh.ray.org_z[i] + (rh.ray.dir_z[i]*rh.ray.tfar[i]);
  //  px.push( hitP.x );
  //  py.push( hitP.y );
  //  pz.push( hitP.z );
  //
  //  v3 hitN;
  //  hitN.x = hitP.x + rh.hit.Ng_x[i];
  //  hitN.y = hitP.y + rh.hit.Ng_y[i];
  //  hitN.z = hitP.z + rh.hit.Ng_z[i];
  //  px.push( hitN.x );
  //  py.push( hitN.y );
  //  pz.push( hitN.z );
  //
  //  cr.push(0.5f);
  //  cr.push(1.0f);
  //  cg.push(0.0f);
  //  cg.push(1.0f);
  //  ca.push(0.25f);
  //  ca.push(0.05f);
  //
  //  u64 rc2 = rayCnt * 2;
  //  ind.push( (u32)(rc2 + (i*2+0)) );
  //  ind.push( (u32)(rc2 + (i*2+1)) );
  //}

  iv("positions x")  = &px;
  iv("positions y")  = &py;
  iv("positions z")  = &pz;
  iv("colors red")   = &cr;
  iv("colors green") = &cg;
  //iv("colors blue")  = &pz;
  iv("colors alpha") = &ca;
  iv("indices")      = &ind;
  iv("mode")         = 1;            // 0 should be points, 1 should be lines
  iv("type")         = tbl::StrToInt("IdxVerts");
  iv.flatten();

  return move(iv);
}
tbl rayHitToIdxVerts(LavaParams const* lp, RTCRayHit const& rh)
{
  using namespace std;

  tbl  px = LavaMakeTbl(lp);
  tbl  py = LavaMakeTbl(lp);
  tbl  pz = LavaMakeTbl(lp);
  tbl  cr = LavaMakeTbl(lp);
  tbl  cg = LavaMakeTbl(lp);
  tbl ind = LavaMakeTbl(lp);
  tbl  iv = LavaMakeTbl(lp);

  px.setArrayType<f32>();
  py.setArrayType<f32>();
  pz.setArrayType<f32>();
  cr.setArrayType<f32>();
  cg.setArrayType<f32>();
  ind.setArrayType<u32>();
  iv.setArrayType<i8>();

  px.push( rh.ray.org_x  );
  py.push( rh.ray.org_y  );
  pz.push( rh.ray.org_z  );

  px.push( rh.ray.org_x + (rh.ray.dir_x*rh.ray.tfar) );
  py.push( rh.ray.org_y + (rh.ray.dir_y*rh.ray.tfar) );
  pz.push( rh.ray.org_z + (rh.ray.dir_z*rh.ray.tfar) );

  cr.push(0.5f);
  cr.push(1.0f);
  cg.push(0.0f);
  cg.push(1.0f);

  ind.push(0u);
  ind.push(1u);

  iv("positions x")  = &px;
  iv("positions y")  = &py;
  iv("positions z")  = &pz;
  iv("colors red")   = &cr;
  iv("colors green") = &cg;
  //iv("colors blue")  = &pz;
  iv("indices")      = &ind;
  iv("mode")         = 1;            // 0 should be points, 1 should be lines
  iv("type")         = tbl::StrToInt("IdxVerts");
  iv.flatten();

  return move(iv);
}
tbl    bndToIdxVerts(LavaParams const* lp, RTCBounds const& b)
{
  using namespace std;
    
  tbl  px = LavaMakeTbl(lp);
  tbl  py = LavaMakeTbl(lp);
  tbl  pz = LavaMakeTbl(lp);
  tbl  iv = LavaMakeTbl(lp);

  px.setArrayType<f32>();
  py.setArrayType<f32>();
  pz.setArrayType<f32>();

  px.push(b.lower_x);
  px.push(b.upper_x);
  px.push(b.upper_x);
  px.push(b.lower_x);
  px.push(b.lower_x); //
  px.push(b.upper_x);
  px.push(b.upper_x);
  px.push(b.lower_x);

  py.push(b.lower_y);
  py.push(b.lower_y);
  py.push(b.upper_y);
  py.push(b.upper_y);
  py.push(b.lower_y); //
  py.push(b.lower_y);
  py.push(b.upper_y);
  py.push(b.upper_y);

  pz.push(b.lower_z);
  pz.push(b.lower_z);
  pz.push(b.lower_z);
  pz.push(b.lower_z);
  pz.push(b.upper_z); //
  pz.push(b.upper_z);
  pz.push(b.upper_z);
  pz.push(b.upper_z);

  tbl ind = LavaMakeTbl(lp);
  ind.setArrayType<u32>();

  ind.push( 0u ); 
  ind.push( 1u );
  ind.push( 1u );
  ind.push( 2u );
  ind.push( 2u );
  ind.push( 3u );
  ind.push( 3u );
  ind.push( 0u );

  ind.push( 0u ); 
  ind.push( 4u );
  ind.push( 1u );
  ind.push( 5u );
  ind.push( 2u );
  ind.push( 6u );
  ind.push( 3u );
  ind.push( 7u );

  ind.push( 4u ); 
  ind.push( 5u );
  ind.push( 5u );
  ind.push( 6u );
  ind.push( 6u );
  ind.push( 7u );
  ind.push( 7u );
  ind.push( 4u );

  TO(8,i){ 
    ind.push( (u32)i ); 
  };

  //px[0] = b.lower_x;
  //px[1] = b.upper_x;
  //px[2] = b.upper_x;
  //px[3] = b.lower_x;
  //
  //py[0] =  b.lower_y;
  //py[1] =  b.lower_y;
  //py[2] =  b.upper_y;
  //py[3] =  b.upper_y;
  //
  //TO(4,i){
  //  px[i]   = b.lower_x;
  //  px[i+4] = b.upper_x;
  //}
  //TO(4,i){
  //  py[i*2]  =  b.lower_y;
  //  py[i+4]  =  b.upper_y;
  //}

  iv("indices")      = &ind;
  iv("positions x")  = &px;
  iv("positions y")  = &py;
  iv("positions z")  = &pz;
  iv("colors red")   = &px;
  iv("colors green") = &py;
  iv("colors blue")  = &pz;
  iv("mode")         = 1;            // 0 should be points, 1 should be lines
  iv("type")         = tbl::StrToInt("IdxVerts");
  iv.flatten();

  return move(iv);
}

extern "C"          // Embree3 Scene Message Node
{
  const char* description = "Takes in geometry that will be sorted into an acceleration structure and ultimatly used to trace rays";                                     // description

  const char*  InTypes[]  = {"IdxVerts",                  "Rays",                                nullptr};
  const char*  InNames[]  = {"Scene Geometry",            "Rays To Trace",                       nullptr};
  const char* OutTypes[]  = {"IdxVerts",                  "Ray Hits",                            nullptr};
  const char* OutNames[]  = {"Traced Rays Visualization", "Traced rays, ready to be shaded",     nullptr};

  void Tracer_construct()
  {
    g_bnd.lower_x = 0.f;
    g_bnd.lower_y = 0.f;
    g_bnd.lower_z = 0.f;
    g_bnd.upper_x = 1.f;
    g_bnd.upper_y = 1.f;
    g_bnd.upper_z = 1.f;

    /* create new Embree device */
    g_device = rtcNewDevice("");
    //error_handler(nullptr,rtcGetDeviceError(g_device));

    /* set error handler */
    //rtcSetDeviceErrorFunction(g_device,error_handler,nullptr);

    /* create scene */
    g_scene = rtcNewScene(g_device);
    rtcSetSceneFlags(g_scene, RTC_SCENE_FLAG_ROBUST);

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
    rtcReleaseScene(g_scene);
    rtcReleaseDevice(g_device);
  }

  uint64_t Tracer(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {
    using namespace std;

    if( in->slotMask[IN_GEOMETRY] ) SECTION(create geometry from input and put it in the embree scene)
    {
      tbl idxVerts( (void*)(in->packets[IN_GEOMETRY].val.value) );

      RTCGeometry mesh = rtcNewGeometry(g_device, RTC_GEOMETRY_TYPE_TRIANGLE);

      tbl      px = idxVerts("positions x");
      tbl      py = idxVerts("positions y");
      tbl      pz = idxVerts("positions z");
      u64 vertCnt = px.size();
      v4f*  verts = (v4f*)rtcSetNewGeometryBuffer(mesh,RTC_BUFFER_TYPE_VERTEX,0,RTC_FORMAT_FLOAT3,sizeof(v4f),vertCnt);

      TO(vertCnt,i){
        verts[i].x  =  px[i];
        verts[i].y  =  py[i];
        verts[i].z  =  pz[i];
        verts[i].a  =  1.f;
      }

      tbl     ind = idxVerts("indices");
      u64  triCnt = (u64)ind.size() / 3;
      Triangle* triangles = (Triangle*)rtcSetNewGeometryBuffer(mesh,RTC_BUFFER_TYPE_INDEX,0,RTC_FORMAT_UINT3, sizeof(Triangle), (int)triCnt);

      TO(triCnt,i){
        auto idx = i * 3;
        triangles[i].v0  =  ind[idx + 0];
        triangles[i].v1  =  ind[idx + 1];
        triangles[i].v2  =  ind[idx + 2];
      }

      rtcSetGeometryBuildQuality(mesh, RTC_BUILD_QUALITY_HIGH);
      rtcCommitGeometry(mesh);

      u32 geomID = rtcAttachGeometry(g_scene, mesh);
      rtcReleaseGeometry(mesh);

      rtcCommitScene(g_scene);
    }
    if( in->slotMask[IN_RAYS] )     SECTION(trace the input rays)
    {
      RTCIntersectContext context;
      rtcInitIntersectContext(&context);

      RTCRayHitNp rh;

      u64 rayCnt = 0;
      tbl   rays = LavaTblFromPckt(lp, in, IN_RAYS);

      tbl   ox = rays("origin x");
      tbl   oy = rays("origin y");
      tbl   oz = rays("origin z");
      tbl   dx = rays("direction x");
      tbl   dy = rays("direction y");
      tbl   dz = rays("direction z");

      rayCnt = ox.size();

      vecf      zeros(rayCnt, 0.f);
      vecf       infs(rayCnt, numeric_limits<f32>::infinity() );
      vec_u32    mask(rayCnt, -1);
      vec_u32   flags(rayCnt,  0);
      vec_u32     ids(rayCnt);

      tbl Nx = LavaMakeTbl(lp, rayCnt, 0.f);
      tbl Ny = LavaMakeTbl(lp, rayCnt, 0.f);
      tbl Nz = LavaMakeTbl(lp, rayCnt, 0.f);
      tbl  u = LavaMakeTbl(lp, rayCnt, 0.f);
      tbl  v = LavaMakeTbl(lp, rayCnt, 0.f);
      vec_u32 geomID(rayCnt, 0);
      vec_u32 instID(rayCnt, 0);
      vec_u32 primID(rayCnt, 0);

      SECTION(fill the ray hit structure of arrays)
      {
        rh.ray.org_x = ox.data<f32>();
        rh.ray.org_y = oy.data<f32>();
        rh.ray.org_z = oz.data<f32>();
        rh.ray.dir_x = dx.data<f32>();
        rh.ray.dir_y = dy.data<f32>();
        rh.ray.dir_z = dz.data<f32>();

        TO(rayCnt,i){ ids[i] = (u32)i; }

        rh.ray.time  = zeros.data();
        rh.ray.tnear = zeros.data();
        rh.ray.tfar  = infs.data();
        rh.ray.id    = ids.data();
        rh.ray.mask  = mask.data();
        rh.ray.flags = flags.data();

        rh.hit.geomID = geomID.data();
        rh.hit.primID = primID.data();
        rh.hit.Ng_x   = Nx.data<f32>();
        rh.hit.Ng_y   = Ny.data<f32>();
        rh.hit.Ng_z   = Nz.data<f32>();
        rh.hit.u      =  u.data<f32>();
        rh.hit.v      =  v.data<f32>();

        TO(RTC_MAX_INSTANCE_LEVEL_COUNT,i){ rh.hit.instID[i] = 0; }
      }

      rtcIntersectNp(g_scene, &context, &rh, rayCnt);

      SECTION(normalize ray hit normals)
      {
        TO(rayCnt,i){
          v3 Ng(rh.hit.Ng_x[i], rh.hit.Ng_y[i], rh.hit.Ng_z[i]);
          Ng = norm(Ng);
          rh.hit.Ng_x[i] = Ng.x;
          rh.hit.Ng_y[i] = Ng.y;
          rh.hit.Ng_z[i] = Ng.z;
        }
      }

      tbl raysIV = raysToIdxVerts(lp, rh, rayCnt);
      out->push( LavaTblToOut(move(raysIV), OUT_RAY_VISUALIZATION) );

      tbl tfar = LavaMakeTbl(lp, rayCnt, 0.f);
      TO(rayCnt,i){
        tfar[i] = rh.ray.tfar[i];
      }

      tbl rayHits     = rays;
      rayHits("Ng x") = &Nx;
      rayHits("Ng y") = &Ny;
      rayHits("Ng z") = &Nz;
      rayHits("tfar") = &tfar;
      rayHits.flatten();
      out->push( LavaTblToOut(move(rayHits), OUT_RAY_HITS) );
    }

    return 1;
  }

  LavaNode LavaNodes[] =
  {
    {
      Tracer,                                      // function
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

    LavaNodeListEnd                                // This is a constant that has all the members of the LavaNode struct set to 0 or nullptr - it is used as a special value that ends the static list of LavaNodes. This negates the need for a separate static variable that gives the size of the list, which would be have to be kept in sync and therefore be error prone.
  };

  __declspec(dllexport) LavaNode* GetLavaNodes()
  {
    return (LavaNode*)LavaNodes;
  }
}





//SECTION(copy pointers to component arrays from the input rays into the RayHit structure of arrays)
//{

//rh.ray.org_x = (f32*)ox.data();
//rh.ray.org_y = (f32*)oy.data();
//rh.ray.org_z = (f32*)oz.data();
//rh.ray.dir_x = (f32*)dx.data();
//rh.ray.dir_y = (f32*)dy.data();
//rh.ray.dir_z = (f32*)dz.data();

//vecf    Nx(rayCnt, 0.f);
//vecf    Ny(rayCnt, 0.f);
//vecf    Nz(rayCnt, 0.f);
//vecf     u(rayCnt, 0.f);
//vecf     v(rayCnt, 0.f);
//
//rh.hit.instID = instID.data();
//
//rh.hit.Ng_x   = Nx.data();
//rh.hit.Ng_y   = Ny.data();
//rh.hit.Ng_z   = Nz.data();
//rh.hit.u      =  u.data();
//rh.hit.v      =  v.data();

//tbl Nx = LavaMakeTbl(lp);
//Nx.resize<f32>(rayCnt, 0.f);
//
//vecf    Nx(rayCnt, 0.f);
//vecf    Ny(rayCnt, 0.f);
//vecf    Nz(rayCnt, 0.f);
//vecf     u(rayCnt, 0.f);
//vecf     v(rayCnt, 0.f);

//SECTION(input packet loop)
//{
//  u32 i=0;
//  while( LavaNxtPckt(in, &i) )
//  {
//    RTCIntersectContext context;
//    rtcInitIntersectContext(&context);
//
//    RTCRayHit rh;
//    rh.ray.mask   =   -1;
//    rh.ray.id     =    RTC_INVALID_GEOMETRY_ID;
//    rh.ray.flags  =    0;
//    rh.ray.time   =    0;
//    rh.ray.tnear  =   0.f;
//    rh.ray.tfar   =   numeric_limits<float>::infinity();
//    rh.ray.org_x  =   0.f;
//    rh.ray.org_y  =   1.f;
//    rh.ray.org_z  =   4.f;
//    rh.ray.dir_x  =   0.f;
//    rh.ray.dir_y  =   0.f;
//    rh.ray.dir_z  =  -1.f;
//
//    int rayCnt = 100000;
//    vec_ray rays(rayCnt);
//    TO(rayCnt,i){
//      rays[i] = rh;
//      rays[i].ray.org_x = randomf(-2.f, 3.f);
//      rays[i].ray.org_y = randomf(-2.f, 4.f);
//    }
//
//    TO(rayCnt,i){
//      rtcIntersect1(g_scene, &context, &rays[i] );
//    }
//
//    tbl raysIV = raysToIdxVerts(lp, rays.data(), rays.size());
//    out->push( LavaTblToOut(move(raysIV), OUT_RAYS) );
//
//    //tbl rayIV = rayHitToIdxVerts(lp, rh);
//    //out->push( LavaTblToOut(move(rayIV), RAYS_OUT) );                // this demonstrates how to output a tbl into the first output slot
//    //out->push( LavaTblToOut(move(rayIV), BOUNDING_BOX_OUT ) );                // this demonstrates how to output a tbl into the first output slot
//  }
//}

//if(g_scene) SECTION(bounding box of scene output)
//{
//  rtcGetSceneBounds(g_scene, &g_bnd);
//  tbl bndIV = bndToIdxVerts(lp, g_bnd);
//  out->push( LavaTblToOut(move(bndIV),OUT_BOUNDING_BOX) );           // this demonstrates how to output a tbl into the first output slot
//}

//RTCRay r;
//r.org_x = 0.f;   // (Vec3fa(camera.xfm.p), Vec3fa(normalize(x*camera.xfm.l.vx + y*camera.xfm.l.vy + camera.xfm.l.vz)), 0.0f, inf);
//r.org_y = 0.f;
//r.org_z = 0.f;
//RTCHit h;

//tbl rayHitToIdxVerts(LavaParams const* lp, RTCRayHit const& rh)
//tbl rayHitToIdxVerts(LavaParams const* lp, RTCRayHit* rh, u32 sz)

//Vertex* vertices = (Vertex*) rtcSetNewGeometryBuffer(mesh,RTC_BUFFER_TYPE_VERTEX,0,RTC_FORMAT_FLOAT3,sizeof(Vertex),8);
//int tri = 0;
//
// geometry attribute for vert colors
//rtcSetGeometryVertexAttributeCount(mesh,1);
//rtcSetSharedGeometryBuffer(mesh,RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,0,RTC_FORMAT_FLOAT3,vertex_colors,0,sizeof(Vec3fa),8);
//
//return geomID;
//
//for(auto& kv : inputTbl){  // this loop demonstrates how to iterate through non-empty map elements
//}	
