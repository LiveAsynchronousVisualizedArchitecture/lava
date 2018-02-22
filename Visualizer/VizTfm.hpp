
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
inline Shape            tbl_to_shape(tbl const& iv)  // todo: try to change this to a const reference
{
  using namespace std;
  static float tmpImg[] = { 0.f, 0.f, 0.f, 0.f };

  Shape shp;
  u32 mode = (u32)((u64)iv("mode"));
  tbl ind, px, py, pz, nx, ny, nz, cr, cg, cb, ca, tx, ty, img;
  ind = iv("indices");
  px  = iv("positions x");  auto ff = px.memStart();
  py  = iv("positions y");
  pz  = iv("positions z");
  nx  = iv("normals x");
  ny  = iv("normals y");
  nz  = iv("normals z");
  cr  = iv("colors red");
  cg  = iv("colors green");
  cb  = iv("colors blue");
  ca  = iv("colors alpha");
  tx  = iv("texture coordinates x");
  ty  = iv("texture coordinates y");
  img = iv("image");

  shp.owner = true;
  shp.mode  = mode;
  shp.indsz = ind.size();

  SECTION(put the image in to shp.tx - if there was an image in the IdxVerts tbl, use that, if not, use a dummy img)
  {
    glGenTextures(1, &shp.tx);
    glBindTexture(GL_TEXTURE_2D, shp.tx);
     if(img){
       u64 chans = img("channels");
       u64     w = img("width");
       u64     h = img("height");
       if(chans==4 && w!=0 && h!=0 && img.arrayType()==tbl::TblType::F32)
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, img.data());
     }else
       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, tmpImg); 
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  glGenVertexArrays(1, &shp.vertary);
  glGenBuffers(1,      &shp.vertbuf);
  glGenBuffers(1,      &shp.idxbuf );

  glBindVertexArray(shp.vertary);

  vec<vert> verts;
  verts.resize(px.size());
  auto sz = verts.size();
  SECTION(convert/extract the individual component arrays into one array of Vertex structs)
  {
    TO(sz,i){
      verts[i] = { {0,0,0}, {0,0,0}, {1.f,0,0,1.f}, {0,0} };  // initialize - position (3 floats), normals (3 floats), colors (4 floats) and textures coordinates (2 floats) 
    }

    if(px) TO(sz,i) verts[i].p[0] = px.at<f32>(i);
    if(py) TO(sz,i) verts[i].p[1] = py.at<f32>(i);
    if(pz) TO(sz,i) verts[i].p[2] = pz.at<f32>(i);

    if(nx) TO(sz,i) verts[i].n[0] = nx.at<f32>(i);
    if(ny) TO(sz,i) verts[i].n[1] = ny.at<f32>(i);
    if(nz) TO(sz,i) verts[i].n[2] = nz.at<f32>(i);

    if(cr) TO(sz,i) verts[i].c[0] = cr.at<f32>(i);
    if(cg) TO(sz,i) verts[i].c[1] = cg.at<f32>(i);
    if(cb) TO(sz,i) verts[i].c[2] = cb.at<f32>(i);
    if(ca) TO(sz,i) verts[i].c[3] = ca.at<f32>(i);

    if(tx) TO(sz,i) verts[i].tx[0] = tx.at<f32>(i);
    if(ty) TO(sz,i) verts[i].tx[1] = ty.at<f32>(i);
  }

  glBindBuffer(GL_ARRAY_BUFFER, shp.vertbuf);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vert) * verts.size(), verts.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shp.idxbuf);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)*ind.size(), ind.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vert), (void*)0);                      
  glVertexAttribPointer(NORMAL,   3, GL_FLOAT, GL_FALSE, sizeof(vert), (void*)(sizeof(float) *  3));
  glVertexAttribPointer(COLOR,    4, GL_FLOAT, GL_FALSE, sizeof(vert), (void*)(sizeof(float) *  6));
  glVertexAttribPointer(TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(vert), (void*)(sizeof(float) * 10));

  glEnableVertexAttribArray(POSITION);
  glEnableVertexAttribArray(NORMAL);
  glEnableVertexAttribArray(COLOR);
  glEnableVertexAttribArray(TEXCOORD);

  glBindVertexArray(0);

  return move(shp);
}
inline Shape            tblbuf_to_shape(vec<u8> const& tblBuf)  // todo: try to change this to a const reference
{
  if( tblBuf.size() < sizeof(tbl::TblFields) ||
      ((i8*)tblBuf.data())[0] != 't'         ||
      ((i8*)tblBuf.data())[1] != 'b'
    ){ 
    return Shape();
  }

  tbl iv((void*)tblBuf.data(), false, false);

  if( tbl::StrToInt("IdxVerts") != (u64)iv("type") ){
    return Shape();
  }

  return tbl_to_shape(iv);
}
inline mat4           camera_to_mat4(Camera const& cam, float w, float h)
{  
  const static auto XAXIS = vec4(1.f, 0.f, 0.f, 1.f);
  const static auto YAXIS = vec4(0.f, 1.f, 0.f, 1.f);

  mat4 look  =  lookAt(cam.pos, cam.lookAt, cam.up);
  mat4 view  =  look;
        
  mat4 projection = glm::perspective(cam.fov, (w/h), cam.nearClip, cam.farClip);
  return projection * view;
}






//assert( iv.m_mem != nullptr );   auto f = iv.memStart();
//assert( ((i8*)iv.memStart())[0] = 't' );
//assert( ((i8*)iv.memStart())[1] = 'b' );

//if( iv.m_mem != nullptr               ||
//    ((i8*)iv.memStart())[0]   == 't'  ||
//    ((i8*)iv.memStart())[1]   == 'b'  ||
//    tbl::StrToInt("IdxVerts") == (u64)iv("type")
//  ){
//  return Shape();
//}

//u64 typenum = iv("type");
//assert( memcmp(&typenum, (u64*)"IdxVerts", sizeof(u64))==0 );

//TO(sz,i){
//  verts[i].p[0] = px.at<f32>(i);
//  verts[i].p[1] = py.at<f32>(i);
//  verts[i].p[2] = pz.at<f32>(i);
//}

//inline vec4         shapes_to_bndsph(VizData const& vd)
//{
//  f32  r = 0;
//  vec3 p(0,0,0);
//
//  for(auto& kv : vd.shapes)
//  {
//    if(!kv.second.active) continue;
//
//    auto&    key = kv.first;
//    u32     vlen = 0;
//    u32  version = 0;
//    auto     len = db.len(key.data(), (u32)key.length(), &vlen, &version);          // todo: make ui64 as the input length
//
//    vec<i8> ivbuf(vlen);
//    db.get(key.data(), (u32)key.length(), ivbuf.data(), (u32)ivbuf.size());
//
//    IndexedVerts* iv = (IndexedVerts*)IndexedVertsLoad(ivbuf.data());
//    vec3*          v = (vec3*)iv->verts;
//    TO(iv->vertsLen,i){
//      f32 dist = distance(v[i], p); 
//      p = (v[i] + p) / 2.f;
//      r = (r + dist) / 2.f;
//    }
//    IndexedVertsDestroy(iv);
//  }
//  return vec4(p, r);
//}

#endif














//Shape s = ivbuf_to_shape(ivbuf.data(), len);
//
//for(auto const& shp : shapes){
//  Shape const& s = shp.second;
//  if(s.active){
//    
//  }
//}

//using namespace glm;
//
//const static auto YAXIS = vec3(0.f, 1.f, 0.f);
//
//mat4 view  =  glm::inverse(look);
//
//mat4 projection;
//
//camMtx = vd.camera.tfm * projection;

//static float tmpImg[] = { 0.5f, 0.5f, 0.5f, 0.5f };
// = {0,0,0,0,0};         // Shape of all 0s
//mode = 0;  // GL_POINTS

//inline Shape          ivbuf_to_shape(void* buf, u64 len)    //IndexedVerts* iv)
//{
//  using namespace std;
//  
//  Shape shp;   // = {0,0,0,0,0};         // Shape of all 0s
//
//  if(!buf) return shp;
//  
//  auto iv = (IndexedVerts*)IndexedVertsLoad(buf);
//  if(!iv) return shp;
//
//  shp.owner = true;
//  shp.mode  = iv->mode;
//  shp.indsz = iv->indicesLen;
//
//  glGenTextures(1, &shp.tx);
//  glBindTexture(GL_TEXTURE_2D, shp.tx);
//  switch(iv->imgChans){
//  case 1:
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_R, iv->imgWidth, iv->imgHeight, 0, GL_R, GL_FLOAT, iv->pixels); 
//    break;
//  case 2:
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, iv->imgWidth, iv->imgHeight, 0, GL_RG, GL_FLOAT, iv->pixels); 
//    break;
//  case 3:
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iv->imgWidth, iv->imgHeight, 0, GL_RGB, GL_FLOAT, iv->pixels); 
//    break;
//  case 4:
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iv->imgWidth, iv->imgHeight, 0, GL_RGBA, GL_FLOAT, iv->pixels); 
//    break;
//  default:
//    ;
//  }
//  glBindTexture(GL_TEXTURE_2D, 0);
//
//  glGenVertexArrays(1, &shp.vertary);
//  glGenBuffers(1,      &shp.vertbuf);
//  glGenBuffers(1,      &shp.idxbuf );
//
//  glBindVertexArray(shp.vertary);
//
//  glBindBuffer(GL_ARRAY_BUFFER, shp.vertbuf);
//  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* iv->vertsLen, iv->verts, GL_STATIC_DRAW);
//
//  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shp.idxbuf);
//  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)* iv->indicesLen, iv->indices, GL_STATIC_DRAW);
//
//  IndexedVertsDestroy(iv);
//  
//  glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);                      
//  glVertexAttribPointer(NORMAL,   3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) *  3));
//  glVertexAttribPointer(COLOR,    4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) *  6));
//  glVertexAttribPointer(TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 10));
//
//  glEnableVertexAttribArray(POSITION);
//  glEnableVertexAttribArray(NORMAL);
//  glEnableVertexAttribArray(COLOR);
//  glEnableVertexAttribArray(TEXCOORD);
//
//  glBindVertexArray(0);
//
//  return move(shp);
//}

//tu32  ind = t("IND");
//const tbl* const ind;
//tbl const* px, const* py, const* pz;
//tbl const* nx, const* ny, const* nz;
//tbl const* cr, const* cg, const* cb;
//tbl const* tx, const* ty;

//verts[i].position[0] = px->at<f32>(i);
//verts[i].position[1] = py->at<f32>(i);
//verts[i].position[2] = pz->at<f32>(i);
//
//verts[i].normal[0] = nx->at<f32>(i);
//verts[i].normal[1] = ny->at<f32>(i);
//verts[i].normal[2] = nz->at<f32>(i);
//
//verts[i].color[0] = cr->at<f32>(i);
//verts[i].color[1] = cg->at<f32>(i);
//verts[i].color[2] = cb->at<f32>(i);
//
//verts[i].texCoord[0] = tx->at<f32>(i);
//verts[i].texCoord[1] = ty->at<f32>(i);

//
//u64 typenum = typenum=t("type");

//if(!buf) return shp;
//
//auto iv = (IndexedVerts*)IndexedVertsLoad(buf);
//if(!iv) return shp;

//shp.owner = true;
//shp.mode  = iv->mode;
//shp.indsz = iv->indicesLen;

//tf32    p = t("P");
//tf32    n = t("N");
//tf32    c = t("C");
//tf32   tx = t("TX");

// t("mode").kv->as<u32>()
//u32 mode  = t("mode").as<u32>();

//switch(iv->imgChans){
//case 1:
//  glTexImage2D(GL_TEXTURE_2D, 0, GL_R, iv->imgWidth, iv->imgHeight, 0, GL_R, GL_FLOAT, iv->pixels); 
//  break;
//case 2:
//  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, iv->imgWidth, iv->imgHeight, 0, GL_RG, GL_FLOAT, iv->pixels); 
//  break;
//case 3:
//  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iv->imgWidth, iv->imgHeight, 0, GL_RGB, GL_FLOAT, iv->pixels); 
//  break;
//case 4:
//
//  break;
//default:
//  ;
//}

//glGenVertexArrays(1, &shp.vertary);
//glGenBuffers(1,      &shp.vertbuf);
//glGenBuffers(1,      &shp.idxbuf );

//
//glBindVertexArray(shp.vertary);

//glBindBuffer(GL_ARRAY_BUFFER, shp.vertbuf);
////glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* iv->vertsLen, iv->verts, GL_STATIC_DRAW);
//glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* iv->vertsLen, iv->verts, GL_STATIC_DRAW);

//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shp.idxbuf);
//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)* iv->indicesLen, iv->indices, GL_STATIC_DRAW);

//
//IndexedVertsDestroy(iv);

//glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);                      
//glVertexAttribPointer(NORMAL,   3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) *  3));
//glVertexAttribPointer(COLOR,    4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) *  6));
//glVertexAttribPointer(TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 10));

//
//return Shape();

//template<class T>
//inline Shape          tbl_to_shape(tbl<T>&& t)
