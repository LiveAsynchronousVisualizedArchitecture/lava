//Screen         screen;
//Window*        keyWin = nullptr;
//Window*         dbWin = nullptr;
//BoxLayout*     keyLay = nullptr;
//ComboBox*       dbLst = nullptr;
//int          dbLstIdx = -1;
//NVGcontext*       nvg = nullptr;
//veci           dbIdxs;
//vecstr        dbNames;
//
//f64 avgFps=60,prevt=0,cpuTime=0, t=0, dt=0;
//
//
//FormHelper*      keys = nullptr;

//new (&db) simdb("test", 4096, 1<<14);             // inititialize the DB with placement new into the data segment
//
//vd.ui.screen;
//vd.ui.dbIdxs;
//vd.ui.dbNames;

//
//printf(" selected %d \n",i);

// start nanogui test stuff
//enum test_enum {
//  Item1 = 0,
//  Item2,
//  Item3
//};
//
//using uWindow = std::unique_ptr<nanogui::Window>;
//
//FormHelper*       gui = nullptr;
//Window*           win = nullptr;
//bool          enabled = true;
//bool             bvar = true;
//int              ivar = 12345678;
//double           dvar = 3.1415926;
//float            fvar = (float)dvar;
//std::string    strval = "A string";
//test_enum     enumval = Item2;
//Color          colval(0.5f, 0.5f, 0.7f, 1.f);
// end nanogui test stuff

//dbNames = simdbNames_to_name(simdb_listDBs());                                         // all of these are globals
//vecstr names = dbNames;

//  auto dbKeys = db.getKeyStrs();                                      // Get all keys in DB - this will need to be ran in the main loop, but not every frame
//  dbKeys      = shapesFromKeys(db, move(dbKeys), &vd);
//  dbKeys      = eraseMissingKeys(move(dbKeys), &vd.shapes);
//  
//  sort( ALL(dbIdxs) );                                                // sort the indices so the largest are removed first and the smaller indices don't change their position
//  FROM(dbIdxs.size(),i){ keyWin->removeChild(dbIdxs[i]); }
//  dbIdxs.resize(0);
//  dbIdxs.shrink_to_fit();
//  for(auto key : dbKeys)                                              // add the buttons back and keep track of their indices
//  {
//    auto b = new Button(keyWin, key.str);
//    int  i = keyWin->childIndex(b);
//    if(i > -1){ dbIdxs.push_back(i); }
//    b->setFlags(Button::ToggleButton);
//    b->setChangeCallback( [k=key.str](bool pushed){ buttonCallback(k,pushed); } );
//    b->setPushed(vd.shapes[key.str].active);
//    b->setFixedHeight(25);
//  }
//  screen.performLayout();
//
//  vd.keyRefreshClock -= vd.keyRefresh;
//  vd.verRefreshClock -= vd.verRefresh;
//
//  if(vd.keyRefreshClock > vd.keyRefresh)                                 // if there is already enough time saved up for another update make sure that less that two updates worth of time is kept 
//    vd.keyRefreshClock = vd.keyRefresh + fmod(vd.keyRefreshClock, vd.keyRefresh);
//}else if( vd.verRefreshClock > vd.verRefresh ){
//  for(auto& kv : vd.shapes){
//    if(kv.second.active){
//      updateKey(db, kv.first, kv.second.version, &vd);
//    }
//  }
//  vd.verRefreshClock -= vd.verRefresh;
//
//  if(vd.verRefreshClock > vd.verRefresh)                                // if there is already enough time saved up for another update make sure that less that two updates worth of time is kept 
//    vd.verRefreshClock = vd.verRefresh + fmod(vd.verRefreshClock, vd.verRefresh);

//SECTION(main loop from nanogui / common.cpp)
//{
//  int numScreens = 0;
//  for(auto kv : __nanogui_screens){
//    Screen *screen = kv.second;
//    if(!screen->visible()){
//      continue;
//    }else if(glfwWindowShouldClose(screen->glfwWindow())){
//      screen->setVisible(false);
//      continue;
//    }
//
//    screen->drawAll();
//    numScreens++;
//  }
//
//  if(numScreens == 0){
//    //mainloop_active = false;                          /* Give up if there was nothing to draw */
//    break;
//  }
//}

//dbLst = new ComboBox(keyWin, { "  simdb_test  ", "  simdb_viz  ", "  simdb_run  " });
//dbLst->setChangeCallback([](bool pressed){
//  if(pressed){
//    dbNames = simdb_listDBs();                                         // all of these are globals
//    dbLst->setItems(dbNames);
//    screen.performLayout();
//  }
//  //printf(" pressed: %d \n", pressed); 
//});

//#define NANOVG_GL3_IMPLEMENTATION
//#include <nanovg_gl.h>

//SECTION(db selector)
//{
//}
//
//dbWin->center();

//auto dbLay = new BoxLayout(Orientation::Vertical, Alignment::Fill, 5, 5);
//dbWin      = new Window(&screen, "Keys");
//
//dbWin->setLayout(keyLay);
//
//auto dbPth = new TextBox(dbWin, "simdb_");

//if(dbLst){ 
//  dbLst->setPushed(false);
//  dbLst->setVisible(false);
//}
//
//keyWin->removeChild(dbLst);
//keyWin->removeChildren();
//
//for(auto i : dbIdxs){ keyWin->removeChild(i); }

//keys   = new FormHelper(&screen);
//
//auto dbLst = new ComboBox(dbWin, { "simdb_test", "simdb_viz", "simdb_run" } );
//auto dbSel = new nanogui::detail::FormWidget(dbWin);
//dbWin->addGroup("Complex types");
//  gui->addVariable("Enumeration", enumval, enabled)
//    ->setItems({ "Item 1", "Item 2", "Item 3" });
//  gui->addVariable("Color", colval);

//NAMESPACE_BEGIN(nanogui)
//  extern std::map<GLFWwindow *, Screen *> __nanogui_screens;
//NAMESPACE_END(nanogui)

//vd.camera.mouseDelta      = vec2(0, 0);
//vd.camera.btn2Delta       = vec2(0, 0);

//screen.setWidth(winsz.x());
//
//glReadPixels( (GLint)(vd.camera.mouseDelta.x), (GLint)(vd.camera.mouseDelta.y), 1, 1, GL_RGB, GL_FLOAT, vd.mouseRGB);

//
//vd.ui.bgclr         =  nk_rgb(16,16,16);         // darker than this may risk not seeing the difference between black and the background

//SECTION(example 2)
//{
//  //FormHelper *gui = new FormHelper(&screen);  // leak
//  gui = new FormHelper(&screen);                // leak
//
//  //ref<Window> window = gui->addWindow(v2i(10, 10), "Form helper example");
//  //auto window = gui->addWindow(v2i(10, 10), "Form helper example");
//  win = gui->addWindow(v2i(10, 10), "Form helper example");
//
//  gui->addGroup("Basic types");
//  gui->addVariable("bool", bvar);
//  gui->addVariable("string", strval);
//
//  gui->addGroup("Validating fields");
//  gui->addVariable("int", ivar)->setSpinnable(true);
//  gui->addVariable("float", fvar);
//  gui->addVariable("double", dvar)->setSpinnable(true);
//
//  gui->addGroup("Complex types");
//  gui->addVariable("Enumeration", enumval, enabled)
//    ->setItems({ "Item 1", "Item 2", "Item 3" });
//  gui->addVariable("Color", colval);
//
//  gui->addGroup("Other widgets");
//  gui->addButton("A button", []() { std::cout << "Button pressed." << std::endl; });
//}

//screen.setBackground(Color(0.0625f,0.f));                                       //  16 / 256 for a dark background that still leaves enough of a difference to black 
//keyWin = keys->addWindow(v2i(10,10), "Keys");
//keyWin->setTooltip("The Keys in the database");
//keys->addGroup("IdxVrt");
//keyWin->removeChildren();
//((Widget*)keys)->removeChildren();
//keys->addWidget("label label", new Label(keyWin, "One Key"));
//keys->addButton("Key One", []() { std::cout << "One pressed." << std::endl; });
//keys->addButton("Key Two", []() { std::cout << "Two pressed." << std::endl; });

//Label* lbl = new Label(keyWin, key.str);          
//auto tgl = new 
//b->setFlags(Button::ToggleButton);
//button->setCallback(cb);
//if (keyLay->rowCount() > 0)
//keyLay->appendRow(mVariableSpacing);
//keyLay->appendRow(0);
//keyLay->setAnchor(button, AdvancedGridLayout::Anchor(1, keyLay->rowCount() - 1, 3, 1));
//keyWin ->addButton(key.str, [](){ printf("wat\n\n"); });
//
//keys->addWidget("", new Label(keyWin, "One Key"));

//Screen*     screen = nullptr;
//uWindow        keyWin = nullptr;

//screen.setBackground(Color(0, 0, 0, 0));
//
//Theme* scrnThm = screen.theme();
//
//scrnThm->mTransparent = v4f(0,0,0,0);
//scrnThm->mWindowFillFocused   = v4f(0,0,0,0);
//scrnThm->mWindowFillUnfocused = v4f(0,0,0,0);
//
//win->center();
//
//nanogui::mainloop(0);

//set_pos(&cam.pantfm, vec3(0,0,0) );
//
//cam.tfm     =  lookAt(cam.pos, cam.lookAt, cam.up);
//cam.pantfm  =  cam.tfm;
//set_pos(&cam.pantfm, vec3(0,0,0) );

//auto              initNuklear(GLFWwindow* win) -> struct nk_context*
//{
//  struct nk_context*      ctx = nk_glfw3_init(win, NK_GLFW3_INSTALL_CALLBACKS);
//  struct nk_font_atlas* atlas;
//  nk_glfw3_font_stash_begin(&atlas);
//  nk_glfw3_font_stash_end();
//
//  return ctx;
//}

//float bg[4];
//nk_color_fv(bg, vd.ui.bgclr);
//glClearColor(bg[0], bg[1], bg[2], bg[3]);

//int                sidebar(struct nk_context *ctx, struct nk_rect rect, KeyShapes* shps) // VizData* vd)
//{
//  using namespace std;
//
//  nk_flags window_flags = 0;                 /* window flags */
//  struct nk_panel layout;                    /* popups */
//
//  ctx->style.window.header.align = NK_HEADER_RIGHT;   // header_align;   /* window flags */
//
//  if(ctx->active){
//    memcpy( &ctx->active->bounds, &rect, sizeof(struct nk_rect) );
//  }
//
//  if(nk_begin(ctx, &layout, "Overview", rect, window_flags))
//  //if(nk_group_begin(ctx, &layout, "Overview", window_flags))
//  {
//    nk_layout_row_static(ctx, 18, (int)(rect.w-25.f), 1);
//    //nk_layout_row_static(ctx, 18, 120, 1);
//    for(auto& kv : *shps){
//      if(kv.first.length()>0)
//        nk_selectable_label(ctx, kv.first.c_str(), NK_TEXT_RIGHT, &kv.second.active);
//    }
//    //nk_group_end(ctx);
//  }
//  nk_end(ctx);
//
//  return !nk_window_is_closed(ctx, "Overview");
//}

//keyWin->setTheme(new Theme() );
//thm->mTransparent.w() = 0.1f;
//thm->mTransparent = v4f(0.9f, 0.9f, 0.9f, 0.9f);

//keyWin = make_unique<Window>(new Window(&screen, "Keys"));
//keyWin = new Window(&screen, "Keys");
//keyWin->setEnabled(false);
//keyWin->setModal(true);
//keys->add<Label>();

//keyWin->absolutePosition();
//v2i keyspos = v2i(screen.width(), 0);
//printf("\nhit\n");

//nanogui::init();
//screen = new Screen(Vector2i(500, 700), "NanoGUI test");
//screen = new Screen();  // Vector2i(500, 700), "NanoGUI test");

//glDisable(GL_CLIP_PLANE0);
//glDisable(GL_CLIP_PLANE1);
//glDisable(GL_CLIP_PLANE2);
//glDisable(GL_CLIP_PLANE3);
//glDisable(GL_FOG);
//glDisable(GL_CULL_FACE);
//glDisable(GL_SCISSOR_TEST);
//glDisable(GL_STENCIL_TEST);
//glDisable(GL_DEPTH);
//glDisable(GL_DEPTH_TEST);                                    // glDepthFunc(GL_LESS);

//if(cam.dis > 0)
//else
//  nxtDst = cam.dist * 1.f/abs(sense);
//
//if     (nxtDst < prvDst*0.9f) cam.dist = prvDst*0.9f;
//else if(nxtDst > prvDst/0.9f) cam.dist = prvDst/0.9f;

//SECTION(draw nuklear)
//{
//  nk_glfw3_new_frame();
//  vd.ui.rect = winbnd_to_sidebarRect((float)vd.ui.w, (float)vd.ui.h);
//
//  //struct nk_panel layout;
//  //if(nk_begin(vd.ctx, &layout, "fps", vd.ui.rect, NK_WINDOW_BACKGROUND))
//  //{
//
//  sidebar(vd.ctx, vd.ui.rect, &vd.shapes);                     // alters the shapes by setting their active flags
//
//  //nk_flags window_flags = NK_WINDOW_DYNAMIC;                 /* window flags */
//    //struct nk_rect fpsRect;
//    //struct nk_panel fpsLayout;
//    //fpsRect.x = fpsRect.y = 0.f;
//    //fpsRect.w = fpsRect.h = 256.f;
//    //if(nk_group_begin(vd.ctx, &fpsLayout, "fps", window_flags)){
//    //  nk_layout_row_begin(vd.ctx, NK_DYNAMIC, 20, 3);
//    //    nk_layout_row_push(vd.ctx, 1.f);
//    //    nk_label(vd.ctx, "Frames Per Second: ", NK_TEXT_LEFT);
//    //  nk_layout_row_end(vd.ctx);
//    //
//    //  nk_group_end(vd.ctx);
//    //}
//  //}  
//  //nk_end(vd.ctx);
//  
//  PRINT_GL_ERRORS
//}

//
//nk_glfw3_shutdown();

//glReadPixels( (GLint)(vd.camera.xDiff * vd.ui.w), (GLint)(vd.camera.yDiff * vd.ui.h), 1, 1, GL_RGB, GL_FLOAT, vd.mouseRGB);
//printf("%f %f: %f %f %f \n", vd.camera.xDiff, vd.camera.yDiff, vd.mouseRGB[0], vd.mouseRGB[1], vd.mouseRGB[2]);

//vd.ctx = initNuklear(vd.win);                    assert(vd.ctx!=nullptr);
//struct nk_color table[NK_COLOR_COUNT];
//table[NK_COLOR_TEXT] = nk_rgba(210, 210, 210, 255);
//table[NK_COLOR_WINDOW] = nk_rgba(0, 0, 0, 0);
//table[NK_COLOR_HEADER] = nk_rgba(51, 51, 56, 220);
//table[NK_COLOR_BORDER] = nk_rgba(46, 46, 46, 255);
//table[NK_COLOR_BUTTON] = nk_rgba(48, 83, 111, 255);
//table[NK_COLOR_BUTTON_HOVER] = nk_rgba(58, 93, 121, 255);
//table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(63, 98, 126, 255);
//table[NK_COLOR_TOGGLE] = nk_rgba(50, 58, 61, 255);
//table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 53, 56, 255);
//table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(48, 83, 111, 255);
//table[NK_COLOR_SELECT] = nk_rgba(57, 67, 61, 0);
//table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(48, 83, 111, 128);
//table[NK_COLOR_SLIDER] = nk_rgba(50, 58, 61, 255);
//table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(48, 83, 111, 245);
//table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
//table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
//table[NK_COLOR_PROPERTY] = nk_rgba(50, 58, 61, 255);
//table[NK_COLOR_EDIT] = nk_rgba(50, 58, 61, 225);
//table[NK_COLOR_EDIT_CURSOR] = nk_rgba(210, 210, 210, 255);
//table[NK_COLOR_COMBO] = nk_rgba(50, 58, 61, 255);
//table[NK_COLOR_CHART] = nk_rgba(50, 58, 61, 255);
//table[NK_COLOR_CHART_COLOR] = nk_rgba(48, 83, 111, 255);
//table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
//table[NK_COLOR_SCROLLBAR] = nk_rgba(50, 58, 61, 255);
//table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(48, 83, 111, 255);
//table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
//table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
//table[NK_COLOR_TAB_HEADER] = nk_rgba(48, 83, 111, 255);
//nk_style_from_table(vd.ctx, table);

//nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
//

/*
* IMPORTANT: `nk_glfw_render` modifies some global OpenGL state
* with blending, scissor, face culling, depth test and viewport and
* defaults everything back into a default state.
* Make sure to either a.) save and restore or b.) reset your own state after
* rendering the UI.
*/

//mat4 view, projection;
//view          = lookAt(vec3(0.f, 0.f, 1.f), vd.camera.lookAt, vd.camera.up);
//projection    = perspective(vd.camera.fov, (GLfloat)1024 / (GLfloat)768, vd.camera.nearClip, vd.camera.farClip);
//vd.camera.tfm = view;
//set_pos( &vd.camera.tfm, vec3(0.f,0.f,3.f) );

//glBegin(GL_LINE);
//  glVertex3f(0,0,0);
//  glVertex3f(camx.x, camx.y, camx.z);
//glEnd();

//cam.pantfm = cam.tfm;
//set_pos(&cam.pantfm, vec3(0,0,0) );
//
//cam.tfm      =  lookAt(cam.pos, cam.lookAt, cam.up);

// vecb del(dbKeys.size(), false);
// /*&& cur->second.version==vs.v*/
//
//del[i] = true;
//dbKeys[i] = {0,""};
//
//vd->shapes.erase(vs.s);
//
//VerStr nxtKey = vs;
//nxtKey.v      = version;
//
//vd->shapes.insert( make_pair(k, s.version) );
//
//TO(del.size(),i){
//  if(del[i]) dbKeys[i] = {0,""};
//}
//
//db.flush();

//
//bool           updateKey(simdb const& db, VerStr const& key, VizData* vd)

//vd.camera.pos += camx * vd.camera.btn2Delta.x;
//vd.camera.pos += camy * vd.camera.btn2Delta.y;
//
//mat4 camMtx = camera_to_mat4(vd.camera);

//vd.camera.fov             = 75.0f;
//vd.camera.mouseDelta      = vec2(0.0f, 0.0f);
//vd.camera.btn2Delta       = vec2(0.0f, 0.0f);
//vd.camera.sensitivity     = 0.01f;
//vd.camera.pansense        = 0.05f;
////vd.camera.viewDirection   = vec3(0.0f, 0.0f, 0.0f);
//vd.camera.pos             = vec3(0,0,-10.0f);
//vd.camera.rot             = vec3(0,0,0);
//vd.camera.lookAt          = vec3(0.0f, 0.0f, 0.0f);
//vd.camera.dist            = 10.0f;
//vd.camera.up              = vec3(0.0f, 1.0f, 0.0f);
//vd.camera.oldMousePos     = vec2(0.0f, 0.0f);
//vd.camera.rightButtonDown = false;
//vd.camera.leftButtonDown  = false;
//vd.camera.nearClip        = 0.01f;
//vd.camera.farClip         = 1000.0f;

//mat4 p;
//mat4 p = translate(mat4(), vd.camera.pos);
//f32   dist = (float)(vd.camera.pos-vd.camera.lookAt).length();
//f32   dstRoot = sqrtf(vd.camera.dist);

//vd->camera.P( vd->camera.P()*sense );
//vd->camera.P( vd->camera.P() * (1.f/abs(sense)) );

//auto P = -vd->camera.P();
//P /= 

//vd->camera.fov += (GLfloat)(yoffset / 10); 
//vd->camera.fov  = max(45.f, min(90.f, vd->camera.fov));

//if(vd->camera.fieldOfView < 45.0f) {
//  vd->camera.fieldOfView = 45.0f;
//}
//
//if(vd->camera.fieldOfView > 90.0f) {
//  vd->camera.fieldOfView = 90.0f;
//}

//vec3               pos(mat4 const& m)
//{ return vec3(m[0].w, m[1].w, m[2].w); }

//const static auto XAXIS = vec4(1.f, 0.f, 0.f, 1.f);
//const static auto YAXIS = vec4(0.f, 1.f, 0.f, 1.f);
//
//mat4 view;
//auto tfm   =  vd.camera.tfm;
//auto P     =  pos(tfm);
//auto plen  =  length(P);
////auto lkmtx =  lookAt(P, vd.camera.lookAt, normalize(vec3(vec4(vd.camera.up,1.f)*tfm)) );
//auto lkmtx =  lookAt(P, vd.camera.lookAt, vd.camera.up);
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
//
//mat4 projection;
//projection = perspective(vd.camera.fov,
//                         (GLfloat)vd.ui.w / (GLfloat)vd.ui.h,
//                         vd.camera.nearClip, 
//                         vd.camera.farClip);
//camMtx = projection * vd.camera.tfm;
////camMtx = vd.camera.tfm * projection;

//if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
//  vd->camera.leftButtonDown = true;
//}
//if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
//  vd->camera.leftButtonDown = false;
//}

//if(vd->camera.leftButtonDown){
//  vd->camera.mouseDelta = (newMousePosition - vd->camera.oldMousePos);
//}else{ vd->camera.btn2Delta = vec2(0,0); }

//vd->camera.mouseDelta.x = fmodf(vd->camera.mouseDelta.x, _2PI);
//vd->camera.mouseDelta.y = fmodf(vd->camera.mouseDelta.y, _2PI);

//vd->camera.mouseDelta.x = wrapAngleRadians(vd->camera.mouseDelta.x);
//vd->camera.mouseDelta.y = wrapAngleRadians(vd->camera.mouseDelta.y);

//vec3 yview = YAXIS * lkmtx;
//vd.camera.lookAt += normalize(xpan) * vd.camera.btn2Delta.x * vd.camera.pansense;
//vd.camera.lookAt += normalize(ypan) * vd.camera.btn2Delta.y * vd.camera.pansense;
//panpos     += normalize(xpan) * vd.camera.btn2Delta.x * vd.camera.pansense;
//panpos     c+= normalize(ypan) * vd.camera.btn2Delta.y * vd.camera.pansense;
//viewP += yview * vd.camera.btn2Delta.y;

//vec3 dbgP = pos(vd.camera.tfm);
//Printf("%f  %f  %f\n", dbgP.x, dbgP.y, dbgP.z);

//using namespace glm;
//vd.camera.tfm = projection * view;
//vd.camera.position        = vec3(0.0f, 0.0f, 3.0f);

////using namespace glm;
//const static auto XAXIS = vec4(1.f, 0.f, 0.f, 1.f);
//const static auto YAXIS = vec4(0.f, 1.f, 0.f, 1.f);

//mat4 view;
////auto tfm = vd.camera.transformMtx;
//auto tfm =  vd.camera.tfm;
//auto P   = -vd.camera.P();   // vec3(-tfm[0].w, -tfm[1].w, -tfm[2].w);   // sbassett - not sure why these need to be negated, I'm guessing the projection turns them negative
//auto len =  length(P);
////view = lookAt(vd.camera.position, vd.camera.viewDirection, vd.camera.up);
//view = lookAt(P, vd.camera.viewDirection, vd.camera.up);
//set_pos(&view, P);
//view = rotate(view, vd.camera.mouseDelta.x, vec3(YAXIS*view) );  // todo: these need to be moved to the cursor callback
//view = rotate(view, vd.camera.mouseDelta.y, vec3(XAXIS*view) ); //vec3(1.0f, 0.0f, 0.0f));
//
////auto scaledP = pos(view)*len;
////set_pos(&view, scaledP );
//
//mat4 projection;
//projection = perspective(vd.camera.fov, (GLfloat)1024 / (GLfloat)768, 0.01f, 100.0f);
//
//// todo: here the coordinate system of the camera will need to be used for the axises 
//// - Z will be easy, it is the vector pointing at the origin (normalized)  
//// X and Y will be (I think) the X and Y vectors each multiplied by the camera's current transformation matrix. 
//// this will create camera transforms always relative to the camera and will hopefully give elegant rotation control  
//vd.camera.tfm = projection * view;
////vd.camera.tfm = view;
//
////auto tfm = vd.camera.transformMtx;
////Printf("%f  %f  %f\n", tfm[0].w, tfm[1].w, tfm[2].w);

//glDrawElements(shp.mode, size/sizeof(uint32_t), GL_UNSIGNED_INT, 0);
//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

// sbassett - not sure why these need to be negated, I'm guessing the projection turns them negative
//
//auto scaledP = pos(view)*len;
//set_pos(&view, scaledP );
//projection = perspective(vd.camera.fov, (GLfloat)1024 / (GLfloat)768, 0.01f, 100.0f);
//
//vd.camera.proj = perspective(vd.camera.fov, (GLfloat)1024 / (GLfloat)768, 0.01f, 100.0f);
//
// todo: here the coordinate system of the camera will need to be used for the axises 
// - Z will be easy, it is the vector pointing at the origin (normalized)  
// X and Y will be (I think) the X and Y vectors each multiplied by the camera's current transformation matrix. 
// this will create camera transforms always relative to the camera and will hopefully give elegant rotation control  

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
//
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
*/

