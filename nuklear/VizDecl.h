

#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __VIZ_DECL_HEADERGUARD_HPP__
#define __VIZ_DECL_HEADERGUARD_HPP__

#include <map>
#include "GL/glew.h"
#include "glm/glm.hpp"

#include <nanogui/nanogui.h>

#include "../DataStore/ConcurrentMap/simdb.hpp"
#include "IndexedVerts.h"

using namespace nanogui;

enum AtrId : GLuint { POSITION = 0, NORMAL = 1, COLOR = 2, TEXCOORD = 3 };  // this coresponds to the the Vertex struct in IndexedVerts

template<class T, class A = std::allocator<T> > using vec = std::vector<T, A>;  // will need C++ ifdefs eventually

using   i8 = int8_t;
using   u8 = uint8_t;
using  u32 = uint32_t;
using  u64 = uint64_t;
using  i32 = int32_t;
using  i64 = int64_t;
using  au64 = std::atomic<u64>;
using  f32 = float;
using  f64 = double;
using  str = std::string;
using  VerStr = simdb::VerStr;
using  vec2 = glm::vec2;
using  vec3 = glm::vec3;
using  vec4 = glm::vec4;
using  quat = glm::quat;
using  mat4 = glm::mat4;
using  std::map;
using  glm::perspective;
using  glm::rotate;
using  glm::length;

using vec_vs = std::vector<VerStr>;
using vecstr = std::vector<str>;
using   vecb = std::vector<bool>;
using   vecu = std::vector<u32>;
using   veci = std::vector<i32>;

struct Shape {                     // todo: make rvalue constructor - make all constructors?
private:
  void del()
  {
    if (owner) {
      glDeleteVertexArrays(1, &vertary);
      glDeleteBuffers(1, &vertbuf);
      glDeleteBuffers(1, &idxbuf);
      glDeleteTextures(1, &tx);
    }
    owner = false;
  }
  void  mv(Shape&& rval)
  {
    del();

    memcpy(this, &rval, sizeof(Shape));
    memset(&rval, 0, sizeof(Shape));
    //rval.owner = false;
  }

public:
  bool      owner;
  int      active;
  u32      version, mode, indsz;          // mode is the openGL type of geometry to draw, indsz is the number of indices
  GLuint   vertbuf, vertary, idxbuf, tx;  // normals, colors, uvcoords, image data 
  GLuint   shader;                        // the shader ID is not owned by the shape 

  Shape() :
    owner(false),
    version(0),
    active(0),
    vertbuf(0),
    vertary(0),
    idxbuf(0),
    tx(0),
    shader(0)
  {}
  Shape(Shape&& rval)
  {
    mv(std::move(rval));
  }
  Shape(Shape const&) = delete;
  void operator=(Shape&& rval)
  {
    mv(std::move(rval));
  }
  void operator=(Shape const&) = delete;

  ~Shape()  // todo: put in checking to make sure that indices to delete are ok     // todo: need to build in ownership to not run the destructor twice?
  {
    del();
  }
};

struct Camera
{
  float fov, pansense, sensitivity, nearClip, farClip, dist;                     // field of view is fov
  vec2 mouseDelta;
  vec2 btn2Delta;
  vec2 oldMousePos;
  //vec3 position;
  //vec3 viewDirection;
  vec3 pos;
  vec3 rot;
  vec3 lookAt;
  vec3 up;
  mat4 pantfm;
  mat4 tfm;     //glm::mat4 transformMtx;
  mat4 proj;
  bool rightButtonDown, leftButtonDown;

  vec3 P() const
  {
    return vec3(tfm[0].w, tfm[1].w, tfm[2].w);
  }
  void P(const vec3 p)
  {
    tfm[0].w = p.x;
    tfm[1].w = p.y;
    tfm[2].w = p.z;
  }
};

using  KeyShapes = map<str, Shape>;
struct VizData
{
  GLFWwindow*            win;                     // Platform 
  struct nk_context*     ctx;
  Camera              camera;
  KeyShapes           shapes;
  GLuint            shaderId;
  struct
  {
    int w, h;
    f32 ptSz, hudSz, guideSz;                              // ptSz is point size  |  hudSz is  heads up display size
    Screen         screen;
    Window*        keyWin = nullptr;
    Window*         dbWin = nullptr;
    BoxLayout*     keyLay = nullptr;
    ComboBox*       dbLst = nullptr;
    int          dbLstIdx = -1;
    NVGcontext*       nvg = nullptr;
    veci           dbIdxs;
    vecstr        dbNames;
    bool        showGuide;

  } ui;
  f64 avgFps, prevt, cpuTime, t, dt;
  f64 keyRefresh, keyRefreshClock, verRefresh, verRefreshClock, prev, now;
  f32 mouseRGB[3];

  // todo: VizData deconstructor - will need this to clean up shader programs 
};



#endif

