
#ifndef __LAVA_INDEXED_VERTS_HEADER_GUARD_H__
#define __LAVA_INDEXED_VERTS_HEADER_GUARD_H__

#include <stdint.h>
#include <stdlib.h>

extern "C"
{
  enum iv_mode  // same as openGL
  {
    IV_POINTS = 0x0000,
    IV_LINES = 0x0001,
    IV_LINE_LOOP = 0x0002,
    IV_LINE_STRIP = 0x0003,
    IV_TRIANGLES = 0x0004,
    IV_TRIANGLE_STRIP = 0x0005,
    IV_TRIANGLE_FAN = 0x0006,
    IV_QUADS = 0x0007,
    IV_QUAD_STRIP = 0x0008,
    IV_POLYGON = 0x0009
  };

  struct Vertex
  {
    float position[3];
    float   normal[3];
    float    color[4];
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

  /**
  @arg0 constructor mode
  @arg1 uint32_t - number of arguments (3 or 6)
  @arg2 uint32_t - openGL geometry mode
  @arg3 uint32_t - number of vertices
  @arg4 uint32_t - number of indices
  @arg5 uint32_t - image width
  @arg6 uint32_t - image height
  @arg7 uint32_t - image channels

  Memory for the image is ordered in rows of rgb pixels.

  a pixel is indexed by:
  pixels[y*imgWidth*imgChans + x*imgChans + channel]
  */
  void* IndexedVertsCreate(uint32_t constructor, uint32_t arglen, ...);
  void* IndexedVertsLoad(void*const bytes);
  void  IndexedVertsSave(void*  ivPtr, void*const bytes, size_t* byteLen);
  void  IndexedVertsDestroy(void*  dataTypePtr);

  inline float* iv_px(IndexedVerts* iv, int64_t x, int64_t y, int64_t c)
  {
    size_t ofst = y*iv->imgWidth*iv->imgChans + x*iv->imgChans + c;
    return iv->pixels + ofst;
  }
}

#endif

#ifdef LAVA_INDEXED_VERTS_IMPL

#ifndef __LAVA_INDEXED_VERTS_IMPL_GUARD_H__
#define __LAVA_INDEXED_VERTS_IMPL_GUARD_H__

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
extern "C"
{
    void* IndexedVertsCreate(uint32_t constructor, uint32_t arglen, ...)
    {
        uint32_t  mode = IV_POINTS;  // GL_POINTS
        uint32_t  vertsLen = 0;
        uint32_t  indicesLen = 0;
        uint32_t  imgWidth = 0;
        uint32_t  imgHeight = 0;
        uint32_t  imgChans = 0;

        va_list ap;
        va_start(ap, arglen);
        switch(arglen)
        {
        case 0:
            return nullptr;
        case 1:
            vertsLen = va_arg(ap, uint32_t);
            break;
        case 3:
            mode = va_arg(ap, uint32_t);
            vertsLen = va_arg(ap, uint32_t);
            indicesLen = va_arg(ap, uint32_t);
            break;
        case 6:
            mode = va_arg(ap, uint32_t);
            vertsLen = va_arg(ap, uint32_t);
            indicesLen = va_arg(ap, uint32_t);
            imgWidth = va_arg(ap, uint32_t);
            imgHeight = va_arg(ap, uint32_t);
            imgChans = va_arg(ap, uint32_t);
            break;
        default:
            ;
        }
        va_end(ap);

        IndexedVerts* iv = (IndexedVerts*)malloc(sizeof(IndexedVerts));
        if(iv){
          iv->mode       = mode;
          iv->vertsLen   = vertsLen;
          iv->indicesLen = indicesLen;
          iv->imgWidth   = imgWidth;
          iv->imgHeight  = imgHeight;
          iv->imgChans   = imgChans;
          int64_t pxLen  = imgWidth*imgHeight*imgChans;
          iv->verts = (Vertex*)malloc(vertsLen * sizeof(Vertex));
          if(iv->verts){ memset(iv->verts, 0, vertsLen   * sizeof(Vertex)); }
          iv->indices = (uint32_t*)malloc(indicesLen * sizeof(uint32_t));
          if(iv->indices){ memset(iv->indices, 0, indicesLen * sizeof(uint32_t)); }
          iv->pixels = (float*)malloc(pxLen * sizeof(float));
          if(iv->pixels){ memset(iv->pixels, 0, pxLen * sizeof(float)); }
        }

        return iv;
    }
    void* IndexedVertsLoad(void*const  bytes)
    {
        IndexedVerts* iv = (IndexedVerts*)malloc(sizeof(IndexedVerts));
        if(!iv) return nullptr;

        auto p = (uint8_t*)bytes;

        iv->mode = *((uint32_t*)p);
        p += sizeof(uint32_t);
        iv->vertsLen = *((uint32_t*)p);
        p += sizeof(uint32_t);
        iv->indicesLen = *((uint32_t*)p);
        p += sizeof(uint32_t);
        iv->imgWidth = *((uint32_t*)p);
        p += sizeof(uint32_t);
        iv->imgHeight = *((uint32_t*)p);
        p += sizeof(uint32_t);
        iv->imgChans = *((uint32_t*)p);
        p += sizeof(uint32_t);

        size_t vertSz = iv->vertsLen   * sizeof(Vertex);
        iv->verts = (Vertex*)malloc(vertSz);
        size_t idxSz = iv->indicesLen * sizeof(uint32_t);
        iv->indices = (uint32_t*)malloc(idxSz);
        size_t pxSz = iv->imgWidth * iv->imgHeight * iv->imgChans * sizeof(float);
        iv->pixels = (float*)malloc(pxSz);

        if(iv->verts) memcpy(iv->verts, p, vertSz);
        p += vertSz;
        if(iv->indices) memcpy(iv->indices, p, idxSz);
        p += idxSz;
        if(iv->pixels) memcpy(iv->pixels, p, pxSz);

        return iv;
    }
    void  IndexedVertsSave(void*  ivPtr, void*const  bytes, size_t* byteLen)
    {
        //using namespace std;

        const int params = 6;

        auto inLen = *byteLen;
        auto iv = (IndexedVerts*)ivPtr;
        size_t vertSz = iv->vertsLen   * sizeof(Vertex);
        size_t idxSz = iv->indicesLen * sizeof(uint32_t);
        size_t pxSz = iv->imgWidth*iv->imgHeight*iv->imgChans * sizeof(float);
        size_t needs = params*sizeof(uint32_t)+vertSz + idxSz + pxSz;

        if(!bytes || needs > *byteLen) {
            *byteLen = needs;
        }
        else
        {
            uint8_t* p = (uint8_t*)bytes;

            ((uint32_t*)p)[0] = iv->mode;
            ((uint32_t*)p)[1] = iv->vertsLen;
            ((uint32_t*)p)[2] = iv->indicesLen;
            ((uint32_t*)p)[3] = iv->imgWidth;
            ((uint32_t*)p)[4] = iv->imgHeight;
            ((uint32_t*)p)[5] = iv->imgChans;

            p += params * sizeof(uint32_t);
            memcpy(p, iv->verts, vertSz);
            p += vertSz;
            memcpy(p, iv->indices, idxSz);
            p += idxSz;
            memcpy(p, iv->pixels, pxSz);
        }
    }
    void  IndexedVertsDestroy(void* dataTypePtr)
    {
        auto iv = (IndexedVerts*)dataTypePtr;
        free(iv->verts);
        free(iv->indices);
        free(iv->pixels);
        free(iv);
    }
}
#endif  // #define __LAVA_INDEXED_VERTS_IMPLs_GUARD_H__
#endif  // #ifdef LAVA_INDEXED_VERTS_IMPL

