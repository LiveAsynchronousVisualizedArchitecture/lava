
#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __FISSURE_DECL_HEADERGUARD_HPP__
#define __FISSURE_DECL_HEADERGUARD_HPP__

#include "../no_rt_util.h"
#include "vec.hpp"
#include <nanogui/nanogui.h>

using namespace nanogui;

const int   TITLE_MAX_LEN = 256;
const v2    NODE_SZ       = { 256.f, 64.f };
const v2    NODE_HALF_SZ  = NODE_SZ/2.f;
const auto  NODE_CLR      = nvgRGBf(.1f,.4f,.5f);
const float INFf          = std::numeric_limits<float>::infinity();
const float SIG_NANf      = std::numeric_limits<float>::signaling_NaN();
const f32   IORAD         = 15.f;

template<class T> using vec = std::vector<T>;
using str = std::string;

using e2i  =  Eigen::Vector2i;
using e4f  =  Eigen::Vector4f;

using     veci     =    vec<int>;
using   vecstr     =    std::vector<str>;
using   vec_v2     =    vec<v2>;
using cnct_tbl     =    std::unordered_multimap<int,int>;

struct  node { v2 P; str txt; LavaNode ln; };
using   vec_nd     =    vec<node>;

union      bnd 
{
  struct { float xmn, ymn, xmx, ymx; };  // todo: make into a union?
  struct { v2 mn; v2 mx; };
  struct { v4 mnmx; };
  float c[4];

  bnd() : xmn(INFf), ymn(INFf), xmx(-INFf), ymx(-INFf) {}
  bnd(float Xmn, float Ymn, float Xmx, float Ymx) :
    xmn(Xmn), ymn(Ymn), xmx(Xmx), ymx(Ymx)
  {}

  float&       operator[](int i)       {return c[i];}
  float const& operator[](int i) const {return c[i];}
  bnd& operator()(float Xmn, float Ymn, float Xmx, float Ymx)
  {
    xmn=Xmn; ymn=Ymn; xmx=Xmx; ymx=Ymx; return *this;
  }
  operator v4&(){ return mnmx; }

  float w(){return abs(xmx-xmn);}
  float h(){return abs(ymx-ymn);}
};
using vec_nbnd     =    vec<bnd>;

struct  cnct { 
  int src, dest;
  ui8 src_out, dest_in;
  bool operator<(cnct const& r){ return src < r.src; }
  bool operator==(cnct const& r){ return src==r.src && dest==r.dest; }

  static bool lessDest(cnct const& l, cnct const& r){
    return l.dest < r.dest;
  }
};
using  vec_con     =    vec<cnct>;

struct  slot { v2 P; u64 nidx; };
using vec_slot     =    vec<slot>;

struct FisData
{
  GLFWwindow*            win;                     // Platform 

  struct
  {
    int w, h;
    f32 ptSz, hudSz, guideSz;                              // ptSz is point size  |  hudSz is  heads up display size
    Screen         screen;
    Window*        keyWin = nullptr;
    Window*         dbWin = nullptr;
    BoxLayout*     keyLay = nullptr;
    ComboBox*       dbLst = nullptr;
    int          dbLstIdx = -1;
    NVGcontext*       nvg = nullptr;
    veci           dbIdxs;
    vecstr        dbNames;
    int         dbNameIdx;
    str            dbName;
    bool        showGuide;

  } ui;
};

#endif
