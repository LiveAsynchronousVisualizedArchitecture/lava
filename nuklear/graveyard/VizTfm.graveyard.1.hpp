
//inline auto    winbnd_to_sidebarRect(float w, float h) -> struct nk_rect
//{
//  using namespace std;
//  
//  float sbw = max(192.f, (1/6.f)*w );            // sbw is sidebar width
//  float sbx = w - sbw;                           // sbx is sidebar x position
//  
//  return nk_rect(sbx, 0, sbw, h);    //struct nk_rect rect =
//}

//auto tfm    =  vd.camera.tfm;
//auto P      =  pos(tfm);
//auto plen   =  length(P);
//auto lkmtx =  lookAt(P, vd.camera.lookAt, normalize(vec3(vec4(vd.camera.up,1.f)*tfm)) );
//view       =  lkmtx;
//set_pos(&view, P);
//    
//vec3 yview = vec3(YAXIS * lkmtx);
//vec3 xview = vec3(XAXIS * lkmtx);
//
//auto sens  =  plen*2.f*PIf * vd.camera.sensitivity;
//auto xrot  =  -vd.camera.mouseDelta.x * sens;
//auto yrot  =  -vd.camera.mouseDelta.y * sens;
//view       =  rotate(view, yrot, xview);
//view       =  rotate(view, xrot, yview);                // todo: these need to be moved to the cursor callback, although input updates are only called once per frame so it likely doesn't matter
//auto viewP =  pos(view);
////tfm        =  lookAt(viewP, vd.camera.lookAt, normalize(vec3(vec4(vd.camera.up,1.f)*lkmtx)) );
//tfm        =  lookAt(viewP, vd.camera.lookAt, vd.camera.up);
//set_pos(&tfm, viewP);
//vd.camera.tfm = tfm;
//
//mat4 panmtx = vd.camera.tfm;
//set_pos(&panmtx, vec3(0));
//vec3 xpan   = vec3(XAXIS * panmtx);
//vec3 ypan   = vec3(YAXIS * panmtx) * -1.f;
//vec3 xofst  = normalize(xpan) * vd.camera.btn2Delta.x * vd.camera.pansense;
//vec3 yofst  = normalize(ypan) * vd.camera.btn2Delta.y * vd.camera.pansense;
//vd.camera.lookAt += xofst;
//vd.camera.lookAt += yofst;
//vec3 panpos = pos(vd.camera.tfm);
//panpos += xofst;
//panpos += yofst;
//set_pos(&vd.camera.tfm, panpos);

//glVertexAttribPointer( (GLuint)AtrId::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);                  // position 
//glVertexAttribPointer( (GLuint)AtrId::NORMAL,   4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 6));   // normals 
//glVertexAttribPointer( (GLuint)AtrId::COLOR,    4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 6));   // color 

//str         vertexCode;
//str       fragmentCode;
//  
//ifstream   vShaderFile;
//ifstream   fShaderFile;
//
//vShaderFile.exceptions(ifstream::badbit);
//fShaderFile.exceptions(ifstream::badbit);
//try
//{
//    vShaderFile.open(vertShader.c_str());
//    fShaderFile.open(fragShader.c_str());
//    stringstream vShaderStream, fShaderStream;
//    vShaderStream << vShaderFile.rdbuf();
//    fShaderStream << fShaderFile.rdbuf();
//    vShaderFile.close();
//    fShaderFile.close();
//    vertexCode = vShaderStream.str();
//    fragmentCode = fShaderStream.str();
//}
//catch(ifstream::failure e)
//{
//    printf("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n");
//}

//
//inline GLuint shadersrc_to_shaderid(str const& vert, str const& frag)

//inline Shape       ivbuf_to_shape( )    //IndexedVerts* iv)
//
//inline GLuint shaderstrs_to_shaderid(str const& vertShader, str const& fragShader)
//{
//  using namespace std;
//  
//  str         vertexCode;
//  str       fragmentCode;
//  ifstream   vShaderFile;
//  ifstream   fShaderFile;
//
//  vShaderFile.exceptions(ifstream::badbit);
//  fShaderFile.exceptions(ifstream::badbit);
//  try
//  {
//      vShaderFile.open(vertShader.c_str());
//      fShaderFile.open(fragShader.c_str());
//      stringstream vShaderStream, fShaderStream;
//      vShaderStream << vShaderFile.rdbuf();
//      fShaderStream << fShaderFile.rdbuf();
//      vShaderFile.close();
//      fShaderFile.close();
//      vertexCode = vShaderStream.str();
//      fragmentCode = fShaderStream.str();
//  }
//  catch(ifstream::failure e)
//  {
//      printf("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n");
//  }
//
//  GLuint vertexShader;
//  vertexShader = glCreateShader(GL_VERTEX_SHADER);
//  const GLchar* vShaderSource = vertexCode.c_str();
//  glShaderSource(vertexShader, 1, &vShaderSource, NULL);
//  glCompileShader(vertexShader);
//  GLint success;
//  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
//  if(!success) {
//      GLchar infoLog[1024];
//      glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
//      printf("Compiling vertex shader failed: %s\n", infoLog);
//  }
//
//  GLuint fragmentShader;
//  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//  const GLchar* fShaderSource = fragmentCode.c_str();
//  glShaderSource(fragmentShader, 1, &fShaderSource, NULL);
//  glCompileShader(fragmentShader);
//  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
//  if(!success) {
//      GLchar infoLog[1024];
//      glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
//      printf("Compiling fragment shader failed: %s\n", infoLog);
//  }
//
//  GLuint shaderProgramId = glCreateProgram();
//  glAttachShader(shaderProgramId, vertexShader);
//  glAttachShader(shaderProgramId, fragmentShader);
//  glLinkProgram(shaderProgramId);
//  glGetShaderiv(fragmentShader, GL_LINK_STATUS, &success);
//  if(!success) {
//      GLchar infoLog[1024];
//      glGetProgramInfoLog(shaderProgramId, 1024, NULL, infoLog);
//      printf("Linking failed: %s\n", infoLog);
//  }
//  glDeleteShader(vertexShader);
//  glDeleteShader(fragmentShader);
//
//  return shaderProgramId;
//}

