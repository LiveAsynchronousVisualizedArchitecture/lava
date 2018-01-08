
// -todo: update nana to 1.5.6 - was already at 1.5.6, the release notes page only went to 1.5.4
// -todo: turn off run time type information
// -todo: try stripping the binary (didn't decrease size, but there are still 116KB of strings left inside)
// -todo: try to use nana from source instead of from a compiled library - works, binary same size
// -todo: bring back altered permission source
// -todo: debug build
// -todo: try stdcall on release build - 50KB smaller?
// -todo: rough in labels for the tbl header data
// -todo: put back pixel_buffer tests
// -todo: refine labels layout
// -todo: make initial array string
// -todo: run event on unfold
// -todo: check expanded()
// -todo: move comments out of main()
// -todo: move no_rt_util.h to higher directory to share 
// -todo: organize in to sections
// -todo: put in open, save, and help/about menu items
// -todo: check on nana drag and drop - works by enabling file drag and drop - nana gives an iterator of files
// -todo: make fold event only print on unfold
// -todo: make array string be created only when unfolded
// -todo: get including simdb to compile - needed strings to be cast to PSTR for windows functions
// -todo: wrap filling treebox in to a function
// -todo: wrap label updating in the regen function

// todo: fix table element count
// todo: make list or tree of simdb databases - just put them at root level in the tree
// todo: make selecting simdb database list the keys in that database - just make dbs the root level tree items
// todo: compile with png and jpeg libs
// todo: implement clearing of the tbl and gui on menu new
// todo: make tables drag and droppable to files
// todo: implement saving of the tbl
// todo: implement opening on drag and drop of a tbl
// todo: make drag and drop of a tbl file drop it into simdb
// todo: make simdb tables drag and droppable to other simdb dbs 
// todo: make table files drag and droppable to sub tables 
// todo: implement opening of the tbl through the menu and file dialog
// todo: try compiling with clang

// idea: should tbls exist as either files, memory, or sub tables?
//       | icon in the tree could show which is which
//       | editing a memory mapped table could edit the table on disk

#include <iostream>
#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/treebox.hpp>
#include <nana/gui/widgets/menu.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/paint/pixel_buffer.hpp>
#include <../source/paint/detail/image_pixbuf.hpp>
#include <../source/paint/pixel_buffer.cpp>
#include "../../no_rt_util.h"
#include "../../simdb.hpp"
#include "../../tbl.hpp"
#include "../../str_util.hpp"

using str = std::string;

tbl<>                          t;
nana::form                    fm;
nana::treebox               tree;
nana::treebox::item_proxy aryViz;
nana::label sz, elems, szBytes, cap, mapcap, owned;

//nana::label sz(fm), elems(fm), szBytes(fm), cap(fm), mapcap(fm), owned(fm);
//nana::form  fm(nana::API::make_center(768, 768));

void regenTblInfo()
{
  SECTION(list simdb files and insert them at the top level of the tree)
  {
    simdb_error err;
    auto dbs = simdb_listDBs(&err);

    for(auto& pth : dbs){
      tree.insert(pth, pth);
    }
  }
  SECTION(insert tbl data into the tree)
  {
    str aszStr = toString("Array Size: ", t.size());
    tree.insert("ary", aszStr);
    aryViz     = tree.insert("ary/arystr", "");

    tree.insert("elems", toString("Elements: ", t.elems()) );
    TO(t.map_capacity(),i){
      auto e = t.elemStart()[i];
      if(e.isEmpty()) continue;

      auto k = e.key;
      tree.insert( toString("elems/", k), toString(k,":  ", e.as<tbl<>::i64>()) ); 
    }
  }
  SECTION(label captions)
  {
    sz.caption(     toString("Size: ",          t.size()        ));
    elems.caption(  toString("Elements: ",      t.elems()       ));
    szBytes.caption(toString("Size in Bytes: ", t.sizeBytes()   ));
    cap.caption(    toString("Capacity: ",      t.capacity()    ));
    mapcap.caption( toString("Map Capacity: ",  t.map_capacity()));
    owned.caption(  toString("Owned: ",         t.owned()? "True" : "False"));
  }
}

int  main()
{
  using namespace  std;
  using namespace nana;
  using namespace nana::paint;

  SECTION(initialize the table)
  {
    TO(12,i){
      t.push_back( (tbl<>::i8)i );
    }
    t("wat")       = 21;
    t("skidoosh")  = 42;
    t("wut")       = 84;
    t("squidoosh") = 168;
    t("table key") = 336;
  }

  SECTION(set up the main window and its events)
  {
    API::enable_dropfiles(fm, true);
    fm.caption("Tabler");
    fm.size({768, 768});
    fm.events().mouse_dropfiles([](const arg_dropfiles& arg)
    {
      Println("drop file event");

      int i = 0;
      for(auto& f : arg.files){
        Println("file ", i, ": ", f);
      }
    });
  }
  SECTION(initialize components with the main window handle)
  {
    sz.create(fm);
    elems.create(fm);
    szBytes.create(fm);
    cap.create(fm);
    mapcap.create(fm);
    owned.create(fm);
  }

  menubar mb;
  SECTION(set up the menu bar)
  {
    mb.create(fm);
    mb.push_back("&File");
    mb.push_back("&Help");
    mb.caption("menu wat");
    mb.enabled(true);

    auto& fileMenu = mb.at(0);
    fileMenu.append("&New", [](auto& itmprxy){
      t.clear();
      SECTION(clear tree)
      {
        //tree.create(fm,true);
        tree.clear();
        tree.erase("ary");
        tree.erase("elems");
      }
      regenTblInfo();
    });
    fileMenu.append("&Open", [](auto& itmprxy){
      cout << "Open pressed" << endl;
    });
    fileMenu.append("&Save", [](auto& itmprxy){
      cout << "Save pressed" << endl;
    });

    auto& helpMenu = mb.at(1);
    helpMenu.append("&About", [](auto& itmprxy){
      cout << "About pressed" << endl;
    });

    mb.show();
  }

  SECTION(all treebox setup)
  {
    tree.create(fm, true);
    SECTION(treebox item insertion from the current tbl)
    {
      regenTblInfo();
    }
    SECTION(treebox set up including events)
    {
      tree.auto_draw(true);
      tree.borderless(false);
      tree.events().expanded([](const arg_treebox& tbArg) mutable  // lambda captures by value are const by default, so mutable is used
      {
        if(tbArg.item.expanded())
        {
          str aryStr = "";
          if(aryViz.text() == ""){
            //Println("creating array visualization");
            TO(t.size(),i){
              aryStr += toString( i==0? "" : ", ", (tbl<>::i64)t[i] );
            }
            aryViz.text(aryStr);
          }

          Println("");
          Println("txt: ", tbArg.item.text(), " key: ", tbArg.item.key() );
          Println("expanded: ", tbArg.item.expanded() );
          Println("");
        }
      });
    }
    SECTION(treebox custom drawing)
    {
      auto& img = tree.icon("ID1");
      auto& nrm = img.normal;

      pixel_buffer pxbuf(128,128);
      pixel_buffer::pixel_buffer_storage* stor = pxbuf.storage_.get();
      auto rawpx = stor->raw_pixel_buffer;
      auto    w = stor->pixel_size.width;
      auto    h = stor->pixel_size.height;
      for(unsigned int y=0; y<h; ++y)
       for(unsigned int x=0; x<w; ++x){
         pixel_argb_t p;
         p.element.red   = 0;
         p.element.green = 0;
         p.element.blue  = 0;
         p.element.alpha_channel = 1;
         rawpx[y*w + x]  = p;
       }

      img.normal.open("normal1.png");
      img.hovered.open("hovered1.png");
      img.expanded.open("expanded1.png");
      tree.auto_draw(true);
    }
  }

  SECTION(place and gui component layout)
  {
    place       plc(fm);
    place  lblPlace(fm);
    plc["mb"]      << mb;
    plc["sz"]      << sz;
    plc["elems"]   << elems;
    plc["szBytes"] << szBytes;
    plc["cap"]     << cap;
    plc["owned"]   << owned;
    plc["mapcap"]  << mapcap;
    plc["tree"]    << tree;
    plc.div("vert"
            "<mb weight=30>"
            "<weight=20 margin=[0,0,5,10] " // weight=20 "<weight=10% "
             " <fit sz margin=[0,10]> <fit elems margin=[0,10]> <fit szBytes margin=[0,10]> <fit cap margin=[0,10]> <fit mapcap margin=[0,10]> <fit owned>"
            ">" //  gap=5 margin=[10,40,10,0]" //margin=[10,10,10,10]>"
            "<tree weight=90%>"
            );
    plc.collocate();
  }

  SECTION(layout the main window, show it, and start the event loop)
  {
    fm.collocate();
    fm.show();         //Show the form

    exec();            //Start to event loop process, it blocks until the form is closed.
  }
}







//tree.clear();
//Println("tbl elems: ", t.elems() );

//treebox tree(fm, true);
//treebox::item_proxy aryViz;
//tree.events().expanded([&t, aryViz](const arg_treebox& tbArg) mutable  // lambda captures by value are const by default, so mutable is used
//tree.events().expanded([aryViz](const arg_treebox& tbArg) mutable  // lambda captures by value are const by default, so mutable is used

//cout << "New pressed" << endl;
//tree.clear();
//tree.create(fm,true);
//tree.erase();
//tree.clear();
//tree.create(

//str aszStr = toString("Array Size: ", t.size());
//tree.insert("ary", aszStr);
////str aryStr = "";
////TO(t.size(),i){
////  aryStr += toString( i==0? "" : ", ", (tbl<>::i64)t[i] );
////}
////tree.insert("ary/arystr", aryStr);
//aryViz = tree.insert("ary/arystr", "");
//
//tree.insert("elems", toString("Elements: ", t.elems()) );
//TO(t.elems(),i){
//  auto e = t.elemStart()[i];
//  if(e.isEmpty()) continue;
//
//  auto k = e.key;
//  tree.insert( toString("elems/", k), toString(k,":  ", e.as<tbl<>::i64>()) ); 
//}

//str aryStr = "";
//TO(t.size(),i){
//  aryStr += toString( i==0? "" : ", ", (tbl<>::i64)t[i] );
//}
//tree.insert("ary/arystr", aryStr);

//aryViz.clear();
//aryViz.append("ary/arystr", aryStr);
//aryViz.text("wat skidoosh");
//Println("array expanded ");
//Println(tbArg.item.owner().key(), "  ", tbArg.item.child().key() );

//label     sz(fm,  toString("Size: ",          t.size()),         true);
//label  elems(fm,  toString("Elements: ",      t.elems()),        true);
//label szBytes(fm, toString("Size in Bytes: ", t.sizeBytes()),    true);
//label     cap(fm, toString("Capacity: ",      t.capacity()),     true);
//label  mapcap(fm, toString("Map Capacity: ",  t.map_capacity()), true);
//label   owned(fm, toString("Owned: ",         t.owned()? "True" : "False"), true);

//fm["mb"]      << mb;
//fm["szBytes"] << szBytes;
//fm["tree"]    << tree;
//fm.div("vert <margin=[75,75,75,75]> <mb weight=30>"
//       "<fit szBytes weight=10%>" //  gap=5 margin=[10,40,10,0]" //margin=[10,10,10,10]>"
//       "<tree weight=90%>"
//       );

//fm.div("vert<mb weight=10%>");
//
//fm.div("vert<><<><weight=80% text><>><><weight=24<><button><>><>");
//fm.div("vert<><<><weight=80% text><>><><weight=24<><button><>><>");
//
//place plc(fm);
//plc.field("tree") << tree;
//plc.div("vert<tree weight=100%>");
//plc.collocate();
//
//Sets the images
//auto & img = tree.icon("ID1");
//img.normal.open("normal1.png");
//img.hovered.open("hovered1.png");
//img.expanded.open("expanded1.png");
//
//auto prx = tree.insert("root", "ROOT");
//prx.icon("ID1"); //Sets the images for the item.
//
//auto & img2 = tree.icon("ID2");
//img2.normal.open("normal2.png");
//img2.hovered.open("hovered2.png");
//img2.expanded.open("expanded2.png");
////If hovered or expanded is not set, it uses normal image for these 2 states.
//
//prx = tree.insert("root/abc", "abc");
//prx.icon("ID2");
//tree.insert("root/def", "def").icon("ID2");
//images ID2 are shared for item "abc" and "def".
//
////Layout management
//fm.div("vert <><<><weight=80% text><>><><weight=24<><button><>><>");
//fm["text"]   << lab;
//fm["button"] << btn;
//fm["tree"]   << tree;

//root.icon("ID1");
//auto trplc = tree.placer();

//pxbuf.storage_->
//nrm.image_ptr_
//nrm

//paint::image* img = &(tree.icon("ID1"));
//drawerbase::treebox::node_image_tag* img = &(tree.icon("ID1"));

//nrm.
//auto ib = new paint::detail::basic_image_pixbuf
//nrm.image_ptr_ = make_shared<paint::detail::basic_image_pixbuf>(); // ::basic_image_pixbuf>();
//paint::detail::basic_image_pixbuf pxbuf;

//for(auto e : t){
//  tree.insert( 
//}

//auto root = tree.insert("root", "wat");
//tree.insert("root/wat",       "wat");
//tree.insert("root/squidoosh", "squidoosh");

//tree.insert("root", "skidoosh");
//tree.insert("root/long", "alkjs alksjfklasjlk sj klasjkfklajsj klasfkjas jkas adl;fjlkasjfl;jalfkjalsjkdflkjaslkfdjl;ajfklja sklfj klsajfljsalfjklajs klf;jlas;jdfkljaksjfkl;ajs klf;jklas jfkljksaldjfklsajdkl;jkla;sjfkljasklkljfklajklfjaskljfklajs klf jkla;jklfej;lajklfjklaj;lfjm;amvlksdljmvl;kajs;klfdjlak;sjv;mnsdklfjm;klnvm;jlznljvnm;ldfnmh;ln;fjnbxl;fjjsklklfjasklfjlsd;ajf;as ");

//auto   fileSub = fileMenu.create_sub_menu(0);
//fileSub->append("&New");
//fileSub->append("&Open");
//fileSub->append("&Save");

//
//mb.size({400, 50});

//t("wat")       = (int)21;
//t("skidoosh")  = (int)42;
//t("wut")       = (int)84;
//t("squidoosh") = (int)168;
//t("table key") = (int)336;

//Define a form.
//API::make_center

//
//#include "../../nanovg_test/tbl.hpp"

//menu main_menu;
//main_menu.append("Item 0");
//main_menu.append("Item 1");
//
//main_menu.gaps({ 3, -2 }); //Sets gaps
//
//menu* submenu = main_menu.create_sub_menu(0);
//submenu->append("Item 0");
//submenu->append("Item 1");

////Define a label and display a text.
//label lab{fm, "Hello, <bold blue size=16>Nana C++ Library</>"};
//lab.format(true);
//
////Define a button and answer the click event.
//button btn{fm, "Quit"};
//btn.events().click([&fm]{
//  fm.close();
//});

//
//treebox tree(fm, {10,10,300,300}, true);

//#include <nana/gui.hpp>
//#include <nana/gui/widgets/treebox.hpp>
//#include <nana/filesystem/filesystem.hpp>
//
//int main()
//{
//  using namespace nana;
//  form fm{ API::make_center(300, 300), appear::decorate<appear::taskbar>() };
//  fm.caption(L"Nana C++ Library - Treebox Sample.");
//
//  nana::treebox tree(fm, { 0, 0, 300, 300 });
//
//  namespace fs = nana::experimental::filesystem;
//#if defined(NANA_WINDOWS)
//  auto node = tree.insert(L"C:", L"Local Drive(C:)");
//  fs::directory_iterator i(L"C:\\"), end;
//
//  for (; i != end; ++i)
//  {
//    if (!is_directory(*i)) continue;
//
//    auto name = to_utf8(i->path().filename().native());
//    tree.insert(node, name, name);
//    break;
//  }
//
//  tree.events().expanded([&tree](const arg_treebox& arg)
//  {
//    if (!arg.operated) return; //If this is contracted.
//
//                               //Windows supports the path separator '/'
//    auto path = tree.make_key_path(arg.item, L"/") + L"/";
//
//    //avoids frequent useless refreshing
//    tree.auto_draw(false);
//
//    //Walk in the path directory for sub directories.
//    for (fs::directory_iterator i{ path }, end; i != end; ++i)
//    {
//      if (!i->directory) continue; //If it is not a directory.
//
//      auto name = to_utf8(i->path().filename().native());
//      auto child = tree.insert(arg.item, name, name);
//      if (child.empty()) continue;
//
//      //Find a directory in child directory, if there is a directory,
//      //insert it into the child, just insert one node to indicate the
//      //node has a child and an arrow symbol will be?displayed in the
//      //front of the node.
//      for (fs::directory_iterator u{ path + name }; u != end; ++u)
//      {
//        if (!is_directory(*u)) continue; //If it is not a directory.
//        auto name = to_utf8(u->path().filename().native());
//        tree.insert(child, name, name);
//        break;
//      }
//    }
//    tree.auto_draw(true);
//  });
//
//  fm.show();
//  exec();
//}
