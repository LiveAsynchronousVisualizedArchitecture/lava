
#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __VIZ_TRANSFORMS_HEADERGUARD_HPP__
#define __VIZ_TRANSFORMS_HEADERGUARD_HPP__

#ifdef _MSC_VER
  #define _CRT_SECURE_NO_WARNINGS 1
  #define _SCL_SECURE_NO_WARNINGS 1
#endif

#include <GL/glew.h>
#include "glfw3.h"
#include "IndexedVerts.h"
#include "VizDataStructures.hpp"

inline GLuint  shadersrc_to_shaderid(const char* vert, const char* frag)
{
  using namespace std;
  
  //str         vertexCode;
  //str       fragmentCode;
  //  
  //ifstream   vShaderFile;
  //ifstream   fShaderFile;
  //
  //vShaderFile.exceptions(ifstream::badbit);
  //fShaderFile.exceptions(ifstream::badbit);
  //try
  //{
  //    vShaderFile.open(vertShader.c_str());
  //    fShaderFile.open(fragShader.c_str());
  //    stringstream vShaderStream, fShaderStream;
  //    vShaderStream << vShaderFile.rdbuf();
  //    fShaderStream << fShaderFile.rdbuf();
  //    vShaderFile.close();
  //    fShaderFile.close();
  //    vertexCode = vShaderStream.str();
  //    fragmentCode = fShaderStream.str();
  //}
  //catch(ifstream::failure e)
  //{
  //    printf("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n");
  //}

  GLuint vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  const GLchar* vShaderSource =  vert; //.c_str();   // vertexCode.c_str();
  glShaderSource(vertexShader, 1, &vShaderSource, NULL);
  glCompileShader(vertexShader);
  GLint success;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if(!success) {
      GLchar infoLog[1024];
      glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
      printf("Compiling vertex shader failed: %s\n", infoLog);
  }

  GLuint fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  const GLchar* fShaderSource = frag;  //.c_str(); // fragmentCode.c_str();
  glShaderSource(fragmentShader, 1, &fShaderSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if(!success) {
      GLchar infoLog[1024];
      glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
      printf("Compiling fragment shader failed: %s\n", infoLog);
  }

  GLuint shaderProgramId = glCreateProgram();
  glAttachShader(shaderProgramId, vertexShader);
  glAttachShader(shaderProgramId, fragmentShader);
  glLinkProgram(shaderProgramId);
  glGetShaderiv(fragmentShader, GL_LINK_STATUS, &success);
  if(!success) {
      GLchar infoLog[1024];
      glGetProgramInfoLog(shaderProgramId, 1024, NULL, infoLog);
      printf("Linking failed: %s\n", infoLog);
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgramId;
}
inline Shape          ivbuf_to_shape(void* buf, ui64 len)    //IndexedVerts* iv)
{
  using namespace std;
  
  Shape shp;   // = {0,0,0,0,0};         // Shape of all 0s

  if(!buf) return shp;
  
  auto iv = (IndexedVerts*)IndexedVertsLoad(buf);
  if(!iv) return shp;

  glGenVertexArrays(1, &shp.vertary);
  glGenBuffers(1,      &shp.vertbuf);
  glGenBuffers(1,      &shp.idxbuf );

  glBindVertexArray(shp.vertary);

  glBindBuffer(GL_ARRAY_BUFFER, shp.vertbuf);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* iv->vertsLen, iv->verts, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shp.idxbuf);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)* iv->indicesLen, iv->indices, GL_STATIC_DRAW);

  IndexedVertsDestroy(iv);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 6));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  return move(shp);
}
inline auto    winbnd_to_sidebarRect(float w, float h) -> struct nk_rect
{
  using namespace std;
  
  float sbw = max(192.f, (1/6.f)*w );            // sbw is sidebar width
  float sbx = sbw - w;                           // sbx is sidebar x position
  
  return nk_rect(sbx, 0, sbw, h);    //struct nk_rect rect =
}

#endif


//
//inline GLuint shadersrc_to_shaderid(str const& vert, str const& frag)

//inline Shape       ivbuf_to_shape( )    //IndexedVerts* iv)
//
//inline GLuint shaderstrs_to_shaderid(str const& vertShader, str const& fragShader)
//{
//  using namespace std;
//  
//  str         vertexCode;
//  str       fragmentCode;
//  ifstream   vShaderFile;
//  ifstream   fShaderFile;
//
//  vShaderFile.exceptions(ifstream::badbit);
//  fShaderFile.exceptions(ifstream::badbit);
//  try
//  {
//      vShaderFile.open(vertShader.c_str());
//      fShaderFile.open(fragShader.c_str());
//      stringstream vShaderStream, fShaderStream;
//      vShaderStream << vShaderFile.rdbuf();
//      fShaderStream << fShaderFile.rdbuf();
//      vShaderFile.close();
//      fShaderFile.close();
//      vertexCode = vShaderStream.str();
//      fragmentCode = fShaderStream.str();
//  }
//  catch(ifstream::failure e)
//  {
//      printf("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n");
//  }
//
//  GLuint vertexShader;
//  vertexShader = glCreateShader(GL_VERTEX_SHADER);
//  const GLchar* vShaderSource = vertexCode.c_str();
//  glShaderSource(vertexShader, 1, &vShaderSource, NULL);
//  glCompileShader(vertexShader);
//  GLint success;
//  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
//  if(!success) {
//      GLchar infoLog[1024];
//      glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
//      printf("Compiling vertex shader failed: %s\n", infoLog);
//  }
//
//  GLuint fragmentShader;
//  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//  const GLchar* fShaderSource = fragmentCode.c_str();
//  glShaderSource(fragmentShader, 1, &fShaderSource, NULL);
//  glCompileShader(fragmentShader);
//  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
//  if(!success) {
//      GLchar infoLog[1024];
//      glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
//      printf("Compiling fragment shader failed: %s\n", infoLog);
//  }
//
//  GLuint shaderProgramId = glCreateProgram();
//  glAttachShader(shaderProgramId, vertexShader);
//  glAttachShader(shaderProgramId, fragmentShader);
//  glLinkProgram(shaderProgramId);
//  glGetShaderiv(fragmentShader, GL_LINK_STATUS, &success);
//  if(!success) {
//      GLchar infoLog[1024];
//      glGetProgramInfoLog(shaderProgramId, 1024, NULL, infoLog);
//      printf("Linking failed: %s\n", infoLog);
//  }
//  glDeleteShader(vertexShader);
//  glDeleteShader(fragmentShader);
//
//  return shaderProgramId;
//}
