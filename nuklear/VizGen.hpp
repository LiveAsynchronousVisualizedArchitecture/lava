
#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __VIZ_GENERATORS_HEADERGUARD_HPP__
#define __VIZ_GENERATORS_HEADERGUARD_HPP__

//#ifdef _MSC_VER
//  #define _CRT_SECURE_NO_WARNINGS 1
//  #define _SCL_SECURE_NO_WARNINGS 1
//#endif

#include "VizData.hpp"

inline vec<ui8>      makeCube(size_t& byteLen)
{
  const unsigned int NUM_VERTICES = 8;
  const unsigned int NUM_INDICES = 36;

  IndexedVerts* iv = (IndexedVerts*)IndexedVertsCreate(0, 6, IV_TRIANGLES, NUM_VERTICES, NUM_INDICES, 1, 1, 4);

  iv->verts[0] = {
      {-0.5f, -0.5f, 0.5f},     //pos
      {0.0f, 0.0f, -1.0f},      //norm
      {1.0f, 0.0f, 0.0f, 1.0f}, //color
      {0.0f, 0.0f}              //texCoord
  };
  iv->verts[1] = {
      {0.5f, -0.5f, 0.5f},      //pos
      {0.0f, 0.0f, -1.0f},      //norm
      {0.0f, 1.0f, 0.0f, 1.0f}, //color
      {0.0f, 0.0f}              //texCoord
  };
  iv->verts[2] = {
      {0.5f, 0.5f, 0.5f},       //pos
      {0.0f, 0.0f, -1.0f},      //norm
      {0.0f, 0.0f, 1.0f, 1.0f}, //color
      {0.0f, 0.0f}              //texCoord
  };
  iv->verts[3] = {
      {-0.5f, 0.5f, 0.5f},      //pos
      {0.0f, 0.0f, -1.0f},      //norm
      {1.0f, 0.5f, 0.25f, 1.0f}, //color
      {0.0f, 0.0f}              //texCoord
  };
  iv->verts[4] = {
      {-0.5f, -0.5f, -0.5f},    //pos
      {0.0f, 0.0f, -1.0f},      //norm
      {1.0f, 0.0f, 0.0f, 1.0f}, //color
      {0.0f, 0.0f}              //texCoord
  };
  iv->verts[5] = {
      {0.5f, -0.5f, -0.5f},     //pos
      {0.0f, 0.0f, -1.0f},      //norm
      {0.0f, 1.0f, 0.0f, 1.0f}, //color
      {0.0f, 0.0f}              //texCoord
  };
  iv->verts[6] = {
      {0.5f, 0.5f, -0.5f},      //pos
      {0.0f, 0.0f, -1.0f},      //norm
      {0.0f, 0.0f, 1.0f, 1.0f}, //color
      {0.0f, 0.0f}              //texCoord
  };
  iv->verts[7] = {
      {-0.5f, 0.5f, -0.5f},     //pos
      {0.0f, 0.0f, -1.0f},      //norm
      {1.0f, 0.5f, 0.25f, 1.0f}, //color
      {0.0f, 0.0f}              //texCoord
  };

  uint32_t cubeIndices[NUM_INDICES] = {
      // front
      0, 1, 2,
      2, 3, 0,
      // top
      1, 5, 6,
      6, 2, 1,
      // back
      7, 6, 5,
      5, 4, 7,
      // bottom
      4, 0, 3,
      3, 7, 4,
      // left
      4, 5, 1,
      1, 0, 4,
      // right
      3, 2, 6,
      6, 7, 3,
  };

  // Copy index data into IndexedVerts.indices
  memcpy(iv->indices, cubeIndices, sizeof(GLuint) * NUM_INDICES);

  // copy pixel data 
  //memset(iv->pixels, 0, sizeof(float)*4);                 // 4 floats, all 0, meaning black color and black alpha
  TO(4,i) iv->pixels[i] = 0.5f;

  // Call once to get byteLen
  IndexedVertsSave(iv, nullptr, &byteLen);
  vec<ui8> bytes(byteLen);
  // Call again to serialize
  IndexedVertsSave(iv, bytes.data(), &byteLen);
  IndexedVertsDestroy(iv);

  return bytes;
}
inline vec<ui8>  makeTriangle(size_t& byteLen, bool left)
{
  // Create triangle Vertex data
  const unsigned int NUM_VERTICES = 3;
  const unsigned int NUM_INDICES = 3;

  IndexedVerts* iv = (IndexedVerts*)IndexedVertsCreate(0, 6, IV_TRIANGLES, NUM_VERTICES, 3, 1, 1, 4);

  if(left) {
      iv->verts[0] = {
          {-1.0, -1.0f, 0.0f},     //pos
          {0.0f, 0.0f, -1.0f},      //norm
          {1.0f, 1.0f, 1.0f, 1.0f}, //color
          {0.0f, 0.0f}              //texCoord
      };
      iv->verts[1] = {
          {-0.17f, -1.0f, 0.0f},      //pos
          {0.0f, 0.0f, -1.0f},      //norm
          {1.0f, 1.0f, 1.0f, 1.0f}, //color
          {0.0f, 0.0f}              //texCoord
      };
      iv->verts[2] = {
          {-0.58f, 1.0f, 0.0f},       //pos
          {0.0f, 0.0f, -1.0f},      //norm
          {1.0f, 1.0f, 1.0f, 1.0f}, //color
          {0.0f, 0.0f}              //texCoord
      };
  }
  else {
      iv->verts[0] = {
          {-0.17, -1.0f, 0.0f},     //pos
          {0.0f, 0.0f, -1.0f},      //norm
          {1.0f, 1.0f, 1.0f, 1.0f}, //color
          {0.0f, 0.0f}              //texCoord
      };
      iv->verts[1] = {
          {0.66f, -1.0f, 0.0f},      //pos
          {0.0f, 0.0f, -1.0f},      //norm
          {1.0f, 1.0f, 1.0f, 1.0f}, //color
          {0.0f, 0.0f}              //texCoord
      };
      iv->verts[2] = {
          {0.25, 1.0f, 0.0f},       //pos
          {0.0f, 0.0f, -1.0f},      //norm
          {1.0f, 1.0f, 1.0f, 1.0f}, //color
          {0.0f, 0.0f}              //texCoord
      };
  }

  GLuint indices[NUM_INDICES] = {0, 1, 2};

  // Copy index data into IndexedVerts.indices
  memcpy(iv->indices, indices, sizeof(GLuint)* NUM_INDICES);

  // copy pixel data 
  //memset(iv->pixels, 0, sizeof(float)*4);                 // 4 floats, all 0, meaning black color and black alpha
  TO(4,i) iv->pixels[i] = 0.5f;

  // Call once to get byteLen
  IndexedVertsSave(iv, nullptr, &byteLen);
  vec<ui8> bytes(byteLen);
  // Call again to serialize
  IndexedVertsSave(iv, bytes.data(), &byteLen);
  IndexedVertsDestroy(iv);

  return bytes;
}



#endif
