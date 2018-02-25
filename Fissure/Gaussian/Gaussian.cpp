

#include <random>
#include "../../str_util.hpp"
#include "../../no_rt_util.h"
#include "../../tbl.hpp"
#include "../LavaFlow.hpp"

enum Slots
{  
  GAUSS_IDXVERTS_OUT = 0

  //GAUSS_PARAMS_IN    = 0,
  //GAUSS_IDXVERTS_OUT = 1
};

namespace RNG
{
  using rng_t  = ::std::mt19937;
  using urng_t = ::std::unique_ptr<rng_t>;

  //extern __declspec(thread) rng_t*  m_genPtr;
  //
  //void Init(int s);
  //void Destruct();

  rng_t   gen(0);
  rng_t*  m_genPtr = &gen; //nullptr;

  void Init(int s)
  {
    //if (m_genPtr == nullptr) {
    //  m_genPtr = new rng_t(s);
    //}
  }
  void Destruct()
  {
    //if(m_genPtr != nullptr) 
    //  delete m_genPtr;
  }
}

float     randomf(float lo, float hi)
{
  ::std::uniform_real_distribution<float> dis(lo, hi);
  return dis(*RNG::m_genPtr);
}

void           PrintLavaMem(LavaMem lm)
{
  if(lm.ptr)
    printf("\n addr: %llu  data addr: %llu  ref count: %llu   size bytes: %llu \n", 
      (u64)(lm.ptr), (u64)(lm.data()), (u64)lm.refCount(), (u64)lm.sizeBytes() );
}
void           PrintTblMem(tbl const& t)
{
  if( !t.m_mem ){
    printf("nullptr\n");
    return;
  }

  LavaMem lm;
  lm.ptr = (void*)( (u8*)t.memStart() - 16 );
  PrintLavaMem(lm);

  //assert(lm.refCount() < 1000);
  //
  //lm.fromDataAddr( (u64)t.memStart() );
}

extern "C"
{
  const char*  InTypes[]  = {"IdxVerts",           nullptr};            // This array contains the type that each slot of the same index will accept as input.
  const char*  InNames[]  = {"Gaussian Slot In",   nullptr};            // This array contains the names of each input slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  const char* OutTypes[]  = {"IdxVerts",           nullptr};            // This array contains the types that are output in each slot of the same index
  const char* OutNames[]  = {"Gaussian Slot Out",  nullptr};            // This array contains the names of each output slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.

  void GaussConstruct()
  {
    RNG::Init(0);
  }
  void GaussDestruct()
  {
    RNG::Destruct();
  }

  uint64_t Gaussian(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {
    using namespace std;

    u32 i=0;
    while( LavaNxtPckt(in, &i) )
    {
      tbl gParams = LavaTblFromPckt(lp, in, i);
      //f32     var = gParams("Variance");
      //f32      ev = gParams("Expected Value");
      //u64   verts = gParams("Vertex Count");

      f32     var = randomf(0.01f, 1.f);
      f32      ev = 0.f;  //randomf(0f, 0.2f);
      u64   verts = 16;

      tbl gcurve = LavaMakeTbl(lp);
      gcurve("type") = tbl::StrToInt("IdxVerts");
      gcurve("mode") = (u64)0; // GL_POINTS for now                                   // needs to be openGL lines - GL_LINES - 0x01

      tbl ind(verts*2 - 1, (u32)0u);
      tbl  px(verts, 0.f);
      tbl  py(verts, 0.f);
      tbl  cg(verts, 1.f);

      f32 vrecip = 1.f / verts;
      f32 coeff  = 1.f / sqrtf(2*PIf*var);
      TO(verts,i){
        f32     x = ( (f32)i - (verts/2.f) ) * vrecip * 4.f;
        f32  eExp = -( (x*x)/(2.f*var) ); 
        f32     y = coeff * expf( eExp );
        px[i] = x + ev;
        py[i] = y;
      }
      u64 indsz = ind.size();
      for(u64 i=0; i < verts-1; ++i){
        ind[i*2]   = (u32)i;
        ind[i*2+1] = (u32)(i+1);
      }

      gcurve("positions x")  = &px;
      gcurve("positions y")  = &py;
      gcurve("colors green") = &cg;
      gcurve("indices")      = &ind;
      gcurve.flatten();

      out->push( LavaTblToOut(gcurve, GAUSS_IDXVERTS_OUT) );      // this demonstrates how to output a tbl into the first output slot
    }

    return 1;
  }

  LavaNode LavaNodes[] =
  {
    {
      Gaussian,                                      // function
      GaussConstruct,                                // constructor - this can be set to nullptr if not needed
      GaussDestruct,                                 // destructor  - this can also be set to nullptr 
      LavaNode::FLOW,                                // node_type   - this should be eighther LavaNode::MSG (will be run even without input packets) or LavaNode::FLOW (will be run only when at least one packet is available for input)
      "Gaussian",                                    // name
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
//PrintTblMem(gcurve);

//u32   verts = 8;
//
//tbl  pz(verts, 0.f);
//
//gcurve("positions z") = &pz;

//tbl inputTbl( (void*)(in->packets[i].val.value) );
//
//for(auto& kv : gaussParams){  // this loop demonstrates how to iterate through non-empty map elements
//}	
