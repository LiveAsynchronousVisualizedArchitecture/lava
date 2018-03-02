
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
// -todo: update brandisher to use new tbl - aliasing IvTbl to tbl works
// -todo: show sub trees
// -todo: don't make the tbl array string dynamic anymore
// -todo: change all IvTbl to tbl
// -todo: make a full tbl cache on expand and clear on contract - just regular tbl cache so far
// -todo: debug why isTableKey returns 0 for a sub table - sub table keys were not being put into the mapping between the gui tree's keys and the tbl sub-path keys
// -todo: debug why the sub table size() is 0 - sub tables were hardcoded to put nullptr into the global table as a temporary measure
// -todo: sort elements alphabetically by key instead of using the hashed order 
// -todo: make listing the keys of a db happen on expand - not important until a resource limitation is hit / shown
// -todo: make insertion of tbls from a db key happen on expand - worry about this when it is actually solving a problem
// -todo: split out statistics of a tbl's array into its own function 
// -todo: fix mode and mean statistics calculations 
// -todo: make template to process array statistics - will it ultimatly need to give back a string?
// -todo: fix index of mouse cursor in status bar - array size was still being multed by 12 
// -todo: make switch case typed min max function 
// -todo: put types into normal value elements, not just sub tables - needed to make a typeStr() method in KV
// -todo: visualize arrays with proper types - just need to use a switch case to make a vector of f64? 
// -todo: use a const operator() in tbl to get the sub table by key
// -todo: take out debug printing
// -todo: change extractKey to use long form db.get() that will return the length
// -todo: debug crashes from refreshing while db is written to rapidly - changes to reading seem to have fixed it
// -todo: print off concurrent list chain for every hash map entry
// -todo: print off block list chain for every hash map entry
// -todo: print off full block list - can't if their default index is 0, it will create a loop

// todo: print off all blocks
// todo: add ability to delete keys 
// todo: delete with multi-selection and right click menu
// todo: treat the array as a string if it is u8, i8, (or a string type?) - then show statistics for a string if the string is too long to fit in the gui

// idea: make switch case for fundamental types that the map elements can be - need the non-templated table
// idea: make multiple selections additivly draw multiple colored lines?
// idea: should tbls exist as either files, memory, or sub tables?
//       | icon in the tree could show which is which
//       | editing a memory mapped table could edit the table on disk

#include <cstdio>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <filesystem>

#define NOMINMAX                                // to make sure windows headers don't define min and max
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

using         str = std::string;
using     vec_str = std::vector<str>;
using      vec_u8 = std::vector<u8>;
using     vec_f64 = std::vector<f64>;
using     vec_dbs = std::vector<simdb>;
using        path = std::experimental::filesystem::path;         // why is this still experimental?
//using       IvTbl = tbl;
using  vec_verstr = std::vector<simdb::VerStr>;

struct IdxKey { bool subTbl; u32 idx; str key; };            // this represents an index into the db vector and a key  
using TblKeys     = std::unordered_map<str,IdxKey>;                // why do the TblKeys need indices with them? - To know which DB they came from
using TblCache    = std::vector< std::unordered_map<str,tbl> >;
//struct IdxKey { bool subTbl; u32 idx; vec_str path; };       // this represents an index into the db vector and a vector of keys to get the table from the   

template<class T> struct MnMx 
{ 
  T mn = std::numeric_limits<T>::max(); 
  T mx = std::numeric_limits<T>::lowest();

  operator MnMx<f64>()
  {
    MnMx<f64> ret;
    ret.mn = (f64)mn;
    ret.mx = (f64)mx;

    return ret;
  }
};

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

f64         mn =  std::numeric_limits<f32>::lowest();  //::infinity();
f64         mx =  std::numeric_limits<f32>::max();     //::infinity(); 
f64                  msX=0, msY=0;
tbl                         glblT;
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
TblCache                 tblCache;

namespace { 
  template <class T> T remap(T val, T fromLo, T fromHi, T toLo, T toHi)
  {
    auto  dif = fromHi - fromLo;
    auto norm = dif!=0?  (val - fromLo) / dif  :  0.f;
    return norm * (toHi - toLo) + toLo;
  }

  template<class T> auto tblToVec(tbl const& t) -> std::vector<T>
  {
    auto sz = t.size();
    std::vector<T> ret(sz);

    switch(t.arrayType()){
    case tbl::TblType::I8:
      TO(sz,i){ ret[i] = (T)t.at<i8>(i);  } break;
    case tbl::TblType::I16:
      TO(sz,i){ ret[i] = (T)t.at<i16>(i); } break;
    case tbl::TblType::I32:
      TO(sz,i){ ret[i] = (T)t.at<i32>(i); } break;
    case tbl::TblType::I64:
      TO(sz,i){ ret[i] = (T)t.at<i64>(i); } break;
    case tbl::TblType::U8:
      TO(sz,i){ ret[i] = (T)t.at<u8>(i);  } break;
    case tbl::TblType::U16:
      TO(sz,i){ ret[i] = (T)t.at<u16>(i); } break;
    case tbl::TblType::U32:
      TO(sz,i){ ret[i] = (T)t.at<u32>(i); } break;
    case tbl::TblType::U64:
      TO(sz,i){ ret[i] = (T)t.at<u64>(i); } break;
    case tbl::TblType::F32:
      TO(sz,i){ ret[i] = (T)t.at<f32>(i); } break;
    case tbl::TblType::F64:
      TO(sz,i){ ret[i] = (T)t.at<f64>(i); } break;
    default:
      return ret;
    }

    return ret;                                               // this should invoke copy elision
  }
}

class  VizDraw : public nana::drawer_trigger
{
  void    drawGradient(nana::paint::graphics& g)
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
  void       drawBlack(nana::paint::graphics& g)
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
  void    drawTblGraph(tbl& t, nana::paint::graphics& g)
  {
    using namespace std;
    using namespace nana;
    using namespace nana::paint;

    if( !t.m_mem || t.size()==0 ){ 
      drawBlack(g);
    }else
    {
      //auto&    t = glblT;
      auto  ary = tblToVec<f64>(t);
      auto  len = ary.size();      // 12 floats in a vert struct
      //f32*      f = (float*)t.m_mem;
      //mx = -numeric_limits<f64>::infinity();
      //mn =  numeric_limits<f64>::infinity();
      mx =  numeric_limits<f64>::lowest();
      mn =  numeric_limits<f64>::max(); // infinity();
      TO(len,i){
        mx = max<f64>(mx, ary[i]);
        mn = min<f64>(mn, ary[i]);
      }
      if( !g.impl_ || !g.impl_->handle || !g.impl_->handle->pixbuf_ptr ) return;
      pixel_argb_t* pxbuf = g.impl_->handle->pixbuf_ptr;
      auto     w = g.width();
      auto     h = g.height();
      f64  ratio = w / (f64)len;
      f64 hRatio = h / (mx-mn);
      f64    rng = mx - mn;
      //f32 hRemap = rng 
      for(unsigned int y=0; y<h; ++y)
        for(unsigned int x=0; x<w; ++x)
        {
          pixel_argb_t p;
          p.element.red   = 0;
          p.element.green = 0;
          p.element.blue  = 0;
          //f32     val = f[ (u64)(x/ratio) ];
          f64     val = ary[ (u64)(x/ratio) ];
          f64      re =  (val - mn) / rng;
          f64   flipY = h - y - 1.f;
          f64   rempY = remap(flipY, 0., (f64)h, mn, mx);
          bool  inVal = false;
          if(val>=0 && rempY>=0 && rempY<=val){ inVal = true; }
          else if(val<=0 && rempY<=0 && rempY>=val){ inVal = true; }
          if( inVal ){ // this flips the graph vertically, but increasing y goes down in screen space, so we want it flipped
            p.element.red   = 255;
            p.element.green = 255;
            p.element.blue  = 255;
          }
          p.element.alpha_channel = 1;

          pxbuf[y*w + x] = p;
        }
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

Viz        viz;

namespace {

void  printDbList(simdb const& db, u64 idx)
{
  Println("\n\nList:");
  auto&  lv = db.s_cs.s_cl.s_lv;
  auto blks = db.blocks();
  auto prev = idx;
  auto  cur = lv[idx];
  while(cur != simdb::LIST_END){
    Print("|",prev," ",cur,"|-");
    prev = cur;
    cur  = lv[cur];
  }
  Println("|",prev," ","LIST_END|\n");
}
void   printBlock(u64 idx, CncrStr::BlkLst b)
{
  Println(
    "|",         idx,
    " hash:",    b.hash,
    " idx:",     b.idx,
    " del:",     b.isDeleted,
    " key:",     b.isKey,
    " klen:",    b.klen, 
    " len:",     b.len,
    " readers:", b.readers,
    " ver:",     b.version,
    " ");
}
void  printBlList(simdb const& db, u64 idx)
{
  Println("\n\nBlock List:");
  auto&  bl = db.s_cs.s_bls;
  auto blks = db.blocks();
  //auto prev = idx;
  auto  cur = bl[idx];
  while(idx != simdb::LIST_END){
    cur = bl[idx];
    printBlock(idx, cur);
    //Println("|",idx,
    //       " hash:",cur.hash,
    //       " idx:",cur.idx,
    //       " del:",cur.isDeleted,
    //       " key:",cur.isKey,
    //       " klen:",cur.klen, 
    //       " len:",cur.len,
    //       " readers:",cur.readers,
    //       " ver:",cur.version,
    //       " ");
    idx = cur.idx;
  }
  Println("|",idx," ","LIST_END|\n");
}
void     printdb(simdb const& db)
{
  using namespace std;

  SECTION(start off by printing the total size in bytes and all they key strings)
  {
    Println("\n\nSize: ", db.size(),"\n");

    Println("Key Strs: ");
    auto keystrs = db.getKeyStrs();
    TO(keystrs.size(),i){
      Println("Version: ", keystrs[i].ver, "   |", keystrs[i].str,"|");
    }
    Println("\n");
  }
  SECTION(print the hash map versioned index entries)
  {
    TO(db.s_ch.size(),i){ 
      simdb::VerIdx vi = db.s_ch.at( (u32)i );
      if(vi.idx == simdb::EMPTY)
        Print("|EMPTY ",vi.version,"|  ");
      else if(vi.idx == simdb::DELETED)
        Print("|DELETED ",vi.version,"|  ");
      else
        Print("|",vi.idx," ",vi.version,"|  ");
    }
    Println("\n");
  }
  SECTION(go through the versioned index entries and print off any that arent empty)
  {
    TO(db.s_ch.size(),i)
    { 
      simdb::VerIdx vi = db.s_ch.at( (u32)i );
      if(vi.idx < simdb::DELETED)
      {
        Print("\n\n|",vi.idx," ",vi.version,"|   ");

        printDbList(db, vi.idx);
        printBlList(db, vi.idx);

        u32    vlen = 0;
        u32 readLen = 0;
        u32     len = db.s_cs.len(vi.idx, vi.version, &vlen);

        vec_u8 buf(len+1);
        db.s_cs.get(vi.idx, vi.version, buf.data(), len, &readLen);
        buf.back() = '\0';
      
        Println("Len: ", len,
                "  Value Len: ", vlen,
                "  Read Len: ", readLen,
                "\n|");
      
        TO(buf.size(),i){
          if(buf[i]==7) putc(' ',stdout); 
          else          putc(buf[i],stdout);
        }
        putc('|',stdout);
      }
    }
  }
  SECTION(loop through the concurrent list and print off all the chains)
  {
    Println("\n\nConcurrent List:");
    auto&  lv = db.s_cs.s_cl.s_lv;
    auto&  bl = db.s_cs.s_bls;
    auto blks = db.blocks();
    TO(blks,i)
    {
      printDbList(db, i);
    }
    Println("\n");
    TO(blks,i){
      printBlock(i, bl[i]);
    }
    Println("\n");
  }
}

template<class T> MnMx<T> calcMnMx(tbl const& t)
{
  using namespace std;

  MnMx<T> ret;
  TO(t.size(),i){
    ret.mn = min<T>( ret.mn, (T)t[i] );
    ret.mx = max<T>( ret.mx, (T)t[i] );
  }

  return ret;
}
template<class T> str      statStr(tbl const& t)
{
  using namespace std;

  u64 vlen = t.size();           // 12 floats in a vert struct
  if(vlen==0) return "";
  if(vlen==1) return toString( (T)t[0] );
  T*     v = (T*)t.data();
  sort(v, v+vlen);               // sort for both the median and the mode

  f64 mean=0;
  T median=0; 
  SECTION(calc mean and median)
  {
    //f64 total = 0;
    TO(vlen,i){ mean += (f64)((T)v[i]) / (f64)vlen; } // divide every number by the total first to avoid overflow, even though this will be slower
    //mean = total / (f64)vlen;

    u64 mid = vlen / 2;
    median  = (T)v[mid];
  }

  T hiVal=0; u64 hiCnt=0;
  SECTION(calc Mode - the value with the highest frequency)
  {
    T   curVal=0;
    u64 curCnt=0;
    TO(vlen,i)
    {
      if( (T)v[i] == curVal ){ ++curCnt; }
      else{ 
        curCnt = 1; 
        curVal = (T)v[i];
      }

      if( curCnt > hiCnt ){
        hiVal  = curVal;
        hiCnt  = curCnt;
      }

    }

    if( curCnt > hiCnt ){
      hiVal = curVal;
      hiCnt = curCnt;
    }
  }

  f64 variance=0;
  SECTION(calc variance)
  {
    f64 difSqr = 0;
    TO(vlen,i){
      f64 dif  = (T)v[i] - mean;
      difSqr  += dif * dif;
    }
    variance = sqrt( (f64)difSqr / (f64)vlen  );
  }

  T tmn, tmx;
  SECTION(calc min and max)
  {
    tmx =  numeric_limits<T>::lowest(); 
    tmn =  numeric_limits<T>::max();
    TO(vlen,i){
      tmx = max<T>(tmx, (T)v[i]);
      tmn = min<T>(tmn, (T)v[i]);
    }
  }

  str txtStr = toString(
    "Min: ",tmn,
    "   Max: ",tmx,
    "   Mean: ",mean,
    "   Median: ",median,
    "   Mode: ", hiVal, " (",hiCnt,
    ")   Variance: ", variance);

  return txtStr;
}

MnMx<f64>      makeMnMx(tbl const& t)
{
  switch(t.arrayType())
  {
  case tbl::TblType::I8:
    return calcMnMx<i8>(t);
  case tbl::TblType::I16:
    return calcMnMx<i16>(t);
  case tbl::TblType::I32:
    return calcMnMx<i32>(t);
  case tbl::TblType::I64:
    return calcMnMx<i64>(t);
  case tbl::TblType::U8:
    return calcMnMx<u8>(t);
  case tbl::TblType::U16:
    return calcMnMx<u16>(t);
  case tbl::TblType::U32:
    return calcMnMx<u32>(t);
  case tbl::TblType::U64:
    return calcMnMx<u64>(t);
  case tbl::TblType::F32:
    return calcMnMx<f32>(t);
  case tbl::TblType::F64:
    return calcMnMx<f64>(t);
  default:
    return {0,0};
  }
}
str         makeStatStr(tbl const& t)
{
  switch(t.arrayType())
  {
  case tbl::TblType::I8:
    return statStr<i8>(t);
  case tbl::TblType::I16:
    return statStr<i16>(t);
  case tbl::TblType::I32:
    return statStr<i32>(t);
  case tbl::TblType::I64:
    return statStr<i64>(t);
  case tbl::TblType::U8:
    return statStr<u8>(t);
  case tbl::TblType::U16:
    return statStr<u16>(t);
  case tbl::TblType::U32:
    return statStr<u32>(t);
  case tbl::TblType::U64:
    return statStr<u64>(t);
  case tbl::TblType::F32:
    return statStr<f32>(t);
  case tbl::TblType::F64:
    return statStr<f64>(t);
  default:
    return "";
  }
}
str              valStr(tbl::KV const& kv)
{
  switch(kv.type)
  {
  case tbl::TblType::I8:
    return toString( *((i8*)&kv.val) );
  case tbl::TblType::I16:
    return toString( *((i16*)&kv.val) );
  case tbl::TblType::I32:
    return toString( *((i32*)&kv.val) );
  case tbl::TblType::I64:
    return toString( *((i64*)&kv.val) );
  case tbl::TblType::U8:
    return toString( *((u8*)&kv.val) );
  case tbl::TblType::U16:
    return toString( *((u16*)&kv.val) );
  case tbl::TblType::U32:
    return toString( *((u32*)&kv.val) );
  case tbl::TblType::U64:
    return toString( *((u64*)&kv.val) );
  case tbl::TblType::F32:
    return toString( *((f32*)&kv.val) );
  case tbl::TblType::F64:
    return toString( *((f64*)&kv.val) );
  default:
    return "";
  }
}
void          insertTbl(str const& parentKey, tbl const& t, IdxKey ik)
{
  using namespace std;
  
  SECTION(array visualization)
  {
    str aszStr = toString("Type: ", t.typeStr(), " Size: ", t.size());
    str aryKey = parentKey+"/ary"; 
    tree.insert(aryKey, aszStr);

    //tree.insert(aryKey+"/arystr", "");

    tree.insert(aryKey+"/arystr", makeStatStr(t) );   // make the stat string immediatly
  }
  SECTION(hash map elements)
  {
    str elemsTreeKey = parentKey+"/elems";
    tree.insert(elemsTreeKey, toString("Elements: ", t.elems(), "  Map Capacity: ", t.map_capacity() ) );

    vector<tbl::KV> kvs;
    SECTION(extract the non empty key-value structs in the map segment and sort them)
    {
      TO(t.map_capacity(),i)
      {
        tbl::KV kv = t.elemStart()[i];
        if(kv.isEmpty()) continue;

        kvs.push_back(kv);
      }
      sort(ALL(kvs), [](tbl::KV const& a, tbl::KV const& b){ return str(a.key) < str(b.key); });
    }

    for(auto const& kv : kvs)
    {
      str  elemKey = kv.key; // get the key into a string so it can be compared to "type" easily
      str    title;
      SECTION(create the title which will include the key name, the type and the value)
      {
        tbl  elemTbl;
        if(kv.type & tbl::TblType::TABLE){  // if the element is a table, make a tbl out of it and recurse this function 
          elemTbl = tbl( ((u8*)t.childData() + kv.val) );
          title   = toString(elemKey," (", elemTbl.typeStr() ,"):   ", kv.val);
        }else if(elemKey == "type"){
          char typeNumAsStr[9];                                                   // this is type number as a string - this is a special case for an element with the key "type", which can be a 64 bit number that can also be read as 8 characters 
          memcpy(typeNumAsStr, &kv.val, 8);
          typeNumAsStr[8] = '\0';                                                 // make the 9th character (offset 8) 0 so that it is the end of a C string 
          title = toString(elemKey,":  ", typeNumAsStr, "  -  (",kv.val,")");
        }else{
          title = toString(elemKey," (", kv.typeStr() ,"):   ", valStr(kv) );
        }
      }
      
      str thsTreeKey = toString(elemsTreeKey,"/", elemKey);
      tree.insert(thsTreeKey, title); // e.as<tbl<>::i64>()) ); 

      if(kv.type & tbl::TblType::TABLE){  // if the element is a table, make a tbl out of it and recurse this function 
        tbl elemTbl( ((u8*)t.childData()+kv.val) );
        str subKey = ik.key + "/" + kv.key;
        tblCache[ik.idx][subKey] = elemTbl;
        IdxKey subIk;
        subIk.subTbl = true;
        subIk.idx    = ik.idx;
        subIk.key    = subKey;
        insertTbl(thsTreeKey, elemTbl, subIk);
      }
    }
  }

  tblKeys[parentKey] = ik;
}
vec_u8     extractDbKey(simdb const& db, str key)
{
  u32     vlen = 0;
  u32  version = 0;
  u64      len = db.len(key, &vlen, &version);          // todo: make ui64 as the input length

  vec_u8 ret(vlen);
  u32 readLen = 0;
  bool ok = db.get( (void*)key.c_str(), (u32)key.length(), ret.data(), vlen, &readLen);
  //bool ok = db.get(key.c_str(), ret.data(), vlen);

  if( !ok  ||  readLen < vlen ) return vec_u8();

  return ret;
}
void        regenLabels(tbl const& t)
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
void       regenTblInfo()
{
  SECTION(list simdb files and insert them at the top level of the tree)
  {
    glblT.m_mem = nullptr;
    tblBuf.resize(0);
    dbs.clear();
    tblKeys.clear();

    simdb_error err;
    auto dbPths = simdb_listDBs(&err);
    tblCache.resize( dbPths.size() );
    TO(dbPths.size(),i)
    {
      auto const& pth = dbPths[i];
      tree.insert(pth, pth);
      
      dbs.emplace_back(pth.c_str(), 4096, 1 << 14);
      simdb&         db = dbs.back();
      vec_verstr dbKeys = db.getKeyStrs();                                      // Get all keys in DB - this will need to be ran in the main loop, but not every frame
      //tblCache[i].resize(dbKeys.size());
      TO(dbKeys.size(),j)
      {
        auto const& key = dbKeys[j];
        auto     tblKey = pth+"/"+key.str;
        tree.insert(tblKey, key.str);

        tblBuf     = extractDbKey(db, key.str);
        if(tblBuf.size() < sizeof(tbl::TblFields)){
          continue;
        }
        //IvTbl ivTbl(tblBuf.data());
        tbl ivTbl(tblBuf.data());
        tblCache[i][key.str] = tbl(ivTbl);

        IdxKey ik;
        ik.subTbl = false;
        ik.idx    = (u32)i;
        ik.key    = key.str;
        insertTbl(tblKey, ivTbl, ik);
        //tblKeys[tblKey] = ik;
      }
    }
  }
  regenLabels( glblT );
}
str          getFullKey(nana::treebox::item_proxy const& ip)
{
  auto* cur = &ip;
  str   key = cur->key();
  while(cur->level() > 1){
    cur = &(cur->owner());
    key = cur->key() + "/" + key;
  }

  return key;
}
bool         isTableKey(str const& key)
{
  auto iter = tblKeys.find(key);
  return iter != tblKeys.end();
}
bool         isTableKey(nana::treebox::item_proxy const& ip)
{
  return isTableKey( getFullKey(ip) );
}
tbl          tblFromKey(str key)
{
  tbl ret;
  auto iter = tblKeys.find(key);
  if( iter == tblKeys.end() ) return ret;

  IdxKey& ik = iter->second;
  if( ik.subTbl ) return ret;

  auto buf = extractDbKey( dbs[ik.idx], ik.key );

  if(buf.size() >= tbl::memberBytes() ){
    ret = tbl(buf.data(),false,true);
    return ret;
  }else{ return ret; }
}
tbl*    setCurTblFromTreeKey(str key)  // set current table from tree key
{
  //if( !isTableKey(key) ) return nullptr;

  auto iter = tblKeys.find(key);
  if( iter == tblKeys.end() ) return nullptr;
  
  IdxKey& ik = iter->second;
  if( ik.subTbl ){
    glblT = tblCache[ik.idx][ik.key];
    return &glblT;
  }else{
    tblBuf = extractDbKey( dbs[ik.idx], ik.key );

    if(tblBuf.size() >= tbl::memberBytes() ){
      glblT  = tbl(tblBuf.data());
      return &glblT;
    }else{ return nullptr; }
  }
}
vec_u8         readFile(const char* path)
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
void         refreshViz()
{
  viz.caption("Setting this caption is the only way I know to refresh the component");
}
void          openFiles(vec_str const& files)
{
  int i = 0;
  for(auto& f : files){
    //Println("file ", i, ": ", f);
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
      //Println("drop file event");

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
        msX         = (f32)arg.pos.x;
        msY         = (f32)arg.pos.y;
        auto    vsz = viz.size();
        f32       h = (f32)vsz.height;
        auto&     t = glblT;
        if( t.m_mem && t.size()>0 )
        {
          auto    ary = tblToVec<f64>(t);
          u64     len = ary.size();                 // 12 floats in a vert struct
          f64   ratio = vsz.width  / (f64)len;
          auto   mnmx = makeMnMx(t);
          f64     dif = mnmx.mx - mnmx.mn;
          u64     idx = (u64)(msX/ratio);
          auto    val = ary[idx];
          f64  remapY = (((h-msY)/h));
          remapY     *= dif;
          remapY     += mn;
          auto capStr = toString("  ",val,
          "   X: ",arg.pos.x,
          "   Y: ",arg.pos.y,
          "   Index: ",idx,
          "   Cursor Pos: ", (f32)remapY);
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
    //mb.push_back("&Print Database");
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
      glblT  = tbl(tblBuf.data(), false, false);
      if(glblT.sizeBytes() > tblBuf.size()){
        glblT.m_mem = nullptr;
        tblBuf.resize(0);
        //Println("File size and tbl size did not match");
      }
      refreshViz();

      openFiles({path});

      //Println("File Opened");
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

      //Println("File Written");
    });

    auto& helpMenu = mb.at(1);
    helpMenu.append("&About", [](auto& itmprxy){
      cout << "About pressed" << endl;
    });

    //auto& printMenu = mb.at(2);
    helpMenu.append("&Print Database", [](auto& itmprxy){
      printdb( dbs[1] );
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
            auto    t = tblFromKey(getFullKey( tbArg.item.owner() ));

            str statStr = makeStatStr(t);
            tbArg.item.child().text(statStr);
          }

          //Println("");
          //Println("txt: ", tbArg.item.text(), " key: ", tbArg.item.key() );
          //Println("expanded: ", tbArg.item.expanded() );
          //Println("");
        }
      });
      tree.events().selected([](const arg_treebox& tbArg) mutable
      {
        sel.clear();

        //Println("key:  ", tree.selected().key() );
        //Println("selected: ", tbArg.item.key() );
        //Println("owner: ",    tbArg.item.owner().key() );

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
        //Println(key);

        bool isTbl = isTableKey(key);
        //Println("isTableKey: ", isTbl);

        tbl* curT = setCurTblFromTreeKey(key);
        if(curT){
          regenLabels( *curT );
        }
        if(curT == nullptr) return;

        refreshViz();

        auto&   t = *curT;
        auto flen = t.size();// * 12;      // 12 floats in a vert struct
        f32*    f = (float*)t.m_mem;

        mx = numeric_limits<f32>::lowest(); 
        mn = numeric_limits<f32>::max();
        TO(flen,i){
          //Print( (f32)f[i], " " );
          mx = max<f32>((f32)mx, (f32)f[i] );
          mn = min<f32>((f32)mn, (f32)f[i] );
        }
        //Println("\n Table Key mx: ",mx, " mn: ", mn, " \n" );

        plc.collocate();
        fm.collocate();
        fm.activate();

        //Println("\n\n");
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












//
//printBlList(db, i);

//  auto prev = i;
//  auto  cur = lv[i];
//  while(cur != simdb::LIST_END){
//    //if(prev==i){ ++i; }
//    Print("|",prev," ",cur,"|-");
//    prev = cur;
//    cur  = lv[cur];
//  }
//  //if(prev==i){ ++i; }
//  Println("|",prev," ","LIST_END|\n");

//
//if(cur==(i+2)){ ++i; }

//std::vector<i8> memv(db.memsize(), 0);
//memcpy( (void*)memv.data(), db.mem(), db.memsize() );
//
//Println("\n");
//
//u64 blksz = db.blockSize();
//TO(memv.size(),i){ 
//  if(i % blksz == 0){
//    putc('|', stdout);
//  }
//  putc(memv[i] ,stdout);
//}

//str tblKey = getFullKey( tbArg.item );
//
//auto    t = tblFromKey( tblKey );

//auto flen = t.size() * 12;      // 12 floats in a vert struct
//f32*    f = (float*)t.m_mem;
//sort(f, f+flen);                // sort for both the median and the mode
//
//f32 mean=0, median=0; 
//SECTION(calc mean and median)
//{
//  f32 total = 0;
//  TO(flen,i){ total += f[i]; }
//  mean = total / flen;
//
//  u64 mid = flen / 2;
//  median  = f[mid];
//
//  Println("\n");
//  TO(flen,i){
//    Print(f[i]," "); 
//  }
//  Println("\n");
//}
//
//f32 hiVal=0; u64 hiCnt=0;
//SECTION(calc Mode - the value with the highest frequency)
//{
//  f32 curVal=0;
//  u64 curCnt=0;
//  TO(flen,i){
//    if(f[i] == curVal){ ++curCnt; }
//    else if( curCnt > hiCnt ){
//      hiVal  = curVal;
//      hiCnt  = curCnt;
//      curVal = f[i];
//      curCnt = 0;
//    }
//  }
//  if( curCnt > hiCnt ){
//    hiVal = curVal;
//    hiCnt = curCnt;
//  }
//}
//
//f32 variance=0;
//SECTION(calc variance)
//{
//  f32 difSqr = 0;
//  TO(flen,i){
//    f32 dif  = f[i] - mean;
//    difSqr  += dif * dif;
//  }
//  variance = flen>1?  difSqr/(flen-1)  :   difSqr;  // length of the array needs to be at least 2 to use the n-1 'unbiased' variance  
//}
//
//f32 tmn, tmx;
//SECTION(calc min and max)
//{
//  tmx = -numeric_limits<float>::infinity(); 
//  tmn =  numeric_limits<float>::infinity();
//  TO(flen,i){
//    tmx = max<f32>(tmx, f[i]);
//    tmn = min<f32>(tmn, f[i]);
//  }
//}
//
//str txtStr = toString(
//  "Min: ",tmn,
//  "   Max: ",tmx,
//  "   Mean: ",mean,
//  "   Median: ",median,
//  "   Mode: ", hiVal, " (",hiCnt,
//  ")   Variance: ", variance);

//f32*      f = (f32*)t.m_mem;
//f64  hRatio = vsz.height / dif;
//auto    val = (f32)t[idx];

//ik.subTbl  = true;
//ik.key     = subKey;

//IdxKey ik;
//ik.subTbl = true;
//ik.idx    = idx;
//ik.key    = elemKey;

//TO(t.map_capacity(),i)
//{
//  tbl::KV kv = t.elemStart()[i];
//  if(kv.isEmpty()) continue;

//variance = vlen>1?  difSqr/(vlen-1)  :   difSqr;  // length of the array needs to be at least 2 to use the n-1 'unbiased' variance  
//variance = 1.0; //std::sqrt<double>( 2.0 );
//variance = std::sqrt( (double)( ((double)difSqr) / ((double)vlen) ) );
//variance = std::sqrt( 2.0 );

//str       makeStatStr(tbl const& t)
//{
//  using namespace std;
//  
//  auto flen = t.size();           // 12 floats in a vert struct
//  if(flen==0) return "";
//  if(flen==1) return toString( (f32)t[0] );
//  f32*    f = (f32*)t.data();
//  sort(f, f+flen);                // sort for both the median and the mode
//
//  f32 mean=0, median=0; 
//  SECTION(calc mean and median)
//  {
//    f32 total = 0;
//    TO(flen,i){ total += f[i]; }
//    mean = total / flen;
//
//    u64 mid = flen / 2;
//    median  = f[mid];
//
//    Println("\n");
//    TO(flen,i){
//      Print(f[i]," "); 
//    }
//    Println("\n");
//  }
//
//  f32 hiVal=0; u64 hiCnt=0;
//  SECTION(calc Mode - the value with the highest frequency)
//  {
//    f32 curVal=0;
//    u64 curCnt=0;
//    TO(flen,i){
//      if(f[i] == curVal){ ++curCnt; }
//      else if( curCnt > hiCnt ){
//        hiVal  = curVal;
//        hiCnt  = curCnt;
//        curVal = f[i];
//        curCnt = 0;
//      }
//    }
//    if( curCnt > hiCnt ){
//      hiVal = curVal;
//      hiCnt = curCnt;
//    }
//  }
//
//  f32 variance=0;
//  SECTION(calc variance)
//  {
//    f32 difSqr = 0;
//    TO(flen,i){
//      f32 dif  = f[i] - mean;
//      difSqr  += dif * dif;
//    }
//    variance = flen>1?  difSqr/(flen-1)  :   difSqr;  // length of the array needs to be at least 2 to use the n-1 'unbiased' variance  
//  }
//
//  f32 tmn, tmx;
//  SECTION(calc min and max)
//  {
//    tmx = -numeric_limits<float>::infinity(); 
//    tmn =  numeric_limits<float>::infinity();
//    TO(flen,i){
//      tmx = max<f32>(tmx, f[i]);
//      tmn = min<f32>(tmn, f[i]);
//    }
//  }
//
//  str txtStr = toString(
//    "Min: ",tmn,
//    "   Max: ",tmx,
//    "   Mean: ",mean,
//    "   Median: ",median,
//    "   Mode: ", hiVal, " (",hiCnt,
//    ")   Variance: ", variance);
//
//  return txtStr;
//}

//TO(vlen,i){ total += v[i]; }
//mean = total / flen;

//Println("\n");
//TO(flen,i){
//  Print(f[i]," "); 
//}
//Println("\n");

//bool inVal = (h-y)/(f32)h < re;
//if(h-y < val*hRatio){ // this flips the graph vertically, but increasing y goes down in screen space, so we want it flipped
//
//str mxStr = toString("Max: ", mx);
//g.bidi_string( {0,0}, mxStr.c_str(), mxStr.length() );
//g.string( {0,0}, mxStr, {255,255,255,255} );

//vector<f32>  tst(512);
//TO(tst.size(),i){ tst[i] = rand() / (f32)RAND_MAX; }
//flen = 512;
//f = tst.data();

//vizImg.image_ptr_ = mempxbuf;
//viz.load(vizImg);

//tblKeys[parentKey] = parentKey;
//tblKeys.insert(parentKey);
//
//aryViz     = tree.insert(aryKey+"/arystr", "");

//struct   vert { f32 p[3],n[3],c[4],tx[2]; };
//using   IvTbl = tbl<vert>;
//
//IvTbl                       glblT;

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
