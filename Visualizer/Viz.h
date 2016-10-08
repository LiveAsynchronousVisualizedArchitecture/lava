

#ifndef __VIZ_HEADER_GUARD__
#define __VIZ_HEADER_GUARD__

extern "C"
{

struct Vertex
{
  float position[3];
  float normal[3];
  float colour[4];
  float texCoord[2];
};
struct IndexedVerts
{
  int      vertsLen; 
  int      indicesLen; 
  Vertex*  verts;
  int*     indices;
};
struct Viz;
struct VizAndData;

using VizCreate            =  void(*)(void* data, IndexedVerts*);
using VizDestroy           =  void(*)(IndexedVerts* indVerts);
using GetVisualizations_t  =  Viz*(*)();

struct Viz
{
  const char*     type;
  const char*     name;
  VizCreate       viz_create;
  VizDestroy      viz_destroy;
  size_t          sizeBytes;
  size_t          version;
  size_t          id;
};
struct VizAndData
{
  Viz*   viz;
  void*  data;
};

void VizCreateIfNotNull (Viz* v, void* data, IndexedVerts* idxVerts)
{
  if(v && v->viz_create)
    v->viz_create(data,idxVerts);
}

void VizDestroyIfNotNull(Viz* v, IndexedVerts* idxVerts)
{
  if(v && v->viz_destroy)
    v->viz_destroy(idxVerts);
}


}

#endif 





//using VizFunc    =  void(*)(void* data, int  vertLen, int  indicesLen, Vertex* outVerts, int* outIndices);
//using VizLenFunc =  void(*)(void* data, int* vertLen, int* indicesLen);

//VizLenFunc      viz_len;
//unsigned char*  data;
//size_t          parameters;
// viz_func
