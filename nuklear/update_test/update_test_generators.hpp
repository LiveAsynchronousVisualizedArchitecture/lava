
#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __UPDATE_TEST_GENERATORS_HEADERGUARD_HPP__
#define __UPDATE_TEST_GENERATORS_HEADERGUARD_HPP__

//#ifdef _MSC_VER
//  #if !defined(_CRT_SECURE_NO_WARNINGS)
//    #define _CRT_SECURE_NO_WARNINGS
//  #endif
//
//  #if !defined(_SCL_SECURE_NO_WARNINGS)
//    #define _SCL_SECURE_NO_WARNINGS
//  #endif
//#endif

#include "update_test_data.hpp"

inline vec<ui8>      makeCube()
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
  memcpy(iv->indices, cubeIndices, sizeof(ui32) * NUM_INDICES);

  // copy pixel data 
  //memset(iv->pixels, 0, sizeof(float)*4);                 // 4 floats, all 0, meaning black color and black alpha
  TO(4,i) iv->pixels[i] = 0.5f;


  // Call once to get byteLen
  size_t byteLen = 0;
  IndexedVertsSave(iv, nullptr, &byteLen);
  vec<ui8> bytes(byteLen);
  // Call again to serialize
  IndexedVertsSave(iv, bytes.data(), &byteLen);
  IndexedVertsDestroy(iv);

  return bytes;
}
inline vec<ui8>  makeTriangle(bool left)
{
  // Create triangle Vertex data
  const unsigned int NUM_VERTICES = 3;
  const unsigned int NUM_INDICES  = 3;

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

  ui32 indices[NUM_INDICES] = {0, 1, 2};

  // Copy index data into IndexedVerts.indices
  memcpy(iv->indices, indices, sizeof(ui32)* NUM_INDICES);

  // copy pixel data 
  memset(iv->pixels, 0, sizeof(float)*4);                 // 4 floats, all 0, meaning black color and black alpha
  TO(4,i) iv->pixels[i] = 1.f;

  // Call once to get byteLen
  size_t byteLen = 0;
  IndexedVertsSave(iv, nullptr, &byteLen);
  vec<ui8> bytes(byteLen);
  // Call again to serialize
  IndexedVertsSave(iv, bytes.data(), &byteLen);
  IndexedVertsDestroy(iv);

  return bytes;
}

#endif
