

#ifndef __TRANSFORM_HEADER_GUARD__
#define __TRANSFORM_HEADER_GUARD__

#include <cstdint>

extern "C"
{

struct Transform;
using  DoTransform      =  void*(*)(void* in); // void** out);
using  GetTransforms_t  =  Transform*(*)();

struct Transform
{
  const char*     in_type;
  const char*     out_type;
  const char*     name;
  DoTransform     do_transform;
  size_t          version;
  size_t          id;
};

}

inline void* DoTransformIfNotNull(Transform* t, void* in) // void** out)
{
  if(t && t->do_transform)
    return t->do_transform(in); //out);
}


#endif 







//struct Vertex
//{
//  float position[3];
//  float normal[3];
//  float colour[4];
//  float texCoord[2];
//};
//struct IndexedVerts
//{
//  int      vertsLen; 
//  int      indicesLen; 
//  Vertex*  verts;
//  int*     indices;
//};
//struct VizAndData;

//using VizCreate            =  void(*)(void* data, IndexedVerts*);
//using VizDestroy           =  void(*)(IndexedVerts* indVerts);
