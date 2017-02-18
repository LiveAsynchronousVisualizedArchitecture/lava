
#include <cstdio>
#include "../no_rt_util.h"
#include "../LavaNode.h"

extern "C"
{

LavaArg* GenPath(LavaArg* in)
{
  TO(in->len,i){
    printf(" %l \n", in->idx);
  }

  return nullptr;
}

const char* GenPathIn[]  = {"string"};
const char* GenPathOut[] = {"string"};

LavaNode nodes[] =
{
  {GenPath, "GenPath",                     // function, name
  1, GenPathIn, 1, GenPathOut,             // inputs, in_types, outputs, out_types 
  0, 0},                                     // version, id

  {nullptr, nullptr, 0, nullptr, 0, nullptr, 0, 0}
};

__declspec(dllexport) LavaNode* GetNodes()
{
  return (LavaNode*)nodes;
}


}

