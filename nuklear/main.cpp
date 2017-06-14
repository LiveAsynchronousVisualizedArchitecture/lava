
/*
  Copyright 2017 Simeon Bassett

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

/*

  NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
  The widget drawing code is based on the NanoVG demo application
  by Mikko Mononen.

  All rights reserved. Use of NanoGUI and NanoVG source code is governed by a
  BSD-style license that can be found in the LICENSE.txt file.

*/

// -todo: write visualizer overview for Readme.md  
// -todo: put sensitivity into a separate settings struct - not broke, don't fix
// -todo: visualize one variable from tbl
// -todo: visualize more variables from tbl
// -todo: visualize map keys as u64 
// -todo: visualize array as u64
// -todo: draw table under UI
// -todo: use toString to find all string bounds ahead of time for fields
// -todo: find bounds of key-values ahead of time and draw
// -todo: find bounds of array values ahead of time and draw
// -todo: make bnd2f in vec.hpp - put in VizDecl
// -todo: figure out bounding box for window minus sidebar
// -todo: layout bounds within a specified bounding box
// -todo: figure out why resolution goes down - device pixel ratio needs to just be 1.f
// -todo: use templated tbl for layout of table cell drawing
// -todo: sort keys so that they are in alphabetical order
// -todo: fix drawing of empty key
// -todo: use tbl for ofsts
// -todo: convert to using tbl instead of vectors for drawing labels - needed to make push use the copy constructor instead of the assignment operator ( operator= )
// -todo: draw blue line as a separator between tbl sections
// -todo: draw a line around table visualization
// -todo: draw graph of array values
// -todo: make sure selecting same db doesn't close the db
// -todo: fix wrong simdb on first switch - problem is index not being changed when the dbNames is updated? - also needed to change setSelectionIndex(int) on the nanogui ComboBox

// todo: make tbl to Shape function in VizTfm.hpp
// todo: use tbl for IndexedVerts after adding sub-tables
// todo: fix tbl visualization cells going outside the bounds of bounding box - need to wrap sooner, possibly based on more margins
// todo: make camera fitting use the field of view and change the dist to fit all geometry 
//       |  use the camera's new position and take a vector orthongonal to the camera-to-lookat vector. the acos of the dot product is the angle, but tan will be needed to set a position from the angle?
//       |  visualize the fit position and camera frustum in real time to debug

// todo: move and rename project to LavaViz or any non test name
// todo: try out tiny/nano file dialog for saving and loading of serialized data 
// todo: make save button or menu to save serialized files 
// todo: make a load button or menu to load serialized files - would need to have a visualizer specific simdb that would keep the files? 
// todo: make label switches not only turn keys on and off, but fade their transparency too?
// todo: keep databases in memory after listing them?

// idea: draw histogram from array
// idea: draw variance per pixel when graph of values ends up sub-pixel
// idea: zoom in on array visualization by clicking on a place in the graph or histogram - graph would zoom in on a section in memory - histogram would need to show the indices that are within a certain value range
// idea: visualize memory layout with hex numbers
// idea: use libtcc to interpret a struct declaration for the type of tbl array? 
// idea: use field key values in the tbl to interpret the tbl array type?
// idea: look into drag and drop to load indexed verts objects by dragging from a file
// idea: ability to display points of an indexed verts type as numbers - this would give the ability to have numbers floating in space - could be take care of with a specialized tbl

#include <chrono>
#include <memory>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "GL/glew.h"

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include "glfw3.h"

#include "VizDecl.h"
#include "VizData.hpp"
#include "VizGen.hpp"
#include "VizTfm.hpp"

#include "no_rt_util.h"

#define ENTRY_DECLARATION int main(void)
#ifdef _MSC_VER
  #pragma comment(lib, "glfw3dll.lib")
  #pragma comment(lib, "opengl32.lib")
  //#pragma comment(lib, "glew32.lib")

  #if defined(_DEBUG)
    #define USE_CONSOLE                                 // turning this off will use the windows subsystem in the linker and change the entry point to WinMain() so that no command line/console will appear
  #endif
  #ifndef USE_CONSOLE
    #pragma comment( linker, "/subsystem:windows" )
    #undef ENTRY_DECLARATION
    #define ENTRY_DECLARATION int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
  #endif
#endif

#define MAX_VERTEX_BUFFER  512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

namespace {  // functions that are a transform from one datatype to another are in VizTfm.hpp - functions here are more state based

using v2i  =  Eigen::Vector2i;
using v4f  =  Eigen::Vector4f;

const u32 TITLE_MAX_LEN = 256;

template<class T> inline std::string
toString(T const& x)
{
  std::ostringstream convert;
  convert << x;
  return convert.str();
}
template<class T1, class... T> inline std::string
toString(const T1& a, const T&... args)
{
  return toString(a) + toString(args...);
}

vec3                      pos(mat4 const& m)
{ return m[3];                      }
void                  set_pos(mat4* m, vec3 const p)
{
  //(*m)[0].w = p.x;
  //(*m)[1].w = p.y;
  //(*m)[2].w = p.z;
  (*m)[3].x = p.x;
  (*m)[3].y = p.y;
  (*m)[3].z = p.z;
}
float        wrapAngleRadians(float angle)     
{
  using namespace glm;
  const static float _2PI = 2.f*pi<float>();

  return fmodf(angle, _2PI);
}

void                initSimDB(str const& name)
{
  db.close();
  new (&db) simdb(name.c_str(), 4096, 1 << 14);             // inititialize the DB with placement new into the data segment

  if(vd.win){
    str title = "Visualizer - simdb_" + name;
    glfwSetWindowTitle(vd.win, title.c_str());
  }
}
void                 initGlew()
{
  //glewExperimental = 1;
  if(glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to setup GLEW\n");
    exit(1);
  }
}
Camera             initCamera()
{
  Camera cam;
  cam.fov             = PIf * 0.5f;
  cam.mouseDelta      = vec2(0.0f, 0.0f);
  cam.btn2Delta       = vec2(0.0f, 0.0f);
  cam.sensitivity     = 0.005f;
  cam.pansense        = 0.002f;
  cam.pos             = vec3(0,0,-10.0f);
  cam.rot             = vec3(0,0,0);
  cam.lookAt          = vec3(0.0f, 0.0f, 0.0f);
  cam.dist            = 10.0f;
  cam.up              = vec3(0.0f, 1.0f, 0.0f);
  cam.oldMousePos     = vec2(0.0f, 0.0f);
  cam.rightButtonDown = false;
  cam.leftButtonDown  = false;
  cam.nearClip        = 0.01f;
  cam.farClip         = 1000.0f;

  return cam;
}

void            errorCallback(int e, const char *d) {
  printf("Error %d: %s\n", e, d);
  fflush(stdout);
}
void              keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  //using namespace glm;

  if(key==GLFW_KEY_ESCAPE && action==GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);

  VizData* vd = (VizData*)glfwGetWindowUserPointer(window);
  auto&   cam = vd->camera;
  switch(key)
  {
  case GLFW_KEY_K: {
  } break; 
  case GLFW_KEY_H: {
    vd->camera = initCamera();
  } break; 
  case GLFW_KEY_J:
  case GLFW_KEY_F: {
    if(action==GLFW_RELEASE){
      vec4   sph   =  shapes_to_bndsph(*vd);
      vec3  cntr   =  vec3(sph.x,sph.y,sph.z);
      vec3  ofst   =  cam.pos - cam.lookAt;
      cam.lookAt   =  cntr;
      cam.pos      =  cam.lookAt + ofst;
    }
    //cam.dist     =  sph.r;
  } break;
  case GLFW_KEY_PAGE_UP: {
    vd->ui.ptSz *= 2;
  } break;
  case GLFW_KEY_PAGE_DOWN: {
    vd->ui.ptSz /= 2;
  } break;
  default:
    break;
  }

  vd->ui.screen.keyCallbackEvent(key, scancode, action, mods);
}
void           scrollCallback(GLFWwindow* window, double xofst, double yofst)
{
  using namespace std;
  
  const float dollySens = 0.1f;

  VizData*   vd = (VizData*)glfwGetWindowUserPointer(window);
  auto&     cam = vd->camera;
  float    ofst = (float) -(cam.dist*yofst*dollySens);
  float  prvDst = cam.dist;                                        // prvDst is previous distance
  float  nxtDst = cam.dist;
  nxtDst        = cam.dist + ofst;
  cam.dist      = nxtDst;

  vd->ui.screen.scrollCallbackEvent(xofst, yofst);
}
void         mouseBtnCallback(GLFWwindow* window, int button, int action, int mods)
{
  VizData* vd = (VizData*)glfwGetWindowUserPointer(window);

  vd->ui.showGuide = false;

  if(button==GLFW_MOUSE_BUTTON_LEFT){
    if(action==GLFW_PRESS) vd->camera.leftButtonDown = true;
    else if(action==GLFW_RELEASE) vd->camera.leftButtonDown = false;
  }

  if(button==GLFW_MOUSE_BUTTON_RIGHT){
    if(action==GLFW_PRESS) vd->camera.rightButtonDown = true;
    else if(action==GLFW_RELEASE){
      vd->camera.rightButtonDown = false;
    }
  }

  vd->ui.screen.mouseButtonCallbackEvent(button, action, mods);
}
void        cursorPosCallback(GLFWwindow* window, double x, double y)
{
  //using namespace glm;
  const static float _2PI = 2.f* PIf; //  pi<float>();

  VizData* vd = (VizData*)glfwGetWindowUserPointer(window);

  Camera& cam = vd->camera;
  vec2 newMousePosition = vec2((float)x, (float)y);

  if(cam.leftButtonDown){
    cam.mouseDelta = (newMousePosition - cam.oldMousePos);
  }else{ cam.mouseDelta = vec2(0,0); }
    
  if(cam.rightButtonDown){
    cam.btn2Delta  = (newMousePosition - cam.oldMousePos);
  }else{ 
    cam.btn2Delta  = vec2(0,0);

    cam.tfm     =  lookAt(cam.pos, cam.lookAt, cam.up);
    vec3 scale, pos, skew; vec4 persp;
    quat rot;
    decompose(cam.tfm, scale, rot, pos, skew, persp);
    cam.pantfm  =  (mat4)conjugate(rot);
  }

  cam.oldMousePos = newMousePosition;

  vd->ui.screen.cursorPosCallbackEvent(x,y);
}
void             charCallback(GLFWwindow* window, unsigned int codepoint)
{
  VizData* vd = (VizData*)glfwGetWindowUserPointer(window);

  vd->ui.screen.charCallbackEvent(codepoint);
}
void             dropCallback(GLFWwindow* window, int count, const char** filenames)
{
  VizData* vd = (VizData*)glfwGetWindowUserPointer(window);

  vd->ui.screen.dropCallbackEvent(count, filenames);
}
void  framebufferSizeCallback(GLFWwindow* window, int w, int h)
{
  VizData* vd = (VizData*)glfwGetWindowUserPointer(window);

  vd->ui.screen.resizeCallbackEvent(w, h);
}
GLFWwindow*          initGLFW(VizData* vd)
{
  glfwSetErrorCallback(errorCallback);
  if( !glfwInit() ){
    fprintf(stdout, "[GFLW] failed to init!\n");
    exit(1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
  glfwWindowHint(GLFW_SAMPLES, 32);

  GLFWwindow* win = glfwCreateWindow(vd->ui.w, vd->ui.h, "Visualizer", NULL, NULL);    assert(win!=nullptr);
  glfwMakeContextCurrent(win);
  glfwGetWindowSize(win, &vd->ui.w, &vd->ui.h);

  glfwSetCharCallback(win,              charCallback);         // in glfw charCallback is for typing letters and is different than the keyCallback for keys like backspace 
  glfwSetKeyCallback(win,                keyCallback);
  glfwSetScrollCallback(win,          scrollCallback);
  glfwSetCursorPosCallback(win,    cursorPosCallback);
  glfwSetMouseButtonCallback(win,   mouseBtnCallback);
  glfwSetDropCallback(win,              dropCallback);
  glfwSetFramebufferSizeCallback(win, framebufferSizeCallback);
  //glfwSetScrollCallback(win, )

  #ifdef _WIN32
    //GLFWimage images[2];
    //images[0] = LoadIcon("lava.jpg");
    //images[1] = LoadIcon("lava.jpg");
    //glfwSetWindowIcon(win, 2, images);
  #endif

  return win;
}

void           buttonCallback(str key, bool pushed)
{
  vd.shapes[key].active = pushed;
}
void            dbLstCallback(bool pressed)
{
  if(pressed){
    str  name;
    if(vd.ui.dbNameIdx>=0 && vd.ui.dbNameIdx<vd.ui.dbNames.size()){
      name = vd.ui.dbNames[vd.ui.dbNameIdx];
    }

    vd.ui.dbNames = simdb_listDBs();                            // all of these are globals
    auto     iter = find( ALL(vd.ui.dbNames), name);
    if(iter != vd.ui.dbNames.end()){
      vd.ui.dbNameIdx = iter - vd.ui.dbNames.begin();
    }else{ vd.ui.dbNameIdx = 0; }                                                        // if the name isn't found in the new listing, then just reset to the first in the list of db names

    if(vd.ui.dbLst){
      vd.ui.dbLst->setItems(vd.ui.dbNames);
      vd.ui.dbLst->setSelectedIndex(vd.ui.dbNameIdx);
    }
    vd.ui.screen.performLayout();
  }
}

void              RenderShape(Shape const& shp, mat4 const& m) // GLuint shaderId)
{
  glUseProgram(shp.shader);  //shader.use();

  GLint transformLoc = glGetUniformLocation(vd.shaderId, "transform");
  glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(m));

  int size;
  glActiveTexture(GL_TEXTURE0);	// Activate the texture unit first before binding texture
  glBindTexture(GL_TEXTURE_2D, shp.tx);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_LINEAR);
  PRINT_GL_ERRORS

  auto loc = glGetUniformLocation(shp.shader, "tex0");
  glUniform1i(loc, 0);

  glBindVertexArray(shp.vertary);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shp.idxbuf);                       PRINT_GL_ERRORS
  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);  PRINT_GL_ERRORS   // todo: keep size with shape instead of querying it
  glDrawElements(shp.mode, shp.indsz, GL_UNSIGNED_INT, 0);                 PRINT_GL_ERRORS   // todo: can't use quads with glDrawElements?
  
  //glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
  
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}
vec_vs         shapesFromKeys(simdb const& db, vec_vs dbKeys, VizData* vd)  // vec<str> const& dbKeys
{
  using namespace std;

  for(auto& vs : dbKeys)
    TO(dbKeys.size(),i)
    {
      auto& vs = dbKeys[i];
      auto cur = vd->shapes.find(vs.str);
      if(cur!=vd->shapes.end() ){
        continue;
      }

      u32     vlen = 0;
      u32  version = 0;
      auto     len = db.len(vs.str.data(), (u32)vs.str.length(), &vlen, &version);          // todo: make ui64 as the input length
      vs.ver = version;

      //vec<i8> ivbuf(vlen);
      //db.get(vs.str.data(), (u32)vs.str.length(), ivbuf.data(), (u32)ivbuf.size());
      //Shape  s      = ivbuf_to_shape(ivbuf.data(), len);      PRINT_GL_ERRORS

      auto ivbuf = (u8*)malloc(vlen);   // todo: check to make sure this succeeds 
      db.get(vs.str.data(), (u32)vs.str.length(),  ivbuf, vlen);
      IvTbl iv(ivbuf);
      //iv.m_mem   = ivbuf;
      auto     f = iv.memStart();

      Shape  s   = tbl_to_shape(iv);      PRINT_GL_ERRORS
      s.shader   = vd->shaderId;
      s.active   = vd->shapes[vs.str].active;
      s.version  = version; //vs.v; // version;
      vd->shapes[vs.str] = move(s);
    };

  return dbKeys;
}
vec_vs       eraseMissingKeys(vec_vs dbKeys, KeyShapes* shps)           // vec<str> dbKeys,
{
  int cnt = 0;
  sort( ALL(dbKeys) );
  //assert( shps->size() == 0 || dbKeys.size() != 0 );
  if( shps->size() > 0 && dbKeys.size() == 0 )
    printf("wat");

  for(auto it = shps->begin(); it != shps->end(); )
  {
    auto const& kv = *it;
    VerStr vs;
    vs.ver = kv.second.version;
    vs.str = kv.first;
    bool isKeyInDb = binary_search(ALL(dbKeys),vs, [](VerStr const& a, VerStr const& b){ return a.str < b.str; } );
    if( !isKeyInDb ){
      it = shps->erase(it);
      ++cnt;
    }else ++it;
  }

  return dbKeys;
  //return cnt;
}
bool                updateKey(simdb const& db, str const& key, u32 version, VizData* vd)
{
  using namespace std;

  u32 dbVersion = 0;
  u32      vlen = 0;
  auto       len = db.len(key.data(), (u32)key.length(), &vlen, &dbVersion);

  if(len>0 && dbVersion!=version){ //key.v){
    vec<i8> ivbuf(vlen);
    db.get(key.data(), (u32)key.length(), ivbuf.data(), (u32)ivbuf.size());

    Shape  s  = ivbuf_to_shape(ivbuf.data(), len);
    s.shader  = vd->shaderId;
    s.active  = true;                     // because updates only happen when the shape is active, always setting an updated shape to active should work
    s.version = dbVersion;
    vd->shapes[key] = move(s);
    
    return true;
  }

  return false;
}
double                   nowd()
{
  using namespace std;
  using namespace std::chrono;

  auto nano = duration_cast<nanoseconds>( chrono::high_resolution_clock::now().time_since_epoch() );

  return nano.count() / 1000000000.0; 
}
void                refreshDB(VizData* vd)
{
  auto dbKeys = db.getKeyStrs();                                      // Get all keys in DB - this will need to be ran in the main loop, but not every frame
  dbKeys      = shapesFromKeys(db, move(dbKeys), vd);
  dbKeys      = eraseMissingKeys(move(dbKeys), &vd->shapes);
  sort(ALL(dbKeys));
  sort(ALL(vd->ui.dbIdxs));                                                  // sort the indices so the largest are removed first and the smaller indices don't change their position
  FROM(vd->ui.dbIdxs.size(), i){ vd->ui.keyWin->removeChild(vd->ui.dbIdxs[i]); }
  vd->ui.dbIdxs.resize(0);
  vd->ui.dbIdxs.shrink_to_fit();
  for(auto key : dbKeys)                                              // add the buttons back and keep track of their indices
  {
    auto b = new Button(vd->ui.keyWin, key.str);
    int  i = vd->ui.keyWin->childIndex(b);
    if(i > -1){ vd->ui.dbIdxs.push_back(i); }
    b->setFlags(Button::ToggleButton);
    b->setChangeCallback([k = key.str](bool pushed){ buttonCallback(k, pushed); });
    b->setPushed(vd->shapes[key.str].active);
    b->setFixedHeight(25);
  }
  vd->ui.screen.performLayout();

  vd->keyRefreshClock -= vd->keyRefresh;
  vd->verRefreshClock -= vd->verRefresh;

  if(vd->keyRefreshClock > vd->keyRefresh){                           // if there is already enough time saved up for another update make sure that less that two updates worth of time is kept 
    vd->keyRefreshClock = vd->keyRefresh + fmod(vd->keyRefreshClock, vd->keyRefresh);
  }
}

void       drawBnd(NVGcontext* nvg, f32  b[4])
{
  nvgBeginPath(nvg);
   nvgRect(nvg, b[0], b[1], b[2]-b[0], b[3]-b[1]);
   nvgStrokeWidth(nvg, 1.f);
   nvgStrokeColor(nvg, nvgRGBAf(.8f, .6f, .2f, 1.f));
  nvgStroke(nvg);
}
void      drawLine(NVGcontext* nvg, f32 x, f32 y, f32 w)
{
  nvgBeginPath(nvg);
    nvgRect(nvg, x, y, w, 1.f);
    nvgStrokeWidth(nvg, 1.f);
    nvgStrokeColor(nvg, nvgRGBAf(0,.25f,.8f, 1.f));
  nvgStroke(nvg);
}
void      drawHist(NVGcontext* nvg, u64* buf, u64 len, bnd2f b)
{
  
}
v2         strOfst(NVGcontext* nvg, str    const&    s)                                      // strOfst is string offset
{
  bnd2f bnd;
  nvgTextBounds(nvg, 0,0, s.c_str(), NULL, bnd.b);
  
  return { bnd.xmx-bnd.xmn, bnd.ymx-bnd.ymn };
}
f32       drawStrs(NVGcontext* nvg, tblstr const& strs, tblv2 const& ofsts, f32 w, f32 x, f32 y, f32 margin)
{
  f32  h = 0.f;
  f32 m2 = margin * 2.f;
  v2   o = {x,y};                                                                         // o is offset
  TO(strs.size(),i){
    if(o.x+ofsts[i].x+m2 > w){
      o.x  = x;
      o.y += ofsts[i].y + m2;
      h   += ofsts[i].y + m2;
    }else if(i==0){ h += ofsts[i].y + m2; }

    bnd2f bnd = { o.x, o.y, o.x+ofsts[i].x+m2, o.y+ofsts[i].y+m2 };                       //  o.x + margin * 2, o.y + margin * 2 };
    drawBnd(nvg, bnd.b);

    nvgText(nvg, o.x+margin, o.y+ofsts[i].y+margin, strs[i].c_str(), NULL);

    o.x += ofsts[i].x + m2;
  }

  return h;
}
f32      drawGraph(NVGcontext* nvg, tblu   const&    t, bnd2f b)
{
  using namespace std;

  f32 mxf, inc, hgt;

  u64 mx = 0;
  TO(t.size(),i){ mx = max(mx, t[i]);  }
  mxf = (f32)mx;
  inc = (b.w()-4) / (f32)t.size();
  hgt = b.h()-4;

  nvgStrokeWidth(nvg, 1.f);
  nvgStrokeColor(nvg, nvgRGBAf(0,.8f,.25f, 1.f));
  nvgFillColor(nvg, nvgRGBAf(0,.5f,.25f, .5f));
  TO(t.size(),i)
  {
    f32 x = b.xmn + i*inc + 2;
    f32 y = b.ymx - 2;
    f32 h = hgt * (t[i]/mxf);
    nvgBeginPath(nvg);
      nvgRect(nvg, x, y-h, inc, h);
    nvgFill(nvg);
    nvgStroke(nvg);
  }

  return b.ymx - b.ymn;
}
void       drawTbl(NVGcontext* nvg, tblu   const&    t, f32 w, f32 h, f32 x=0.f, f32 y=0.f, f32 sz=50.f, f32 margin=5.f)
{
  using namespace std;
  
  f32 xo=0, yo=0;                                                    // xo is x offset   yo is y offset
  f32 vizW = w - x - margin*2;

  nvgFontSize(nvg,  sz);
  nvgFontFace(nvg,  "sans");
  nvgTextAlign(nvg, NVG_ALIGN_LEFT); // | NVG_ALIGN_MIDDLE);
  nvgFillColor(nvg, nvgRGBAf(1.f, 1.f, .65f, 1.f));

  tblstr labels;  labels.reserve(5);
  tblv2   ofsts;  ofsts.reserve(5);

  v2      o = { x+margin, y+margin };                                                                    // o is offset
  f32  xrem = w; 
  f32   mxY = 0.f;
  f32    dh = 0.f;                                                                         // dh is draw height
  SECTION(draw fields)
  {
    labels.push_back( toString("sizeBytes:  ",     t.sizeBytes())      );
    labels.push_back( toString("capacity:  ",      t.capacity())       );
    labels.push_back( toString("size:  ",          t.size())           );
    labels.push_back( toString("map elems:  ",     t.elems())          );
    labels.push_back( toString("map capacity:  ",  t.map_capacity())   );
    labels.push_back( toString("child capcity:  ", t.child_capacity()) );
    TO(labels.size(),i){ ofsts.push_back( strOfst(nvg,labels[i]) ); }
    TO(ofsts.size(),i){ mxY = max<f32>(mxY, ofsts[i].y); }

    dh = drawStrs(nvg, labels, ofsts, w, o.x, o.y, margin);
  }

  o.x  = x+margin;
  o.y += dh + margin*2;                                                                   // o is offset
  drawLine(nvg, x, o.y-margin, w-x-margin*2);
  SECTION(draw key value pairs)
  {
    labels.clear();
    auto e = t.elemStart();
    TO(t.map_capacity(),i) if(!e[i].isEmpty()){
      labels.push_back( toString(e[i].key,":  ", e[i].val) );
    }
    sort( ALL(labels) );
    ofsts.clear();
    TO(labels.size(),i){ ofsts.push_back(strOfst(nvg, labels[i])); }
    mxY=0.f;
    TO(ofsts.size(),i){ mxY = max<f32>(mxY, ofsts[i].y); }

    dh = drawStrs(nvg, labels, ofsts, w, o.x, o.y, margin);
  }

  o.x   =  x+margin;
  o.y  +=  dh + margin*2;
  drawLine(nvg, o.x-margin, o.y-margin, w-x-margin*2);
  SECTION(draw array elements)
  {
    labels.clear();
    TO(t.size(),i){ labels.push_back( toString(i,":  ",t[i]) ); }

    ofsts.clear();
    //new (&ofsts) tblv2;
    TO(labels.size(), i) { ofsts.push_back(strOfst(nvg, labels[i])); }
    mxY=0.f;
    TO(ofsts.size(),i){ mxY = max<f32>(mxY, ofsts[i].y); }

    dh = drawStrs(nvg, labels, ofsts, w-margin, o.x, o.y, margin);
  }

  o.x  =  x+margin;
  o.y +=  dh + margin*2;
  dh   =  drawGraph(nvg, t, {o.x, o.y, x+vizW-margin, o.y+100.f});

  o.y  +=  dh + margin*2;
  nvgBeginPath(nvg);
    nvgRect(nvg, x, y, w-x-margin*2, o.y);
    nvgStrokeWidth(nvg, 1.f);
    nvgStrokeColor(nvg, nvgRGBAf(0,.1f,.6f, 1.f));
  nvgStroke(nvg);

  //nvgRect(nvg, x, y, w-x-margin*2, o.y+dh);
}

}

static tblu tst;
void       genTestGeo(simdb* db)
{
  using namespace std;
  
  static simdb db1("test 1", 4096, 1 << 14);
  static simdb db2("test 2", 4096, 1 << 14);

  // new (&db2) simdb(name.c_str(), 4096, 1 << 14);             // inititialize the DB with placement new into the data segment

  initSimDB("Viz Default");

  // Create serialized IndexedVerts
  size_t leftLen, rightLen, cubeLen;
  vec<u8>  leftData = makeTriangle(leftLen,   true);
  vec<u8> rightData = makeTriangle(rightLen, false);
  vec<u8>  cubeData = makeCube(cubeLen);

  // Store serialized IndexedVerts in the db
  str  leftTriangle = "leftTriangle";
  str rightTriangle = "rightTriangle";
  str          cube = "cube";

  //db->put(leftTriangle, leftData);
  //db->put(rightTriangle, rightData);
  //db->put(cube, cubeData);

  db1.put("1", leftData);
  db1.put("2", rightData);
  db1.put("3", cubeData);

  db2.put("one",    leftData);
  db2.put("two",   rightData);
  db2.put("three",  cubeData);
  db2.put("super long key name as a test", cubeData);

  //iv->verts[0] = {
  //  {-1.0, -1.0f, 0.0f},      //pos
  //  {0.0f, 0.0f, -1.0f},      //norm
  //  {1.0f, 1.0f, 1.0f, 1.0f}, //color
  //  {0.0f, 0.0f}              //texCoord
  //};
  //iv->verts[1] = {
  //  {-0.17f, -1.0f, 0.0f},      //pos
  //  {0.0f, 0.0f, -1.0f},      //norm
  //  {1.0f, 1.0f, 1.0f, 1.0f}, //color
  //  {0.0f, 0.0f}              //texCoord
  //};
  //iv->verts[2] = {
  //  {-0.58f, 1.0f, 0.0f},       //pos
  //  {0.0f, 0.0f, -1.0f},        //norm
  //  {1.0f, 1.0f, 1.0f, 1.0f},   //color
  //  {0.0f, 0.0f}                //texCoord
  //};

  //u64 type = lftTri("type");
  //char* tstr = (char*)&type;

  //Tbl lftTri;
  //auto typenum = "IdxVerts";
  //lftTri("type") = *((u64*)typenum);
  //lftTri("mode") = GL_TRIANGLES;
  //tf32 p   = { -1.0, -1.0f,  0.0f, -0.17f, -1.0f,  0.0f, -0.58f, 1.0f,  0.0f };
  //tf32 n   = { 0.0f,  0.0f, -1.0f,   0.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f };
  //tf32 c   = { 1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f };
  //tf32 tx  = { 0.0f,  0.0f,  0.0f,   0.0f,  0.0f,  0.0f };
  //tu32 ind = { 0, 1, 2};
  //
  //lftTri("P")   =  &p;
  //lftTri("N")   =  &n;
  //lftTri("C")   =  &c;
  //lftTri("TX")  =  &tx;


  IvTbl lftTri = {
   {{-1.0, -1.0f, 0.0f},       //pos
    {0.0f, 0.0f, -1.0f},       //norm
    {1.0f, 1.0f, 1.0f, 1.0f},  //color
    {0.0f, 0.0f}},             //texCoord
  
   {{-0.17f, -1.0f, 0.0f},      //pos
    {0.0f, 0.0f, -1.0f},        //norm
    {1.0f, 1.0f, 1.0f, 1.0f},   //color
    {0.0f, 0.0f}},              //texCoord

   {{-0.58f, 1.0f, 0.0f},       //pos
    {0.0f, 0.0f, -1.0f},        //norm
    {1.0f, 1.0f, 1.0f, 1.0f},   //color
    {0.0f, 0.0f}}               //texCoord
  };
  auto typenum    =  "IdxVerts";
  lftTri("type")  =  *((u64*)typenum);
  lftTri("mode")  =  GL_TRIANGLES;
  tu32 ind        =  {0, 1, 2};
  lftTri("IND")   =  &ind; 
  lftTri.flatten();

  auto f = lftTri.memStart();

  db->put("tb left triangle", lftTri.memStart(), lftTri.sizeBytes() );
}

ENTRY_DECLARATION
{
  using namespace std;
  using namespace nanogui;

  genTestGeo(&db);

  SECTION(initialization)
  {
    SECTION(static VizData)
    {
      vd.ui.w             = 1024; 
      vd.ui.h             = 1024;
      vd.ui.ptSz          =    0.25f;
      vd.ui.hudSz         =   16.0f;
      vd.ui.guideSz       =   32.0f;

      vd.now              =  nowd();
      vd.prev             =  vd.now;
      vd.verRefresh       =  1.0/144.0;
      vd.verRefreshClock  =  0.0;
      vd.keyRefresh       =  2.0;
      vd.keyRefreshClock  =  vd.keyRefresh;
      vd.camera           =  initCamera();

      vd.avgFps   =  60; 
      vd.prevt    =   0; 
      vd.cpuTime  =   0; 
      vd.t        =   0; 
      vd.dt       =   0;

      vd.ui.keyWin     =  nullptr;
      vd.ui.dbWin      =  nullptr;
      vd.ui.keyLay     =  nullptr;
      vd.ui.dbLst      =  nullptr;
      vd.ui.dbLstIdx   =  -1;
      vd.ui.nvg        =  nullptr;
      vd.ui.showGuide  =  true;
      vd.ui.dbNameIdx  =   0;
    }
    SECTION(glfw and glew)
    {
      vd.win      = initGLFW( &vd );                        assert(vd.win!=nullptr);
      initGlew();
      vd.shaderId = shadersrc_to_shaderid(vertShader, fragShader);   PRINT_GL_ERRORS

      glfwSetWindowUserPointer(vd.win, &vd);
      glfwSwapInterval(0);
      glfwSwapBuffers(vd.win);
      glfwMakeContextCurrent(vd.win);

      PRINT_GL_ERRORS
    }
    SECTION(nanogui)
    {
      vd.ui.screen.initialize(vd.win, false);

      SECTION(sidebar)
      {
        vd.ui.keyLay = new BoxLayout(Orientation::Vertical, Alignment::Fill, 2, 5);
        vd.ui.keyWin = new Window(&vd.ui.screen,  "");
        auto spcr1   = new Label(vd.ui.keyWin,    "");
        auto spcr2   = new Label(vd.ui.keyWin,    "");
        auto spcr3   = new Label(vd.ui.keyWin,    "");
        vd.ui.dbLst  = new ComboBox(vd.ui.keyWin, {"None"} );
        vd.ui.dbLst->setChangeCallback( dbLstCallback );
        dbLstCallback(true);
        vd.ui.dbLst->setCallback([](int i){                                          // callback for the actual selection
          if(i < vd.ui.dbNames.size() && i!=vd.ui.dbNameIdx){
            vd.ui.dbNameIdx = i;
            vd.ui.dbName = vd.ui.dbNames[i];
            initSimDB(vd.ui.dbName);
            refreshDB(&vd);
            vd.ui.dbLst->setSelectedIndex(i);
          }
          vd.ui.screen.performLayout();
        });
        auto spcr4   = new Label(vd.ui.keyWin, "");
        auto spcr5   = new Label(vd.ui.keyWin, "");
        auto spcr6   = new Label(vd.ui.keyWin, "");

        vd.ui.dbLstIdx = vd.ui.keyWin->childIndex(vd.ui.dbLst);                          // only done once so not a problem even though it is a linear search
        vd.ui.keyWin->setLayout(vd.ui.keyLay);
        vd.ui.dbLst->setSide(Popup::Left);

        Theme* thm = vd.ui.keyWin->theme();
        thm->mTransparent         = v4f( .0f,  .0f,  .0f,    .0f );
        thm->mWindowFillUnfocused = v4f( .2f,  .2f,  .225f,  .3f );
        thm->mWindowFillFocused   = v4f( .3f,  .28f, .275f,  .3f );
      }

      vd.ui.screen.setVisible(true);
      vd.ui.screen.performLayout();
    }
    SECTION(nanovg)
    {
      vd.ui.nvg =  nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
      if(vd.ui.nvg == NULL){
        printf("Could not init nanovg.\n");
        return -1;
      }
      
      //nvgCreateFont(vd.ui.nvg, "sans",      "Roboto-Regular.ttf" );
      //int font = nvgCreateFont(vd.ui.nvg, "sans-bold", "Roboto-Bold.ttf"    );
      int font = nvgCreateFontMem(vd.ui.nvg, "sans", Roboto_Regular_ttf, Roboto_Regular_ttf_len, 0);
      if(font == -1){
        printf("Could not add font bold.\n");
        return -1;
      }
    }

    //str s = "wat";
    //vecstr ts; ts.reserve(5);
    //ts.push_back(s);

    Printf("\n sizeof(KV): %d \n", sizeof(KV) );
    Printf("\n sizeof(tbl::fields): %d \n\n", sizeof(tbl<u8>::fields) );

    tu64 chld;
    chld.push(4);
    chld.push(8);
    chld("wat") = 259;
    i64 wat = chld("wat");

    Printf("\n %d \n", wat);

    //chld.push(.1);
    //chld.push(.2);
    
    //tst("wat") =    84;
    //KV kv      =  tst("child");
    //kv         =  chld;
    tst("child") =  &chld;

    //KV kv        =  tst("child");
    //tf64& chldp  =  kv.operator tf64 &();
    //tf64& chldp  =  kv;
    //tf64* chldp  =  kv.operator tf64 *();

    tst.flatten();
    //tst.shrink_to_fit();
    //KV       kv  =  tst("child");

    //auto      f  =  (tst("child").operator tu64() ).memStart();

    tbl<f32> nonChld;
    nonChld.push(1.0f);
    nonChld.push(0.5f);
    tst(0) = &nonChld;

    //tu64*  chldc  =  tst("child");   // should and does create an error because the table is a child and being cast to a pointer
    //Printf("\n\n %d %d \n\n", (*chldc)[0], (*chldc)[1] );

    tu64  chldc  =  tst("child");
    Printf("\n\n %d %d \n\n", chldc[0], chldc[1] );


    tf32 a = {0.1f, 0.2f, 0.4f, 0.8f, 1.6f}; 
    tf32 b = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    //a.push({0.1f, 0.2f, 0.4f, 0.8f, 1.6f});
    //b.push({1.0f, 2.0f, 3.0f, 4.0f, 5.0f});

    auto c = a * b;
    TO(5,i) Printf("  %f  ", c[i]);

    //auto      f  =  chldc.memStart();
    //tst = chldc;

    //chldc[0] *= 4.f;
    //chldc[1] *= 4.f;

    //tf64* chldp  =  tst("child"); //kv;
    //Printf("\n\n %f %f \n\n", (*chldp)[0], (*chldp)[1] );
    //(*chldp)[0] *= 4.f; 
    //(*chldp)[1] *= 4.f;

    //tf64& chldr  =  tst("child"); //kv;
    //Printf("\n\n %f %f \n\n", chldr[0], chldr[1] );


    //tst = c;
    //tst.flatten();
    
    //tst("child")  =  chld;

    //tst("wat")       =   84;
    //tst("bamf")      =   (u64)36789;
    //tst("skidoosh")  =   (u64)6371;
    //tst("wat")       =   464;
    //tst("luv and peace") = 99;
    //tst.put("squidoosh", (u64)109);
    //tst.put("zzz", (u64)(21) );
    //tst.put("aaa", (u64)(7217) );

    ////tst.expand();
    ////tst.expand();
    ////tst.expand();
    ////tst.expand();
    ////tst.expand();

    //tst.push(82);
    //tst.push(83);
    //tst.push(84);
    //tst.push(85);
    //tst.push(0);
    //tst.push(1);
    //tst.push(6);
    //tst.push(101);
    //tst.push(45);
    //tst.push({0,1,6,101,45});
    //tst.push({0,1,6,101, 45, 86, 87, 33, 45,45,45,45,45,45,24 });
    ////tst.push({0,1,2,3,4,5,6,7,8,9});
    //TO(20,i){ tst.push(i); }
    //TO(12,i){   tst.push(i*i); }
    //FROM(12,i){ tst.push(i*i); }

    //auto tst2 = tst;
    //sort(ALL(tst2));
    //TO(tst2.size(),i){ tst.push( tst2[i] ); }

  }

  while(!glfwWindowShouldClose(vd.win))
  {
    PRINT_GL_ERRORS

    SECTION(time)
    {
      vd.now  = nowd();
      double passed = vd.now - vd.prev;
      vd.prev = vd.now;
      vd.keyRefreshClock += passed;
      vd.verRefreshClock += passed;

      vd.t     = glfwGetTime();
      vd.dt    = vd.t - vd.prevt;
      vd.prevt = vd.t;
    }
    SECTION(database)
    {
      if(vd.keyRefreshClock > vd.keyRefresh){
        glfwPollEvents();
        refreshDB(&vd);
      } // end of updates to shapes 
      PRINT_GL_ERRORS
    }
    SECTION(input)
    {
      vd.camera.mouseDelta = vec2(0,0);
      vd.camera.btn2Delta  = vec2(0,0);
      glfwPollEvents();                                             // PollEvents must be done after zeroing out the deltas
      glfwGetWindowSize(vd.win, &vd.ui.w, &vd.ui.h);

      PRINT_GL_ERRORS
    }
    SECTION(OpenGL setup)
    {
      glViewport(0, 0, vd.ui.w, vd.ui.h);

      //glEnable(GL_TEXTURE_2D);
      //glEnable(GL_DEPTH);

      glEnable(GL_DEPTH_TEST);                                   // glDepthFunc(GL_LESS);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      PRINT_GL_ERRORS

      glEnable(GL_BLEND);
      glLineWidth(vd.ui.ptSz);
      glPointSize(vd.ui.ptSz);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClearColor(0.0625, 0.0625, 0.0625, 0.0625);
      PRINT_GL_ERRORS
    }
    SECTION(render shapes)
    {
      const static auto XAXIS = vec3(1.f, 0.f, 0.f);
      const static auto YAXIS = vec3(0.f, 1.f, 0.f);

      mat4  viewProj;
      vec3      camx;    //  =  normalize( vec3(cam.pantfm*vec4(1.f,0,0,1.f)) );              // use a separate transform for panning that is frozen on mouse down so that the panning space won't constantly be changing due to rotation from the lookat function
      vec3      camy;    //  =  normalize( vec3(cam.pantfm*vec4(0,1.f,0,1.f)) );              // why does the transform change though if both the lookat and offset are being translated? are they not translated at the same time?
      SECTION(update camera)
      {
        auto&  cam    =  vd.camera;
        f32    dst    =  cam.dist;

        float   ry    =  -(cam.mouseDelta.x * cam.sensitivity);
        float   rx    =  -(cam.mouseDelta.y * cam.sensitivity);

        mat4    xzmat = rotate(mat4(), ry, YAXIS);
        mat4     ymat = rotate(mat4(), rx, XAXIS);
        vec4        p = vec4(cam.pos, 1.f);
        cam.pos       =  xzmat * p;
        vec4     ypos(0, p.y, dst, 1.f);
        cam.pos.y     = (ymat * ypos).y;

        vec3 lookOfst = normalize(cam.pos-cam.lookAt)*dst;
        cam.pos = cam.lookAt + lookOfst;
        
        camx   =  normalize( vec3(cam.pantfm*vec4(1.f,0,0,1.f)) );              // use a separate transform for panning that is frozen on mouse down so that the panning space won't constantly be changing due to rotation from the lookat function
        camy   =  normalize( vec3(cam.pantfm*vec4(0,1.f,0,1.f)) );              // why does the transform change though, if both the lookat and offset are being translated? are they not translated at the same time?

        vec3  ofst(0,0,0); 

        ofst        +=  camx *  -cam.btn2Delta.x * cam.pansense * dst;
        ofst        +=  camy *   cam.btn2Delta.y * cam.pansense * dst;
        cam.lookAt  +=  ofst;
        cam.pos     +=  ofst;
        
        viewProj     =  camera_to_mat4(cam, (f32)vd.ui.w, (f32)vd.ui.h);
      }
      SECTION(draw shapes)
      {
        //for(auto& kv : vd.shapes){
        //  kv.second.active = true;
        //}

        for(auto& kv : vd.shapes){
          if(kv.second.active)
            RenderShape(kv.second, viewProj);
        }
      }

      PRINT_GL_ERRORS
    }
    SECTION(table)
    {
      auto w = vd.ui.w - vd.ui.keyWin->width();
      auto h = vd.ui.h - vd.ui.keyWin->height();

      nvgBeginFrame(vd.ui.nvg, vd.ui.w, vd.ui.h, 1.f);             // vd.ui.w / (f32)vd.ui.h);
        //drawTbl(vd.ui.nvg, tst("chld"), (f32)w, (f32)h, 25, 50, 20, 10);
        drawTbl(vd.ui.nvg, tst, (f32)w, (f32)h, 25, 50, 20, 10);
      nvgEndFrame(vd.ui.nvg);
    }
    SECTION(nanogui)
    {
      if(vd.ui.keyWin->focused()){
        vd.camera.leftButtonDown  = false;
        vd.camera.rightButtonDown = false;
      }

      v2i   winsz = vd.ui.keyWin->size();
      v2i keyspos = v2i(vd.ui.screen.width() - winsz.x(), 0);
      vd.ui.keyWin->setPosition(keyspos);
      vd.ui.keyWin->setSize(v2i(winsz.x(), vd.ui.screen.height()));

      vd.ui.screen.drawContents();
      vd.ui.screen.drawWidgets();
    }
    SECTION(nanovg | tbl visualization, frames per second, color under cursor) 
    {
      nvgBeginFrame(vd.ui.nvg, vd.ui.w, vd.ui.h, 1.f); // vd.ui.w/(f32)vd.ui.h);
        f32  tb = 0;
        char nvgStr[TITLE_MAX_LEN];

        SECTION(frames per second)
        {
          vd.avgFps *= 0.9;
          vd.avgFps += (1.0 / vd.dt)*0.1;

          sprintf(nvgStr, "%04.01f", vd.avgFps);

          tb = nvgTextBounds(vd.ui.nvg, 0, 0, nvgStr, NULL, NULL);
          nvgFontSize(vd.ui.nvg, vd.ui.hudSz);
          nvgFontFace(vd.ui.nvg, "sans");
          nvgTextAlign(vd.ui.nvg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);  // NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
          nvgFillColor(vd.ui.nvg, nvgRGBA(255, 255, 255, 255));
          nvgText(vd.ui.nvg, 15.f, vd.ui.hudSz, nvgStr, NULL);
        }
        SECTION(rgb under cursor)
        {
          auto rgb = vd.mouseRGB;
          sprintf(nvgStr, "%.2f  %.2f  %.2f", rgb[0], rgb[1], rgb[2]);
          nvgFontSize(vd.ui.nvg, vd.ui.hudSz);
          nvgFontFace(vd.ui.nvg, "sans");
          nvgTextAlign(vd.ui.nvg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
          nvgFillColor(vd.ui.nvg, nvgRGBA(255, 255, 255, 255));
          nvgText(vd.ui.nvg, tb + 45.f, vd.ui.hudSz, nvgStr, NULL);
        }

        vd.ui.showGuide = false;
        if(vd.ui.showGuide){
          SECTION(hotkey guide)
          {
            nvgFontSize(vd.ui.nvg, vd.ui.guideSz);
            nvgFontFace(vd.ui.nvg, "sans");
            nvgTextAlign(vd.ui.nvg, NVG_ALIGN_LEFT ); // | NVG_ALIGN_MIDDLE);
            nvgFillColor(vd.ui.nvg, nvgRGBA(255, 255, 255, 235));
            nvgText(vd.ui.nvg, 100, 100,                   hotkeyGuideH,    NULL);
            nvgText(vd.ui.nvg, 100, 100 + vd.ui.guideSz*2, hotkeyGuideF,    NULL);
            nvgText(vd.ui.nvg, 100, 100 + vd.ui.guideSz*4, hotkeyGuidePage, NULL);
          }
        }
      nvgEndFrame(vd.ui.nvg);
    }

    glfwSwapBuffers(vd.win);
    PRINT_GL_ERRORS

    glReadPixels( (GLint)(vd.camera.oldMousePos.x), (GLint)(vd.camera.oldMousePos.y), 1, 1, GL_RGB, GL_FLOAT, vd.mouseRGB);
  }

  nanogui::shutdown();
  glfwTerminate();

  //int a; cin >> a;

  return 0;
}








//nvgBeginPath(nvg);
//  nvgRect(nvg, b.xmn, b.ymn, b.xmx-b.xmn, b.ymx-b.ymn);
//  nvgStrokeWidth(nvg, 1.f);
//  nvgStrokeColor(nvg, nvgRGBAf(0,.25f,.8f, 1.f));
//nvgStroke(nvg);
//
//nvgMoveTo(nvg, x, y);
//nvgLineTo(nvg, x, y - h*(t[i]/mxf) );

//char s[TITLE_MAX_LEN]; 
//
//vecstr labels;  labels.reserve(5);
//vecv2   ofsts;  ofsts.reserve(5);
//tbl<v2>   ofsts;  ofsts.reserve(5);
//
//nvgBeginPath(nvg);
//  nvgRect(nvg, x, o.y-margin, w-x-margin*2, 1.f);
//  nvgStrokeWidth(nvg, 1.f);
//  nvgStrokeColor(nvg, nvgRGBAf(0,.25f,.8f, 1.f));
//nvgStroke(nvg);
//
//nvgBeginPath(nvg);
//  nvgRect(nvg, o.x-margin, o.y-margin, w-x-margin*2, 1.f);
//  nvgStrokeWidth(nvg, 1.f);
//  nvgStrokeColor(nvg, nvgRGBAf(0,.25f,.8f, 1.f));
//nvgStroke(nvg);

//f32       drawStrs(NVGcontext* nvg, vecstr const& strs, vecv2 ofsts, f32 w, f32 x, f32 y, f32 margin)
//f32       drawStrs(NVGcontext* nvg, vecstr const& strs, tblv2 const& ofsts, f32 w, f32 x, f32 y, f32 margin)

//tst("skidoosh") =   109l;
//tst.put("wat",       84l);
//tst.put("bamf",   36789l);
//tst("bamf")     =  (u64)36789;

//str s = "wat";
//vecstr ts; ts.reserve(5);
//ts.push_back(s);

//v2      drawU64(NVGcontext* nvg, const char* label, u64 n, f32 x, f32 y, f32 sz, f32 margin)
//{
//  char s[TITLE_MAX_LEN]; f32 bnds[4];
//
//  sprintf(s, "%s:   %lu", label, (unsigned long)n );
//  f32 m2 = margin * 2;
//  f32 xx = x + m2;
//  f32 yy = y + m2;
//  nvgText(nvg, xx,yy, s, NULL);
//  nvgTextBounds(nvg, xx, yy, s, NULL, bnds);
//  //TO(4,i){ bnds[i] += margin; }
//  drawBnd(nvg, bnds);
//
//  v2 ofst = { bnds[2]-bnds[0] + m2, bnds[3]-bnds[1] + m2 };
//  return ofst;
//}

//bnd = { o.x+margin, o.y+ofsts[i].y+margin, o.x+ofsts[i].x+margin, o.y+margin };                                       //  o.x + margin * 2, o.y + margin * 2 };
//drawBnd(nvg, bnd.b);

//o.x = 0;
//TO(t.size(),i){
//  sprintf(s, "%lu", (unsigned long)i); //(unsigned long)t[i] );
//  o.x += drawU64(nvg, s, (u64)t[i], x+o.x, y+o.y, sz, margin).x;
//}

//void   drawBnd(NVGcontext* nvg, f32 b[4], f32 margin)
//
//nvgRect(nvg, b[0]-margin, b[1]-margin, b[2]-b[0] + margin*2.f, b[3]-b[1] + margin*2.f);

//v2 o = { 0, mxY+margin*2.f };                                                                   // o is offset
//o.x += drawU64(nvg, szbytes.c_str(), t.sizeBytes(), x + xo, y, sz, margin).x;
//o.x += drawU64(nvg, "sizeBytes",    t.sizeBytes(),    x+xo,   y,   sz, margin).x;
//o.x += drawU64(nvg, "capacity",     t.capacity(),     x+o.x,  y,   sz, margin).x;
//o.x += drawU64(nvg, "size",         t.size(),         x+o.x,  y,   sz, margin).x;
//o.x += drawU64(nvg, "map elems",    t.elems(),        x+o.x,  y,   sz, margin).x;
//o   += drawU64(nvg, "map capacity", t.map_capacity(), x+o.x,  y,   sz, margin);
//
//o.y += margin;

//auto e = t.elemStart();
//o.x = 0;
//f32   yofst = 0;
//auto mapcap = t.map_capacity();
//TO(mapcap, i) if(!e[i].isEmpty()) {
//  v2 ofst = drawU64(nvg, e[i].key, e[i].val, x + o.x, y + o.y, sz, margin);
//  o.x    += ofst.x;
//  yofst   = ofst.y;
//}
//o.y += yofst + margin;
//
//auto e = t.elemStart();
//o.x         = 0;
//f32   yofst = 0;
//auto mapcap = t.map_capacity();
//TO(mapcap, i) if(!e[i].isEmpty()){
//  v2 ofst = drawU64(nvg, e[i].key, e[i].val, x+o.x, y+o.y, sz, margin);
//  o.x  += ofst.x;
//  yofst = ofst.y;
//}
//o.y += yofst + margin;

//o.y += yofst + margin;
//o += drawU64(nvg, e[mapcap-1].key, e[mapcap-1].val, x + o.x, y + o.y, sz, margin);

//sprintf(s, "sizeBytes:   %lu", (unsigned long)t.sizeBytes());
//nvgText(nvg, x+xo, y+yo, s, NULL);
//nvgTextBounds(nvg, x + xo, y + yo, s, NULL, bnds);
//drawBnd(nvg, bnds, margin);
