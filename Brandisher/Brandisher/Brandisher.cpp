
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
// -todo: make the tree text under an array be the mean, median, mode and variance of the array values
// -todo: debug crash when selecting key of first table in the tree - might have been the way the table is being handled by the viewer
// -todo: change regen function to a refreshDBs function name - regen is good enough
// -todo: debug tbl elem count coming from the visualizer - tbl initialization bugs
// -todo: put back table graphing in visualization component
// -todo: figure out how to refresh on table selection
// -todo: make a graph visualization of the current table in the picture widget
// -todo: compile with png and jpeg libs - not needed for now
// -todo: add min and max to table array stats
// -todo: look in to making a custom nana drawing component that will expand to the space it has - made from looking at the implementation of picture
// -todo: implement saving of the tbl - native file dialog used for file selection
// -todo: implement opening of the tbl through the menu and file dialog - opens to the current tbl but not into a scratch db now
// -todo: check the file size on load to make sure it matches the tbl size
// -todo: try compiling with clang - can work, but there are many errors from the filesystem 
// -todo: load file into Brandisher simdb instead of current tbl
// -todo: make open and file drag the same underlying operation
// -todo: implement opening on drag and drop of a tbl
// -todo: make a Brandisher simdb database on start, as scratch space for dragged in files and dragged tables from other dbs
// -todo: make a refresh button to refresh the db - only need to refresh the dbs if they get their keys on expand
// -todo: make status have the remapped value of the cursor's Y
// -todo: make all values cut off at 0
// -todo: make values below 0 face down, and values above 0 face upwards
// -todo: make status show the array value being used at each pixel

// todo: show sub trees
// todo: treat the array as a string if it is u8, i8, (or a string type?) - then show statistics for a string if the string is too long to fit in the gui
// todo: make listing the keys of a db happen on expand
// todo: make insertion of tbls from a db key happen on expand

// idea: make switch case for fundamental types that the map elements can be - need the non-templated table
// idea: make multiple selections additivly draw multiple colored lines?
// idea: should tbls exist as either files, memory, or sub tables?
//       | icon in the tree could show which is which
//       | editing a memory mapped table could edit the table on disk

#include <cstdio>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <filesystem>
#include <nana/gui.hpp>
#include <nana/gui/dragger.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/treebox.hpp>
#include <nana/gui/widgets/menu.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/paint/pixel_buffer.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/wvl.hpp>
#include <nana/gui/drawing.hpp>
#include <nana/paint/image_process_selector.hpp>
#include <../source/paint/detail/image_pixbuf.hpp>
#include <../source/paint/pixel_buffer.cpp>
#include <../source/paint/graphics.cpp>
#include "../../nfd.h"
#include "../../no_rt_util.h"
#include "../../simdb.hpp"
#include "../../tbl.hpp"
#include "../../str_util.hpp"

struct   vert { f32 p[3],n[3],c[4],tx[2]; };
using     str = std::string;
using vec_str = std::vector<str>;
using  vec_u8 = std::vector<u8>;
using vec_dbs = std::vector<simdb>;
using    path = std::experimental::filesystem::path;         // why is this still experimental?
using   IvTbl = tbl<vert>;

struct IdxKey { bool subTbl; u32 idx; str key; };            // this represents an index into the db vector and a key  
using TblKeys = std::unordered_map<str,IdxKey>;

std::ostream& operator<<(std::ostream& os, const nana::point&     p)
{
  return os << "point{" << p.x << "," << p.y << "}";
}
std::ostream& operator<<(std::ostream& os, const nana::size&      z)
{
  return os << "size{" << z.width << "," << z.height << "}";
}
std::ostream& operator<<(std::ostream& os, const nana::rectangle& r)
{
  return os << "rect{" << r.position() << "," << r.dimension() << "}";
}

f32       mx = -std::numeric_limits<float>::infinity(); 
f32       mn =  std::numeric_limits<float>::infinity();
f32                  msX=0, msY=0;
IvTbl                       glblT;
vec_u8                     tblBuf;
vec_str                       sel;
vec_dbs                       dbs;
TblKeys                   tblKeys;
simdb                          db;
nana::form                     fm;
nana::treebox                tree;
nana::paint::image         vizImg;
nana::button              refresh;
nana::place                   plc;
nana::label sz, elems, szBytes, cap, mapcap, owned, status;

namespace { 
  template <class T> T remap(T val, T fromLo, T fromHi, T toLo, T toHi)
  {
    auto  dif = fromHi - fromLo;
    auto norm = dif!=0?  (val - fromLo) / dif  :  0.f;
    return norm * (toHi - toLo) + toLo;
  }
}

class  VizDraw : public nana::drawer_trigger
{
  void  drawGradient(nana::paint::graphics& g)
  {
    using namespace nana;
    using namespace nana::paint;

    pixel_argb_t* pxbuf = g.impl_->handle->pixbuf_ptr;
    if(pxbuf)
    {
      auto w = g.width();
      auto h = g.height();
      for(unsigned int y=0; y<h; ++y)
        for(unsigned int x=0; x<w; ++x)
        {
          pixel_argb_t p;
          p.element.red   = (u8)(y/(f32)h * 255.f);
          p.element.green = (u8)(x/(f32)w * 255.f);
          p.element.blue  = 0;
          p.element.alpha_channel = 1;

          pxbuf[y*w + x] = p;
        }
    }
  }
  void     drawBlack(nana::paint::graphics& g)
  {
    using namespace nana;
    using namespace nana::paint;

    if( !g.impl_ || !g.impl_->handle || !g.impl_->handle->pixbuf_ptr )
      return;

    pixel_argb_t* pxbuf = g.impl_->handle->pixbuf_ptr;
    if(pxbuf)
    {
      auto w = g.width();
      auto h = g.height();
      for(unsigned int y=0; y<h; ++y)
        for(unsigned int x=0; x<w; ++x){
          pixel_argb_t p;
          p.element.red   = 0;
          p.element.green = 0;
          p.element.blue  = 0;
          p.element.alpha_channel = 1;
          pxbuf[y*w + x] = p;
        }
    }
  }
  void  drawTblGraph(IvTbl& t, nana::paint::graphics& g)
  {
    using namespace std;
    using namespace nana;
    using namespace nana::paint;

    if( !t.m_mem || t.size()==0 ){ 
      drawBlack(g);
    }else
    {
      auto&    t = glblT;
      auto  flen = t.size() * 12;      // 12 floats in a vert struct
      f32*     f = (float*)t.m_mem;
      mx = -numeric_limits<float>::infinity();
      mn =  numeric_limits<float>::infinity();
      TO(flen,i){
        mx = max<f32>(mx, f[i]);
        mn = min<f32>(mn, f[i]);
      }
      if( !g.impl_ || !g.impl_->handle || !g.impl_->handle->pixbuf_ptr ) return;
      pixel_argb_t* pxbuf = g.impl_->handle->pixbuf_ptr;
      auto     w = g.width();
      auto     h = g.height();
      f32  ratio = w / (f32)flen;
      f32 hRatio = h / (mx-mn);
      f32    rng = mx - mn;
      //f32 hRemap = rng 
      for(unsigned int y=0; y<h; ++y)
        for(unsigned int x=0; x<w; ++x)
        {
          pixel_argb_t p;
          p.element.red   = 0;
          p.element.green = 0;
          p.element.blue  = 0;
          f32     val = f[ (u64)(x/ratio) ];
          f32      re =  (val - mn) / rng;
          f32   flipY = h - y - 1;
          f32   rempY = remap(flipY,0.f,(f32)h,mn,mx);
          bool  inVal = false;
          if(val>=0 && rempY>=0 && rempY<=val){ inVal = true; }
          else if(val<=0 && rempY<=0 && rempY>=val){ inVal = true; }
          //bool inVal = (h-y)/(f32)h < re;
          //if(h-y < val*hRatio){ // this flips the graph vertically, but increasing y goes down in screen space, so we want it flipped
          if( inVal ){ // this flips the graph vertically, but increasing y goes down in screen space, so we want it flipped
            p.element.red   = 255;
            p.element.green = 255;
            p.element.blue  = 255;
          }
          p.element.alpha_channel = 1;

          pxbuf[y*w + x] = p;
        }

      //str mxStr = toString("Max: ", mx);
      //g.bidi_string( {0,0}, mxStr.c_str(), mxStr.length() );
      //g.string( {0,0}, mxStr, {255,255,255,255} );
    }
  }

public:
  VizDraw(){}
  ~VizDraw(){}
  void attached(widget_reference, nana::paint::graphics&){}
  void refresh(nana::paint::graphics& g)
  {
    using namespace std;
    using namespace nana;
    using namespace nana::paint;

    //rectangle rw{ point{ 0,0 }, nana::size() };
    //g.rectangle(rectangle{ 5, 5, 50, 50 }, true, colors::red);
    //g.line(point(5, 5), point(55, 55), colors::blue);
    //g.line_begin(200, 100);
    //g.line_to(point(300, 300));
    //g.line_to(point(100, 200));
    //g.line_to(point(300, 200));
    //g.line_to(point(100, 300));
    //g.line_to(point(200, 100));

    drawTblGraph(glblT, g);

    //g.line( point(0,(int)msY), point((int)g.width(),(int)msY), {255,255,255,255} );
  }
};
class      Viz : public nana::widget_object<nana::category::widget_tag, VizDraw>
{
public:
  Viz()
  {
    using namespace nana;
  }
private:
  void _m_click()
  {  //Switchs the algorithm between two algorithms in every click on the form.
  }
};

Viz                           viz;

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
    glblT.m_mem = nullptr;
    tblBuf.resize(0);
    dbs.clear();
    tblKeys.clear();

    simdb_error err;
    auto dbPths = simdb_listDBs(&err);

    TO(dbPths.size(),i)
    {
      auto const& pth = dbPths[i];
      tree.insert(pth, pth);
      
      dbs.emplace_back(pth.c_str(), 4096, 1 << 14);
      simdb&   db = dbs.back();
      auto dbKeys = db.getKeyStrs();                                      // Get all keys in DB - this will need to be ran in the main loop, but not every frame
      TO(dbKeys.size(),j)
      {
        auto const& key = dbKeys[j];
        auto tblKey = pth+"/"+key.str;
        tree.insert(tblKey, key.str);

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
IvTbl*   setCurTblFromTreeKey(str key)  // set current table from tree key
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
vec_u8       readFile(const char* path)
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
void       refreshViz()
{
  viz.caption("Setting this caption is the only way I know to refresh the component");
}
void        openFiles(vec_str const& files)
{
  int i = 0;
  for(auto& f : files){
    Println("file ", i, ": ", f);
    path p(f);
    auto fileBytes = readFile(f.c_str());
    auto  justName = p.filename().replace_extension().string();
    db.put(justName, fileBytes);
  }

  regenTblInfo();
}

}

int  main()
{
  using namespace  std;
  using namespace nana;
  using namespace nana::paint;

  new (&db) simdb("Brandisher", 1024, 1 << 12);

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

      openFiles(arg.files);
    });
  }
  SECTION(initialize components with the main window handle)
  {
    SECTION(initialize the Viz component and add events)
    {
      viz.create(fm, true);
      //viz.stretchable(true);
      viz.borderless(false);
      viz.bgcolor(color(0,0,0,1.0));

      viz.events().mouse_move([](arg_mouse const& arg)
      {
        msX         = arg.pos.x;
        msY         = arg.pos.y;
        auto    vsz = viz.size();
        f32       h = vsz.height;
        auto&     t = glblT;
        if( t.m_mem && t.size()>0 ){
          auto   flen = t.size() * 12;      // 12 floats in a vert struct
          f32*      f = (float*)t.m_mem;
          f32   ratio = vsz.width  / (f32)flen;
          f32     dif = mx - mn;
          f32  hRatio = vsz.height / dif;
          u64     idx = (u64)(msX/ratio);
          auto    val = f[ idx ];
          f32  remapY = (((h-msY)/h));
          remapY     *= dif;
          remapY     += mn;
          auto capStr = toString("  ",val,
          "   X: ",arg.pos.x,
          "   Y: ",arg.pos.y,
          "   Index: ",idx,
          "   Remapped Y: ",remapY);
          status.caption( capStr );
        }
      });
    }
    SECTION(refresh button and push event)
    {
      refresh.create(fm);
      refresh.caption("REFRESH");
      refresh.events().click([](){
        regenTblInfo();
      });
    }
    SECTION(initialize the table header labels)
    {
      sz.create(fm);
      elems.create(fm);
      szBytes.create(fm);
      cap.create(fm);
      mapcap.create(fm);
      owned.create(fm);
    }

    status.create(fm);

    //initViz();
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
    fileMenu.append("&Open", [](auto& itmprxy)
    {
      nfdchar_t    *path = NULL;
      nfdresult_t result = NFD_OpenDialog("tbl", NULL, &path );
      if(!path) return;

      tblBuf = readFile(path);
      glblT  = IvTbl(tblBuf.data(), false, false);
      if(glblT.sizeBytes() > tblBuf.size()){
        glblT.m_mem = nullptr;
        tblBuf.resize(0);
        Println("File size and tbl size did not match");
      }
      refreshViz();

      openFiles({path});

      Println("File Opened");
    });
    fileMenu.append("&Save", [](auto& itmprxy)
    {
      nfdchar_t *path = NULL;
      nfdresult_t result = NFD_SaveDialog("tbl", NULL, &path );
      if(!path) return;

      FILE* tblFile = fopen(path,"wb");
      if(!tblFile) return;
        fwrite(glblT.memStart(), sizeof(u8), glblT.sizeBytes(), tblFile);
      fclose(tblFile);

      Println("File Written");
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

      tree.auto_draw(true);
    }
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
          str tblKey = getFullKey( tbArg.item.owner() );
          if( isTableKey(tblKey) && 
              tbArg.item.key() == "ary"      &&
              tbArg.item.child().text() == "")
          {
            auto    t = tblFromKey( tblKey );

            auto flen = t.size() * 12;      // 12 floats in a vert struct
            f32*    f = (float*)t.m_mem;
            sort(f, f+flen);                // sort for both the median and the mode

            f32 mean=0, median=0; 
            SECTION(calc mean and median)
            {
              f32 total = 0;
              TO(flen,i){ total += f[i]; }
              mean = total / flen;

              u64 mid = flen / 2;
              median  = f[mid];

              Println("\n");
              TO(flen,i){
                Print(f[i]," "); 
              }
              Println("\n");
            }

            f32 hiVal=0; u64 hiCnt=0;
            SECTION(calc Mode - the value with the highest frequency)
            {
              f32 curVal=0;
              u64 curCnt=0;
              TO(flen,i){
                if(f[i] == curVal){ ++curCnt; }
                else if( curCnt > hiCnt ){
                  hiVal  = curVal;
                  hiCnt  = curCnt;
                  curVal = f[i];
                  curCnt = 0;
                }
              }
              if( curCnt > hiCnt ){
                hiVal = curVal;
                hiCnt = curCnt;
              }
            }

            f32 variance=0;
            SECTION(calc variance)
            {
              f32 difSqr = 0;
              TO(flen,i){
                f32 dif  = f[i] - mean;
                difSqr  += dif * dif;
              }
              variance = flen>1?  difSqr/(flen-1)  :   difSqr;  // length of the array needs to be at least 2 to use the n-1 'unbiased' variance  
            }

            f32 tmn, tmx;
            SECTION(calc min and max)
            {
              tmx = -numeric_limits<float>::infinity(); 
              tmn =  numeric_limits<float>::infinity();
              TO(flen,i){
                tmx = max<f32>(tmx, f[i]);
                tmn = min<f32>(tmn, f[i]);
              }
            }

            str txtStr = toString(
              "Min: ",tmn,
              "   Max: ",tmx,
              "   Mean: ",mean,
              "   Median: ",median,
              "   Mode: ", hiVal, " (",hiCnt,
              ")   Variance: ", variance);
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
        if(curT == nullptr) return;

        refreshViz();

        auto&   t = *curT;
        auto flen = t.size() * 12;      // 12 floats in a vert struct
        f32*    f = (float*)t.m_mem;

        //vector<f32>  tst(512);
        //TO(tst.size(),i){ tst[i] = rand() / (f32)RAND_MAX; }
        //flen = 512;
        //f = tst.data();

        mx = -numeric_limits<float>::infinity(); 
        mn =  numeric_limits<float>::infinity();
        TO(flen,i){
          mx = max<f32>(mx, f[i]);
          mn = min<f32>(mn, f[i]);
        }
        Println("\n Table Key mx: ",mx, " mn: ", mn, " \n" );

        //vizImg.image_ptr_ = mempxbuf;
        //viz.load(vizImg);

        plc.collocate();
        fm.collocate();
        fm.activate();

        Println("\n\n");
      });
    }
  }

  SECTION(place and gui component layout)
  {
    plc.bind(fm);
    plc["mb"]      << mb;
    plc["refresh"] << refresh;
    plc["sz"]      << sz;
    plc["viz"]     << viz;
    plc["elems"]   << elems;
    plc["szBytes"] << szBytes;
    plc["cap"]     << cap;
    plc["owned"]   << owned;
    plc["mapcap"]  << mapcap;
    plc["tree"]    << tree;
    plc["status"]  << status;
    plc.div("vertical"
            "<mb weight=30>"
            "<weight=30 margin=[0,0,5,10] " // weight=20 "<weight=10% "
              // labels and refresh button
              "<fit refresh margin=[0,10]> <fit sz margin=[0,10]> <fit elems margin=[0,10]> <fit szBytes margin=[0,10]> <fit cap margin=[0,10]> <fit mapcap margin=[0,10]> <fit owned>"
            ">" //  gap=5 margin=[10,40,10,0]" //margin=[10,10,10,10]>"
            "<fit viz margin=[5,5,5,5] >"// weight=30%>" // margin=[10,10,10,10] > "
            //"<splitter>"
            "<tree>" // weight=70%>"
            "<status weight=30 margin=[5,5,5,5]>"
            );
    plc.collocate();
  }

  dragger drg;
  SECTION(layout the main window, show it, and start the event loop)
  {
    //tsf.show();
    //tsfm.show();

    fm.collocate();
    fm.show();         //Show the form
    exec();            //Start to event loop process, it blocks until the form is closed.
  }

  SECTION(shutdown)
  {
    glblT.m_mem = nullptr;
  }
}















//int i = 0;
//for(auto& f : arg.files){
//  Println("file ", i, ": ", f);
//  path p(f);
//  auto fileBytes = readFile(f.c_str());
//  auto  justName = p.filename().replace_extension().string();
//  db.put(justName, fileBytes);
//}

//FILE* tblFile = fopen("test_table.tbl","w");
//FILE* tblFile = fopen(path,"r");
//if(!tblFile) return;
//
//fread(glblT.memStart(), sizeof(u8), glblT.sizeBytes(), tblFile);
//fclose(tblFile);

//sz.caption(     toString("Size: "));
//elems.caption(  toString("Elements: "));
//szBytes.caption(toString("Size in Bytes: "));
//cap.caption(    toString("Capacity: "));
//mapcap.caption( toString("Map Capacity: "));
//owned.caption(  toString("Owned: "));
//
//auto& t = glblT;

//plc["dviz"]    << dviz;
//plc["tsf"]     << tsf;

//void          initViz()
//{
//  using namespace std;
//  using namespace nana;
//  using namespace nana::paint;
//
//  //mempxbuf = make_shared<mem_pixbuf>();
//  //mempxbuf->pixbuf_ = pixel_buffer(512,256);
//  //auto* stor = mempxbuf->pixbuf_.storage_.get();
//  //auto rawpx = stor->raw_pixel_buffer;
//  //auto     w = stor->pixel_size.width;
//  //auto     h = stor->pixel_size.height;
//  //for(unsigned int y=0; y<h; ++y)
//  //  for(unsigned int x=0; x<w; ++x){
//  //    pixel_argb_t p;
//  //    p.element.red   = (u8)(y/(f32)h * 255.f);
//  //    p.element.green = (u8)(x/(f32)w * 255.f);
//  //    p.element.blue  = 0;
//  //    p.element.alpha_channel = 1;
//  //    rawpx[y*w + x]  = p;
//  //  }
//  
//  //vizImg.image_ptr_ = mempxbuf;
//  //viz.load(vizImg);
//}

//fm.events().resized.emit(
//fm.show();
//viz.focus();
//fm.focus();
//fm.restore();

//class mem_pixbuf : public nana::paint::detail::basic_image_pixbuf
//{
//  bool open(const std::experimental::filesystem::path& file){ return false; }
//  bool open(const void* data, std::size_t bytes){ return false; }
//};
//using MemPixbuf = std::shared_ptr<mem_pixbuf>;
//
//MemPixbuf                mempxbuf;
//nana::picture                 viz;

//p.element.red   = (u8)(y/(f32)h * 255.f);
//p.element.green = (u8)(x/(f32)w * 255.f);
//
//f32 val = f[ (u64)(x*ratio) ];

//pixel_argb_t* pxbuf = g.impl_->handle->pixbuf_ptr;
//if(pxbuf)
//{
//  auto     w = g.width();
//  auto     h = g.height();
//  for(unsigned int y=0; y<h; ++y)
//    for(unsigned int x=0; x<w; ++x)
//    {
//      pixel_argb_t p;
//      p.element.red   = (u8)(y/(f32)h * 255.f);
//      p.element.green = (u8)(x/(f32)w * 255.f);
//      //p.element.red   = 0;
//      //p.element.green = 0;
//      p.element.blue  = 0;
//      p.element.alpha_channel = 1;
//
//      pxbuf[y*w + x] = p;
//    }
//}
//
//auto* stor = mempxbuf->pixbuf_.storage_.get();
//auto rawpx = stor->raw_pixel_buffer;
//auto     w = stor->pixel_size.width;
//auto     h = stor->pixel_size.height;
//
//auto&    t = *curT;

// : public nana::picture // : public nana::panel<true>   // : public nana::widget // : public nana::form
//
//nana::paint::image     img_;
//nana::paint::graphics   gr_;
//mem_pixbuf mempb;
//
//tsform()
//Viz(nana::window wd)

//friend class ::nana::picture;
//
//private:
//
//rectangle r { point{ 0,0 }, img_.size() };
//
//nana::size sz{size()};
//
//g.flush();
//g.pixmap();

//pixel_buffer& pxbuf = graph.impl_.get()->pxbuf;
//pixel_buffer& pxbuf = graph.impl_->handle->pixbuf_ptr;
//
//auto* stor = pxbuf.storage_.get();
//if(stor){
//
//auto rawpx = stor->raw_pixel_buffer;
//auto     w = stor->pixel_size.width;
//auto     h = stor->pixel_size.height;

//image img;
//initViz();
//vizImg.image_ptr_ = mempxbuf;
//viz.load(vizImg);

//drawing dw(wd);
////drawing dw
//dw.draw([this](paint::graphics& graph)
//{
//  using namespace nana;
//  using namespace nana::paint;
//
//  //rectangle r { point{ 0,0 }, img_.size() };
//  rectangle rw{ point{ 0,0 }, size() };
//  //nana::size sz{size()};
//  graph.rectangle(rectangle{ 5, 5, 50, 50 }, true, colors::red);
//  graph.line(point(5, 5), point(55, 55), colors::blue);
//  graph.line_begin(200, 100);
//  graph.line_to(point(300, 300));
//  graph.line_to(point(100, 200));
//  graph.line_to(point(300, 200));
//  graph.line_to(point(100, 300));
//  graph.line_to(point(200, 100));
//  graph.flush();
//  graph.pixmap();
//
//  pixel_argb_t* pxbuf = graph.impl_->handle->pixbuf_ptr;
//  if(pxbuf)
//  {
//    auto     w = graph.width();
//    auto     h = graph.height();
//    for(unsigned int y=0; y<h; ++y)
//      for(unsigned int x=0; x<w; ++x)
//      {
//        pixel_argb_t p;
//        p.element.red   = (u8)(y/(f32)h * 255.f);
//        p.element.green = (u8)(x/(f32)w * 255.f);
//        //p.element.red   = 0;
//        //p.element.green = 0;
//        p.element.blue  = 0;
//        p.element.alpha_channel = 1;
//
//        pxbuf[y*w + x] = p;
//      }
//  }
//});
//dw.update();
////Register events
//events().click  ( [this](){_m_click();} );  
////events().resized( [this](){_m_size ();} );  

//static bool interop;
//nana::paint::image_process::selector sl;
//sl.stretch(interop ? "bilinear interoplation" : "proximal interoplation");
//interop = !interop;
//std::cout << (interop ? "Click: bilinear interoplation\n" 
//  : "Click: proximal interoplation\n") ;

//  //When the window size is changed, it stretches the image to fit the window.
//  void _m_size()
//  {
//      drawing dw(*this);
//     
//      dw.clear();  //Before stretch, it should clear the operations that are generated before.
//      
//img_.stretch(rectangle{ point{ 0,0 }, img_.size() },
//      gr_,
//      size());
//dw.update();
//std::cout << "resize\n";
//  }

//      // get the graphycs !?
//if (!API::window_graphics(*this, gr_))
//      throw std::runtime_error("Imposible to get the graph");
//      
//      //Copy the image to the window
//img_.paste( rectangle{ point{0,0}, img_.size() },
//                        gr_,
//                        {0,0});
//img_.stretch( rectangle{ point{0,0}, img_.size() },
//                        gr_,
//                        size()  );

//nana::drawing                dviz(viz);
//tsform                        tsf;

//auto rawpx = stor->raw_pixel_buffer;
//auto     w = stor->pixel_size.width;
//auto     h = stor->pixel_size.height;
//
//img_.paste(graph, 0, 0);
//graph.p
//
//img_.stretch(r, gr_, rw );
//std::cout << "Pict: " << r << ", Windows: " << rw << "\n";

//pixel_buffer& pxbuf = graph.impl_.get()->pxbuf;
//pixel_buffer& pxbuf = graph.impl_->handle->pixbuf_ptr;
//
//auto* stor = pxbuf.storage_.get();
//if(stor){

//img_.image_ptr_ = mempb;
//
//if (!img_.open(("../Examples/bground.6states.bmp")))
//throw std::runtime_error("Imposible to open the image");    

//struct Ary
//{
//   u8 m_mem[16];
//
//   struct Ret
//   {
//     void* p;
//
//     template<class T> void operator=(T val)
//     {
//       T* tp = (T*)p;
//       *tp = val;
//     }
//
//     template<class T> operator T()
//     {
//       return *((T*)p);
//     }
//   };
//
//   Ret operator[](u64 i)
//   {
//     Ret r;
//     r.p = m_mem + sizeof(f32)*i;
//     return r;
//   }
//};
//
//template<class T> T& operator[](u64 i)
//{
//  u8* p = m_mem + sizeof(T)*i;
//  return *((T*)p);
//}
//f32& operator[](u64 i)
//{
//  u8* p = m_mem + sizeof(f32)*i;
//  return *((f32*)p);
//}
//u32& operator[](u64 i)
//{
//  u8* p = m_mem + sizeof(u32)*i;
//  return *((u32*)p);
//}
//
//template<class T> void operator=(Ary::Ret& r, T val)
//{
//  T* p = (T*)r.p;
//  *P = val;
//}
//void operator=(Ary::Ret& r, f32 val)
//{
//  f32* p = (f32*)r.p;
//  *p = val;
//}
//
//f32 a = ary_inst.operator[]<f32>(3);

//Ary a;
//TO(4,i){
//  //a[i] = (f32)i;
//  //a.operator[]<f32>(i) = (f32)i;
//  a[i] = (f32)i;
//}
//TO(4,i){
//  f32 b = a[i];
//  Println("b: ", b);
//}

//place       plc(fm);
//place  lblPlace(fm);
//plc["drgBtn"]  << drgBtn;

//mempxbuf = make_shared<mem_pixbuf>();
//mempxbuf->pixbuf_ = pixel_buffer(512,256);
//auto* stor = mempxbuf->pixbuf_.storage_.get();
//auto rawpx = stor->raw_pixel_buffer;
//auto     w = stor->pixel_size.width;
//auto     h = stor->pixel_size.height;
//for(unsigned int y=0; y<h; ++y)
//  for(unsigned int x=0; x<w; ++x){
//    pixel_argb_t p;
//    p.element.red   = (u8)(y/(f32)h * 255.f);
//    p.element.green = (u8)(x/(f32)w * 255.f);
//    p.element.blue  = 0;
//    p.element.alpha_channel = 1;
//    rawpx[y*w + x]  = p;
//  }
//
//vizImg.image_ptr_ = mempxbuf;
//viz.load(vizImg);

//auto     mempxbuf = make_shared<mem_pixbuf>(); // todo: initialize this buffer 
//mempxbuf->pixbuf_ = pixel_buffer(512,256);
//auto* stor = mempxbuf->pixbuf_.storage_.get();
//auto rawpx = stor->raw_pixel_buffer;
//auto     w = stor->pixel_size.width;
//auto     h = stor->pixel_size.height;
//for(unsigned int y=0; y<h; ++y)
//  for(unsigned int x=0; x<w; ++x){
//    pixel_argb_t p;
//    p.element.red   = (u8)(y/(f32)h * 255.f);
//    p.element.green = (u8)(x/(f32)w * 255.f);
//    p.element.blue  = 0;
//    p.element.alpha_channel = 1;
//    rawpx[y*w + x]  = p;
//  }
//
//img.normal.open("normal1.png");
//img.hovered.open("hovered1.png");
//img.expanded.open("expanded1.png");
//auto ptr = img.normal.image_ptr_.get();
//
//img.normal.image_ptr_ = mempxbuf;
//viz.load(img.normal);

//for(auto& pth : dbPths)
//
//simdb db(pth.c_str(), 4096, 1 << 14);
//auto dbKeys = db.getKeyStrs();                                      // Get all keys in DB - this will need to be ran in the main loop, but not every frame
//vec_u8 tblBuf; 
//
//for(auto const& key : dbKeys)
//
//tree.insert(pth+"/"+key.str, key.str);
//
//str tblKey = pth+"/"+key.str;

//tree.insert("place", "place");
//auto watPrxy = tree.insert("place/wat", "wat");
//watPrxy.icon("TestIcon");
//tree.insert( toString("skidoosh"), toString("skidoosh") );
//insertTbl("", t);

//paint::detail::basic_image_pixbuf bipb;
//
//pixel_buffer pxbuf(128,128);
//pixel_buffer::pixel_buffer_storage* stor = pxbuf.storage_.get();

//&& tbArg.item.key() == "ary"
//tree.insert( tbArg.item, "/watsquidoosh", "watsquidoosh");

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
