
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

// todo: run event on unfold
// todo: try compiling with clang
// todo: compile with png and jpeg labels
// todo: make array string be created only when unfolded

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
#include "../../nanovg_test/tbl.hpp"
#include "../../str_util.hpp"

using str = std::string;

int main()
{
  using namespace  std;
  using namespace nana;
  using namespace nana::paint;

  tbl<> t;
  TO(12,i){
    t.push_back( (tbl<>::i8)i );
  }
  t("wat")       = 21;
  t("skidoosh")  = 42;
  t("wut")       = 84;
  t("squidoosh") = 168;
  t("table key") = 336;
  //t("wat")       = (int)21;
  //t("skidoosh")  = (int)42;
  //t("wut")       = (int)84;
  //t("squidoosh") = (int)168;
  //t("table key") = (int)336;



  //Define a form.
  //API::make_center
  form fm(API::make_center(768, 768));
  fm.caption("Tabler");

  menubar mb;
  mb.create(fm);
  mb.push_back("&File");
  mb.push_back("&Help");
  mb.caption("menu wat");
  mb.enabled(true);
  //mb.size({400, 50});

  //std::cout << mb.length() << "  ";
  auto& fileMenu = mb.at(0);
  fileMenu.append("&New", [](auto& itmprxy){
    cout << "New pressed" << endl;
  });
  //auto   fileSub = fileMenu.create_sub_menu(0);
  //fileSub->append("&New");
  //fileSub->append("&Open");
  //fileSub->append("&Save");

  mb.show();

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

  //treebox tree(fm, {10,10,300,300}, true);
  treebox tree(fm, true);
  tree.borderless(false);
  tree.events().expanded([](const auto& tbArg){
    Println("array expanded ");
    Println(tbArg.item.text(), "  ", tbArg.item.key() );
    Println(tbArg.item.owner().key(), "  ", tbArg.item.child().key() );
  });

  str aszStr = toString("Array Size: ", t.size());
  tree.insert("ary", aszStr);
  str aryStr = "";
  TO(t.size(),i){
    aryStr += toString( i==0? "" : ", ", (tbl<>::i64)t[i] );
  }
  tree.insert("ary/arystr", aryStr);

  tree.insert("elems", toString("Elements: ", t.elems()) );
  TO(t.elems(),i){
    auto e = t.elemStart()[i];
    if(e.isEmpty()) continue;

    auto k = e.key;
    tree.insert( toString("elems/", k), toString(k,":  ", e.as<tbl<>::i64>()) ); 
  }

  //for(auto e : t){
  //  tree.insert( 
  //}

  //auto root = tree.insert("root", "wat");
  //tree.insert("root/wat",       "wat");
  //tree.insert("root/squidoosh", "squidoosh");

  //tree.insert("root", "skidoosh");
  //tree.insert("root/long", "alkjs alksjfklasjlk sj klasjkfklajsj klasfkjas jkas adl;fjlkasjfl;jalfkjalsjkdflkjaslkfdjl;ajfklja sklfj klsajfljsalfjklajs klf;jlas;jdfkljaksjfkl;ajs klf;jklas jfkljksaldjfklsajdkl;jkla;sjfkljasklkljfklajklfjaskljfklajs klf jkla;jklfej;lajklfjklaj;lfjm;amvlksdljmvl;kajs;klfdjlak;sjv;mnsdklfjm;klnvm;jlznljvnm;ldfnmh;ln;fjnbxl;fjjsklklfjasklfjlsd;ajf;as ");
  auto& img = tree.icon("ID1");
  //paint::image* img = &(tree.icon("ID1"));
  //drawerbase::treebox::node_image_tag* img = &(tree.icon("ID1"));
  auto& nrm = img.normal;
  //nrm.
  //auto ib = new paint::detail::basic_image_pixbuf
  //nrm.image_ptr_ = make_shared<paint::detail::basic_image_pixbuf>(); // ::basic_image_pixbuf>();
  //paint::detail::basic_image_pixbuf pxbuf;

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

  //pxbuf.storage_->
  //nrm.image_ptr_
  //nrm

  img.normal.open("normal1.png");
  img.hovered.open("hovered1.png");
  img.expanded.open("expanded1.png");
  //root.icon("ID1");
  tree.auto_draw(true);
  //auto trplc = tree.placer();

  label     sz(fm,  toString("Size: ",          t.size()),         true);
  label  elems(fm,  toString("Elements: ",      t.elems()),        true);
  label szBytes(fm, toString("Size in Bytes: ", t.sizeBytes()),    true);
  label     cap(fm, toString("Capacity: ",      t.capacity()),     true);
  label  mapcap(fm, toString("Map Capacity: ",  t.map_capacity()), true);
  label   owned(fm, toString("Owned: ",         t.owned()? "True" : "False"), true);

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

  fm.collocate();

  //Show the form
  fm.show();

  //Start to event loop process, it blocks until the form is closed.
  exec();
}


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
