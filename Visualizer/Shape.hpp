

#ifndef __SHAPE_HEADER_GUARD__
#define __SHAPE_HEADER_GUARD__


#include "JuceHeader.h"
#include <SIM/Types.h>
#include "../IndexedVerts_datatype/IndexedVerts_datatype.h"

static void checkGLError (const char* file, const int line)
{
    for (;;)
    {
        const GLenum e = glGetError();

        if (e == GL_NO_ERROR)
            break;

        //DBG ("***** " << getGLErrorMessage (e) << "  at " << file << " : " << line);
        jassertfalse;
    }
}

 #define JUCE_CHECK_OPENGL_ERROR checkGLError (__FILE__, __LINE__);
static OpenGLShaderProgram::Uniform* createUniform (OpenGLContext& openGLContext,
                                                    OpenGLShaderProgram& shaderProgram,
                                                    const char* uniformName)
{
    if (openGLContext.extensions.glGetUniformLocation (shaderProgram.getProgramID(), uniformName) < 0)
        return nullptr;

    return new OpenGLShaderProgram::Uniform (shaderProgram, uniformName);
}


struct Attributes
{
  Attributes(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram)
  {
      position      = createAttribute (openGLContext, shaderProgram, "position");
      normal        = createAttribute (openGLContext, shaderProgram, "normal");
      sourceColour  = createAttribute (openGLContext, shaderProgram, "sourceColour");
      texureCoordIn = createAttribute (openGLContext, shaderProgram, "texureCoordIn");
  }
  void enable(OpenGLContext& openGLContext)
  {
      if (position != nullptr)
      {
          openGLContext.extensions.glVertexAttribPointer (position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), 0);
          openGLContext.extensions.glEnableVertexAttribArray (position->attributeID);
      }

      if (normal != nullptr)
      {
          openGLContext.extensions.glVertexAttribPointer (normal->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 3));
          openGLContext.extensions.glEnableVertexAttribArray (normal->attributeID);
      }

      if (sourceColour != nullptr)
      {
          openGLContext.extensions.glVertexAttribPointer (sourceColour->attributeID, 4, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 6));
          openGLContext.extensions.glEnableVertexAttribArray (sourceColour->attributeID);
      }

      if (texureCoordIn != nullptr)
      {
          openGLContext.extensions.glVertexAttribPointer (texureCoordIn->attributeID, 2, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 10));
          openGLContext.extensions.glEnableVertexAttribArray (texureCoordIn->attributeID);
      }
  }
  void disable(OpenGLContext& openGLContext)
  {
      if (position != nullptr)       openGLContext.extensions.glDisableVertexAttribArray (position->attributeID);
      if (normal != nullptr)         openGLContext.extensions.glDisableVertexAttribArray (normal->attributeID);
      if (sourceColour != nullptr)   openGLContext.extensions.glDisableVertexAttribArray (sourceColour->attributeID);
      if (texureCoordIn != nullptr)  openGLContext.extensions.glDisableVertexAttribArray (texureCoordIn->attributeID);
  }
  ScopedPointer<OpenGLShaderProgram::Attribute> position, normal, sourceColour, texureCoordIn;

private:
  static OpenGLShaderProgram::Attribute* createAttribute(OpenGLContext& openGLContext,
                                                          OpenGLShaderProgram& shader,
                                                          const char* attributeName)
  {
      if (openGLContext.extensions.glGetAttribLocation (shader.getProgramID(), attributeName) < 0)
          return nullptr;

      return new OpenGLShaderProgram::Attribute (shader, attributeName);
  }
};
struct Uniforms
{
  using ScopedUniform = ScopedPointer<OpenGLShaderProgram::Uniform>;

  //ScopedUniform       m_tex0;

  Uniforms (OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram)
  {
    projectionMatrix = createUniform(openGLContext, shaderProgram, "projectionMatrix");
    viewMatrix       = createUniform(openGLContext, shaderProgram, "viewMatrix");
    //m_tex0           = createUniform(openGLContext, shaderProgram, "tex0");
  }

  ScopedPointer<OpenGLShaderProgram::Uniform> projectionMatrix, viewMatrix;

private:
};
struct VertexBuffer
{
private:
  //bool            m_moved         =   false;
  //GLenum          m_mode          =   GL_POINTS;
  bool            m_owned         =   true;
  i32             m_numIndices    =   0;
  GLenum          m_mode          =   GL_POINTS;
  GLuint          vertexBuffer    =  -1;
  GLuint          indexBuffer     =  -1;
  OpenGLContext*  openGLContext   =   nullptr;

  void mv(VertexBuffer&& rval)
  {
    m_numIndices   =  rval.m_numIndices;
    m_mode         =  rval.m_mode;
    vertexBuffer   =  rval.vertexBuffer;
    indexBuffer    =  rval.indexBuffer;
    openGLContext  =  rval.openGLContext;

    rval.m_owned     =  false;
  }

public:
  VertexBuffer(OpenGLContext* context, IndexedVerts* idxVerts) : 
    openGLContext(context),
    m_mode(idxVerts->mode)
  {
    m_numIndices = (i32)idxVerts->indicesLen;
    
    openGLContext->extensions.glGenBuffers(1, &vertexBuffer);
    openGLContext->extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        
    openGLContext->extensions.glBufferData(GL_ARRAY_BUFFER,
                                          static_cast<GLsizeiptr>( static_cast<size_t>(idxVerts->vertsLen)*sizeof(Vertex) ),
                                          idxVerts->verts, 
                                          GL_STATIC_DRAW);
    
    openGLContext->extensions.glGenBuffers(1, &indexBuffer);
    openGLContext->extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    openGLContext->extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                          static_cast<GLsizeiptr>( static_cast<size_t>(m_numIndices)*sizeof(juce::uint32) ),
                                          idxVerts->indices,
                                          GL_STATIC_DRAW);
  }
  VertexBuffer(OpenGLContext* context, vec<Vertex> verts, vec<i32> indices) : openGLContext(context)
  {
    //numIndices = aShape.mesh.indices.size();
    m_numIndices = (i32)indices.size();
    
    openGLContext->extensions.glGenBuffers(1, &vertexBuffer);
    openGLContext->extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    
    //Array<Vertex> vertices;
    //createVertexListFromMesh (aShape.mesh, vertices, Colours::green);
    
    openGLContext->extensions.glBufferData(GL_ARRAY_BUFFER,
                                          static_cast<GLsizeiptr>( static_cast<size_t>(verts.size())*sizeof(Vertex) ),
                                          verts.data(), 
                                          GL_STATIC_DRAW);
    
    openGLContext->extensions.glGenBuffers(1, &indexBuffer);
    openGLContext->extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    openGLContext->extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                          static_cast<GLsizeiptr>( static_cast<size_t>(m_numIndices)*sizeof(juce::uint32) ),
                                          indices.data(), 
                                          GL_STATIC_DRAW);
  }
  ~VertexBuffer()
  {
    if(m_owned) {
      openGLContext->extensions.glDeleteBuffers(1, &vertexBuffer);
      openGLContext->extensions.glDeleteBuffers(1, &indexBuffer);
    }
  }
  
  VertexBuffer           (VertexBuffer const& vb) = delete;
  VertexBuffer& operator=(VertexBuffer const& vb) = delete;

  VertexBuffer           (VertexBuffer&& vb)
  { mv(move(vb)); }
  VertexBuffer& operator=(VertexBuffer&& vb) 
  { 
    mv(move(vb));
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
    openGLContext->extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    openGLContext->extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  }
  i32     numIndices() const
  {
    return m_numIndices;
  }
  GLenum  mode()       const
  {
    return m_mode;
  }
      
  JUCE_LEAK_DETECTOR(VertexBuffer)
};  
struct Shape
{
private:
  using ScopedUniform = ScopedPointer<OpenGLShaderProgram::Uniform>;

  bool                m_visible       =  false;
  vec<VertexBuffer>   vertexBuffers;
  GLuint              m_texID         =  0;
  GLsizei             w               =  0;
  GLsizei             h               =  0;
  IndexedVerts*       m_iv;

  void mv(Shape&& rval)
  {
    m_visible      =  rval.m_visible;
    vertexBuffers  =  move(rval.vertexBuffers);
    //memcpy(&m_tex, &rval.m_tex, sizeof(decltype(m_tex)) );
    m_texID        =  rval.m_texID;
    w              =  rval.w;
    h              =  rval.h;
    //m_iv           =  rval.m_iv;
  }

public:
  Shape(){}
  Shape(OpenGLContext* openGLContext, IndexedVerts* iv)
  {
    vertexBuffers.emplace_back( move(VertexBuffer(openGLContext,iv) ) );
    
    // check for iv having more than 4 dimensions?
    openGLContext->extensions.glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &m_texID);
    glBindTexture(GL_TEXTURE_2D, m_texID);
    glTexImage2D (GL_TEXTURE_2D, 
                 0, GL_RGBA,  
                 (GLint)iv->imgWidth, (GLint)iv->imgHeight, 
                 0, GL_RGBA, GL_FLOAT, iv->pixels );
  }

  Shape           (Shape const& s) = delete;
  Shape& operator=(Shape const& s) = delete;

  Shape(Shape&& rval)
  { 
    mv(move(rval));
  }
  Shape& operator=(Shape&& rval)
  {
    mv(move(rval));
    return *this;
  }

  void release()
  {
    //m_tex.release();
    vertexBuffers.clear();
  }
  void draw(OpenGLContext& openGLContext, Attributes& glAttributes, GLuint shaderHnd)
  {
    if(m_visible)
    {
      for(auto& vb : vertexBuffers)
      {
        //m_tex.bind();

        vb.bind();

        openGLContext.extensions.glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_LINEAR);
        auto loc = openGLContext.extensions.glGetUniformLocation(shaderHnd, "tex0");
        openGLContext.extensions.glUniform1i(loc, 0);

        JUCE_CHECK_OPENGL_ERROR

        glAttributes.enable(openGLContext);
        //glPointSize(1.f);
        glDrawElements(vb.mode(), vb.numIndices(), GL_UNSIGNED_INT, 0);
        glAttributes.disable(openGLContext);

        //m_tex.unbind();
      }
      //glDrawPixels(m_iv->imgWidth, m_iv->imgHeight, GL_RGBA, GL_FLOAT, m_iv->pixels );
    }
  }
  void visible(bool b) { m_visible = b;    }
  bool visible() const { return m_visible; }
};


#endif











//
//JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VertexBuffer)

//vec<ui8> px(iv->imgWidth * iv->imgHeight * iv->imgChans );
//TO(px.size(),i) px[i] = 0xFF;
//m_tex.create(iv->imgWidth, iv->imgHeight, px.data(), GL_RGBA, true);
//
//Image jImg(Image::PixelFormat::RGB, iv->imgWidth, iv->imgHeight, true);
//TO(px.size(),i) px[i] = (ui8)(iv->pixels[i] * 255.f);
//m_tex.loadARGB( (PixelARGB*)px.data(), iv->imgWidth, iv->imgHeight);  

//size_t floatLen = iv->imgWidth * iv->imgHeight * iv->imgChans;
//TO(floatLen,i) iv->pixels[i] = 1.f;

//Shape(OpenGLContext* openGLContext, vec<Vertex> verts, vec<i32> indices)
//{
//  vertexBuffers.emplace_back( move(VertexBuffer(openGLContext, move(verts), move(indices))) );
//  
//  //if (shapeFile.load (BinaryData::teapot_obj).wasOk())
//  //    for (int i = 0; i < shapeFile.shapes.size(); ++i)
//  //        vertexBuffers.add (new VertexBuffer (openGLContext, *shapeFile.shapes.getUnchecked(i)));
//}
//Shape(VertexBuffer vb)
//{
//  vertexBuffers.emplace_back(move(vb));
//}

//Array<Vertex> vertices;
//createVertexListFromMesh (aShape.mesh, vertices, Colours::green);


