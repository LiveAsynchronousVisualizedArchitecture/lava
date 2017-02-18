

//#include "../Transform.h"
#include "../LavaNode.h"

extern "C"
{

LavaArg* TfmTest(LavaArg* in)
{
  //auto  cs     =  (CS*)in;
  //auto& clrs   =  cs->colors();
  //auto& smpls  =  cs->samples();
  //auto  sz     =  cs->size();
  ////auto  sz     =  smpls.w;
  //auto  iv     =  (IndexedVerts*)IndexedVertsCreate(0, 6, IV_POINTS, sz, sz, 1, 1, 4);
  //
  //float* clrPtr  =  cs->colors().p;
  //auto    w      =  cs->colors().w;
  //auto    h      =  cs->colors().h;
  ////assert(clrPtr != nullptr);
  ////assert(w < 10000);
  ////assert(h < 10000);
  //TO(sz,i)
  //{
  //  Vertex& v = iv->verts[i];
  //  //TO(3,c) v.colour[c]  =  clrPtr[i*w + 0];
  //
  //  TO(3,c) v.colour[c]  =  clrs(c,i);
  //  ////TO(3,c) v.colour[c]  =  clrs(c,i) * 8.f;
  //  v.colour[3]          =  1.f;
  //  
  //  v.texCoord[0] = v.position[0] = smpls(i,0);
  //  v.texCoord[1] = v.position[1] = 1.f - smpls(i,1);
  //  v.position[2] = 0.f;
  //  
  //  iv->indices[i]       =  i;
  //}
  //
  //memset(iv->pixels, 0, 4*sizeof(float));
  //
  //return iv;

  return nullptr;
}

const char* SplitLinesIn[]  = {"string"};
const char* SplitLinesOut[] = {"string"};

LavaNode nodes[] =
{
  {TfmTest, "SplitLines",                    // function, name
  1, SplitLinesIn, 1, SplitLinesOut,         // inputs, in_types, outputs, out_types 
  0, 0},                                     // version, id

  {nullptr, nullptr, 0, nullptr, 0, nullptr, 0, 0}
};

__declspec(dllexport) LavaNode* GetNodes()
{
  return (LavaNode*)nodes;
}



//struct LavaNode
//{
//  LavaFunc             func;
//  const char*          name;
//  uint8_t            inputs;
//  const char**     in_types;
//  uint8_t           outputs;
//  const char**    out_types;
//  uint64_t          version;
//  uint64_t               id;
//};

//Transform transforms[] =
//{
//  {"InType", "OutType", "NodeName", 
//   TfmTest, 0, 0},                         // function, version, id
//
//  {"Second In", "Second Out", "Other Name", 
//   TfmTest, 0, 0},                         // function, version, id
//
//  {nullptr, nullptr, nullptr, nullptr, 0, 0}
//};
//
//__declspec(dllexport) Transform* GetTransforms()
//{
//  return (Transform*)transforms;
//}

}
