
#include "IndexedVerts.h"


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
        iv->mode = mode;
        iv->vertsLen = vertsLen;
        iv->indicesLen = indicesLen;
        iv->imgWidth = imgWidth;
        iv->imgHeight = imgHeight;
        iv->imgChans = imgChans;
        int64_t pxLen = imgWidth*imgHeight*imgChans;
        iv->verts = (Vertex*)malloc(vertsLen   * sizeof(Vertex));
        iv->indices = (uint32_t*)malloc(indicesLen * sizeof(uint32_t));
        iv->pixels = (float*)malloc(pxLen      * sizeof(float));
        memset(iv->verts, 0, vertsLen   * sizeof(Vertex));
        memset(iv->indices, 0, indicesLen * sizeof(uint32_t));
        memset(iv->pixels, 0, pxLen      * sizeof(float));

        return iv;
    }
    void* IndexedVertsLoad(void*const  bytes)
    {
        IndexedVerts* iv = (IndexedVerts*)malloc(sizeof(IndexedVerts));
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

        memcpy(iv->verts, p, vertSz);
        p += vertSz;
        memcpy(iv->indices, p, idxSz);
        p += idxSz;
        memcpy(iv->pixels, p, pxSz);

        return iv;
    }
    void  IndexedVertsSave(void*  ivPtr, void*const  bytes, size_t* byteLen)
    {
        using namespace std;

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
