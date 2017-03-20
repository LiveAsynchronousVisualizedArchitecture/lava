
#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __VIZ_TRANSFORMS_HEADERGUARD_HPP__
#define __VIZ_TRANSFORMS_HEADERGUARD_HPP__

//#ifdef _MSC_VER
//  #define _CRT_SECURE_NO_WARNINGS 1
//  #define _SCL_SECURE_NO_WARNINGS 1
//#endif

#include "GL/glew.h"
#include "glfw3.h"
#include "IndexedVerts.h"
#include "VizData.hpp"

// todo: put into a util file?
namespace{

void       print_gl_errors(int line)
{
  for(GLenum err; (err = glGetError()) != GL_NO_ERROR;){
    printf("OpenGL Error: 0x%04x - %d \n", err, line);
  }
}

}
#define PRINT_GL_ERRORS print_gl_errors(__LINE__);

inline GLuint  shadersrc_to_shaderid(const char* vert, const char* frag)
{
  using namespace std;
  
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
  PRINT_GL_ERRORS

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
  PRINT_GL_ERRORS

  GLuint shaderProgramId = glCreateProgram();
  glAttachShader(shaderProgramId, vertexShader);
  glAttachShader(shaderProgramId, fragmentShader);
  glLinkProgram(shaderProgramId);
  glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);
  if(!success) {
      GLchar infoLog[1024];
      glGetProgramInfoLog(shaderProgramId, 1024, NULL, infoLog);
      printf("Linking failed: %s\n", infoLog);
  }
  PRINT_GL_ERRORS

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

  shp.owner = true;
  shp.mode  = iv->mode;
  shp.indsz = iv->indicesLen;

  glGenTextures(1, &shp.tx);
  glBindTexture(GL_TEXTURE_2D, shp.tx);
  switch(iv->imgChans){
  case 1:
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R, iv->imgWidth, iv->imgHeight, 0, GL_R, GL_FLOAT, iv->pixels); 
    break;
  case 2:
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, iv->imgWidth, iv->imgHeight, 0, GL_RG, GL_FLOAT, iv->pixels); 
    break;
  case 3:
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iv->imgWidth, iv->imgHeight, 0, GL_RGB, GL_FLOAT, iv->pixels); 
    break;
  case 4:
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iv->imgWidth, iv->imgHeight, 0, GL_RGBA, GL_FLOAT, iv->pixels); 
    break;
  default:
    ;
  }
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenVertexArrays(1, &shp.vertary);
  glGenBuffers(1,      &shp.vertbuf);
  glGenBuffers(1,      &shp.idxbuf );

  glBindVertexArray(shp.vertary);

  glBindBuffer(GL_ARRAY_BUFFER, shp.vertbuf);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* iv->vertsLen, iv->verts, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shp.idxbuf);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)* iv->indicesLen, iv->indices, GL_STATIC_DRAW);

  IndexedVertsDestroy(iv);
  
  glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);                      
  glVertexAttribPointer(NORMAL,   3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) *  3));
  glVertexAttribPointer(COLOR,    4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) *  6));
  glVertexAttribPointer(TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 10));

  glEnableVertexAttribArray(POSITION);
  glEnableVertexAttribArray(NORMAL);
  glEnableVertexAttribArray(COLOR);
  glEnableVertexAttribArray(TEXCOORD);

  glBindVertexArray(0);

  return move(shp);
}
inline auto    winbnd_to_sidebarRect(float w, float h) -> struct nk_rect
{
  using namespace std;
  
  float sbw = max(192.f, (1/6.f)*w );            // sbw is sidebar width
  float sbx = w - sbw;                           // sbx is sidebar x position
  
  return nk_rect(sbx, 0, sbw, h);    //struct nk_rect rect =
}
inline mat4           camera_to_mat4(Camera const& cam, float w, float h)
{
  //using namespace glm;
  
  const static auto XAXIS = vec4(1.f, 0.f, 0.f, 1.f);
  const static auto YAXIS = vec4(0.f, 1.f, 0.f, 1.f);
  //const static auto YAXIS = vec3(0.f, 1.f, 0.f);

  mat4 look  =  lookAt(cam.pos, cam.lookAt, cam.up);
  //mat4 view  =  glm::inverse(look);
  mat4 view  =  look;
        
  //mat4 projection;
  mat4 projection = glm::perspective(cam.fov, (w/h), cam.nearClip, cam.farClip);
  return projection * view;
  //camMtx = vd.camera.tfm * projection;
}
inline vec4         shapes_to_bndsph(VizData const& vd)
{
  f32  r = 0;
  vec3 p(0,0,0);

  for(auto& kv : vd.shapes)
  {
    if(!kv.second.active) continue;

    auto&    key = kv.first;
    ui32    vlen = 0;
    ui32 version = 0;
    auto     len = db.len(key.data(), (ui32)key.length(), &vlen, &version);          // todo: make ui64 as the input length

    vec<i8> ivbuf(vlen);
    db.get(key.data(), (ui32)key.length(), ivbuf.data(), (ui32)ivbuf.size());

    IndexedVerts* iv = (IndexedVerts*)IndexedVertsLoad(ivbuf.data());
    vec3*          v = (vec3*)iv->verts;
    TO(iv->vertsLen,i){
      f32 dist = distance(v[i], p); 
      p = (v[i] + p) / 2.f;
      r = (r + dist) / 2.f;
    }
    IndexedVertsDestroy(iv);
  }
  return vec4(p, r);


  //Shape s = ivbuf_to_shape(ivbuf.data(), len);
  //
  //for(auto const& shp : shapes){
  //  Shape const& s = shp.second;
  //  if(s.active){
  //    
  //  }
  //}
}
#endif




//auto tfm    =  vd.camera.tfm;
//auto P      =  pos(tfm);
//auto plen   =  length(P);
//auto lkmtx =  lookAt(P, vd.camera.lookAt, normalize(vec3(vec4(vd.camera.up,1.f)*tfm)) );
//view       =  lkmtx;
//set_pos(&view, P);
//    
//vec3 yview = vec3(YAXIS * lkmtx);
//vec3 xview = vec3(XAXIS * lkmtx);
//
//auto sens  =  plen*2.f*PIf * vd.camera.sensitivity;
//auto xrot  =  -vd.camera.mouseDelta.x * sens;
//auto yrot  =  -vd.camera.mouseDelta.y * sens;
//view       =  rotate(view, yrot, xview);
//view       =  rotate(view, xrot, yview);                // todo: these need to be moved to the cursor callback, although input updates are only called once per frame so it likely doesn't matter
//auto viewP =  pos(view);
////tfm        =  lookAt(viewP, vd.camera.lookAt, normalize(vec3(vec4(vd.camera.up,1.f)*lkmtx)) );
//tfm        =  lookAt(viewP, vd.camera.lookAt, vd.camera.up);
//set_pos(&tfm, viewP);
//vd.camera.tfm = tfm;
//
//mat4 panmtx = vd.camera.tfm;
//set_pos(&panmtx, vec3(0));
//vec3 xpan   = vec3(XAXIS * panmtx);
//vec3 ypan   = vec3(YAXIS * panmtx) * -1.f;
//vec3 xofst  = normalize(xpan) * vd.camera.btn2Delta.x * vd.camera.pansense;
//vec3 yofst  = normalize(ypan) * vd.camera.btn2Delta.y * vd.camera.pansense;
//vd.camera.lookAt += xofst;
//vd.camera.lookAt += yofst;
//vec3 panpos = pos(vd.camera.tfm);
//panpos += xofst;
//panpos += yofst;
//set_pos(&vd.camera.tfm, panpos);

//glVertexAttribPointer( (GLuint)AtrId::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);                  // position 
//glVertexAttribPointer( (GLuint)AtrId::NORMAL,   4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 6));   // normals 
//glVertexAttribPointer( (GLuint)AtrId::COLOR,    4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 6));   // color 

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
