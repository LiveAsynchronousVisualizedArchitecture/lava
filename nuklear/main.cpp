/* nuklear - v1.09 - public domain */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#include "../DataStore/ConcurrentMap/ConcurrentMap.hpp"

#include "no_rt_util.h"

#include <GL/glew.h>
#include "glfw3.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#define UNUSED(a) (void)a
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#define LEN(a) (sizeof(a)/sizeof(a)[0])

// IndexedVerts.h
extern "C"
{
    enum iv_mode  // same as openGL
    {
        IV_POINTS = 0x0000,
        IV_LINES = 0x0001,
        IV_LINE_LOOP = 0x0002,
        IV_LINE_STRIP = 0x0003,
        IV_TRIANGLES = 0x0004,
        IV_TRIANGLE_STRIP = 0x0005,
        IV_TRIANGLE_FAN = 0x0006,
        IV_QUADS = 0x0007,
        IV_QUAD_STRIP = 0x0008,
        IV_POLYGON = 0x0009
    };

    struct Vertex
    {
        float position[3];
        float normal[3];
        float colour[4];
        float texCoord[2];
    };

    struct IndexedVerts
    {
        uint32_t    mode;
        uint32_t    vertsLen;
        uint32_t    indicesLen;
        uint32_t    imgWidth;
        uint32_t    imgHeight;
        uint32_t    imgChans;
        Vertex*     verts;
        uint32_t*   indices;
        float*      pixels;
    };

    /**
    @arg0 constructor mode
    @arg1 uint32_t - number of arguments (3 or 6)
    @arg2 uint32_t - openGL geometry mode
    @arg3 uint32_t - number of vertices
    @arg4 uint32_t - number of indices
    @arg5 uint32_t - image width
    @arg6 uint32_t - image height
    @arg7 uint32_t - image channels

    Memory for the image is ordered
    in rows of rgb pixels.

    a pixel is indexed by:
    pixels[y*imgWidth*imgChans + x*imgChans + channel]
    */
    void* IndexedVertsCreate(uint32_t constructor, uint32_t arglen, ...);
    void* IndexedVertsLoad(void*const bytes, size_t  byteLen);
    void  IndexedVertsSave(void*  ivPtr, void*const bytes, size_t* byteLen);
    void  IndexedVertsDestroy(void*  dataTypePtr);

    inline float* iv_px(IndexedVerts* iv, int64_t x, int64_t y, int64_t c)
    {
        size_t ofst = y*iv->imgWidth*iv->imgChans + x*iv->imgChans + c;
        return iv->pixels + ofst;
    }

} // IndexedVerts.h

void* makeTriangle(float leftX, float rightX, size_t& byteLen);

struct Drawables {
    size_t activeIndx = 0;
    std::vector<IndexedVerts*> shapesVec{2};
    std::vector<void*> bytesVecIn;
    std::vector<size_t> bytesLenVecIn;

    void init(size_t numShapes, float* data) {

        for(int i = 0; i < numShapes; i++) {
            size_t bytesLen;
            void* bytes = makeTriangle(data[i * 2], data[i * 2 + 1], bytesLen);
            bytesVecIn.push_back(bytes);
            bytesLenVecIn.push_back(bytesLen);
        }
    }

    void put(std::string key, simdb* db, size_t index) {
        db->put((void*)key.data(), (uint32_t)key.length(), bytesVecIn[index], (uint32_t)bytesLenVecIn[index]);
    }

    void* get(std::string key, simdb* db, size_t index, size_t& length) {
        void* bytes = malloc(bytesLenVecIn[index]);
        length = db->get(key, bytes);
        return bytes;
    }

    void deserialize(size_t index, void* bytes, size_t size) {
        IndexedVerts* iv = (IndexedVerts*)IndexedVertsLoad(bytes, (size_t)size);
        shapesVec.insert(shapesVec.begin() + index, iv);
    }

    IndexedVerts* getActive() {
        return shapesVec[activeIndx];
    }

    void setActive(size_t index) {
        activeIndx = index;
    }

    void destroy() {
        for(auto* p : shapesVec) {
            delete p;
        }
        shapesVec.clear();
        for(auto* p : bytesVecIn) {
            delete p;
        }
        bytesVecIn.clear();
        bytesLenVecIn.clear();
    }
};

/* ===============================================================
 *
 *                          EXAMPLE
 *
 * ===============================================================*/
/* This are some code examples to provide a small overview of what can be
 * done with this library. To try out an example uncomment the include
 * and the corresponding function. */
// #include "overview.c"
// #include "node_editor.c"

void nuklearDemo(struct nk_context* ctx, Drawables* drawables, GLuint& vertexbuffer);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

/* ===============================================================
 *
 *                          DEMO
 *
 * ===============================================================*/
static void error_callback(int e, const char *d)
{printf("Error %d: %s\n", e, d);}

//int CALLBACK WinMain(
//  _In_ HINSTANCE hInstance,
//  _In_ HINSTANCE hPrevInstance,
//  _In_ LPSTR     lpCmdLine,
//  _In_ int       nCmdShow
//)
int main(void)
{
    /* Platform */
    static GLFWwindow *win;
    int width = 0, height = 0;
    struct nk_context *ctx;
    struct nk_color background;

    /* GLFW */
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        fprintf(stdout, "[GFLW] failed to init!\n");
        exit(1);
    }

    win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Demo", NULL, NULL);
    glfwMakeContextCurrent(win);
    glfwGetWindowSize(win, &width, &height);

    /* OpenGL */
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glewExperimental = 1;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
    }

    glfwSetKeyCallback(win, key_callback);

    /* Vertex Array Object */
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create the DB
    simdb db("test", 200, 200);

    Drawables drawables;
    float shapeData[4] = {-1.0f, 1.0f, -0.25f, 0.25f};
    drawables.init(2, shapeData);

    drawables.put("bigTriangle", &db, (size_t)0);
    drawables.put("skinnyTriangle", &db, (size_t)1);

    size_t sizeBig, sizeSkinny;
    void* bytesFromDBBig = drawables.get("bigTriangle", &db, (size_t)0, sizeBig);
    void* bytesFromDBSkinny = drawables.get("skinnyTriangle", &db, (size_t)1, sizeSkinny);

    // Deserialize IndexedVerts
    drawables.deserialize((size_t)0, bytesFromDBBig, sizeBig);
    drawables.deserialize((size_t)1, bytesFromDBSkinny, sizeSkinny);

    // This will identify our vertex buffer
    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    //glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * iv->vertsLen, &iv->verts[0], GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* drawables.getActive()->vertsLen, &drawables.getActive()->verts[0], GL_STATIC_DRAW);

    /* nuklear */
    ctx = nk_glfw3_init(win, NK_GLFW3_INSTALL_CALLBACKS);
    {
        struct nk_font_atlas *atlas;
        nk_glfw3_font_stash_begin(&atlas);
        nk_glfw3_font_stash_end();
    }

    background = nk_rgb(28,48,62);
    while (!glfwWindowShouldClose(win))
    {
        /* Input */
        glfwPollEvents();
        nk_glfw3_new_frame();

        nuklearDemo(ctx, &drawables, vertexbuffer);

        /* -------------- EXAMPLES ---------------- */
        /*overview(ctx);*/
        /* node_editor(ctx);*/
        /* ----------------------------------------- */

        /* Draw */
        {float bg[4];
        nk_color_fv(bg, background);
        glfwGetWindowSize(win, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(bg[0], bg[1], bg[2], bg[3]);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (void*)0);
        glDrawArrays(drawables.getActive()->mode, 0, 3);
        glDisableVertexAttribArray(0);

        /* IMPORTANT: `nk_glfw_render` modifies some global OpenGL state
         * with blending, scissor, face culling, depth test and viewport and
         * defaults everything back into a default state.
         * Make sure to either a.) save and restore or b.) reset your own state after
         * rendering the UI. */
        nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        glfwSwapBuffers(win);}
    }
    free(bytesFromDBBig);
    free(bytesFromDBSkinny);
    nk_glfw3_shutdown();
    glfwTerminate();
    return 0;
}

void nuklearDemo(struct nk_context* ctx, Drawables* drawables, GLuint& vertexbuffer)
{
    /* GUI */
    {struct nk_panel layout;
    if(nk_begin(ctx, &layout, "Visualizer", nk_rect(50, 50, 230, 250),
        NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
        NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
    {
        enum { FAT, SKINNY };
        static int op = FAT;
        static int property = 20;
        nk_layout_row_static(ctx, 30, 80, 1);

        nk_layout_row_dynamic(ctx, 30, 2);
        if(nk_option_label(ctx, "Fat", op == FAT)) {
            op = FAT;
            drawables->setActive((size_t)0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* drawables->getActive()->vertsLen, &drawables->getActive()->verts[0], GL_STATIC_DRAW);
        }
        if(nk_option_label(ctx, "Skinny", op == SKINNY)) {
            op = SKINNY;
            drawables->setActive((size_t)1);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* drawables->getActive()->vertsLen, &drawables->getActive()->verts[0], GL_STATIC_DRAW);
        }
    }
    nk_end(ctx); }
}

void* makeTriangle(float leftX, float rightX, size_t& byteLen) {
    // Create big triangle Vertex data
    const unsigned int NUM_VERTICES = 3;

    Vertex* verts[NUM_VERTICES];
    verts[0] = new Vertex{
        {leftX, -1.0f, 0.0f},     //pos
        {0.0f, 0.0f, -1.0f},      //norm
        {1.0f, 1.0f, 1.0f, 1.0f}, //color
        {0.0f, 0.0f}              //texCoord
    };
    verts[1] = new Vertex{
        {rightX, -1.0f, 0.0f},      //pos
        {0.0f, 0.0f, -1.0f},      //norm
        {1.0f, 1.0f, 1.0f, 1.0f}, //color
        {0.0f, 0.0f}              //texCoord
    };
    verts[2] = new Vertex{
        {0.0f, 1.0f, 0.0f},       //pos
        {0.0f, 0.0f, -1.0f},      //norm
        {1.0f, 1.0f, 1.0f, 1.0f}, //color
        {0.0f, 0.0f}              //texCoord
    };

    IndexedVerts* iv = (IndexedVerts*)IndexedVertsCreate(0, 6, IV_TRIANGLES, NUM_VERTICES, 0, 0, 0, 0);

    // Copy Vertex data into IndexedVerts
    for(int i = 0; i < NUM_VERTICES; i++) {
        memcpy(&iv->verts[i], verts[i], 12 * sizeof(float));
        delete verts[i];
        verts[i] = nullptr;
    }

    // Create space for serialized IndexedVerts
    void* bytes = malloc(sizeof(IndexedVerts) + sizeof(Vertex) * NUM_VERTICES);
    byteLen = sizeof(IndexedVerts) + sizeof(Vertex) * NUM_VERTICES;

    // Serialize IndexedVerts
    IndexedVertsSave(iv, bytes, &byteLen);
    IndexedVertsDestroy(iv);

    return bytes;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// IndexedVerts.cpp
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
    void* IndexedVertsLoad(void*const  bytes, size_t  byteLen)
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
        size_t idxSz = iv->indicesLen * sizeof(int);
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
} // IndexedVerts.cpp