

#ifndef __INDEXEDVERTS_UTIL_HEADER_GUARD__
#define __INDEXEDVERTS_UTIL_HEADER_GUARD__

#include <cstdint>
#include <cstdarg>
#include <SIM/SIM_MacroUtil.h>

extern "C"
{

enum iv_mode  // same as openGL
{
  IV_POINTS           =  0x0000,
  IV_LINES            =  0x0001,
  IV_LINE_LOOP        =  0x0002,
  IV_LINE_STRIP       =  0x0003,
  IV_TRIANGLES        =  0x0004,
  IV_TRIANGLE_STRIP   =  0x0005,
  IV_TRIANGLE_FAN     =  0x0006,
  IV_QUADS            =  0x0007,
  IV_QUAD_STRIP       =  0x0008,
  IV_POLYGON          =  0x0009
};


struct Vertex
{
  float position[3];
  float normal[3];
  float colour[4];
  float texCoord[2];
};
struct IndexedVerts
{
  uint32_t    mode;
  uint32_t    vertsLen;
  uint32_t    indicesLen;
  uint32_t    imgWidth;
  uint32_t    imgHeight;
  uint32_t    imgChans;
  Vertex*     verts;
  uint32_t*   indices;
  float*      pixels;
};
using IV = IndexedVerts;

/**
@arg0 constructor mode
@arg1 uint32_t - number of arguments (3 or 6)
@arg2 uint32_t - openGL geometry mode
@arg3 uint32_t - number of vertices
@arg4 uint32_t - number of indices
@arg5 uint32_t - image width
@arg6 uint32_t - image height
@arg7 uint32_t - image channels

Memory for the image is ordered
in rows of rgb pixels.

a pixel is indexed by:
pixels[y*imgWidth*imgChans + x*imgChans + channel]
 */
void* IndexedVertsCreate (uint32_t constructor, uint32_t arglen, ...);
void* IndexedVertsLoad   (              void*const bytes, size_t  byteLen);
void  IndexedVertsSave   (void*  ivPtr, void*const bytes, size_t* byteLen);
void  IndexedVertsDestroy(void*  dataTypePtr);

inline float* iv_px(IndexedVerts* iv, int64_t x, int64_t y, int64_t c)
{
  size_t ofst = y*iv->imgWidth*iv->imgChans + x*iv->imgChans + c;
  return iv->pixels + ofst;
}

}



#endif








//
//IndexedVerts* IndexedVertsCreate(uint32_t vertsLen, uint32_t indicesLen);

//inline IndexedVerts* 
//IndexedVertsCreate(uint32_t vertsLen, uint32_t indicesLen)
//{
//  IndexedVerts* iv =  (IndexedVerts*)malloc(sizeof(IndexedVerts));
//  iv->vertsLen     =  vertsLen;
//  iv->indicesLen   =  indicesLen;
//  iv->verts        =  (Vertex*)  malloc( vertsLen   * sizeof(Vertex)   );
//  iv->indices      =  (uint32_t*)malloc( indicesLen * sizeof(uint32_t) );
//  memset(iv->verts,   0, vertsLen   * sizeof(Vertex)   );
//  memset(iv->indices, 0, indicesLen * sizeof(uint32_t) );
//
//  return iv;
//}

//inline 
//IndexedVerts* 
//IndexedVertsCreate(uint32_t constructor, uint32_t arglen, ...)
//{
//  uint32_t vertsLen    =  0;
//  uint32_t indicesLen  =  0;
//
//  va_list ap;
//  va_start(ap, arglen);
//  switch(arglen)
//  {
//  case 0:
//    return nullptr;
//  case 1:
//    vertsLen   = va_arg(ap, uint32_t);
//    break;
//  default:
//    vertsLen   = va_arg(ap, uint32_t);
//    indicesLen = va_arg(ap, uint32_t);
//  }
//  va_end(ap);
//    
//  IndexedVerts* iv =  (IndexedVerts*)malloc(sizeof(IndexedVerts));
//  iv->vertsLen     =  vertsLen;
//  iv->indicesLen   =  indicesLen;
//  iv->verts        =  (Vertex*)  malloc( vertsLen   * sizeof(Vertex)   );
//  iv->indices      =  (uint32_t*)malloc( indicesLen * sizeof(uint32_t) );
//  memset(iv->verts,   0, vertsLen   * sizeof(Vertex)   );
//  memset(iv->indices, 0, indicesLen * sizeof(uint32_t) );
//
//  return iv;
//}

