
#include <cstdio>
#include "../no_rt_util.h"
#include "../LavaNode.h"

extern "C"
{

LavaArg* ReadFile(LavaArg* in)
{
  TO(in->len,i){
    printf(" %l \n", in->idx);
  }

  return nullptr;
}

const char* ReadFileIn[]  = {"string"};
const char* ReadFileOut[] = {"string"};

LavaNode nodes[] =
{
  {ReadFile, "ReadFile",                     // function, name
  1, ReadFileIn, 1, ReadFileOut,             // inputs, in_types, outputs, out_types 
  0, 0},                                     // version, id

  {nullptr, nullptr, 0, nullptr, 0, nullptr, 0, 0}
};

__declspec(dllexport) LavaNode* GetNodes()
{
  return (LavaNode*)nodes;
}


}





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
