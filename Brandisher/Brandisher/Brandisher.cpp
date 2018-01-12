
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
// -todo: make list or tree of simdb databases - just put them at root level in the tree
// -todo: make sub list of keys below simdb databases
// -todo: implement clearing of the tbl and gui on menu new
// -todo: make selecting simdb database list the keys in that database - just make dbs the root level tree items
// -todo: fix tree expansion bug - looking for specific item_proxy AND not checking if it was empty before tying to compare it to ""
// -todo: make tbl insert function insert under an arbitrary key
// -todo: visualize 'type' element as an 8 character string first, then as a number
// -todo: take tbls out of dbs and insert them into the tree
// -todo: make tables drag and droppable to files - might not be possible from nana
// -todo: make drag and drop of a tbl file drop it into simdb - nana limitation 
// -todo: make simdb tables drag and droppable to other simdb dbs - nana limitation
// -todo: make table files drag and droppable to sub tables - nana limitation 
// -todo: rename Tabler to Brandisher
// -todo: try array visualization again, maybe extend the base interface - memory image_impl_interface created
// -todo: try picture widget
// -todo: make a full key of the selected tree box element on selected event
// -todo: fix tbl element count - due to initialization problems in both tbl and brandisher
// -todo: figure out when a table is being selected - just look at the level? - keep a hash of all keys that correspond to tables
// -todo: split out label regen into separate function
// -todo: change labels on selection of a different tbl
// -todo: get table from the db and point to it
// -todo: fix crash on selecting the inner array - iterator was being checked if not equal to end instead of equal to end
// -todo: make a pointer to the current tble that is set on clicking within a table
// -todo: make function to get a full key from an item proxy
// -todo: make a function to check if an item proxy is a table key
// -todo: detect when the ary item in a table is expanded 

// todo: make the tree text under an array be the mean, median, mode and variance of the array values
// todo: make a graph visualization of the current table in the picture widget
// todo: debug crash when selecting key of first table in the tree
// todo: make listing the keys of a db happen on expand
// todo: make insertion of tbls from a db key happen on expand
// todo: change regen function to a refreshDBs function name
// todo: debug tbl elem count coming from the visualizer
// todo: make switch case for fundamental types that the map elements can be
// todo: compile with png and jpeg libs
// todo: implement saving of the tbl
// todo: implement opening on drag and drop of a tbl
// todo: implement opening of the tbl through the menu and file dialog
// todo: try compiling with clang
// todo: make a tabler simdb database on start, as scratch space for dragged in files and dragged tables from other dbs

// idea: should tbls exist as either files, memory, or sub tables?
//       | icon in the tree could show which is which
//       | editing a memory mapped table could edit the table on disk

#include <iostream>
#include <unordered_map>
#include <nana/gui.hpp>
#include <nana/gui/dragger.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/treebox.hpp>
#include <nana/gui/widgets/menu.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/paint/pixel_buffer.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <../source/paint/detail/image_pixbuf.hpp>
#include <../source/paint/pixel_buffer.cpp>
#include "../../no_rt_util.h"
#include "../../simdb.hpp"
#include "../../tbl.hpp"
#include "../../str_util.hpp"

struct   vert { f32 p[3],n[3],c[4],tx[2]; };
using     str = std::string;
using vec_str = std::vector<str>;
using  vec_u8 = std::vector<u8>;
using vec_dbs = std::vector<simdb>;
using   IvTbl = tbl<vert>;

struct IdxKey { bool subTbl; u32 idx; str key; };            // this represents an index into the db vector and a key  
using TblKeys = std::unordered_map<str,IdxKey>;

IvTbl                       glblT;
vec_u8                     tblBuf;
vec_str                       sel;
vec_dbs                       dbs;
TblKeys                   tblKeys;
nana::form                     fm;
nana::treebox                tree;
nana::picture              aryViz;
nana::label sz, elems, szBytes, cap, mapcap, owned;

class mem_pixbuf : public nana::paint::detail::basic_image_pixbuf
{
  bool open(const std::experimental::filesystem::path& file){ return false; }
  bool open(const void* data, std::size_t bytes){ return false; }
};

namespace {

void        insertTbl(str const& parentKey, IvTbl const& t)
{
  //tblKeys[parentKey] = parentKey;
  //tblKeys.insert(parentKey);
  SECTION(array visualization)
  {
    str aszStr = toString("Array Size: ", t.size());
    str aryKey = parentKey+"/ary"; 
    tree.insert(aryKey, aszStr);

    //aryViz     = tree.insert(aryKey+"/arystr", "");
    tree.insert(aryKey+"/arystr", "");
  }
  SECTION(hash map elements)
  {
    str elemKey = parentKey+"/elems";
    tree.insert(elemKey, toString("Elements: ", t.elems()) );

    TO(t.map_capacity(),i)
    {
      auto e = t.elemStart()[i];
      if(e.isEmpty()) continue;

      str     k = e.key;
      str title = toString(k,":  ", e.val);
      if(k == "type"){
        char typeStr[9];
        memcpy(typeStr, &e.val, 8);
        typeStr[8] = '\0';
        title = toString(k,":  ", typeStr, "  -  (",e.val,")");
      }
      
      tree.insert( toString(elemKey,"/", k), title); // e.as<tbl<>::i64>()) ); 
    }
  }
}
vec_u8   extractDbKey(simdb const& db, str key)
{
  u32     vlen = 0;
  u32  version = 0;
  u64      len = db.len(key, &vlen, &version);          // todo: make ui64 as the input length

  vec_u8 ret(vlen);
  db.get(key.c_str(), ret.data(), vlen);

  return ret;
}
void      regenLabels(IvTbl const& t)
{
  SECTION(label captions)
  {
    //sz.caption(     toString("Size: "));
    //elems.caption(  toString("Elements: "));
    //szBytes.caption(toString("Size in Bytes: "));
    //cap.caption(    toString("Capacity: "));
    //mapcap.caption( toString("Map Capacity: "));
    //owned.caption(  toString("Owned: "));

    //auto& t = glblT;
    sz.caption(     toString("Size: ",          t.size()        ));
    elems.caption(  toString("Elements: ",      t.elems()       ));
    szBytes.caption(toString("Size in Bytes: ", t.sizeBytes()   ));
    cap.caption(    toString("Capacity: ",      t.capacity()    ));
    mapcap.caption( toString("Map Capacity: ",  t.map_capacity()));
    owned.caption(  toString("Owned: ",         t.owned()? "True" : "False"));
  }
}
void     regenTblInfo()
{
  SECTION(list simdb files and insert them at the top level of the tree)
  {
    dbs.clear();
    tblKeys.clear();

    simdb_error err;
    auto dbPths = simdb_listDBs(&err);

    //for(auto& pth : dbPths)
    TO(dbPths.size(),i)
    {
      auto const& pth = dbPths[i];
      tree.insert(pth, pth);
      
      //simdb db(pth.c_str(), 4096, 1 << 14);
      //auto dbKeys = db.getKeyStrs();                                      // Get all keys in DB - this will need to be ran in the main loop, but not every frame
      //vec_u8 tblBuf; 
      dbs.emplace_back(pth.c_str(), 4096, 1 << 14);
      simdb&   db = dbs.back();
      auto dbKeys = db.getKeyStrs();                                      // Get all keys in DB - this will need to be ran in the main loop, but not every frame
      //for(auto const& key : dbKeys)
      TO(dbKeys.size(),j)
      {
        auto const& key = dbKeys[j];
        auto tblKey = pth+"/"+key.str;
        //tree.insert(pth+"/"+key.str, key.str);
        tree.insert(tblKey, key.str);

        //str tblKey = pth+"/"+key.str;
        tblBuf     = extractDbKey(db, key.str);
        IvTbl ivTbl(tblBuf.data());
        insertTbl(tblKey, ivTbl);

        IdxKey ik;
        ik.subTbl = false;
        ik.idx    = i;
        ik.key    = key.str;
        tblKeys[tblKey] = ik;
      }
    }
  }
  regenLabels( glblT );
}
str        getFullKey(nana::treebox::item_proxy const& ip)
{
  auto* cur = &ip;
  str   key = cur->key();
  while(cur->level() > 1){
    cur = &(cur->owner());
    key = cur->key() + "/" + key;
  }

  return key;
}
bool       isTableKey(str const& key)
{
  auto iter = tblKeys.find(key);
  return iter != tblKeys.end();
}
bool       isTableKey(nana::treebox::item_proxy const& ip)
{
  return isTableKey( getFullKey(ip) );
}
IvTbl      tblFromKey(str key)
{
  IvTbl ret;
  auto iter = tblKeys.find(key);
  if( iter == tblKeys.end() ) return ret;

  IdxKey& ik = iter->second;
  if( ik.subTbl ) return ret;

  auto buf = extractDbKey( dbs[ik.idx], ik.key );

  if(buf.size() >= IvTbl::memberBytes() ){
    ret = IvTbl(buf.data(),false,true);
    return ret;
  }else{ return ret; }
}
IvTbl* setCurTblFromTreeKey(str key)  // set current table from tree key
{
  //if( !isTableKey(key) ) return nullptr;

  auto iter = tblKeys.find(key);
  if( iter == tblKeys.end() ) return nullptr;
  
  IdxKey& ik = iter->second;
  if( ik.subTbl ) return nullptr;

  tblBuf = extractDbKey( dbs[ik.idx], ik.key );

  if(tblBuf.size() >= IvTbl::memberBytes() ){
    glblT  = IvTbl(tblBuf.data());
    return &glblT;
  }else{ return nullptr; }
}

}

int  main()
{
  using namespace  std;
  using namespace nana;
  using namespace nana::paint;

  SECTION(initialize the table)
  {
    //TO(12,i){
    //  t.push_back( (tbl<>::i8)i );
    //}
    //t("wat")       = 21;
    //t("skidoosh")  = 42;
    //t("wut")       = 84;
    //t("squidoosh") = 168;
    //t("table key") = 336;
  }

  SECTION(set up the main window and its events)
  {
    API::enable_dropfiles(fm, true);
    fm.caption("Brandisher");
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
    aryViz.create(fm, true);
    aryViz.stretchable(true);

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
      //t.clear();
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
    SECTION(treebox custom drawing)
    {
      auto& img = tree.icon("TestIcon");
      auto& nrm = img.normal;

      //paint::detail::basic_image_pixbuf bipb;

      //pixel_buffer pxbuf(128,128);
      //pixel_buffer::pixel_buffer_storage* stor = pxbuf.storage_.get();
      auto     mempxbuf = make_shared<mem_pixbuf>(); // todo: initialize this buffer 
      mempxbuf->pixbuf_ = pixel_buffer(512,256);
      auto* stor = mempxbuf->pixbuf_.storage_.get();
      auto rawpx = stor->raw_pixel_buffer;
      auto     w = stor->pixel_size.width;
      auto     h = stor->pixel_size.height;
      for(unsigned int y=0; y<h; ++y)
        for(unsigned int x=0; x<w; ++x){
          pixel_argb_t p;
          p.element.red   = (u8)(y/(f32)h * 255.f);
          p.element.green = (u8)(x/(f32)w * 255.f);
          p.element.blue  = 0;
          p.element.alpha_channel = 1;
          rawpx[y*w + x]  = p;
        }

      img.normal.open("normal1.png");
      img.hovered.open("hovered1.png");
      img.expanded.open("expanded1.png");
      auto ptr = img.normal.image_ptr_.get();

      img.normal.image_ptr_ = mempxbuf;
      aryViz.load(img.normal);
      tree.auto_draw(true);
    }
    SECTION(treebox item insertion from the current tbl)
    {
      regenTblInfo();
      tree.insert("place", "place");
      auto watPrxy = tree.insert("place/wat", "wat");
      watPrxy.icon("TestIcon");
      tree.insert( toString("skidoosh"), toString("skidoosh") );
      //insertTbl("", t);
    }
    SECTION(treebox set up including events)
    {
      tree.auto_draw(true);
      tree.borderless(false);
      tree.events().expanded([](const arg_treebox& tbArg) mutable  // lambda captures by value are const by default, so mutable is used
      {
        
        if(tbArg.item.expanded())
        {          
          //&& tbArg.item.key() == "ary"

          str tblKey = getFullKey( tbArg.item.owner() );
          if( isTableKey(tblKey) && 
              tbArg.item.key() == "ary"      &&
              tbArg.item.child().text() == ""){
            //tree.insert( tbArg.item, "/watsquidoosh", "watsquidoosh");
            auto t = tblFromKey( tblKey );
            str txtStr = toString("size: ", t.size() );
            tbArg.item.child().text(txtStr);
          }

          Println("");
          Println("txt: ", tbArg.item.text(), " key: ", tbArg.item.key() );
          Println("expanded: ", tbArg.item.expanded() );
          Println("");
        }
      });
      tree.events().selected([](const arg_treebox& tbArg) mutable
      {
        sel.clear();

        Println("key:  ", tree.selected().key() );
        Println("selected: ", tbArg.item.key() );
        Println("owner: ", tbArg.item.owner().key() );

        auto* cur = &tbArg.item;
        do{
          sel.push_back(cur->key());
          cur = &(cur->owner());
        }while(cur->level() > 0);

        str key = "";
        FROM(sel.size(),i){             // loop from the last to the first, since they were pushed in reverse order while walking back up the tree
          key += sel[i];
          if(i!=0) key += "/";
        }
        Println(key);

        bool isTbl = isTableKey(key);
        Println("isTableKey: ", isTbl);

        IvTbl* curT = setCurTblFromTreeKey(key);
        if(curT){
          regenLabels( *curT );
        }

        Println("\n\n");
      });
    }
  }

  SECTION(place and gui component layout)
  {
    place       plc(fm);
    place  lblPlace(fm);
    plc["mb"]      << mb;
    //plc["drgBtn"]  << drgBtn;
    plc["sz"]      << sz;
    plc["aryViz"]  << aryViz;
    plc["elems"]   << elems;
    plc["szBytes"] << szBytes;
    plc["cap"]     << cap;
    plc["owned"]   << owned;
    plc["mapcap"]  << mapcap;
    plc["tree"]    << tree;
    plc.div("vert"
            "<mb weight=30>"
            "<weight=20 margin=[0,0,5,10] " // weight=20 "<weight=10% "
             "  <fit sz margin=[0,10]> <fit elems margin=[0,10]> <fit szBytes margin=[0,10]> <fit cap margin=[0,10]> <fit mapcap margin=[0,10]> <fit owned>"
            ">" //  gap=5 margin=[10,40,10,0]" //margin=[10,10,10,10]>"
            "<fit aryViz margin=[10,10,10,10] >"// weight=30%>" // margin=[10,10,10,10] > "
            "<tree>" // weight=70%>"
            );
    plc.collocate();
  }

  dragger drg;
  SECTION(layout the main window, show it, and start the event loop)
  {
    fm.collocate();
    fm.show();         //Show the form
    exec();            //Start to event loop process, it blocks until the form is closed.
  }

  SECTION(shutdown)
  {
    glblT.m_mem = nullptr;
  }
}











//u32     i = 0;
//++i;

//cur = &(cur->owner());
//key = cur->key() + "/" + key;

//sel.push_back(cur->key());
//if(i!=0){ key += "/"; }

//str key = "";
//FROM(sel.size(),i){             // loop from the last to the first, since they were pushed in reverse order while walking back up the tree
//  key += sel[i];
//  if(i!=0) key += "/";
//}

//str aryStr = "";
//if(!aryViz.empty() && aryViz.text() == ""){
//  //Println("creating array visualization");
//  //auto mx = std::min<u64>(t.size(),10); 
//  //TO(mx,i){
//  //  aryStr += toString( i==0? "" : ", ", (tbl<>::i64)t[i] );
//  //}
//  aryViz.text(aryStr);
//}
//
//&& tbArg.item.child().key() == "arystr" ){

//for(auto const& s : sel){
//  Print(s,"/");
//}

//using  IdxKey = std::pair<u32,str>;
//using TblKeys = std::unordered_set<str>;
//
//IvTbl*                        cur;
//vec_u8                curTblBytes;

//for(auto const& key : dbKeys){
//  //db.len(key);
//  auto tblBytes = db.get<u8>(key.str);
//  tbl<> t;
//  memcpy(&t, tblBytes.data(), sizeof(void*));
//  insertTbl(
//}

//Println(pth+"/"+key.str);
//Println(key.str);

//u32     vlen = 0;
//u32  version = 0;
//auto     len = db.len(key.str, &vlen, &version);          // todo: make ui64 as the input length
//tblBuf.resize(vlen);
//db.get(key.str.c_str(), tblBuf.data(), vlen);

//vs.ver = version;
//auto ivbuf = (u8*)malloc(vlen);   // todo: check to make sure this succeeds 
//
//IvTbl iv(ivbuf, false, false);
//auto     f = iv.memStart();
//
//auto tblBytes = db.get<u8>(key.str);
//t.memStart( tblBytes.data() );
//memcpy(&t, tblBytes.data(), sizeof(void*));

//nana::label sz(fm), elems(fm), szBytes(fm), cap(fm), mapcap(fm), owned(fm);
//nana::form  fm(nana::API::make_center(768, 768));

//namespace nana{	namespace paint{
//
//  // class image::image_impl_interface
//  //		the nana::image refers to an object of image::image_impl_interface by nana::refer. Employing nana::refer to refer the image::implement_t object indirectly is used
//  //	for saving the memory that sharing the same image resource with many nana::image objects.
//  class image::image_impl_interface
//    : private nana::noncopyable
//  {
//    image_impl_interface& operator=(const image_impl_interface& rhs);
//  public:
//    using graph_reference = nana::paint::graphics&;
//    virtual ~image_impl_interface() = 0;	//The destructor is defined in ../image.cpp
//    virtual bool open(const std::experimental::filesystem::path& file) = 0;
//    virtual bool open(const void* data, std::size_t bytes) = 0; // reads image from memory
//    virtual bool alpha_channel() const = 0;
//    virtual bool empty() const = 0;
//    virtual void close() = 0;
//    virtual nana::size size() const = 0;
//    virtual void paste(const nana::rectangle& src_r, graph_reference dst, const point& p_dst) const = 0;
//    virtual void stretch(const nana::rectangle& src_r, graph_reference dst, const nana::rectangle& r) const = 0;
//  };//end class image::image_impl_interface
//}//end namespace paint
//}//end namespace nana

//nana::treebox::item_proxy  aryViz;
//nana::button drgBtn;

//drgBtn.create(fm);
//drgBtn.caption("drag me");

//tree.
//tree.events().mouse_down([]() mutable
//{
//  Println("mouse down");
//});

//dragger drg;
//drg.trigger(sz);
//drg.target(sz);

//drg.target(fm);
//drg.target(tree);

//drg.trigger(fm);
//drg.trigger(tree);

//drg.trigger(drgBtn);
//drg.target(drgBtn);

//drg.trigger(tree);
//drg.target(tree);

//SECTION(insert tbl data into the tree)
//{
//}

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
