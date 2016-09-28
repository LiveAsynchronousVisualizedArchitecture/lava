

#include <stdint.h>
#include <atomic>
#include <mutex>
#include <vector>
#include <string>
#include <random>
#include <iostream>
#include <sstream>
#include <thread>

#include "ConcurrentMap.hpp"

#include <windows.h>

//#include <SIM/SIM_GeneralTemplateUtil.hpp>

#ifndef PAUSE
  #define PAUSE std::cout << "Paused at line " << __LINE__ << std::endl; int VAR##__LINE__; std::cin >> VAR##__LINE__;
#endif

#ifndef TO
  #define TO(to, var)      for(std::remove_const<decltype(to)>::type var = 0; var < to; ++var)
#endif

ui32       intHash(ui32    h)
{
  //h += 1;
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;
  return h;
}
ui32  nextPowerOf2(ui32    v)
{
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;

  return v;
}

template <typename T> struct RngInt
{
  std::mt19937                       m_gen;
  std::uniform_int_distribution<T>   m_dis;
  
  RngInt(T lo = 0, T hi = 1, int seed = 16807)
   : m_gen(seed), m_dis(lo, hi)
  { }

  inline T operator()()
  { return m_dis(m_gen); }

  inline T operator()(T lo, T hi)
  {
    std::uniform_int_distribution<T>  dis(lo, hi); 
    return dis(m_gen);
  }
};

template<class STR>
STR keepAlphaNumeric(STR const& s)
{
  using namespace std;

  regex            alphaNumeric("[a-zA-Z\\d]+");
  sregex_iterator  iter( ALL(s), alphaNumeric );
  sregex_iterator  iter_end;

  STR out;
  while( iter != iter_end )
    out += iter++->str();      // ...

  return out;
}

template<class STR1, class STR2>
STR1 subNonFilename(STR1 const& s, STR2 const& substr)
{
  using namespace std;

  //string    patStr("[#%&\\{\\}\\\\<>\\*\\?/\\w\\$!'\":@\\+`\\|=\\.]+");
  //string    patStr("#|%|&|\\{|\\}|\\\\|<|>|\\*|\\?|/|\\w|\\$|!|'|\"|:|@|\\+|`|\\||=|\\.");

  STR1      patStr(":|\\*|\\.|\\?|\\\\|/|\\||>|<");
  regex     pattern(patStr);
  return regex_replace(s, pattern, substr);
}


template<class T> inline auto
Concat(const T& a) -> T
{ return a; }
template<class T1, class... T> inline auto
Concat(const T1& a, const T&... args) -> T1
{
  //T1 ret;
  //ret.append( ALL(a) );
  //ret.append( ALL(Concat(args...)) );
  return a + Concat(args...);
}

inline std::string 
toString(std::vector<std::string> const& v)
{
  using namespace std;
  
  ostringstream convert;
  TO(v.size(),i) convert << v[i] << " ";
  convert << endl;
  return convert.str();
}

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

template< template<class...> class L, class... T, int IDX = 0> std::string 
toString(const std::tuple<T...>& tpl)
{
  using namespace std;
  
  const auto len = mp_len<T...>::value;
  
  string ret;
  ret  +=  toString(get<IDX>(tpl), " ");
  if(IDX < len-1) ret  += toString(get<IDX+1>(tpl));
  return ret;
}

inline std::ostream&  Print(std::ostream& o) { return o; }
template<class... T> inline std::ostream&
 Print(std::ostream& o, const T&... args)
{
  o << toString(args ...);
  o.flush();
  return o;
}
template<class... T> inline std::ostream&
 Println(std::ostream& o, const T&... args)
{
  //o << toString(args...) << std::endl;
  Print(o, args..., "\n");
  return o;
}
template<class... T> inline void
 Print(const T&... args)
{
  Print(std::cout, args...);
  //std::cout << toString(args...);
}
template<class... T> inline void
 Println(const T&... args)
{
  Println(std::cout, args...);
  //std::cout << toString(args...) << std::endl;
}
template<class T> inline void
 PrintSpaceln(const T& a)
{
  Print(std::cout, a);
}
template<class T1, class... T> inline void
 PrintSpaceln(const T1& a, const T&... args)
{
  Print(std::cout, a, " ");
  PrintSpaceln(args...);
  Println();
}


      template<class T, class _Alloc=std::allocator<T> > 
using vec        =  std::vector<T, _Alloc>;

using std::thread;

int main()
{
  using namespace std;
  
  //ui32 sz = 18921703;
  //ui32 sz = 400;
  //ConcurrentMap cm(sz);

  //ScopeTime t;
  //t.start();
  //cm.init(sz);
  //t.stop("Init");

  //Println( (i64)t.stop() );
  //t.start();
  //
  //Println("sz: ", cm.size());
  //
  //TO(100,i) {
  //  Println("i: ",i," ", intHash(i) );
  //}
  //
  //TO(100,i) {
  //  Println("i: ",i," ", nextPowerOf2(i));
  //}

  //
  //ui32 loopSz  =  (ui32)( double(cm.size()) / 1.5);

  //
  //RngInt<int> rng(1, 2);

  //vec<thread> thrds;
  //TO(5,tid)
  //{
  //  thrds.push_back( thread([&cm, &rng, loopSz, tid]()
  //  {
  //    ScopeTime t;
  //
  //    t.start();
  //    TO(loopSz, i) {
  //      auto val = i*10 + tid;
  //      ui32 pidx = cm.put(i, val);
  //      SleepMs( rng() );
  //
  //      //SleepMs( (int)pow(4-tid,2) );
  //      //cout << pidx << "  ";
  //      //Println("Put Idx: ", (i64)pidx);
  //    }
  //    t.stop("Put");
  //    //Println( t.getSeconds() );
  //
  //    //t.start();
  //    //TO(loopSz, i) {
  //    //  ui32 gidx = cm.get(i);
  //    //  cout << gidx << "  ";
  //    //  //Println("Get Idx: ", (i64)gidx);
  //    //}
  //    //t.stop("Get");
  //    //Println( t.getSeconds() );
  //  })); // .detach();
  //  //thrds.back().detach();
  //}
  ////for(auto& th : thrds) th.detach();
  //for(auto& th : thrds) th.join();

  // test getting back from the map
  //t.start();
  //TO(loopSz, i) {
  //  ui32 gidx = cm.get(i);
  //  cout << gidx << "  ";
  //  //Println("Get Idx: ", (i64)gidx);
  //}
  //Println();
  //t.stop("Get");

  //RngInt<int> rngb(0,1);
  //RngInt<int> rngl(0,loopSz-1);
  //ConcurrentList cl(loopSz);

  //// serial test of ConcurrentList
  ////t.start();
  ////TO(loopSz, i){
  ////  Print(cl.idx(),":", cl.alloc(), " ");
  ////}
  ////TO(loopSz, i){
  ////  Print(cl.idx(),":", cl.free(i), " ");
  ////}
  ////Println();
  ////auto lv = cl.list();
  ////TO(lv->size(), i){
  ////  Print( i,":",(*lv)[i], " ");
  ////}
  ////Println();
  ////t.stop("List");

  //Println("\nLinks: ", cl.lnkCnt(), " ");
  //vec<thread> thrds;
  //TO(12,tid)
  //{
  //  thrds.push_back( thread([&cl, &rngb, &rngl, loopSz, tid]()
  //  {
  //    ScopeTime t;

  //    t.start();
  //    TO(loopSz/5, i){
  //      //if(rngb()) 
  //      Print(tid,":",cl.nxt()," ");
  //      //else       Print(tid,":",cl.free(rngl()), " ");
  //      SleepMs( rngb() );
  //    }
  //    t.stop("alloc/free");
  //  }));
  //}
  //for(auto& th : thrds) th.join();

  //Println();
  //auto lv = cl.list();
  //TO(lv->size(), i){
  //  Print( i,":",(*lv)[i], " ");
  //}
  //Println();
  //Println("\nLinks: ", cl.lnkCnt(), " ");

  //i32 blkSz  = 5;
  //i32 blocks = 2;
  //vec<ui8> mem(blocks*blkSz, 0);
  //ConcurrentStore cs(mem.data(), blkSz, (ui32)(blocks) );
  //
  //Println("\n");
  //
  //TO(cs.m_cl.list()->size(), i){
  //  Println( (*cs.m_cl.list())[i] );
  //}
  //Println("\n\n");
  //
  //TO(2,i){
  //  i32  blks = 0;
  //  auto    s = "w";
  //  i32  slen = (i32)strlen(s)+1;
  //  //i32  slen = 1;
  //  auto  idx = cs.alloc(slen, &blks);   // must allocate the exact number of bytes and no more, since that number will be used to read and write
  //  cs.put(idx, (void*)s, slen);
  //
  //  vec<char> gs(slen,0);
  //  cs.get(idx, gs.data(), slen);
  //  Println(gs.data());
  //  cs.free(idx);
  //
  //  TO(cs.m_blockCount, b){
  //    Println(cs.nxtBlock(b));
  //  }
  //  Println("\n\n");
  //  TO(cs.m_cl.list()->size(), i){
  //    Println( (*cs.m_cl.list())[i] );
  //  }
  //  Println("\n\n");
  //
  //}

  //ConcurrentHash ch(64);
  //vec<thread> thrds;
  //TO(24,tid)
  //{
  //  thrds.push_back( thread([&ch, &rng, tid]()
  //  {
  //    TO(64,h)
  //    {
  //      ch.put(h, h*h);
  //      //Print(h,": ", ch.put(h, h*h) );
  //      Print(h,":", ch.get(h)==h*h, " ");
  //    }
  //  } ));
  //  thrds.back().detach();
  //}

  //auto fileHndl = CreateFileMapping(
  //  INVALID_HANDLE_VALUE,
  //  NULL,
  //  PAGE_READWRITE,
  //  0,
  //  0x0000FFFF,
  //  "Global\\simdb_15");
  //
  //if(fileHndl==NULL){/*error*/}
  //
  //i32  memSz  = 256;
  //auto mapmem = MapViewOfFile(fileHndl,   // handle to map object
  //  FILE_MAP_ALL_ACCESS,   // read/write permission
  //  0,
  //  0,
  //  memSz);

  // OpenFileMapping if the file exists
  
  //Println(fileHndl);
  //Println("\n\n");
  //Println(mapmem);

  Println("kv sz: ", sizeof(simdb::KV) );

  simdb db(16, 16);

  str       wat  =       "wat";
  str       wut  =       "wut";
  str  skidoosh  =  "skidoosh";
  str    kablam  =    "kablam";
  Println("put: ", db.put( (void*)wat.data(),   (ui32)wat.length(),    (void*)skidoosh.data(), (ui32)skidoosh.length()) );
  db.rm("wat");
  Println("put: ", db.put( (void*)wut.data(),   (ui32)wut.length(),    (void*)kablam.data(),   (ui32)kablam.length())   ); 
  db.rm("wut");
  Println("put: ", db.put( (void*)kablam.data(),(ui32)kablam.length(), (void*)skidoosh.data(), (ui32)skidoosh.length()) ); 
  db.rm("kablam");
  Println();


  str clear = "                ";
  auto watlen = db.get("wat", (void*)clear.data() );
  Println("watlen: ", watlen);
  Println("get \"wat\": ", clear);

  //auto idx2 = db.get((void*)kablam.data(), (ui32)kablam.length());
  //Println("get \"kablam\": ", idx2);

  clear = "                ";
  db.get("kablam", (void*)clear.data() );
  Println("get \"kablam\": ", clear);

  Println("size: ", db.size());
  str memstr( (const char*)db.data(), (const char*)db.data() + db.size());
  Println("\nmem: ", memstr, "\n\n" );

  //UnmapViewOfFile(mapmem);
  //CloseHandle(fileHndl);

  PAUSE

  return 0;
}
















//DWORD                 dwMaximumSizeHigh,
//DWORD                 dwMaximumSizeLow,
//LPCTSTR               lpName
//);

//auto idx = db.get((void*)wat.data(), (ui32)wat.length());
//Println("get: ", idx);
//db.get(idx, (void*)clear.data() );

//struct keyval
//{
//  uint64_t  readers  :   4;
//  uint64_t      key  :  28;
//  uint64_t      val  :  28;
//};
//Println("kv size: ",   sizeof(ConcurrentHash::kv) );
//Println("kv size: ",   sizeof(keyval) );
//Println("ui64 size: ", sizeof(ui64) );

//Aui32*  key_addr  =  (Aui32*)(&m_keys.get()[i]);
//ui32   probedKey  =  atomic_load( key_addr );                                 
//atomic_store( (Aui32*)(&m_vals.get()[i]), val);

//ui32 i  =  0;
//for(; i < m_sz; ++i)

//copy( m_keys.get(), m_keys.get()+sz, EMPTY_KEY );
//copy( m_keys.get(), m_keys.get()+sz, EMPTY_KEY );

//m_keys.get()[i]  =  EMPTY_KEY;
//m_vals.get()[i]  =  EMPTY_KEY;

//~ConcurrentMap()
//{
//  //m_mut.unlock(); // if this is already unlocked, will this crash?
//}

//using Keys      =  std::vector<ui32>;
//using Values    =  std::vector<ui32>;

//ui32     put(ui32 key, ui32 val) const
//{
//  using namespace std;
//
//  ui32 empty_ref = EMPTY_KEY;
//  
//  for(ui32 i=0; true; ++i)
//  {
//    ui32 prevKey = m_keys[i].compare_exchange_strong(empty_ref,key);   // mint_compare_exchange_strong_32_relaxed(&m_entries[idx].key, 0, key);
//    if( (prevKey==0) || (prevKey==key))
//    {
//      m_vals[i].store(val);
//      return i;
//    }
//  }
//}

