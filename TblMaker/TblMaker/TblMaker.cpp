
//#define _DEBUG

#include <stdlib.h>
#include <stdio.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Tooltip.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <vector>
#include "../../no_rt_util.h"
#include "../../tbl.hpp"
//#include "../../simdb.hpp"

using      str  =  std::string;
using   vec_u8  =  std::vector<u8>;
using  vec_vu8  =  std::vector<vec_u8>;
using  vec_str  =  std::vector<std::string>;
using vec_chce  =  std::vector<Fl_Choice*>;
using   vec_ti  =  std::vector<Fl_Tree_Item*>;

const int       topMargin   = 20;
const char*      typeStrs[] = {"i64","u64","f64","tbl"};
const char*       lblStrs[] = {"wut","skadoosh","squidoosh","table"};

struct BrandisherData
{
  tbl                     t;

  Fl_Double_Window*     win;
  Fl_Tree*             tree;
  Fl_Tree_Item*          ti;
  Fl_Tree_Item*         rti;
  Fl_Choice*       typeMenu;
  Fl_Group*          rgtGrp;
  Fl_Menu_Bar*      menubar;
  //Fl_Menu_Bar       menubar;

  vec_ti          treeItems;
  vec_str          treeStrs;
  vec_chce         treeChcs;
};

static BrandisherData   bd;
//static simdb            db;

namespace 
{
  static Fl_Menu_Item keyTypes[] = 
  {
    {  "i64",   0,  0,  0,  FL_MENU_HORIZONTAL},
    {  "u64",   0,  0,  0,  FL_MENU_HORIZONTAL},
    {  "f64",   0,  0,  0,  FL_MENU_HORIZONTAL},
    {  "tbl",   0,  0,  0,  FL_MENU_DIVIDER},
    {0}
  };

  static void   cb_typeMenu(Fl_Widget* widg)
  {
    auto chce = (Fl_Choice*)widg;
    auto    i = chce->value();

    //auto  mb = (Fl_Menu_Button*)widg;
    //auto   i = mb->value();
    //mb->label(typeStrs[i]);
    //mb->label(lblStrs[i]);

    //int lw=0,lh=0;
    //mb->measure_label(lw, lh);
    //auto grp = mb->parent();
    //int grpX = grp->x();
    //mb->resize(grpX + lw, mb->y(), lw, mb->h() );

    //mb->position(w,0);
    //ti->x();
    //ti->

    //printf("val: %d str: %s \n", i, typeStrs[i]);

    bd.tree->redraw();

    printf("val: %d str: %s \n", i, keyTypes[i].text );
  }
  static vec_u8    readFile(const char* path)
  {
    vec_u8 ret;

    FILE *f = fopen(path, "rb");
    if(!f) return ret;

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  //same as rewind(f);

    ret.resize(fsize);

    auto rdSz = fread(ret.data(), 1, fsize, f);
    fclose(f);

    ret.resize(rdSz);

    return ret;
  }
  static tbl        loadTbl(const char* path)
  {
    using namespace std;
    
    vec_u8  bytes = readFile(path);
    tbl    refTbl(bytes.data());
    tbl     cpTbl = refTbl;

    return move(cpTbl);
  }
  static void   rebuildTree(tbl const&     t, const char* path)
  {
    bd.treeItems.clear();
    bd.treeStrs.clear();
    bd.tree->clear();
    
    bd.tree->end();
     bd.tree->root_label(path);
    bd.tree->begin();
    
    SECTION(make menu buttons from the keys)
    {
      int cnt=0;
      for(auto kv : t)
      {
        //Fl_Choice* chce = new Fl_Choice(1, 1, 50, 50);

        Fl_Choice* chce = new Fl_Choice(1, 1, 50, 50);
        bd.treeChcs.push_back( chce );
        chce->align( Fl_Align(FL_ALIGN_RIGHT) );

        bd.treeStrs.emplace_back( kv.key );
        //chce->label( bd.treeStrs.back().c_str() );
        printf("%s \n", kv.key);

        chce->menu( keyTypes );
        chce->show();
        chce->callback(cb_typeMenu);
        chce->value(0);
        chce->do_callback();

        bd.treeItems.push_back( bd.tree->add(kv.key) );
        bd.treeItems.back()->widget(chce);
        
        ++cnt;
      }

      TO(cnt,i){
        bd.treeChcs[i]->label( bd.treeStrs[i].c_str() );
      }
    }

    //auto typeStrsSz = sizeof(typeStrs) / sizeof(void*);
    //TO(typeStrsSz,i){
    //  chce->add( typeStrs[i] );
    //}
  }
  static void       cb_open(Fl_Widget*, void*)
  {
    Fl_Native_File_Chooser fc;
    fc.title("Choose Tbl File");
    fc.type(Fl_Native_File_Chooser::BROWSE_FILE);
    //fc.filter("Text\t * .txt\n C Files\t * .{cxx,h,c}");
    fc.filter("Tbl Files\t * .{tbl,const}");
    fc.directory("");                                            // default directory to use // Show native chooser
    int userAction = fc.show();
    switch( userAction ){
      case -1: printf("ERROR: %s\n", fc.errmsg()); break;        // ERROR
      case  1: printf("CANCEL\n"); break;                        // CANCEL
      default: printf("PICKED: %s\n", fc.filename()); break;     // FILE CHOSE  {
    }

    bd.t = loadTbl(fc.filename());

    rebuildTree(bd.t, fc.filename());

    //return userAction;
  }

  static void       cb_quit(Fl_Widget*, void*){exit(0);}
  static void       cb_tree(Fl_Tree*, void*){}

  static Fl_Menu_Item menutable[] = 
  {
    {"&File",0,0,0,FL_SUBMENU},
      {  "&Open",  FL_ALT+'o',  cb_open,  0,  FL_MENU_HORIZONTAL},
      { "&Close",           0,        0,  0,  FL_MENU_HORIZONTAL},
      {  "&Quit",  FL_ALT+'q',  cb_quit,  0,     FL_MENU_DIVIDER},
      {0},
    {0}
  };
}


int main(int argc, char ** argv)
{
  SECTION(initialization)
  {
    bd.win       =  nullptr;
    bd.tree      =  nullptr;
    bd.rti       =  nullptr;
    bd.typeMenu  =  nullptr;
    bd.menubar   =  nullptr;
  }

  bd.win = new Fl_Double_Window(512,512,"");
  bd.win->resizable(bd.win);

  Fl_Menu_Bar menubar(0,0,512,30);
  menubar.menu(menutable);

  //bd.menubar = new Fl_Menu_Bar(0,0,512,30); 
  //bd.menubar->menu(menutable);

  SECTION(create tree)
  {
    auto tree = bd.tree = new Fl_Tree(topMargin, 
                       topMargin + menubar.h(), 
                       bd.win->w() / 2,
                       bd.win->h() - topMargin*2 - menubar.h(), 
                       nullptr);
    tree->resizable(tree);
    tree->begin();
      tree->root_label("Tbl Root");
      tree->tooltip("Tbl");
      tree->box(FL_DOWN_BOX);
      tree->color((Fl_Color)55);
      tree->selection_color(FL_SELECTION_COLOR);
      tree->labeltype(FL_NORMAL_LABEL);
      tree->labelfont(0);
      tree->labelsize(14);
      tree->labelcolor(FL_FOREGROUND_COLOR);
      tree->callback((Fl_Callback*)cb_tree, (void*)(1234));
      tree->align(Fl_Align(FL_ALIGN_TOP));
      tree->when(FL_WHEN_RELEASE);
    tree->end();

    bd.rti = tree->root();

    //bd.typeMenu = new Fl_Choice(1, 1, 50, 50);
    //SECTION(menu button)
    //{
    //  bd.typeMenu->align( Fl_Align(FL_ALIGN_RIGHT) );
    //  auto typeStrsSz = sizeof(typeStrs) / sizeof(void*);
    //  TO(typeStrsSz,i){
    //    bd.typeMenu->add( typeStrs[i] );
    //  }
    //}
    //bd.typeMenu->show();
    //bd.typeMenu->callback(cb_typeMenu);
    //bd.typeMenu->value(0);
    //bd.typeMenu->do_callback();
    //bd.ti = tree->add(bd.rti, "wat");
    //bd.ti->widget(bd.typeMenu);
  }
  SECTION(side group)
  {
    bd.rgtGrp = new Fl_Group(bd.tree->x() + bd.tree->w() + topMargin, 
                               bd.tree->y(),
                               bd.tree->w() - topMargin*3,
                               bd.tree->h(), 
                               "");
    bd.rgtGrp->resizable(bd.rgtGrp);
    bd.rgtGrp->begin();
    Fl_Box*  bx = new Fl_Box(bd.tree->x() + bd.tree->w() + topMargin, 
                             bd.tree->y(),
                             bd.tree->w() - topMargin*3,
                             bd.tree->h(), 
                             "");
    bx->box(FL_GTK_DOWN_BOX);
    bx->color(FL_DARK1);
    bx->labelsize(12);
    bx->align(Fl_Align(FL_ALIGN_TOP));

    bd.rgtGrp->end();
  }

  bd.win->end();
  bd.win->show(0,nullptr);

  const char* pth = "H:\\projects\\lava\\lava\\TblMaker\\CameraParams.CamParam.const";
  bd.t = loadTbl(pth);
  rebuildTree(bd.t, pth);

  return Fl::run();
}






//Fl_Menu_Bar menubar(0,0,512,30);
//menubar.menu(menutable);
//
//bd.menubar = Fl_Menu_Bar(0,0,512,30);
//bd.win->add(*bd.menubar);

//void          openFiles(vec_str const& files)
//{
//  int i = 0;
//  for(auto& f : files){
//    //Println("file ", i, ": ", f);
//    //path p(f);
//    str          p = f.data();
//    auto fileBytes = readFile(f.c_str());
//    auto  justName = p.filename().replace_extension().string();
//    //db.put(justName, fileBytes);
//  }
//  //regenTblInfo();
//}

//typeMenu = new Fl_Menu_Button(bx->x(), bx->y(), bx->w(), bx->h() );
//typeMenu = new Fl_Menu_Button(1,1, 200, 75);

//tree->add("wat");
//auto ti = tree->add("wat/skidoosh");
//ti = tree->add("wat/skidoosh");
//Fl_Menu_Button* typeMenu = new Fl_Menu_Button(200,200,100,25);
//ti->widget(typeMenu);
//
//Fl_Tree_Item ti;
//tree->add("squidoosh", &ti);
//
//tree->end();

//Fl_Box*    bx = new Fl_Box(350, 23, 330, 389, "Tree Globals");
//
//topMargin + menubar.h(),
//
//auto typeMenu = new Fl_Choice(0,0,150,50);

//o->tooltip("These controls only affect the selected items. If no items are selected, all existing items in tree are modified.");
//o->box(FL_GTK_DOWN_BOX);
//o->color(FL_DARK1);
//o->labelsize(12);
//o->align(Fl_Align(FL_ALIGN_TOP));

//Fl_Menu_Item* typeList = new Fl_Menu_Item[4];
//for(int i=0; i<4; ++i){
//  typeList[i].text = nullptr;
//  //typeList->activate();
//}
//typeList->add("i64", 0, nullptr);
//typeList->add("u64", 0, nullptr);
//typeList->add("f64", 0, nullptr);

//typeMenu->set_changed();
//
//printf("val: %d \n", typeMenu->value() );

//cb_typeMenu( (Fl_Widget*)typeMenu );
//typeMenu->add("i64");
//typeMenu->add("u64");
//typeMenu->add("f64");
//typeMenu->copy(typeList);
//typeMenu->menu(typeList);
//auto  FL UP BOX

//auto chce = new Fl_Choice(520, 273, 140, 21, "Selection Mode");
//typeList[0].text = "wat skidoosh";
//Fl_Labeltype lblType;
//typeList[0].labeltype( = "wat skidoosh";

//typeList[1].text = nullptr;
//chce->menu(typeList);
//grp->end();

//Fl_Button *b = new Fl_Button(10, 10, 130, 30, "Fl_Button");
//b->tooltip("This is a Tooltip.");
//new Fl_Return_Button(150, 10, 160, 30,  "Fl_Return_Button");
//new Fl_Repeat_Button(10,50,130,30,      "Fl_Repeat_Button");
//new Fl_Light_Button(10,90,130,30,       "Fl_Light_Button");
//new Fl_Round_Button(150,50,160,30,      "Fl_Round_Button");
//new Fl_Check_Button(150,90,160,30,      "Fl_Check_Button");

//
//window->show(argc,argv);

//Fl_Menu_Button*  typeMenu;
//
//Fl_Window *window = new Fl_Window(512,512);
