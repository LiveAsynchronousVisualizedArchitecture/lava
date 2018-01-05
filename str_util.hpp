

#ifndef __STR_UTIL_HEADERGUARD_HPP__
#define __STR_UTIL_HEADERGUARD_HPP__

#include <string>
#include <ostream>
#include <iostream>
#include <sstream>

template<class T> inline std::string 
toString(T const& x)
{
  std::ostringstream convert;
  convert << x;
  return convert.str();
}
template<class T1, class... T> inline std::string
toString(const T1& a, const T&... args)
{
  return toString(a) + toString(args...) ;
}

inline std::ostream&  Print(std::ostream& o) { return o; }
template<class... T> inline std::ostream&
Print(std::ostream& o, const T&... args)
{
  o << toString(args ...);
  o.flush();
  return o;
}
template<class... T> inline void Print(const T&... args){ Print(std::cout, args...); }

//template<class... T> inline std::ostream&
//Print(std::ostream& o, const T&... args)
//{
//  o << toString(args ...);
//  o.flush();
//  return o;
//}

template<class... T> inline std::ostream&
Println(std::ostream& o, const T&... args){ Print(o, args..., "\n"); return o; }
template<class... T> inline void Println(const T&... args){ Println(std::cout, args...); }

#endif
