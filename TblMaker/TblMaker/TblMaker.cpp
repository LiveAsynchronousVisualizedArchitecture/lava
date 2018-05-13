
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
#include "../../no_rt_util.h"
#include "../../tbl.hpp"

namespace 
{
  const int       topMargin   = 20;
  const char*      typeStrs[] = {"i64","u64","f64","tbl"};
  const char*       lblStrs[] = {"wut","skadoosh","squidoosh","table"};

  Fl_Double_Window*     win;
  Fl_Tree*             tree;
  Fl_Tree_Item*          ti;
  Fl_Tree_Item*         rti;
  Fl_Choice*       typeMenu;

  void              cb_quit(Fl_Widget*, void*){exit(0);}
  static void       cb_tree(Fl_Tree*, void*){}
  static void   cb_typeMenu(Fl_Widget* widg)
  {
    auto  mb = (Fl_Menu_Button*)widg;
    auto   i = mb->value();
    //mb->label(typeStrs[i]);
    mb->label(lblStrs[i]);

    //int lw=0,lh=0;
    //mb->measure_label(lw, lh);
    //auto grp = mb->parent();
    //int grpX = grp->x();
    //mb->resize(grpX + lw, mb->y(), lw, mb->h() );

    //mb->position(w,0);
    //ti->x();
    //ti->

    tree->redraw();

    printf("val: %d str: %s \n", i, typeStrs[i]);
  }
  
  Fl_Menu_Item menutable[] = {
    {"&File",0,0,0,FL_SUBMENU},
      {  "&Open",  FL_ALT+'o',        0,  0,  FL_MENU_HORIZONTAL},
      { "&Close",           0,        0,  0,  FL_MENU_HORIZONTAL},
      {  "&Quit",  FL_ALT+'q',  cb_quit,  0,     FL_MENU_DIVIDER},
  };
}

int main(int argc, char ** argv)
{
  SECTION(initialization)
  {
    win  = nullptr;
    tree = nullptr;
  }

  win = new Fl_Double_Window(512,512,"");
  win->resizable(win);

  Fl_Menu_Bar menubar(0,0,512,30); 
  menubar.menu(menutable);

  SECTION(create tree)
  {
    tree = new Fl_Tree(topMargin, 
                       topMargin + menubar.h(), 
                       win->w() / 2,
                       win->h() - topMargin*2 - menubar.h(), 
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

    rti = tree->root();
    typeMenu = new Fl_Choice(1, 1, 50, 50);
    SECTION(menu button)
    {
      typeMenu->align( Fl_Align(FL_ALIGN_RIGHT) );
      auto typeStrsSz = sizeof(typeStrs) / sizeof(void*);
      TO(typeStrsSz,i){
        typeMenu->add( typeStrs[i] );
      }
    }
    typeMenu->show();
    typeMenu->callback(cb_typeMenu);
    typeMenu->value(0);
    typeMenu->do_callback();
    ti = tree->add(rti, "wat");
    ti->widget(typeMenu);
  }

  SECTION(side group)
  {
    auto    grp = new Fl_Group(tree->x() + tree->w() + topMargin, 
                               tree->y(),
                               tree->w() - topMargin*3,
                               tree->h(), 
                               "");
    grp->resizable(grp);
    grp->begin();
    Fl_Box*  bx = new Fl_Box(tree->x() + tree->w() + topMargin, 
                             tree->y(),
                             tree->w() - topMargin*3,
                             tree->h(), 
                             "");
    bx->box(FL_GTK_DOWN_BOX);
    bx->color(FL_DARK1);
    bx->labelsize(12);
    bx->align(Fl_Align(FL_ALIGN_TOP));

    grp->end();
  }

  win->end();
  win->show(0,nullptr);


  return Fl::run();
}






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
