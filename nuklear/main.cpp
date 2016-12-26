
// -TODO(Chris): Resize sidebar on window resize
// -todo: fix sidebar not changing with window resize
// -todo: put GLFW window pointer and nuklear context pointer into VizData
// -todo: remove shapes when keys are missing
// -todo: will need to remove keys that aren't in VizData too
// -todo: need simdb.getVersion() - using VerStr for versioned strings
// -todo: need simdb::VerIdx and simdb::VerKey structs
// -todo: make keys check for version before get()
// -todo: add timed key update in main loop
// -todo: add const char* key get() to simdb
// -todo: add timed key version check 
// -todo: figure out why empty string is making it into the db - not making it in, but nxtKey doesn't 
// -todo: check if timing is behind more than 2 updates and wipe out the extra
// -todo: test with updating geometry from separate process
// -TODO: Control camera with mouse
// -todo: make updates by all keys respect previous visibility
// -todo: make IndexedVerts single header with split declaration and implementation sections
// -todo: get working uvs and images
// -TODO: Add all attributes
// -todo: get single pixel images working with compositing
// -todo: figure out why tri key disapears  when using gradient - seems like the number of blocks searched was not doing signed math and was returning a bogus large number, meaning that on wrap around, the number searched would go over the number of blocks even if few blocks had actually been searched
//       - cube, and tri keys 
//       - leftTriangle also dissapears when using original generators
//       - if all the keys shows up the first time, why would one not show up the second time?
//       - off by one error in getKeyStrs() when it loops around?
//       - leftTriangle's return from put() is 0 - this is the block index, not the index in the hash map
// -todo: fix key strings being cut off before the end of the nuklear sidebar - larger width on nk_layout_row_static doesn't cut off the text, but it adds a horizontal scroll bar and clicking keys no longer works - rect.w-25.f seems to work
// -todo: fix camera rotation resetting on mouse down
// -todo: make wrapAngle use modulo operator so that remainder is kept and the angle is not clamped - 2*PI stored as const static, fmodf used to get modulo of two floats, giving the remainer
// -todo: get visualizer compiling on osx

// todo: get visualizer running without segfault on osx
// todo: get simdb working on osx - will have to use mmap(SHARED)
// todo: take camera position directly out of the transformation matrix
// todo: make mouse delta a per frame change and not an accumulated changed
// todo: separate mouse delta and camera sensitivity 
// todo: take camera functions out of render shape
// todo: fix Y switching rotation if camera is rotated 180 degrees with X - Y rotation needs to be relative to camera position
// todo: add panning to right mouse button
// todo: rename VizDataStructures to just VizData
// todo: add fps counter in the corner
// todo: add color under cursor like previous visualizer - use the gl get frame like the previous visualizer - check if the cursor is over the gl window first as an optimization? - sort of in but not working
// todo: move and rename project to LavaViz or any non test name

// idea: make IndexedVerts restore without copying bytes? - this would mean that there would need to be a pointer to the head and each member would be pointers filled in on deserialization? 
// idea: call getKeys asynchronously - use futures? 
// idea: put version next to key value 
// idea: test indexed verts with images
// idea: ability to save indexed verts objects to a file
// idea: look into drag and drop to load indexed verts objects by dragging from a file
// idea: make a text visualizer?
//       - display keys and data from db directly - keys then string on one line, click the line and add a tab with the key name in the tab title and a split window between hex and string form?
//       - display values and strings
// idea: make a strings binary format - will this work for an arbitrary packed binary list? - should there be a data structure type and an underlying data type?
//       - first 8 bytes -> total size in bytes
//       - next  4 bytes  -> data structure type? - binary list here?
//       - next  4 bytes  -> underlying data type?
//       - next  8 bytes  -> number of strings
//       - for the number of strings -> 8 bytes for the offset of each string from the start of the whole binary 
//       - next bytes are all string data - does the encoding matter here? should it be utf8 since you know the length of each string?
// idea: work out a type enum for lava data structures? use the upper 16 bits of the size? this leaves 'only' 65,536 different types and 281 terabytes as the max size - use first 8 bytes for size and next 8 bytes for version?
// idea: ability to display points of an indexed verts type as numbers - this would give the ability to have numbers floating in space

#include <chrono>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "GL/glew.h"
#include "no_rt_util.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/constants.hpp"
#include "glfw3.h"

#include "VizDataStructures.hpp"
#include "VizGenerators.hpp"
#include "VizTransforms.hpp"

#define ENTRY_DECLARATION int main(void)
#ifdef _MSC_VER
  #pragma comment(lib, "glfw3dll.lib")
  #pragma comment(lib, "glew32.lib")

  #define USE_CONSOLE                                 // turning this off will use the windows subsystem in the linker and change the entry point to WinMain() so that no command line/console will appear
  #ifndef USE_CONSOLE
    #pragma comment( linker, "/subsystem:windows" )
    #undef ENTRY_DECLARATION
    #define ENTRY_DECLARATION int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
  #endif
#endif

#define MAX_VERTEX_BUFFER  512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

namespace {

//float wrapAngleRadians(float angle)     
//{
//  using namespace glm;
//  float wrappedAngle = 0;
//
//  if(angle > 2 * pi<float>())
//  {
//    wrappedAngle = 0;
//  }
//  else if(angle < 0)
//  {
//    wrappedAngle = 2 * pi<float>();
//  }
//  else
//  {
//    wrappedAngle = angle;
//  }
//
//  return wrappedAngle;
//}
float wrapAngleRadians(float angle)     
{
  using namespace glm;
  const static float _2PI = 2.f*pi<float>();

  return fmodf(angle, _2PI);
}
void     errorCallback(int e, const char *d) {
    printf("Error %d: %s\n", e, d);
}
void       keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}
void    scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
  using namespace std;
  
  VizData* vd = (VizData*)glfwGetWindowUserPointer(window);
  vd->camera.fieldOfView += (GLfloat)(yoffset / 10); 
  vd->camera.fieldOfView  = max(45.f, min(90.f, vd->camera.fieldOfView));

  //if(vd->camera.fieldOfView < 45.0f) {
  //  vd->camera.fieldOfView = 45.0f;
  //}
  //
  //if(vd->camera.fieldOfView > 90.0f) {
  //  vd->camera.fieldOfView = 90.0f;
  //}
}
void  mouseBtnCallback(GLFWwindow* window, int button, int action, int mods)
{
  VizData* vd = (VizData*)glfwGetWindowUserPointer(window);

  if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
  {
    vd->camera.leftButtonDown = true;
  }
  if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
  {
    vd->camera.leftButtonDown = false;
  }
}
void cursorPosCallback(GLFWwindow* window, double xposition, double yposition)
{
  using namespace glm;
  const static float _2PI = 2.f*pi<float>();

  VizData* vd = (VizData*)glfwGetWindowUserPointer(window);
  vec2 newMousePosition = vec2((float)xposition, (float)yposition);

  if(vd->camera.leftButtonDown){
    vd->camera.mouseDelta += (newMousePosition - vd->camera.oldMousePos) * vd->camera.sensitivity;
  }
  vd->camera.mouseDelta.x = fmodf(vd->camera.mouseDelta.x, _2PI);
  vd->camera.mouseDelta.y = fmodf(vd->camera.mouseDelta.y, _2PI);

  //vd->camera.mouseDelta.x = wrapAngleRadians(vd->camera.mouseDelta.x);
  //vd->camera.mouseDelta.y = wrapAngleRadians(vd->camera.mouseDelta.y);

  vd->camera.oldMousePos = newMousePosition;
}
int           sidebar(struct nk_context *ctx, struct nk_rect rect, KeyShapes* shps) // VizData* vd)
{
  using namespace std;

  nk_flags window_flags = 0;                 /* window flags */
  struct nk_panel layout;                    /* popups */

  ctx->style.window.header.align = NK_HEADER_RIGHT;   // header_align;   /* window flags */

  if(ctx->active){
    memcpy( &ctx->active->bounds, &rect, sizeof(struct nk_rect) );
  }

  if(nk_begin(ctx, &layout, "Overview", rect, window_flags))
  {
      nk_layout_row_static(ctx, 18, (int)(rect.w-25.f), 1);
      //nk_layout_row_static(ctx, 18, 120, 1);
      for(auto& kv : *shps){
        if(kv.first.s.length()>0)
          nk_selectable_label(ctx, kv.first.s.c_str(), NK_TEXT_LEFT, &kv.second.active);
      }
  }
  nk_end(ctx);

  return !nk_window_is_closed(ctx, "Overview");
}
void      RenderShape(Shape const& shp) // GLuint shaderId)
{
  using namespace glm;
  const static auto XAXIS = vec4(1.f, 0.f, 0.f, 1.f);
  const static auto YAXIS = vec4(0.f, 1.f, 0.f, 1.f);

  glUseProgram(shp.shader);  //shader.use();

  mat4 view;
  view = lookAt(vd.camera.position, vd.camera.viewDirection, vd.camera.up);

  mat4 projection;
  projection = perspective((float)vd.camera.fieldOfView, (GLfloat)1024 / (GLfloat)768, 0.1f, 100.0f);

  // todo: here the coordinate system of the camera will need to be used for the axises 
  // - Z will be easy, it is the vector pointing at the origin (normalized)  
  // X and Y will be (I think) the X and Y vectors each multiplied by the camera's current transformation matrix. 
  // this will create camera transforms always relative to the camera and will hopefully give elegant rotation control
  vd.camera.transformMtx = projection * view;
  vd.camera.transformMtx = rotate(vd.camera.transformMtx, vd.camera.mouseDelta.x, vec3(YAXIS*view) );
  vd.camera.transformMtx = rotate(vd.camera.transformMtx, vd.camera.mouseDelta.y, vec3(XAXIS*view) ); //vec3(1.0f, 0.0f, 0.0f));

  GLint transformLoc = glGetUniformLocation(vd.shaderId, "transform");
  glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(vd.camera.transformMtx));

  int size;
  glActiveTexture(GL_TEXTURE0);	// Activate the texture unit first before binding texture
  glBindTexture(GL_TEXTURE_2D, shp.tx);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_LINEAR);

  auto loc = glGetUniformLocation(shp.shader, "tex0");
  glUniform1i(loc, 0);

  glBindVertexArray(shp.vertary);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shp.idxbuf);
  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);   // todo: keep size with shape instead of querying it
  glDrawElements(shp.mode, shp.indsz, GL_UNSIGNED_INT, 0);

  //glDrawElements(shp.mode, size/sizeof(uint32_t), GL_UNSIGNED_INT, 0);
  //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}
//void updateCamera(VizData& vd)
//{
//  using namespace glm;
//  glUseProgram(vd.shaderId);
//
//  mat4 view;
//  view = lookAt(vd.camera.position, vd.camera.viewDirection, vd.camera.up);
//
//  mat4 projection;
//  projection = perspective((float)vd.camera.fieldOfView, (GLfloat)1024 / (GLfloat)768, 0.1f, 100.0f);
//
//  vd.camera.transformMtx = projection * view;
//  vd.camera.transformMtx = rotate(vd.camera.transformMtx, vd.camera.mouseDelta.x, vec3(0.0f, 1.0f, 0.0f));
//  vd.camera.transformMtx = rotate(vd.camera.transformMtx, vd.camera.mouseDelta.y, vec3(1.0f, 0.0f, 0.0f));
//
//  GLint transformLoc = glGetUniformLocation(vd.shaderId, "transform");
//  glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(vd.camera.transformMtx));
//}
GLFWwindow*  initGLFW(VizData* vd)
{
  glfwSetErrorCallback(errorCallback);
  if( !glfwInit() ){
    fprintf(stdout, "[GFLW] failed to init!\n");
    exit(1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow* win = glfwCreateWindow(vd->ui.w, vd->ui.h, "Demo", NULL, NULL);    assert(win!=nullptr);
  glfwMakeContextCurrent(win);
  glfwGetWindowSize(win, &vd->ui.w, &vd->ui.h);

  glfwSetKeyCallback(win, keyCallback);
  glfwSetScrollCallback(win, scrollCallback);
  glfwSetCursorPosCallback(win, cursorPosCallback);
  glfwSetMouseButtonCallback(win, mouseBtnCallback);

  return win;
}
void         initGlew()
{
  //glewExperimental = 1;
  if(glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to setup GLEW\n");
    exit(1);
  }
}
auto      initNuklear(GLFWwindow* win) -> struct nk_context*
{
  struct nk_context*      ctx = nk_glfw3_init(win, NK_GLFW3_INSTALL_CALLBACKS);
  struct nk_font_atlas* atlas;
  nk_glfw3_font_stash_begin(&atlas);
  nk_glfw3_font_stash_end();

  return ctx;
}

void   shapesFromKeys(simdb const& db, vec<VerStr> const& dbKeys, VizData* vd)  // vec<str> const& dbKeys
{
  using namespace std;

  vd->shaderId = shadersrc_to_shaderid(vertShader, fragShader);
  for(auto& k : dbKeys)
  {
    auto cur = vd->shapes.find(k);
    if(cur!=vd->shapes.end() && cur->first.v==k.v) continue;

    ui32    vlen = 0;
    ui32 version = 0;
    auto len = db.len(k.s.data(), (ui32)k.s.length(), &vlen, &version);          // todo: make ui64 as the input length

    vec<i8> ivbuf(vlen);
    db.get(k.s.data(), (ui32)k.s.length(), ivbuf.data(), (ui32)ivbuf.size());

    Shape  s = ivbuf_to_shape(ivbuf.data(), len);
    s.shader = vd->shaderId;
    s.active = vd->shapes[k].active;
    vd->shapes[k] = move(s);
  };
}
int  eraseMissingKeys(vec<VerStr> dbKeys, KeyShapes* shps)           // vec<str> dbKeys,
{

  int cnt = 0;
  sort( ALL(dbKeys) );
  //assert( shps->size() == 0 || dbKeys.size() != 0 );
  if( shps->size() > 0 && dbKeys.size() == 0 )
    printf("wat");

  for(auto it = shps->begin(); it != shps->end(); )
  {
    auto const& kv = *it;
    if( !binary_search(ALL(dbKeys),kv.first) ){
      it = shps->erase(it);
      ++cnt;
    }else ++it;
  }

  return cnt;
}
bool        updateKey(simdb const& db, VerStr const& key, VizData* vd)
{
  using namespace std;

  ui32 version=0;
  ui32    vlen=0;
  auto len = db.len(key.s.data(), (ui32)key.s.length(), &vlen, &version);

  if(len>0 && version!=key.v){
    vec<i8> ivbuf(vlen);
    db.get(key.s.data(), (ui32)key.s.length(), ivbuf.data(), (ui32)ivbuf.size());

    Shape  s = ivbuf_to_shape(ivbuf.data(), len);
    s.shader = vd->shaderId;
    s.active = true;                     // because updates only happen when the shape is active, always setting an updated shape to active should work
    vd->shapes[key] = move(s);
    
    return true;
  }

  return false;
}

double           nowd()
{
  using namespace std;
  using namespace std::chrono;

  auto nano = duration_cast<nanoseconds>( chrono::high_resolution_clock::now().time_since_epoch() );

  return nano.count() / 1000000000.0; 
}

}

void       genTestGeo(simdb* db)
{
  using namespace std;
  
  // Create serialized IndexedVerts
  size_t leftLen, rightLen, cubeLen;
  vec<ui8>  leftData = makeTriangle(leftLen, true);
  vec<ui8> rightData = makeTriangle(rightLen, false);
  vec<ui8>  cubeData = makeCube(cubeLen);

  // Store serialized IndexedVerts in the db
  str  leftTriangle = "leftTriangle";
  str rightTriangle = "rightTriangle";
  str          cube = "cube";

  db->put(leftTriangle, leftData);
  db->put(rightTriangle, rightData);
  db->put(cube, cubeData);
}

ENTRY_DECLARATION
{
  using namespace std;

  SECTION(initialize static simdb and static VizData)
  {
    using namespace glm;

    new (&db) simdb("test", 1024, 1<<10);        // inititialize the DB with placement new into the data segment

    vd.ui.w         =  1024; 
    vd.ui.h         =   768;
    vd.ui.bgclr     =  nk_rgb(16,16,16);     // darker than this may risk not seeing the difference between black and the background
    vd.now          =  nowd();
    vd.prev         =  vd.now;
    vd.verRefresh   =  0.007;                // roughly 144hz
    vd.verRefreshClock = 0.0;
    vd.keyRefresh             =  2.0;
    vd.keyRefreshClock        = vd.keyRefresh;
    vd.camera.fieldOfView     = 65.0f;
    vd.camera.mouseDelta      = vec2(0.0f, 0.0f);
    vd.camera.sensitivity     = 0.003f;
    vd.camera.position        = vec3(0.0f, 0.0f, 3.0f);
    vd.camera.viewDirection   = vec3(0.0f, 0.0f, 0.0f);
    vd.camera.up              = vec3(0.0f, 1.0f, 0.0f);
    vd.camera.oldMousePos     = vec2(0.0f, 0.0f);
    vd.camera.rightButtonDown = false;
    vd.camera.leftButtonDown  = false;
  }
  //genTestGeo(&db);
  SECTION(initialize glfw window, glew, and nuklear)
  {
    vd.win = initGLFW( &vd );                        assert(vd.win!=nullptr);
    glfwSetWindowUserPointer(vd.win, &vd);
    initGlew();
    vd.ctx = initNuklear(vd.win);                    assert(vd.ctx!=nullptr);
  }

  while(!glfwWindowShouldClose(vd.win))
  {
    SECTION(add the time passed to refresh clocks)
    {
      vd.now  = nowd();
      double passed = vd.now - vd.prev;
      vd.prev = vd.now;
      vd.keyRefreshClock += passed;
      vd.verRefreshClock += passed;
    }
    SECTION(check updates if enough time stored in refresh clock variables)
    {
      if( vd.keyRefreshClock > vd.keyRefresh ){
        //auto dbKeys = db.getKeyStrs();                           // Get all keys in DB - this will need to be ran in the main loop, but not every frame
        //shapesFromKeys(db, dbKeys, &vd);
        //eraseMissingKeys(move(dbKeys), &vd.shapes);

        vd.keyRefreshClock -= vd.keyRefresh;
        vd.verRefreshClock -= vd.verRefresh;

        if(vd.keyRefreshClock > vd.keyRefresh)                 // if there is already enough time saved up for another update make sure that less that two updates worth of time is kept 
          vd.keyRefreshClock = vd.keyRefresh + fmod(vd.keyRefreshClock, vd.keyRefresh);
      }else if( vd.verRefreshClock > vd.verRefresh ){
        for(auto& kv : vd.shapes){
          if(kv.second.active){
            //updateKey(db, kv.first, &vd);
          }
        }
        vd.verRefreshClock -= vd.verRefresh;

        if(vd.verRefreshClock > vd.verRefresh)                 // if there is already enough time saved up for another update make sure that less that two updates worth of time is kept 
          vd.verRefreshClock = vd.verRefresh + fmod(vd.verRefreshClock, vd.verRefresh);
      } // end of updates to shapes 
    }
    SECTION(GLFW input)
    {
      glfwPollEvents();                                             /* Input */
      glfwGetWindowSize(vd.win, &vd.ui.w, &vd.ui.h);
    }
    SECTION(draw nuklear)
    {
      nk_glfw3_new_frame();
      vd.ui.rect = winbnd_to_sidebarRect((float)vd.ui.w, (float)vd.ui.h);
      sidebar(vd.ctx, vd.ui.rect, &vd.shapes);                     // alters the shapes by setting their active flags
    }
    SECTION(openGL frame setup)
    {
      glViewport(0, 0, vd.ui.w, vd.ui.h);
      glEnable(GL_BLEND);
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_DEPTH);
      glEnable(GL_DEPTH_TEST);                                   // glDepthFunc(GL_LESS);
      //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      float bg[4];
      nk_color_fv(bg, vd.ui.bgclr);
      glClearColor(bg[0], bg[1], bg[2], bg[3]);
    }
    SECTION(render the shapes in VizData::shapes)
    {
      // updateCamera(vd);
      for(auto& kv : vd.shapes){
        if(kv.second.active)
          RenderShape( kv.second );
      }
    }

    nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);        
       /* 
        * IMPORTANT: `nk_glfw_render` modifies some global OpenGL state
        * with blending, scissor, face culling, depth test and viewport and
        * defaults everything back into a default state.
        * Make sure to either a.) save and restore or b.) reset your own state after
        * rendering the UI. */
    glfwSwapBuffers(vd.win);

    // todo: mouse position goes here to read back the color under the mouse 
    //glReadPixels( (GLint)(vd.camera.xDiff * vd.ui.w), (GLint)(vd.camera.yDiff * vd.ui.h), 1, 1, GL_RGB, GL_FLOAT, vd.mouseRGB);
    glReadPixels( (GLint)(vd.camera.mouseDelta.x), (GLint)(vd.camera.mouseDelta.y), 1, 1, GL_RGB, GL_FLOAT, vd.mouseRGB);
    //printf("%f %f: %f %f %f \n", vd.camera.xDiff, vd.camera.yDiff, vd.mouseRGB[0], vd.mouseRGB[1], vd.mouseRGB[2]);
  }

  //nk_glfw3_shutdown();
  glfwTerminate();
  return 0;
}






  //for(auto const& kv : *shps){
  //  if( !binary_search(ALL(dbKeys),kv.first) ){
  //    shps->erase(kv.first);
  //    ++cnt;
  //  }
  //}

//#undef  _CONSOLE
//#define _WINDOWS
//#pragma comment( linker, "/subsystem:console" )
//
//int    main(void)
//int CALLBACK WinMain(
//  _In_ HINSTANCE hInstance,
//  _In_ HINSTANCE hPrevInstance,
//  _In_ LPSTR     lpCmdLine,
//  _In_ int       nCmdShow
//)

//void       genTestGeo(simdb* db)
//{
//  // Create serialized IndexedVerts
//  size_t leftLen, rightLen, cubeLen;
//  vec<ui8>  leftData = makeTriangle(leftLen,   true);
//  vec<ui8> rightData = makeTriangle(rightLen, false);
//  vec<ui8>  cubeData = makeCube(cubeLen);
//
//  // Store serialized IndexedVerts in the db
//  str  leftTriangle = "leftTriangle";       // todo: make simdb put() that takes a const char* as key
//  str rightTriangle = "rightTriangle";
//  str          cube = "cube";
//
//  db->put(leftTriangle,   leftData);
//  db->put(rightTriangle, rightData);
//  db->put(cube, cubeData);
//}

//VizData vd;
//simdb   db("test", 1024, 8);        // Create the DB
//genTestGeo(&db);

//db->put(leftTriangle.data(), (ui32)leftTriangle.length(), leftData.data(), (ui32)leftLen);
//db->put(rightTriangle.data(), (ui32)rightTriangle.length(), rightData.data(), (ui32)rightLen);
//db->put(cube.data(), (ui32)cube.length(), cubeData.data(), (ui32)cubeLen);

//SECTION(render the previously generated nuklear gui)
//{

//
//vec<str> dbKeys = db.getKeyStrs();    // Get all keys in DB - this will need to be ran in the main loop, but not every frame

//
//using nk_ctxptr = struct nk_contex*;

//sort( ALL(dbKeys) );
//for(auto kv : vd.shapes){
//  if( binary_search(ALL(dbKeys),kv.first) )
//    vd.shapes.erase(kv.first);
//}

// todo: remove shapes from missing keys
//.begin(), dbKeys.end()
//auto& key = kv.first;

//GLFWwindow*            win;                      /* Platform */    //int width = 0, height = 0;
//struct nk_context*     ctx;

//ctx = nk_glfw3_init(win, NK_GLFW3_INSTALL_CALLBACKS);
//struct nk_font_atlas* atlas;
//nk_glfw3_font_stash_begin(&atlas);
//nk_glfw3_font_stash_end();

//struct nk_color background;
//background = nk_rgb(28,48,62);

//static enum nk_style_header_align header_align = NK_HEADER_RIGHT;
//static nk_flags window_flags = 0;                /* window flags */
//
//struct nk_rect winBnds = {0,0, vd.ui.w, vd.ui.h};
//
//ctx->active->bounds. = (float)vd.ui.h;
//ctx->active->bounds.h = (float)vd.ui.h;
// sb - this is a hack until figuring out why nuklear doesn't take the bounds in the sidebar function
//
//struct nk_rect rect = nk_rect( (5/6.0f) * (float)vd->ui.w, 
//                                                        0, 
//                               (1/6.0f) * (float)vd->ui.w, 
//                                          (float)vd->ui.h  );

//float w = max(192.f, (1/6.f)*(float)vd->ui.w );
//float x = (float)vd->ui.w - w;
//struct nk_rect rect = nk_rect(x, 0, w, (float)vd->ui.h );

//
//auto rect = winbnd_to_sidebarRect((float)vd->ui.w, (float)vd->ui.h);

//#include "../DataStore/ConcurrentMap/simdb.hpp"
//#include "IndexedVerts.h"
//
//#define NK_INCLUDE_FIXED_TYPES
//#define NK_INCLUDE_STANDARD_IO
//#define NK_INCLUDE_STANDARD_VARARGS
//#define NK_INCLUDE_DEFAULT_ALLOCATOR
//#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
//#define NK_INCLUDE_FONT_BAKING
//#define NK_INCLUDE_DEFAULT_FONT
//#define NK_IMPLEMENTATION
//#define NK_GLFW_GL3_IMPLEMENTATION
//#include "nuklear.h"
//#include "nuklear_glfw_gl3.h"

//ctx->active->bounds = {0.f,0.f, (float)vd.ui.w, (float)vd.ui.h};
//if(ctx->current)
//ctx->current->bounds = {0.f,0.f, (float)vd.ui.w, (float)vd.ui.h};

//vd.shaderId = shadersrc_to_shaderid(vertShader, fragShader);  
//for(auto& k : dbKeys){
//  ui32 vlen = 0;
//  auto  len = db.len(k.data(), (ui32)k.length(), &vlen);          // todo: make ui64 as the input length
//
//  vec<i8> ivbuf(vlen);
//  db.get(k.data(), (ui32)k.length(), ivbuf.data(), (ui32)len);
//
//  Shape  s = ivbuf_to_shape(ivbuf.data(), len);
//  s.shader = vd.shaderId;
//  vd.shapes[k] = move(s);
//};

//#define WINDOW_WIDTH 1200
//#define WINDOW_HEIGHT 800

//#define UNUSED(a) (void)a
//#define MIN(a,b) ((a) < (b) ? (a) : (b))
//#define MAX(a,b) ((a) < (b) ? (b) : (a))
//#define LEN(a) (sizeof(a)/sizeof(a)[0])

//vec<ui8> makeTriangle(size_t& byteLen, bool left);
//vec<ui8> makeCube(size_t& byteLen);

//int sidebar(struct nk_context *ctx, int width, int height, vec<Key>& keys, const vec<str>& dbKeys);
//
//void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

//for(auto key : keys) {
//    if(key.active) {
//        key.render();
//    }
//}

//
//sidebar(ctx, width, height, keys, dbKeys);

//sort( dbKeys.begin(), dbKeys.end() );
//vd.keys = dbKeys;

//// Create serialized IndexedVerts
//size_t leftLen, rightLen, cubeLen;
//vec<ui8>  leftData = makeTriangle(leftLen,   true);
//vec<ui8> rightData = makeTriangle(rightLen, false);
//vec<ui8>  cubeData = makeCube(cubeLen);
//
//// Store serialized IndexedVerts in the db
//str  leftTriangle = "leftTriangle";       // todo: make simdb put() that takes a const char* as key
//str rightTriangle = "rightTriangle";
//str          cube = "cube";
//
//db.put(leftTriangle.data(), (ui32)leftTriangle.length(), leftData.data(), (ui32)leftLen);
//db.put(rightTriangle.data(), (ui32)rightTriangle.length(), rightData.data(), (ui32)rightLen);
//db.put(cube.data(), (ui32)cube.length(), cubeData.data(), (ui32)cubeLen);

// Retrieve data from the db and create new Keys with it
//ui32 leftSize, rightSize, cubeSize;
//i64 totalCubeLen = db.len(cube.data(), (ui32)cube.length(), &cubeSize);
//i64 totalLeftLen = db.len(leftTriangle.data(), (ui32)leftTriangle.length(), &leftSize);
//i64 totalRightLen = db.len(rightTriangle.data(), (ui32)rightTriangle.length(), &rightSize);
//
//vec<ui8> retrievedLeftData(leftSize);
//vec<ui8> retrievedRightData(rightSize);
//vec<ui8> retrievedCubeData(cubeSize);
//
//if(!db.get(leftTriangle.data(), (ui32)leftTriangle.length(), retrievedLeftData.data(), leftSize)) {
//    printf("Error reading from db. Key %s does not exist.\n", leftTriangle.c_str());
//}
//if(!db.get(rightTriangle.data(), (ui32)rightTriangle.length(), retrievedRightData.data(), rightSize)) {
//    printf("Error reading from db. Key %s does not exist.\n", rightTriangle.c_str());
//}
//if(!db.get(cube.data(), (ui32)cube.length(), retrievedCubeData.data(), cubeSize)) {
//    printf("Error retrieving value for %s\n", cube.c_str());
//}
//
//// Create the Keys
//Key k1(leftTriangle, retrievedLeftData.data());
//Key k2(rightTriangle, retrievedRightData.data());
//Key k3(cube, retrievedCubeData.data());
//
//vec<Key> keys;
//keys.push_back(k1);
//keys.push_back(k2);
//keys.push_back(k3);

//
// vec<Key>& keys, const vec<str>& dbKeys)

//for(auto& key : keys) {
//     if(find(dbKeys.begin(), dbKeys.end(), key.key) != dbKeys.end()) {
//         nk_selectable_label(ctx, key.key.c_str(), NK_TEXT_LEFT, &key.active);
//         if(!key.active && key.iv) {
//             key.deactivate();
//         }
//     }
// }

//vector<ui8> makeCube(size_t& byteLen) {
//    const unsigned int NUM_VERTICES = 8;
//    const unsigned int NUM_INDICES = 36;
//
//    IndexedVerts* iv = (IndexedVerts*)IndexedVertsCreate(0, 6, IV_TRIANGLES, NUM_VERTICES, NUM_INDICES, 0, 0, 0);
//
//    iv->verts[0] = {
//        {-0.5f, -0.5f, 0.5f},     //pos
//        {0.0f, 0.0f, -1.0f},      //norm
//        {1.0f, 0.0f, 0.0f, 1.0f}, //color
//        {0.0f, 0.0f}              //texCoord
//    };
//    iv->verts[1] = {
//        {0.5f, -0.5f, 0.5f},      //pos
//        {0.0f, 0.0f, -1.0f},      //norm
//        {0.0f, 1.0f, 0.0f, 1.0f}, //color
//        {0.0f, 0.0f}              //texCoord
//    };
//    iv->verts[2] = {
//        {0.5f, 0.5f, 0.5f},       //pos
//        {0.0f, 0.0f, -1.0f},      //norm
//        {0.0f, 0.0f, 1.0f, 1.0f}, //color
//        {0.0f, 0.0f}              //texCoord
//    };
//    iv->verts[3] = {
//        {-0.5f, 0.5f, 0.5f},      //pos
//        {0.0f, 0.0f, -1.0f},      //norm
//        {1.0f, 0.5f, 0.25f, 1.0f}, //color
//        {0.0f, 0.0f}              //texCoord
//    };
//    iv->verts[4] = {
//        {-0.5f, -0.5f, -0.5f},    //pos
//        {0.0f, 0.0f, -1.0f},      //norm
//        {1.0f, 0.0f, 0.0f, 1.0f}, //color
//        {0.0f, 0.0f}              //texCoord
//    };
//    iv->verts[5] = {
//        {0.5f, -0.5f, -0.5f},     //pos
//        {0.0f, 0.0f, -1.0f},      //norm
//        {0.0f, 1.0f, 0.0f, 1.0f}, //color
//        {0.0f, 0.0f}              //texCoord
//    };
//    iv->verts[6] = {
//        {0.5f, 0.5f, -0.5f},      //pos
//        {0.0f, 0.0f, -1.0f},      //norm
//        {0.0f, 0.0f, 1.0f, 1.0f}, //color
//        {0.0f, 0.0f}              //texCoord
//    };
//    iv->verts[7] = {
//        {-0.5f, 0.5f, -0.5f},     //pos
//        {0.0f, 0.0f, -1.0f},      //norm
//        {1.0f, 0.5f, 0.25f, 1.0f}, //color
//        {0.0f, 0.0f}              //texCoord
//    };
//
//    uint32_t cubeIndices[NUM_INDICES] = {
//        // front
//        0, 1, 2,
//        2, 3, 0,
//        // top
//        1, 5, 6,
//        6, 2, 1,
//        // back
//        7, 6, 5,
//        5, 4, 7,
//        // bottom
//        4, 0, 3,
//        3, 7, 4,
//        // left
//        4, 5, 1,
//        1, 0, 4,
//        // right
//        3, 2, 6,
//        6, 7, 3,
//    };
//
//    // Copy index data into IndexedVerts.indices
//    memcpy(iv->indices, cubeIndices, sizeof(GLuint) * NUM_INDICES);
//
//    // Call once to get byteLen
//    IndexedVertsSave(iv, nullptr, &byteLen);
//    vector<ui8> bytes(byteLen);
//    // Call again to serialize
//    IndexedVertsSave(iv, bytes.data(), &byteLen);
//    IndexedVertsDestroy(iv);
//
//    return bytes;
//}
//
//std::vector<ui8> makeTriangle(size_t& byteLen, bool left) {
//    // Create triangle Vertex data
//    const unsigned int NUM_VERTICES = 3;
//    const unsigned int NUM_INDICES = 3;
//
//    IndexedVerts* iv = (IndexedVerts*)IndexedVertsCreate(0, 6, IV_TRIANGLES, NUM_VERTICES, 3, 0, 0, 0);
//
//    if(left) {
//        iv->verts[0] = {
//            {-1.0, -1.0f, 0.0f},     //pos
//            {0.0f, 0.0f, -1.0f},      //norm
//            {1.0f, 1.0f, 1.0f, 1.0f}, //color
//            {0.0f, 0.0f}              //texCoord
//        };
//        iv->verts[1] = {
//            {-0.17f, -1.0f, 0.0f},      //pos
//            {0.0f, 0.0f, -1.0f},      //norm
//            {1.0f, 1.0f, 1.0f, 1.0f}, //color
//            {0.0f, 0.0f}              //texCoord
//        };
//        iv->verts[2] = {
//            {-0.58f, 1.0f, 0.0f},       //pos
//            {0.0f, 0.0f, -1.0f},      //norm
//            {1.0f, 1.0f, 1.0f, 1.0f}, //color
//            {0.0f, 0.0f}              //texCoord
//        };
//    }
//    else {
//        iv->verts[0] = {
//            {-0.17, -1.0f, 0.0f},     //pos
//            {0.0f, 0.0f, -1.0f},      //norm
//            {1.0f, 1.0f, 1.0f, 1.0f}, //color
//            {0.0f, 0.0f}              //texCoord
//        };
//        iv->verts[1] = {
//            {0.66f, -1.0f, 0.0f},      //pos
//            {0.0f, 0.0f, -1.0f},      //norm
//            {1.0f, 1.0f, 1.0f, 1.0f}, //color
//            {0.0f, 0.0f}              //texCoord
//        };
//        iv->verts[2] = {
//            {0.25, 1.0f, 0.0f},       //pos
//            {0.0f, 0.0f, -1.0f},      //norm
//            {1.0f, 1.0f, 1.0f, 1.0f}, //color
//            {0.0f, 0.0f}              //texCoord
//        };
//    }
//
//    GLuint indices[NUM_INDICES] = {0, 1, 2};
//
//    // Copy index data into IndexedVerts.indices
//    memcpy(iv->indices, indices, sizeof(GLuint)* NUM_INDICES);
//
//    // Call once to get byteLen
//    IndexedVertsSave(iv, nullptr, &byteLen);
//    std::vector<ui8> bytes(byteLen);
//    // Call again to serialize
//    IndexedVertsSave(iv, bytes.data(), &byteLen);
//    IndexedVertsDestroy(iv);
//
//    return bytes;
//}

// #include "overview.c"
// #include "node_editor.c"

//int CALLBACK WinMain(
//  _In_ HINSTANCE hInstance,
//  _In_ HINSTANCE hPrevInstance,
//  _In_ LPSTR     lpCmdLine,
//  _In_ int       nCmdShow
//)

/*struct Attributes
{
    GLuint position;
    GLuint normal;
    GLuint sourceColour;
    GLuint textureCoordIn;

    Attributes(Shader& shaderProgram)
    {
        position = createAttribute(shaderProgram, "position");
        normal = createAttribute(shaderProgram, "normal");
        sourceColour = createAttribute(shaderProgram, "sourceColour");
        texureCoordIn = createAttribute(shaderProgram, "texureCoordIn");
    }
    void enable()
    {
        if(position != nullptr)
        {
            glVertexAttribPointer(position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), 0);
            glEnableVertexAttribArray(position->attributeID);
        }

        if(normal != nullptr)
        {
            glVertexAttribPointer(normal->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*)(sizeof (float)* 3));
            glEnableVertexAttribArray(normal->attributeID);
        }

        if(sourceColour != nullptr)
        {
            glVertexAttribPointer(sourceColour->attributeID, 4, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*)(sizeof (float)* 6));
            glEnableVertexAttribArray(sourceColour->attributeID);
        }

        if(texureCoordIn != nullptr)
        {
            glVertexAttribPointer(texureCoordIn->attributeID, 2, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*)(sizeof (float)* 10));
            glEnableVertexAttribArray(texureCoordIn->attributeID);
        }
    }
    void disable()
    {
        if(position != nullptr)       glDisableVertexAttribArray(position->attributeID);
        if(normal != nullptr)         glDisableVertexAttribArray(normal->attributeID);
        if(sourceColour != nullptr)   glDisableVertexAttribArray(sourceColour->attributeID);
        if(texureCoordIn != nullptr)  glDisableVertexAttribArray(texureCoordIn->attributeID);
    }
    // ScopedPointer<OpenGLShaderProgram::Attribute> position, normal, sourceColour, texureCoordIn;

private:
    static GLuint createAttribute(Shader& shader, const char* attributeName)
    {
        if(glGetAttribLocation(shader.id, attributeName) < 0)
            return nullptr;

        return new OpenGLShaderProgram::Attribute(shader, attributeName);
    }
};*/

/*struct VertexBuffer
{
private:
    //bool            m_moved         =   false;
    //GLenum          m_mode          =   GL_POINTS;
    bool            m_owned = true;
    i32             m_numIndices = 0;
    GLenum          m_mode = GL_POINTS;
    GLuint          vertexBuffer = -1;
    GLuint          indexBuffer = -1;
    // OpenGLContext*  openGLContext = nullptr;

    void mv(VertexBuffer&& rval)
    {
        m_numIndices = rval.m_numIndices;
        m_mode = rval.m_mode;
        vertexBuffer = rval.vertexBuffer;
        indexBuffer = rval.indexBuffer;
        // openGLContext = rval.openGLContext;

        rval.m_owned = false;
    }

public:
    VertexBuffer(IndexedVerts* idxVerts) :
        // openGLContext(context),
        m_mode(idxVerts->mode)
    {
        m_numIndices = (i32)idxVerts->indicesLen;

        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

        glBufferData(GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(static_cast<size_t>(idxVerts->vertsLen)*sizeof(Vertex)),
            idxVerts->verts,
            GL_STATIC_DRAW);

        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(static_cast<size_t>(m_numIndices)*sizeof(uint32_t)),
            idxVerts->indices,
            GL_STATIC_DRAW);
    }
    VertexBuffer(std::vector<Vertex> verts, std::vector<int32_t> indices)
    {
        //numIndices = aShape.mesh.indices.size();
        m_numIndices = (i32)indices.size();

        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

        //Array<Vertex> vertices;
        //createVertexListFromMesh (aShape.mesh, vertices, Colours::green);

        glBufferData(GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(static_cast<size_t>(verts.size())*sizeof(Vertex)),
            verts.data(),
            GL_STATIC_DRAW);

        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(static_cast<size_t>(m_numIndices)*sizeof(uint32_t)),
            indices.data(),
            GL_STATIC_DRAW);
    }
    ~VertexBuffer()
    {
        if(m_owned) {
            glDeleteBuffers(1, &vertexBuffer);
            glDeleteBuffers(1, &indexBuffer);
        }
    }

    VertexBuffer(VertexBuffer const& vb) = delete;
    VertexBuffer& operator=(VertexBuffer const& vb) = delete;

    VertexBuffer(VertexBuffer&& vb)
    {
        mv(std::move(vb));
    }
    VertexBuffer& operator=(VertexBuffer&& vb)
    {
        mv(std::move(vb));
        return *this;
        //m_numIndices   =  vb.m_numIndices;
        //vertexBuffer   =  vb.vertexBuffer;
        //indexBuffer    =  vb.indexBuffer;
        //openGLContext  =  vb.openGLContext;
        //
        ////vb.m_moved     =  true;
        //vb.m_owned     =  false;
        //return *this;
    }

    void    bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    }
    i32     numIndices() const
    {
        return m_numIndices;
    }
    GLenum  mode()       const
    {
        return m_mode;
    }
};*/

/*struct Shape
{
private:
    // using ScopedUniform = ScopedPointer<OpenGLShaderProgram::Uniform>;

    bool                m_visible = false;
    std::vector<VertexBuffer>   vertexBuffers;
    GLuint              m_texID = 0;
    GLsizei             w = 0;
    GLsizei             h = 0;
    IndexedVerts*       m_iv;

    void mv(Shape&& rval)
    {
        m_visible = rval.m_visible;
        vertexBuffers = move(rval.vertexBuffers);
        //memcpy(&m_tex, &rval.m_tex, sizeof(decltype(m_tex)) );
        m_texID = rval.m_texID;
        w = rval.w;
        h = rval.h;
        //m_iv           =  rval.m_iv;
    }

public:
    Shape(){}
    Shape(IndexedVerts* iv)
    {
        vertexBuffers.emplace_back(std::move(VertexBuffer(iv)));

        // check for iv having more than 4 dimensions?
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &m_texID);
        glBindTexture(GL_TEXTURE_2D, m_texID);
        glTexImage2D(GL_TEXTURE_2D,
            0, GL_RGBA,
            (GLint)iv->imgWidth, (GLint)iv->imgHeight,
            0, GL_RGBA, GL_FLOAT, iv->pixels);
    }

    Shape(Shape const& s) = delete;
    Shape& operator=(Shape const& s) = delete;

    Shape(Shape&& rval)
    {
        mv(std::move(rval));
    }
    Shape& operator=(Shape&& rval)
    {
        mv(std::move(rval));
        return *this;
    }

    void release()
    {
        //m_tex.release();
        vertexBuffers.clear();
    }
    void draw(Attributes& glAttributes, GLuint shaderHnd)
    {
        if(m_visible)
        {
            for(auto& vb : vertexBuffers)
            {
                //m_tex.bind();

                vb.bind();

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m_texID);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_LINEAR);
                auto loc = glGetUniformLocation(shaderHnd, "tex0");
                glUniform1i(loc, 0);
                glAttributes.enable();
                //glPointSize(1.f);
                glDrawElements(vb.mode(), vb.numIndices(), GL_UNSIGNED_INT, 0);
                glAttributes.disable();

                //m_tex.unbind();
            }
            //glDrawPixels(m_iv->imgWidth, m_iv->imgHeight, GL_RGBA, GL_FLOAT, m_iv->pixels );
        }
    }
    void visible(bool b) { m_visible = b; }
    bool visible() const { return m_visible; }
};*/
