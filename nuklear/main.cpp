#include "../DataStore/ConcurrentMap/ConcurrentMap.hpp"
#include "IndexedVerts.h"
#include "no_rt_util.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
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

void* makeTriangle(size_t& byteLen, bool left);

struct Key {
    int active;
    char* key;
    void* bytes;
    GLuint vertexBuffer;
    GLuint vertexArray;
    IndexedVerts* iv;

    void render() {
        if(!iv) {
            // Create IndexedVerts instance
            // TODO(Chris): Generalize this
            iv = (IndexedVerts*)IndexedVertsLoad(bytes);

            glGenVertexArrays(1, &vertexArray);
            glGenBuffers(1, &vertexBuffer);
            glBindVertexArray(vertexArray);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* iv->vertsLen, &iv->verts[0], GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)* 12, (void*)0);
            glEnableVertexAttribArray(0);
            glBindVertexArray(0);
        }
        glBindVertexArray(vertexArray);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
    }

    void deactivate() {
        // TODO(Chris): Delete glBuffer?
        glDeleteVertexArrays(1, &vertexArray);
        IndexedVertsDestroy(iv);
        iv = nullptr;
    }
};

Key* createKey(std::string& key, void* bytes) {
    size_t len = key.length();
    Key* k = (Key*)malloc(sizeof(Key));
    k->active = false;
    k->key = (char*)malloc(sizeof(char) * len);
    k->bytes = bytes;
    k->iv = nullptr;
    memcpy(k->key, key.c_str(), len);

    return k;
}

int sidebar(struct nk_context *ctx, int width, int height, std::vector<Key*>& keys);
// #include "overview.c"
// #include "node_editor.c"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void error_callback(int e, const char *d) {
    printf("Error %d: %s\n", e, d);
}

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

    std::vector<Key*> keys;

    // Create the DB
    simdb db("test", 512 * 1024, 4 * 1024);

    // Create serailized IndexedVerts
    size_t leftLen, rightLen;
    void* leftData = makeTriangle(leftLen, true);
    void* rightData = makeTriangle(rightLen, false);

    // Store serialized IndexedVerts in the db
    std::string leftTriangle = "leftTriangle";
    std::string rightTriangle = "rightTriangle";
    db.put((void*)leftTriangle.c_str(), (ui32)leftTriangle.length(), leftData, (ui32)leftLen);
    db.put((void*)rightTriangle.c_str(), (ui32)rightTriangle.length(), rightData, (ui32)rightLen);

    // Retrieve data from the db and create new Keys with it
    size_t leftSize = db.len(leftTriangle);
    size_t rightSize = db.len(rightTriangle);

    leftData = malloc(sizeof(ui8) * leftSize);
    rightData = malloc(sizeof(ui8) * rightSize);

    if(db.get(leftTriangle, leftData) < 0) {
        printf("Error reading from db. Key %s does not exist.\n", leftTriangle);
    }
    if(db.get(rightTriangle, rightData) < 0) {
        printf("Error reading from db. Key %s does not exist.\n", rightTriangle);
    }

    // Create the Keys
    Key* k1 = createKey(leftTriangle, leftData);
    Key* k2 = createKey(rightTriangle, rightData);
    keys.push_back(k1);
    keys.push_back(k2);

    /* nuklear */
    ctx = nk_glfw3_init(win, NK_GLFW3_INSTALL_CALLBACKS);
    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    nk_glfw3_font_stash_end();

    background = nk_rgb(28,48,62);
    while (!glfwWindowShouldClose(win))
    {
        /* Input */
        glfwPollEvents();
        nk_glfw3_new_frame();

        sidebar(ctx, width, height, keys);

        /* Draw */
        {float bg[4];
        nk_color_fv(bg, background);
        glfwGetWindowSize(win, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(bg[0], bg[1], bg[2], bg[3]);

        for(auto key : keys) {
            if(key->active) {
                key->render();
            }
        }

        /* IMPORTANT: `nk_glfw_render` modifies some global OpenGL state
         * with blending, scissor, face culling, depth test and viewport and
         * defaults everything back into a default state.
         * Make sure to either a.) save and restore or b.) reset your own state after
         * rendering the UI. */
        nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        glfwSwapBuffers(win);}
    }
    nk_glfw3_shutdown();
    glfwTerminate();
    return 0;
}

void* makeTriangle(size_t& byteLen, bool left) {
    // Create triangle Vertex data
    const unsigned int NUM_VERTICES = 3;

    Vertex* verts[NUM_VERTICES];
    if(left) {
        verts[0] = new Vertex{
            {-1.0, -1.0f, 0.0f},     //pos
            {0.0f, 0.0f, -1.0f},      //norm
            {1.0f, 1.0f, 1.0f, 1.0f}, //color
            {0.0f, 0.0f}              //texCoord
        };
        verts[1] = new Vertex{
            {-0.17f, -1.0f, 0.0f},      //pos
            {0.0f, 0.0f, -1.0f},      //norm
            {1.0f, 1.0f, 1.0f, 1.0f}, //color
            {0.0f, 0.0f}              //texCoord
        };
        verts[2] = new Vertex{
            {-0.58f, 1.0f, 0.0f},       //pos
            {0.0f, 0.0f, -1.0f},      //norm
            {1.0f, 1.0f, 1.0f, 1.0f}, //color
            {0.0f, 0.0f}              //texCoord
        };
    }
    else {
        verts[0] = new Vertex{
            {-0.17, -1.0f, 0.0f},     //pos
            {0.0f, 0.0f, -1.0f},      //norm
            {1.0f, 1.0f, 1.0f, 1.0f}, //color
            {0.0f, 0.0f}              //texCoord
        };
        verts[1] = new Vertex{
            {0.66f, -1.0f, 0.0f},      //pos
            {0.0f, 0.0f, -1.0f},      //norm
            {1.0f, 1.0f, 1.0f, 1.0f}, //color
            {0.0f, 0.0f}              //texCoord
        };
        verts[2] = new Vertex{
            {0.25, 1.0f, 0.0f},       //pos
            {0.0f, 0.0f, -1.0f},      //norm
            {1.0f, 1.0f, 1.0f, 1.0f}, //color
            {0.0f, 0.0f}              //texCoord
        };
    }

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

int sidebar(struct nk_context *ctx, int width, int height, std::vector<Key*>& keys)
{
    /* window flags */
    static nk_flags window_flags = 0;

    /* popups */
    static enum nk_style_header_align header_align = NK_HEADER_RIGHT;
    struct nk_panel layout;

    /* window flags */
    ctx->style.window.header.align = header_align;

    if(nk_begin(ctx, &layout, "Overview", nk_rect((5/6.0) * width, 0, (1/6.0) * width, height), window_flags))
    {
        if(nk_tree_push(ctx, NK_TREE_TAB, "Drawables", NK_MINIMIZED))
        {
            nk_layout_row_static(ctx, 18, 100, 1);
            for(auto key : keys) {
                nk_selectable_label(ctx, key->key, NK_TEXT_LEFT, &key->active);
                if(!key->active && key->iv) {
                    key->deactivate();
                }
            }
            nk_tree_pop(ctx);
        }
    }
    nk_end(ctx);
    return !nk_window_is_closed(ctx, "Overview");
}

//struct Attributes
//{
//    GLuint position;
//    GLuint normal;
//    GLuint sourceColour;
//    GLuint textureCoordIn;
//
//    Attributes(Shader& shaderProgram)
//    {
//        position = createAttribute(shaderProgram, "position");
//        normal = createAttribute(shaderProgram, "normal");
//        sourceColour = createAttribute(shaderProgram, "sourceColour");
//        texureCoordIn = createAttribute(shaderProgram, "texureCoordIn");
//    }
//    void enable()
//    {
//        if(position != nullptr)
//        {
//            glVertexAttribPointer(position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), 0);
//            glEnableVertexAttribArray(position->attributeID);
//        }
//
//        if(normal != nullptr)
//        {
//            glVertexAttribPointer(normal->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*)(sizeof (float)* 3));
//            glEnableVertexAttribArray(normal->attributeID);
//        }
//
//        if(sourceColour != nullptr)
//        {
//            glVertexAttribPointer(sourceColour->attributeID, 4, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*)(sizeof (float)* 6));
//            glEnableVertexAttribArray(sourceColour->attributeID);
//        }
//
//        if(texureCoordIn != nullptr)
//        {
//            glVertexAttribPointer(texureCoordIn->attributeID, 2, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*)(sizeof (float)* 10));
//            glEnableVertexAttribArray(texureCoordIn->attributeID);
//        }
//    }
//    void disable()
//    {
//        if(position != nullptr)       glDisableVertexAttribArray(position->attributeID);
//        if(normal != nullptr)         glDisableVertexAttribArray(normal->attributeID);
//        if(sourceColour != nullptr)   glDisableVertexAttribArray(sourceColour->attributeID);
//        if(texureCoordIn != nullptr)  glDisableVertexAttribArray(texureCoordIn->attributeID);
//    }
//    // ScopedPointer<OpenGLShaderProgram::Attribute> position, normal, sourceColour, texureCoordIn;
//
//private:
//    static GLuint createAttribute(Shader& shader, const char* attributeName)
//    {
//        if(glGetAttribLocation(shader.id, attributeName) < 0)
//            return nullptr;
//
//        return new OpenGLShaderProgram::Attribute(shader, attributeName);
//    }
//};
//
//struct VertexBuffer
//{
//private:
//    //bool            m_moved         =   false;
//    //GLenum          m_mode          =   GL_POINTS;
//    bool            m_owned = true;
//    i32             m_numIndices = 0;
//    GLenum          m_mode = GL_POINTS;
//    GLuint          vertexBuffer = -1;
//    GLuint          indexBuffer = -1;
//    // OpenGLContext*  openGLContext = nullptr;
//
//    void mv(VertexBuffer&& rval)
//    {
//        m_numIndices = rval.m_numIndices;
//        m_mode = rval.m_mode;
//        vertexBuffer = rval.vertexBuffer;
//        indexBuffer = rval.indexBuffer;
//        // openGLContext = rval.openGLContext;
//
//        rval.m_owned = false;
//    }
//
//public:
//    VertexBuffer(IndexedVerts* idxVerts) :
//        // openGLContext(context),
//        m_mode(idxVerts->mode)
//    {
//        m_numIndices = (i32)idxVerts->indicesLen;
//
//        glGenBuffers(1, &vertexBuffer);
//        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
//
//        glBufferData(GL_ARRAY_BUFFER,
//            static_cast<GLsizeiptr>(static_cast<size_t>(idxVerts->vertsLen)*sizeof(Vertex)),
//            idxVerts->verts,
//            GL_STATIC_DRAW);
//
//        glGenBuffers(1, &indexBuffer);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
//            static_cast<GLsizeiptr>(static_cast<size_t>(m_numIndices)*sizeof(uint32_t)),
//            idxVerts->indices,
//            GL_STATIC_DRAW);
//    }
//    VertexBuffer(std::vector<Vertex> verts, std::vector<int32_t> indices)
//    {
//        //numIndices = aShape.mesh.indices.size();
//        m_numIndices = (i32)indices.size();
//
//        glGenBuffers(1, &vertexBuffer);
//        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
//
//        //Array<Vertex> vertices;
//        //createVertexListFromMesh (aShape.mesh, vertices, Colours::green);
//
//        glBufferData(GL_ARRAY_BUFFER,
//            static_cast<GLsizeiptr>(static_cast<size_t>(verts.size())*sizeof(Vertex)),
//            verts.data(),
//            GL_STATIC_DRAW);
//
//        glGenBuffers(1, &indexBuffer);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
//            static_cast<GLsizeiptr>(static_cast<size_t>(m_numIndices)*sizeof(uint32_t)),
//            indices.data(),
//            GL_STATIC_DRAW);
//    }
//    ~VertexBuffer()
//    {
//        if(m_owned) {
//            glDeleteBuffers(1, &vertexBuffer);
//            glDeleteBuffers(1, &indexBuffer);
//        }
//    }
//
//    VertexBuffer(VertexBuffer const& vb) = delete;
//    VertexBuffer& operator=(VertexBuffer const& vb) = delete;
//
//    VertexBuffer(VertexBuffer&& vb)
//    {
//        mv(std::move(vb));
//    }
//    VertexBuffer& operator=(VertexBuffer&& vb)
//    {
//        mv(std::move(vb));
//        return *this;
//        //m_numIndices   =  vb.m_numIndices;
//        //vertexBuffer   =  vb.vertexBuffer;
//        //indexBuffer    =  vb.indexBuffer;
//        //openGLContext  =  vb.openGLContext;
//        //
//        ////vb.m_moved     =  true;
//        //vb.m_owned     =  false;
//        //return *this;
//    }
//
//    void    bind()
//    {
//        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
//    }
//    i32     numIndices() const
//    {
//        return m_numIndices;
//    }
//    GLenum  mode()       const
//    {
//        return m_mode;
//    }
//};
//
//struct Shape
//{
//private:
//    // using ScopedUniform = ScopedPointer<OpenGLShaderProgram::Uniform>;
//
//    bool                m_visible = false;
//    std::vector<VertexBuffer>   vertexBuffers;
//    GLuint              m_texID = 0;
//    GLsizei             w = 0;
//    GLsizei             h = 0;
//    IndexedVerts*       m_iv;
//
//    void mv(Shape&& rval)
//    {
//        m_visible = rval.m_visible;
//        vertexBuffers = move(rval.vertexBuffers);
//        //memcpy(&m_tex, &rval.m_tex, sizeof(decltype(m_tex)) );
//        m_texID = rval.m_texID;
//        w = rval.w;
//        h = rval.h;
//        //m_iv           =  rval.m_iv;
//    }
//
//public:
//    Shape(){}
//    Shape(IndexedVerts* iv)
//    {
//        vertexBuffers.emplace_back(std::move(VertexBuffer(iv)));
//
//        // check for iv having more than 4 dimensions?
//        glActiveTexture(GL_TEXTURE0);
//        glGenTextures(1, &m_texID);
//        glBindTexture(GL_TEXTURE_2D, m_texID);
//        glTexImage2D(GL_TEXTURE_2D,
//            0, GL_RGBA,
//            (GLint)iv->imgWidth, (GLint)iv->imgHeight,
//            0, GL_RGBA, GL_FLOAT, iv->pixels);
//    }
//
//    Shape(Shape const& s) = delete;
//    Shape& operator=(Shape const& s) = delete;
//
//    Shape(Shape&& rval)
//    {
//        mv(std::move(rval));
//    }
//    Shape& operator=(Shape&& rval)
//    {
//        mv(std::move(rval));
//        return *this;
//    }
//
//    void release()
//    {
//        //m_tex.release();
//        vertexBuffers.clear();
//    }
//    void draw(Attributes& glAttributes, GLuint shaderHnd)
//    {
//        if(m_visible)
//        {
//            for(auto& vb : vertexBuffers)
//            {
//                //m_tex.bind();
//
//                vb.bind();
//
//                glActiveTexture(GL_TEXTURE0);
//                glBindTexture(GL_TEXTURE_2D, m_texID);
//                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_LINEAR);
//                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_LINEAR);
//                auto loc = glGetUniformLocation(shaderHnd, "tex0");
//                glUniform1i(loc, 0);
//                glAttributes.enable();
//                //glPointSize(1.f);
//                glDrawElements(vb.mode(), vb.numIndices(), GL_UNSIGNED_INT, 0);
//                glAttributes.disable();
//
//                //m_tex.unbind();
//            }
//            //glDrawPixels(m_iv->imgWidth, m_iv->imgHeight, GL_RGBA, GL_FLOAT, m_iv->pixels );
//        }
//    }
//    void visible(bool b) { m_visible = b; }
//    bool visible() const { return m_visible; }
//};
//
//struct Shader {
//    GLuint id;
//
//    Shader() {}
//    Shader& Shader::use()
//    {
//        glUseProgram(id);
//        return *this;
//    }
//
//    void Shader::compile(const GLchar* vertexSource, const GLchar* fragmentSource, const GLchar* geometrySource)
//    {
//        GLuint sVertex, sFragment, gShader;
//
//        sVertex = glCreateShader(GL_VERTEX_SHADER);
//        glShaderSource(sVertex, 1, &vertexSource, NULL);
//        glCompileShader(sVertex);
//        checkCompileErrors(sVertex, "VERTEX");
//
//        sFragment = glCreateShader(GL_FRAGMENT_SHADER);
//        glShaderSource(sFragment, 1, &fragmentSource, NULL);
//        glCompileShader(sFragment);
//        checkCompileErrors(sFragment, "FRAGMENT");
//
//        if(geometrySource != nullptr)
//        {
//            gShader = glCreateShader(GL_GEOMETRY_SHADER);
//            glShaderSource(gShader, 1, &geometrySource, NULL);
//            glCompileShader(gShader);
//            checkCompileErrors(gShader, "GEOMETRY");
//        }
//
//        this->id = glCreateProgram();
//        glAttachShader(this->id, sVertex);
//        glAttachShader(this->id, sFragment);
//        if(geometrySource != nullptr)
//            glAttachShader(this->id, gShader);
//        glLinkProgram(this->id);
//        checkCompileErrors(this->id, "PROGRAM");
//
//        glDeleteShader(sVertex);
//        glDeleteShader(sFragment);
//        if(geometrySource != nullptr)
//            glDeleteShader(gShader);
//    }
//
//    void Shader::setFloat(const GLchar *name, GLfloat value, GLboolean useShader)
//    {
//        if(useShader)
//            this->use();
//        glUniform1f(glGetUniformLocation(this->id, name), value);
//    }
//    void Shader::setInteger(const GLchar *name, GLint value, GLboolean useShader)
//    {
//        if(useShader)
//            this->use();
//        glUniform1i(glGetUniformLocation(this->id, name), value);
//    }
//    void Shader::setVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader)
//    {
//        if(useShader)
//            this->use();
//        glUniform2f(glGetUniformLocation(this->id, name), x, y);
//    }
//    void Shader::setVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader)
//    {
//        if(useShader)
//            this->use();
//        glUniform2f(glGetUniformLocation(this->id, name), value.x, value.y);
//    }
//    void Shader::setVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader)
//    {
//        if(useShader)
//            this->use();
//        glUniform3f(glGetUniformLocation(this->id, name), x, y, z);
//    }
//    void Shader::setVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader)
//    {
//        if(useShader)
//            this->use();
//        glUniform3f(glGetUniformLocation(this->id, name), value.x, value.y, value.z);
//    }
//    void Shader::setVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader)
//    {
//        if(useShader)
//            this->use();
//        glUniform4f(glGetUniformLocation(this->id, name), x, y, z, w);
//    }
//    void Shader::setVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader)
//    {
//        if(useShader)
//            this->use();
//        glUniform4f(glGetUniformLocation(this->id, name), value.x, value.y, value.z, value.w);
//    }
//    void Shader::setMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader)
//    {
//        if(useShader)
//            this->use();
//        glUniformMatrix4fv(glGetUniformLocation(this->id, name), 1, GL_FALSE, glm::value_ptr(matrix));
//    }
//
//
//    void Shader::checkCompileErrors(GLuint object, std::string type)
//    {
//        GLint success;
//        GLchar infoLog[1024];
//        if(type != "PROGRAM")
//        {
//            glGetShaderiv(object, GL_COMPILE_STATUS, &success);
//            if(!success)
//            {
//                glGetShaderInfoLog(object, 1024, NULL, infoLog);
//                std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
//                    << infoLog << "\n -- --------------------------------------------------- -- "
//                    << std::endl;
//            }
//        }
//        else
//        {
//            glGetProgramiv(object, GL_LINK_STATUS, &success);
//            if(!success)
//            {
//                glGetProgramInfoLog(object, 1024, NULL, infoLog);
//                std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
//                    << infoLog << "\n -- --------------------------------------------------- -- "
//                    << std::endl;
//            }
//        }
//    }
//};
