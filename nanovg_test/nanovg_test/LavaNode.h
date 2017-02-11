

#ifndef __LAVA_NODE_HEADER_GUARD__
#define __LAVA_NODE_HEADER_GUARD__

#include <cstdint>

extern "C"
{

struct LavaNode;
using  DoTransform     =  void*(*)(void* in);      // void** out);
using  GetLavaNodes_t  =  LavaNode*(*)();          // A function that returns a pointer to a LavaNode list. The last node is null, which makes the list similiar to a C string. Looping through is easy, though random access is not possible without first looping through to get the length.

struct LavaData                                    // should be 128 bits total
{
  uint64_t  idx : 48;
  uint64_t  len : 48;
  uint64_t type : 32;
};

struct LavaNode
{
  const char*     in_type;
  const char*     out_type;
  const char*     name;
  DoTransform     do_transform;
  size_t          version;
  size_t          id;
};

}

inline void* DoTransformIfNotNull(LavaNode* t, void* in) // void** out)
{
  if(t && t->do_transform)
    return t->do_transform(in); //out);
}


#endif 

