
#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __VIZ_DATA_STRUCTURES_HEADERGUARD_HPP__
#define __VIZ_DATA_STRUCTURES_HEADERGUARD_HPP__

#ifdef _MSC_VER
  #define _CRT_SECURE_NO_WARNINGS 1
  #define _SCL_SECURE_NO_WARNINGS 1
#endif

//#include <unordered_map>

#include <map>
#include <GL/glew.h>

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

#include "../DataStore/ConcurrentMap/simdb.hpp"
#include "IndexedVerts.h"

//template<class T, class A=std::allocator<T> > 
//using   vec  =  std::vector<T, A>;

//template<class KEY, class VALUE,
//  class _Alloc=std::allocator<std::pair<const KEY,VALUE> > >
//using hashmap = std::unordered_map<KEY,VALUE, std::hash<KEY>, std::equal_to<KEY>, _Alloc >

template<class KEY, class VALUE,
  class _Compare = std::less<KEY>,
  class _Alloc   = std::allocator<std::pair<const KEY,VALUE> > >
using map = std::map<KEY,VALUE, _Compare, _Alloc >;

using str = std::string;

static const char*  vShaderPath  =  "../vertexShader.vert";
static const char*  fShaderPath  =  "../fragmentShader.frag";

struct Shader {

    GLuint shaderProgramId;

    void use() {
        glUseProgram(shaderProgramId);
    }

    void loadShaders(std::string& vertShader, std::string& fragShader) {
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        vShaderFile.exceptions(std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::badbit);
        try
        {
            vShaderFile.open(vertShader.c_str());
            fShaderFile.open(fragShader.c_str());
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch(std::ifstream::failure e)
        {
            printf("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n");
        }

        GLuint vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const GLchar* vShaderSource = vertexCode.c_str();
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
        const GLchar* fShaderSource = fragmentCode.c_str();
        glShaderSource(fragmentShader, 1, &fShaderSource, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if(!success) {
            GLchar infoLog[1024];
            glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
            printf("Compiling fragment shader failed: %s\n", infoLog);
        }

        shaderProgramId = glCreateProgram();
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
    }
};

//struct GLShape  // todo: make rvalue constructor - make all constructors?
//{
//  GLuint  vertbuf, vertary, idxbuf, tx, shader;
//  //GLsizei w,h;
//
//  ~GLShape(){ glDeleteVertexArrays(1, &vertary); }
//};

//private:
//public:
//GLuint shaderProgramId;
//GLsizei            w;    //             =  0;
//GLsizei            h;    //             =  0;
//GLuint       m_texID;    //=  0;

struct Key {
    int active;
    std::string key;
    void* bytes;
    GLuint vertexBuffer;
    GLuint vertexArray;
    GLuint indexBuffer;
    IndexedVerts* iv;
    Shader shader;

    Key(std::string& dbKey, void* dbBytes) :
        active(false),
        key(dbKey),
        bytes(dbBytes),
        iv(nullptr) {

        std::string vShader = "../vertexShader.vert";
        std::string fShader = "../fragmentShader.frag";
        shader.loadShaders(vShader, fShader);
    }

    void render() {
        if(!iv) {
            // Create IndexedVerts instance
            // TODO(Chris): Generalize this
            iv = (IndexedVerts*)IndexedVertsLoad(bytes);

            glGenVertexArrays(1, &vertexArray);
            glGenBuffers(1, &vertexBuffer);
            glGenBuffers(1, &indexBuffer);

            glBindVertexArray(vertexArray);

            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* iv->vertsLen, iv->verts, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)* iv->indicesLen, iv->indices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 6));

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);
        }

        // Camera/View transformation
        shader.use();
        glm::mat4 transform;
        transform = glm::rotate(transform, (GLfloat)glfwGetTime() * 50.0f, glm::vec3(0.2f, 0.5f, 1.0f));
        GLint transformLoc = glGetUniformLocation(shader.shaderProgramId, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        // Render
        int size;
        glBindVertexArray(vertexArray);
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
        glDrawElements(GL_TRIANGLES, size/sizeof(uint32_t), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void deactivate() {
        // TODO(Chris): Delete glBuffer?
        glDeleteVertexArrays(1, &vertexArray);
        IndexedVertsDestroy(iv);
        iv = nullptr;
    }
};

//struct VerKey { ui32 version; str key; };
//struct Shape { ui32 version; int active; GLShape glshape; };

//using VizData = hashmap<str, Shape>;

//struct VizData
//{
//  // todo: camera 
//  //vec<VerKey>         keys;
//  //hashmap<str, Shape> ;
//  //vec<str>            keys;
//  //vec<ui32>       versions;
//  //vec<int>          active;
//  //vec<GLShape>    glshapes;
//};

struct Shape {                     // todo: make rvalue constructor - make all constructors?
private:
  void mv(Shape&& rval)
  {
    memcpy(this, &rval, sizeof(Shape));
    memset(&rval,    0, sizeof(Shape));
  }

public:
  ui32    version; 
  int      active;
  GLuint  vertbuf, vertary, idxbuf, tx, shader;  // todo: take out shader 

  Shape() :
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
  Shape(Shape const&)          = delete;
  void operator=(Shape&& rval)
  {
    mv(std::move(rval));
  }
  void operator=(Shape const&) = delete;

  ~Shape()  // todo: put in checking to make sure that indices to delete are ok
  { 
    glDeleteVertexArrays(1, &vertary);
  }
};

//struct ui{};
// todo: make VizData a struct again, have the map be shapes and ui data be in a struct called ui

using   VizData = map<str, Shape>;

using KeyShapes = map<str, Shape>;

struct _VizData
{
  KeyShapes shapes;
  struct ui
  {
    struct nk_rect sidebar;
  };
  GLuint shaderId;
};

#endif
