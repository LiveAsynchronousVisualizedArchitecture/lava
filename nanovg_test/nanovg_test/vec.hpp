
#if defined(_WIN32)
  #pragma once
#endif

#ifndef __VEC_HEADER_GUARD__
#define __VEC_HEADER_GUARD__

#include "no_rt_util.h"

union  vec3i
{
  struct { int v0, v1, v2; };
  int v[3];

  int&       operator[](int i)       {return v[i];}
  int const& operator[](int i) const {return v[i];}
};
using  v3i = vec3i;

union  vec3
{
  struct { float x,y,z;  };
  struct { float r,g,b;  };
  float c[3];

  float&       operator[](int i)       {return c[i];}
  float const& operator[](int i) const {return c[i];}
};
inline vec3   Vec3(float x=0, float y=0, float z=0)
{
  vec3 v; v.x=x; v.y=y, v.z=z; return v;
}
inline vec3   operator+(vec3 const& a, vec3 const& b)
{
  vec3 ret;
  ret.x = a.x + b.x;
  ret.y = a.y + b.y;
  ret.z = a.z + b.z;
  return ret;
}
inline vec3   operator-(vec3 const& a, vec3 const& b)
{
  vec3 ret;
  ret.x = a.x - b.x;
  ret.y = a.y - b.y;
  ret.z = a.z - b.z;
  return ret;
}
inline vec3   operator*(float a, vec3 const& b)
{
  vec3 ret;
  ret.x = a*b.x;
  ret.y = a*b.y;
  ret.z = a*b.z;
  return ret;
}
inline vec3   operator*(vec3 const& a, vec3 const& b)
{
  vec3 ret;
  ret.x = a.x * b.x;
  ret.y = a.y * b.y;
  ret.z = a.z * b.z;
  return ret;
}
inline vec3   operator/(vec3 const& a, vec3 const& b)
{
  vec3 ret;
  ret.x = a.x / b.x;
  ret.y = a.y / b.y;
  ret.z = a.z / b.z;
  return ret;
}
inline bool   operator<(vec3 const& a, vec3 const& b)
{
  if(a.x<b.x) return true; if(a.x>b.x) return false;
  if(a.y<b.y) return true; if(a.y>b.y) return false;
  if(a.z<b.z) return true; if(a.z>b.z) return false;
  return false;
}
inline bool   operator==(vec3 const& a, vec3 const& b)
{
  if(a.x!=b.x || a.y!=b.y || a.z!=b.z) return false;
  return true;
}
inline float  dot(vec3 const& a, vec3 const& b)
{
  return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}
inline vec3   cross(vec3 const& a, vec3 const& b)
{
  vec3 v; 
  v.x = (a.y*b.z - a.z*b.y);
  v.y = (a.z*b.x - a.x*b.z);
  v.z = (a.x*b.y - a.y*b.x);
  return v;

  //cross    ( const Vec3<T>& a, const Vec3<T>& b ) 
  // { return Vec3<T>(
  //  msub(a.y,b.z,a.z*b.y), 
  //  msub(a.z,b.x,a.x*b.z), 
  //  msub(a.x,b.y,a.y*b.x)); }
  //return (a.y*b.z - a.z*b.y)*I - (a.x*b.z - a.z*b.x)J + (a.x*b.y - a.y*b.x)K;
  //vec3 v; 
  //v.x = (a.y*b.z - a.z*b.y);
  //v.y = (a.x*b.z - a.z*b.x);
  //v.z = (a.x*b.y - a.y*b.x);
  //return v;

  //return length(v);
}
inline float  sqrlen(vec3 const& a)
{
  return dot(a,a);
}
inline float  length(vec3 const& a)
{
  return sqrtf(dot(a,a));
}
inline vec3   PtOnLine(vec3 pt, vec3 AA, vec3 BB, float* t=nullptr)
{
  vec3  AB      =  BB  - AA;
  vec3  AP      =  pt  - AA;
  float AB_dot  =  dot(AB,AB);
  float AP_AB   =  dot(AP,AB);
  float _t      =  AB_dot==0? 0 : AP_AB / AB_dot;
  
  if(t) *t=_t;
  return vec3(_t*AB + AA);
}
inline vec3   extractSign(vec3* inout)
{
  vec3 ret;
  TO(3,d) ret[d]      = copysign(1.f,(*inout)[d]);
  TO(3,d) (*inout)[d] = abs( (*inout)[d] );
  return ret;
}
using  v3    = vec3;
using  v3f   = vec3;
//using  vecv3 = vec<v3>; 

union  vec2
{
  struct { float x,y; };
  struct { float r,g; };
  float c[2];

  float&       operator[](int i)       {return c[i];}
  float const& operator[](int i) const {return c[i];}
};
using  v2  = vec2;
using  v2f = vec2;


#endif
