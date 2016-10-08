
#ifndef __VIZCOMPONENT_HEADER_GUARD__
#define __VIZCOMPONENT_HEADER_GUARD__

#include "stdafx.h"
#include "Shape.hpp"
#include "DynLib.hpp"
#include "DataStore.hpp"

struct Viz;

class VizComponent :  public        Component, 
                      private  OpenGLRenderer,
                      public      KeyListener
                      //public  MouseListener
{
private:
    struct VizDataShape
    {
      Viz*       viz;
      void*     data;
      Shape    shape;
      bool     fresh;

      void mv(VizDataShape&& rval)
      {
        viz    =  rval.viz;
        data   =  rval.data;
        shape  =  move(rval.shape);
        fresh  =  rval.fresh;

        rval.viz    =  nullptr;
        rval.data   =  nullptr;
        rval.fresh  =  false;
      }

      VizDataShape(){}
      VizDataShape           (VizDataShape&& rval)
      { mv(move(rval)); }
      VizDataShape& operator=(VizDataShape&& rval)
      { mv(move(rval)); return *this; }

      VizDataShape(Viz* _viz, void* _data, Shape _shape) :
        viz(_viz),
        data(_data),
        shape(move(_shape)),
        fresh(false)
      {}

      VizDataShape           (VizDataShape const& lval) = delete;
      VizDataShape& operator=(VizDataShape const& lval) = delete;

      ~VizDataShape() { shape.release(); }
    };
    struct VizCommand
    {
      Viz*         viz;
      MetaData    meta;
      int          idx;
      bool      remove;

      void mv(VizCommand&&      rval)
      {
        viz     =  rval.viz;
        meta    =  move(rval.meta);
        idx     =  rval.idx;
        remove  =  rval.remove;
      }
      void cp(VizCommand const& lval)
      {
        viz     =  lval.viz;
        meta    =  lval.meta;
        idx     =  lval.idx;
        remove  =  lval.remove;
      }

      VizCommand(){}
      VizCommand(Viz* _viz, MetaData const& _meta, bool _remove=false) :
        viz(_viz),
        meta(_meta),
        remove(_remove),
        idx(0)
      {}
      VizCommand           (VizCommand&& rval)
      { mv(move(rval)); }
      VizCommand& operator=(VizCommand&& rval)
      { mv(move(rval)); return *this; }

      VizCommand           (VizCommand const& lval)
      { cp(lval); }
      VizCommand& operator=(VizCommand const& lval)
      { cp(lval); return *this; }

      bool operator<(VizCommand const& lval) const
      {
        //if(meta.name != lval.meta.name)
        //  return meta.name < lval.meta.name;
        //
        //if(remove!=lval.remove)
        //  return !remove;

        return meta.name < lval.meta.name;
      }
    };

    //using Shapes          =  map<string, VizDataShape>;
    using Shapes          =  map<i64, VizDataShape>;
    using ShapeBuf        =  DoubleBuffer< Shapes >;
    using VizQ            =  moodycamel::ConcurrentQueue<VizCommand>;
    using ScopedShader    =  ScopedPointer<OpenGLShaderProgram>;
    using ScopedAttrs     =  ScopedPointer<Attributes>;
    using ScopedUniforms  =  ScopedPointer<Uniforms>;

    // Mouse and Camera
    float m_sense                =   0.003f;
    float m_rotSense             =   0.005f;
    float m_camZ                 =    -1.5f;
    mutable int         m_glX    =        0;
    mutable int         m_glY    =        0;
    mutable int         m_mvX    =        0;
    mutable int         m_mvY    =        0;
    mutable float       m_posX   =    -0.5f;
    mutable float       m_posY   =    -0.5f;
    mutable float       m_rotX   =     0.0f;
    mutable float       m_rotY   =     0.0f;
    mutable ary<f32,3>  m_rgb;
    mutable int         m_ptSz   =        3;
    // End Mouse

    const char*        vertexShader;
    const char*      fragmentShader;
    String          newVertexShader; 
    String        newFragmentShader;
    ScopedShader             shader;
    ScopedAttrs          attributes;
    ScopedUniforms         uniforms;

    mutable Ai32         m_curShapeId       =      0;
    i64                  m_frames           =      0;
    string               m_status           =     "";
    mutable bool         m_contextFinished  =  false;
    mutable ABool        m_nxtFilled        =  false;
    mutable mutex        m_renderMut;
    mutable VizQ         m_vizQ;
    mutable Shapes       m_shapes;
    
    Matrix3D<float>   getProjectionMatrix()  const
    {
      //float w = 1.0f / (0.5f + 0.1f);
      const float camScale = 1.0f;

      float w = camScale * 0.005f;    // 1.0f / (0.5f + 0.1f);
      float h = w * getLocalBounds().toFloat().getAspectRatio(false);
      
      return Matrix3D<float>::fromFrustum(-w, w, -h, h, 0.01f, 1000.0f);
    }
    Matrix3D<float>         getViewMatrix()  const
    {
      Matrix3D<float>  viewMatrix(Vector3D<float>(m_posX, m_posY, m_camZ));


      Matrix3D<float>  rotationMatrix
          = viewMatrix.rotated(Vector3D<float>(m_rotX, m_rotY, 0.f));

      return rotationMatrix * viewMatrix;

      //Matrix3D<float> rotationMatrix
      //    = viewMatrix.rotated (Vector3D<float> (-0.3f, 5.0f * std::sin (getFrameCounter() * 0.01f), 0.0f));
    }
    void                    createShaders()
    {
      vertexShader =
          "#version 330 \n"
          "attribute vec4 position;\n"
          "attribute vec4 sourceColour;\n"
          "attribute vec2 texureCoordIn;\n"
          "\n"
          "uniform mat4 projectionMatrix;\n"
          "uniform mat4 viewMatrix;\n"
          "\n"
          "out vec4 destinationColour;\n"
          "out vec2 uv;\n"
          "\n"
          "void main()\n"
          "{\n"
          "    destinationColour  =  sourceColour;\n"
          "    uv                 =  texureCoordIn;\n"
          "    gl_Position        =  projectionMatrix * viewMatrix * position;\n"
          "}\n";

      fragmentShader =
          "#version 330 \n"
          #if JUCE_OPENGL_ES
          "varying lowp vec4 destinationColour;\n"
          "varying lowp vec2 textureCoordOut;\n"
          #else
          "in vec4 destinationColour; \n"
          "in vec2 uv;                \n"
          
          "out vec4 Ci; \n" 
          #endif
          "\n"
          "uniform sampler2D tex0;"
          "\n"
          "void main()\n"
          "{\n"
          " vec4 colour  =  destinationColour;      \n"
          " vec4 tClr    =  texture2D(tex0, uv);    \n"
          //" Ci           =  vec4(tClr.xyz,tClr.a);  \n"
          " Ci           =  vec4(tClr.xyz*tClr.a,tClr.a) + ( (1-tClr.a)*colour ); \n"
          //" Ci           =  vec4(tClr.xyz*tClr.a,tClr.a) + ( (1-tClr.a)*vec4(0,0,1,1) ); \n"
          //" Ci           =  vec4(tClr.xyz,1); \n"
          "}\n";
        
      ScopedPointer<OpenGLShaderProgram> newShader(new OpenGLShaderProgram(m_openGLContext));
      String statusText;


      if(newShader->addVertexShader( OpenGLHelpers::translateVertexShaderToV3(vertexShader) )
            && newShader->addFragmentShader( OpenGLHelpers::translateFragmentShaderToV3(fragmentShader) )
            && newShader->link())
      {
        //shape = nullptr;
        attributes  =  nullptr;
        uniforms    =  nullptr;

        shader      =  newShader;
        shader->use();

        //shape      = new Shape (openGLContext);
        attributes  =  new Attributes(m_openGLContext, *shader);
        uniforms    =  new Uniforms  (m_openGLContext, *shader);

        statusText  =  "GLSL: v" + String(OpenGLShaderProgram::getLanguageVersion(), 2);
      }
      else
      {
        statusText  =  newShader->getLastError();
      }
      m_status = statusText.toStdString();
    }

public:
    OpenGLContext        m_openGLContext;

    VizComponent()  :
      m_openGLContext()
    {
      fill(ALL(m_rgb), 0.f);

      setOpaque(true);
      auto pxfmt = juce::OpenGLPixelFormat(32,32,32);
      pxfmt.multisamplingLevel = 8;
      m_openGLContext.setPixelFormat(pxfmt);
      m_openGLContext.setContinuousRepainting(true);
      m_openGLContext.setSwapInterval(1);
      //m_openGLContext.
      m_openGLContext.setMultisamplingEnabled(true);
      m_openGLContext.setRenderer(this);

      m_openGLContext.attachTo(*this);

      addMouseListener(this, true);
      addKeyListener(this);

      setSize(1024, 1024);
    }
    ~VizComponent()
    {
      //shutdownOpenGL();
      //for(auto& kv : m_shapes) kv.second.
      //m_shapes.clear();
      m_openGLContext.detach();
    }

    // Mouse 
    void mouseWheelMove(MouseEvent const& event, MouseWheelDetails const& wheel) override
    {
      //auto chng      =  m_camZ * wheel.deltaY * 0.20f;
      ////auto sgn       =  signbit(chng);
      //auto clmpChng  =  max(0.1f, abs(chng) );
      //auto sgnChng   =  copysign(clmpChng, chng);
      //m_camZ        -=  sgnChng;

      //copysign(max(1.f,abs(m_camZ)),m_camZ)
      //m_camZ  *=  wheel.deltaY * 0.25f;
      //m_camZ  -=  m_camZ * wheel.deltaY * 0.25f;

      //float sn  =  m_camZ==0? copysign(1.f,wheel.deltaY) :  copysign(1.f, m_camZ * wheel.deltaY );
      //m_camZ   -=  max(1.f,abs(m_camZ))*sn * wheel.deltaY * 0.25f;
      //m_camZ  -=  copysign(max(1.f,abs(m_camZ)),-wheel.deltaY) * wheel.deltaY * 0.25f;

      m_camZ   -=  m_camZ * wheel.deltaY * 0.25f;
      this->repaint();
    }
    void mouseMove(MouseEvent const& event) override
    {
      auto p  =  event.getPosition();

      int  x  =  p.getX();
      int  y  =  this->getHeight() - p.getY();

      m_glX   =  x;
      m_glY   =  y;
      
      this->repaint();
    }
    void mouseDrag(MouseEvent const& event) override
    {
      auto p   =  event.getPosition();

      int  x   =  p.getX();
      int  y   =  this->getHeight() - p.getY();

      m_mvX    =  x - m_glX;
      m_mvY    =  y - m_glY;

      m_glX    =  x;
      m_glY    =  y;

      if(event.mods.isLeftButtonDown()) {
        auto zfactor = max(abs(m_camZ), abs(1.f/m_camZ));
        m_posX  +=  m_mvX * m_sense * zfactor;
        m_posY  +=  m_mvY * m_sense * zfactor;
      } else if(event.mods.isRightButtonDown()) {
        m_rotY  +=  m_mvX * m_rotSense;   // rotY uses mvX so that side to side rotates around the object
        m_rotX  -=  m_mvY * m_rotSense;   // minus flips the rotation to dragging up rotates the camera towards the top looking down
      }

      this->repaint();

      //m_posX  +=   (float)(event.getDistanceFromDragStartX() * 0.001);
      //m_posY  +=  -(float)(event.getDistanceFromDragStartY() * 0.001);
    }
    // End Mouse

    // Keyboard
    bool keyPressed(const KeyPress &key, Component *originatingComponent)        override
    {
      //if( key == KeyPress::homeKey )
      if( key.getKeyCode() ==  KeyPress::spaceKey )
      {
        m_posX  =  -0.5f;
        m_posY  =  -0.5f;
        m_rotX  =   0.0f;
        m_rotY  =   0.0f;
      }
      m_status = toString(  key.getKeyCode() );
      this->repaint();
      return true;
    }
    // End Keyboard

    // Shared Variables
    void resetCam()
    {
      m_posX  =  -0.5f;
      m_posY  =  -0.5f;
      m_camZ  =  -1.5f;
      m_rotX  =    0.f;
      m_rotY  =    0.f;
    }
    void incPtSz()
    {
      m_ptSz += 1;
    }
    void decPtSz()
    {
      if(m_ptSz>1) m_ptSz -= 1;
    }
    // End Sahred

    // All called from the openGL thread?
    void    newOpenGLContextCreated()   override
    {
      createShaders();
      m_contextFinished = true;

      m_mvX   =  m_mvY   =      0;
      m_posX  =  m_posY  =  -0.5f;
    }
    void       openGLContextClosing()   override
    {
      shader      =  nullptr;
      attributes  =  nullptr;
      uniforms    =  nullptr;
      m_shapes.clear();
    }
    void              doVizCommands(OpenGLContext* ctx)
    {
      using namespace std;

      unique_lock<mutex> renderLock(m_renderMut);
        //VizCommand vc;

        const int   MXSZ  =  64;
        size_t      len   =   0;
        VizCommand  vcs[MXSZ];

        while(len = m_vizQ.try_dequeue_bulk(vcs,MXSZ))
        {
          set<VizCommand> comSet(vcs,vcs+len);  // only allow one command per name
          for(auto& vc : comSet)                // a copy, but oh well
          {
            //auto key = vc.meta.name;
            auto key = vc.meta.id;
            if(vc.remove)
              m_shapes.erase(key);
            else if(vc.meta.dataType && vc.meta.bytes.size()>0)
            {
              auto   m            =  vc.meta;
              //DataBytes cpy      =  vc.meta.bytes;
              auto   dataTypePtr  =  (DataType*)m.dataType;
              void*  ivVoid       =  DataTypeLoadIfNotNull(dataTypePtr, m.bytes.data(), m.bytes.size() );
              auto   idxVerts     =  (IndexedVerts*)ivVoid;
                Shape shape(ctx, idxVerts);
                shape.visible(true);
                m_shapes[key]     =  VizDataShape(vc.viz, dataTypePtr, move(shape));
              DataTypeDestroyIfNotNull(dataTypePtr, idxVerts);
            }
          }
        }
      renderLock.unlock();
    }
    void                 frameSetup()
    {
      jassert(OpenGLHelpers::isContextActive());

      const float desktopScale  =  (float)m_openGLContext.getRenderingScale();
      OpenGLHelpers::clear(Colour::greyLevel(0.1f));

      glEnable(GL_BLEND);
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_DEPTH);
      glEnable(GL_DEPTH_TEST);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      int w = roundToInt(desktopScale*getWidth()  );
      int h = roundToInt(desktopScale*getHeight() );
      glViewport(0, 0, w, h);

      //if(uniforms->m_tex0 != nullptr)
      //    uniforms->m_tex0->set(0);
    }
    void               renderOpenGL()   override
    {
      if(m_nxtFilled) {
        doVizCommands(&m_openGLContext);
        m_nxtFilled.store(false);
      }

      //m_status = toString(m_frames);
      m_frames++;

      frameSetup();

      shader->use();
      if(uniforms->projectionMatrix != nullptr)
          uniforms->projectionMatrix->setMatrix4(getProjectionMatrix().mat, 1, false);
      if(uniforms->viewMatrix != nullptr)
          uniforms->viewMatrix->setMatrix4(getViewMatrix().mat, 1, false);

      glPointSize( (float)m_ptSz );
      for(auto& kv : m_shapes) {
        auto& vds = kv.second;
        kv.second.shape.draw(m_openGLContext, *attributes, shader.get()->getProgramID() );
      }

      glReadPixels(m_glX, m_glY, 1, 1, GL_RGB, GL_FLOAT, m_rgb.data());

      frameFinish();
    }
    void                frameFinish()
    {
      //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      // Reset the element buffers so child Components draw correctly
      m_openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
      m_openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);    
    }
    // End calls from the openGL thread

    void                  paint(Graphics&     g)   override
    {
      static i64 paints = 0;

      //string vizStr  =  toString( m_shapes.size() );
      string vizStr  =  toString(m_camZ, "   ", m_glX,"   ",m_glY,"   ",
                                 m_rgb[0],"   ",m_rgb[1],"   ",m_rgb[2]);

      g.setColour(Colours::white);
      g.setFont(15);
      g.drawText(vizStr, 15, 5, 1024, 30, Justification::left);
      //g.drawLine(20, 20, 170, 20);
      //g.drawLine(20, 50, 170, 50);
      //g.drawText( status() , 25, 50, 1024, 30, Justification::left);
    }
    void                resized()                  override
    {
      // This is called when the MainContentComponent is resized.
        // If you add any child components, this is where you should
        // update their positions.
      //m_tstSlider.setBounds(500,500,500,500);
      //m_tstCB.setBounds(100,100,200,200);
      //m_tstTLB.setBounds(300,300,200,200);
      //m_tstLB.setBounds(0,400,300,300);
      //m_tstButton.setBounds(0,200,100,100);
      //m_tstPP.setBounds(250,500,500,400);
    }
    string&              status(string const& s)
    {
      m_status = s;
      return m_status;
    }
    auto                 status() const -> string const&
    {
      return m_status;
    }
    bool                command(VizCommand vc)                                        const   //  Viz* v, void* data) const
    {
      //while( !m_contextFinished || m_nxtFilled ){ yield();}

      //unique_lock<mutex> renderLock(m_renderMut);
      bool ok = m_vizQ.enqueue(move(vc));
      m_nxtFilled.store(true);
      return ok;

      //m_nxtFilled.store(true);
      //renderLock.unlock();
      //
      //while( m_nxtFilled ){ yield(); }
      //
      //return vc.idx;
    }
    bool                command(Viz*        v, MetaData        md, bool remove=false) const
    {
      int idx = m_curShapeId.fetch_add(1);
      return command( VizCommand(v, move(md), remove) );
    }
    int        addVisualization(Viz*        v, MetaData const& md)   // void* data) const
    {
      while( !m_contextFinished || m_nxtFilled ){ yield();}

      unique_lock<mutex> renderLock(m_renderMut);
        int idx = this->command(v, md);
        m_nxtFilled.store(true);
      renderLock.unlock();

      while( m_nxtFilled ){ yield(); }

      return idx;
 
      //while( !m_contextFinished || m_nxtFilled ){ yield();}
      //
      //unique_lock<mutex> renderLock(m_renderMut);
      //  VizCommand vc = {v, data, m_curShapeId.fetch_add(1) };
      //  m_vizQ.enqueue(vc);
      //  m_nxtFilled.store(true);
      //renderLock.unlock();
      //
      //while( m_nxtFilled ){ yield(); }

      //while(!m_contextFinished || m_nxtFilled.load()){ yield();}
      //  
      //unique_lock<mutex> renderLock(m_renderMut);
      //  //m_nxtVizData = move(vdVec);
      //  m_curShapeId = m_curShapeId.fetch_add(1);
      //  i32 idx = m_curShapeId;
      //  m_shapes.insert( {idx, VizDataShape(v,data,Shape()) } );
      //  m_nxtFilled.store(true);
      //renderLock.unlock();
      //  
      //while(m_nxtFilled.load()){ yield(); }
    }
    int                 nextIdx()
    {
      return m_curShapeId.fetch_add(1);
    }
    void   destroyVisualization(string name, Viz* v)       const
    {
      VizCommand vc(v, MetaData(0,name,"",{}), true);
      this->command( vc );
    }
    void   destroyVisualization(i64     idx)               const
    {
      VizCommand  vc(nullptr, MetaData(idx,"","",{}), true);
      vc.viz     =   nullptr;
      vc.remove  =   true;
      //vc.idx     =   idx;      
      this->command( vc );
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VizComponent)
};


#endif










//mutable float m_rgb[3];
//mutable float m_r    =  0.f;
//mutable float m_g    =  0.f;
//mutable float m_b    =  0.f;


