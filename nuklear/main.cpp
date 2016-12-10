
// TODO: Add all attributes
// TODO: Control camera with mouse

#include <algorithm>
#include <fstream>
#include <sstream>

//#include "../DataStore/ConcurrentMap/simdb.hpp"
//#include "IndexedVerts.h"

#include "no_rt_util.h"

#include <GL/glew.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glfw3.h"

#ifdef _MSC_VER
  #pragma comment(lib, "glfw3dll.lib")
  #pragma comment(lib, "glew32.lib")
#endif

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

#include "VizDataStructures.hpp"
#include "VizGenerators.hpp"
#include "VizTransforms.hpp"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define MAX_VERTEX_BUFFER  512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#define UNUSED(a) (void)a
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#define LEN(a) (sizeof(a)/sizeof(a)[0])

//vec<ui8> makeTriangle(size_t& byteLen, bool left);
//vec<ui8> makeCube(size_t& byteLen);

//int sidebar(struct nk_context *ctx, int width, int height, vec<Key>& keys, const vec<str>& dbKeys);
//
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


static void error_callback(int e, const char *d) {
    printf("Error %d: %s\n", e, d);
}

void   key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void   genTestGeo(simdb* db)
{
  // Create serialized IndexedVerts
  size_t leftLen, rightLen, cubeLen;
  vec<ui8>  leftData = makeTriangle(leftLen,   true);
  vec<ui8> rightData = makeTriangle(rightLen, false);
  vec<ui8>  cubeData = makeCube(cubeLen);

  // Store serialized IndexedVerts in the db
  str  leftTriangle = "leftTriangle";       // todo: make simdb put() that takes a const char* as key
  str rightTriangle = "rightTriangle";
  str          cube = "cube";

  db->put(leftTriangle.data(), (ui32)leftTriangle.length(), leftData.data(), (ui32)leftLen);
  db->put(rightTriangle.data(), (ui32)rightTriangle.length(), rightData.data(), (ui32)rightLen);
  db->put(cube.data(), (ui32)cube.length(), cubeData.data(), (ui32)cubeLen);
}

int    sidebar(struct nk_context *ctx, VizData* vd)
{
  using namespace std;

  static enum nk_style_header_align header_align = NK_HEADER_RIGHT;
  static nk_flags window_flags = 0;                /* window flags */

  struct nk_panel layout;                          /* popups */

  ctx->style.window.header.align = header_align;   /* window flags */

  struct nk_rect rect = nk_rect( (5/6.0f) * (float)vd->ui.w, 
                                                          0, 
                                 (1/6.0f) * (float)vd->ui.w, 
                                            (float)vd->ui.h  );
  if(nk_begin(ctx, &layout, "Overview", rect, window_flags))
  {
    if(nk_tree_push(ctx, NK_TREE_TAB, "Drawables", NK_MINIMIZED))
    {
      nk_layout_row_static(ctx, 18, 100, 1);
      for(auto& kv : vd->shapes){          
        nk_selectable_label(ctx, kv.first.c_str(), NK_TEXT_LEFT, &kv.second.active);
      }
      nk_tree_pop(ctx);
    }
  }
  nk_end(ctx);

  return !nk_window_is_closed(ctx, "Overview");
}

void   RenderShape(Shape const& shp) // GLuint shaderId)
{
  // Camera/View transformation
  glUseProgram(shp.shader);  //shader.use();
  glm::mat4 transform;
  transform = glm::rotate(transform, (GLfloat)glfwGetTime() * 50.0f, glm::vec3(0.2f, 0.5f, 1.0f));
  GLint transformLoc = glGetUniformLocation(shp.shader, "transform");
  glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

  // Render
  int size;
  glBindVertexArray(shp.vertary);
  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
  glDrawElements(GL_TRIANGLES, size/sizeof(uint32_t), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

int    main(void)
{
  using namespace std;

  static GLFWwindow *win;              /* Platform */    //int width = 0, height = 0;
  struct nk_context *ctx;
  struct nk_color background;

  VizData vd;
  vd.ui.w = 1024; 
  vd.ui.h =  768;
  simdb   db("test", 1024, 8);        // Create the DB
  genTestGeo(&db);

  /* GLFW */
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
      fprintf(stdout, "[GFLW] failed to init!\n");
      exit(1);
  }

  win = glfwCreateWindow(vd.ui.w, vd.ui.h, "Demo", NULL, NULL);
  glfwMakeContextCurrent(win);
  glfwGetWindowSize(win, &vd.ui.w, &vd.ui.h);

  /* OpenGL */
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glewExperimental = 1;
  if (glewInit() != GLEW_OK) {
      fprintf(stderr, "Failed to setup GLEW\n");
      exit(1);
  }

  glfwSetKeyCallback(win, key_callback);

  /* nuklear */
  ctx = nk_glfw3_init(win, NK_GLFW3_INSTALL_CALLBACKS);
  struct nk_font_atlas *atlas;
  nk_glfw3_font_stash_begin(&atlas);
  nk_glfw3_font_stash_end();

  glEnable(GL_DEPTH_TEST);
  // glDepthFunc(GL_LESS);

  // todo: need simdb::VerIdx and simdb::VerKey structs ? need simdb.getVersion() ?
  vec<str> dbKeys = db.getKeyStrs();    // Get all keys in DB - this will need to be ran in the main loop, but not every frame

  GLuint shaderId = shadersrc_to_shaderid(vertShader, fragShader);  
  for(auto& k : dbKeys){
    ui32 vlen = 0;
    auto  len = db.len(k.data(), (ui32)k.length(), &vlen);          // todo: make ui64 as the input length

    vec<i8> ivbuf(vlen);
    db.get(k.data(), (ui32)k.length(), ivbuf.data(), (ui32)len);

    Shape s  = ivbuf_to_shape(ivbuf.data(), len);
    s.shader = shaderId;
    vd.shapes[k] = move(s);
  };

  background = nk_rgb(28,48,62);
  while(!glfwWindowShouldClose(win))
  {
    /* Input */
    glfwPollEvents();
    nk_glfw3_new_frame();

    sidebar(ctx, &vd);        // TODO(Chris): Resize sidebar on window resize  //keys, dbKeys);

    /* Draw */
    {
      float bg[4];
      nk_color_fv(bg, background);
      glfwGetWindowSize(win, &vd.ui.w, &vd.ui.h);
      glViewport(0, 0, vd.ui.w, vd.ui.h);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClearColor(bg[0], bg[1], bg[2], bg[3]);

      for(auto& kv : vd.shapes){
        if(kv.second.active)
          RenderShape( kv.second );
      }

      /* IMPORTANT: `nk_glfw_render` modifies some global OpenGL state
        * with blending, scissor, face culling, depth test and viewport and
        * defaults everything back into a default state.
        * Make sure to either a.) save and restore or b.) reset your own state after
        * rendering the UI. */
      nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
      glfwSwapBuffers(win);
    }
  }
  nk_glfw3_shutdown();
  glfwTerminate();
  return 0;
}





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
