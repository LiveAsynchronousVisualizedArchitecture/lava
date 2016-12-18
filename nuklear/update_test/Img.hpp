

#if defined(_WIN32)
  #pragma once
#endif

#ifndef __IMG_HEADER_GUARD_H__
#define __IMG_HEADER_GUARD_H__

#ifdef _DEBUG 
  #define FAST_DEBUG
#endif

#include <functional>
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <lz4/lz4.h>
#include <lz4/lz4hc.h>
#include "SIM_Matrix.hpp"
#include "SIM_MacroUtil.h"


//class  Img;

      template<typename T = float, typename _Alloc = std::allocator<T> >
class ImgBase
{
public:
  using  type   =  T;
  using  Vec    =  std::vector<T,_Alloc>;
  using  Iter   =  typename Vec::iterator;
  using  CIter  =  typename Vec::const_iterator;
  //using  Mtx    =  SIM::Matrix<T>; 

public:
  i64    m_chans;
  i64    m_width;
  i64    m_height;
  i64    m_chanSz;      // cache of width * height
  Vec    m_mem;

  inline  i64   index(i64 x, i64 y, i64 c) const
  {
    return c*m_chanSz + y*m_width + x;
  }

              template<class A>
  void  construct(ImgBase<T,A> const& _ImgBase)
  {
    m_chans   =  _ImgBase.m_chans;
    m_width   =  _ImgBase.m_width;
    m_height  =  _ImgBase.m_height;
    m_chanSz  =  _ImgBase.m_chanSz;

    //m_mem     =  _ImgBase.m_mem;

    auto sz   =  _ImgBase.m_mem.size();
    m_mem.resize(sz);
    memcpy(m_mem.data(), _ImgBase.m_mem.data(), sizeof(T)*sz);
  }

  void  constructMove(ImgBase&& _ImgBase)
  {
    using namespace std;
    
    m_chans   =  _ImgBase.m_chans;
    m_width   =  _ImgBase.m_width;
    m_height  =  _ImgBase.m_height;
    m_chanSz  =  _ImgBase.m_chanSz;
    
    m_mem     =  move(_ImgBase.m_mem);

    //auto sz   =  _ImgBase.m_mem.size();
    //m_mem.resize(sz);
    //memcpy(m_mem.data(), _ImgBase.m_mem.data(), sizeof(T)*sz);

    _ImgBase.m_chans   =  0;
    _ImgBase.m_width   =  0;
    _ImgBase.m_height  =  0;
    _ImgBase.m_chanSz  =  0;
  }

              //template<class T, class A>
  //void  constructMove(ImgBase<T,A>&& _ImgBase)
  //{
  //  using namespace std;
  //  
  //  m_chans   =  _ImgBase.m_chans;
  //  m_width   =  _ImgBase.m_width;
  //  m_height  =  _ImgBase.m_height;
  //  m_chanSz  =  _ImgBase.m_chanSz;
  //
  //  auto sz   =  _ImgBase.m_mem.size();
  //  m_mem.resize(sz);
  //  memcpy(m_mem.data(), _ImgBase.m_mem.data(), sizeof(T)*sz);
  //  
  //  _ImgBase.m_chans   =  0;
  //  _ImgBase.m_width   =  0;
  //  _ImgBase.m_height  =  0;
  //  _ImgBase.m_chanSz  =  0;
  //}
  
public:
  ImgBase() 
    : m_chans(0), m_width(0), m_height(0), m_chanSz(0), m_mem()
  { }  
  ImgBase(i64 _width, i64 _height, i64 _chans) 
    : m_chans(_chans), m_width(_width), m_height(_height), 
      m_chanSz(_width*_height), m_mem(_chans * m_chanSz, 0.f)  
  { 
    //__ImgBase_inst_count.fetch_add(1);
    //TELL3(__ImgBase_inst_count, __LINE__, m_mem.size())
  }
  ImgBase(i64 _width, i64 _height, i64 _chans, T val) 
    : m_chans(_chans), m_width(_width), m_height(_height), 
      m_chanSz(_width*_height), m_mem(_chans*m_chanSz, val)  
  { 
    sim_assert(m_mem.size()==m_mem.capacity(), m_mem.size(), m_mem.capacity() )
    //__ImgBase_inst_count.fetch_add(1);
    //TELL3(__ImgBase_inst_count, __LINE__, m_mem.size())
  }
  ImgBase(ImgBase const& _ImgBase, T val) :
    m_chans(_ImgBase.chans()), m_width(_ImgBase.width()), m_height(_ImgBase.height()), 
    m_chanSz(m_width*m_height), m_mem(m_chans*m_chanSz, val)
  { 
    //__ImgBase_inst_count.fetch_add(1);
    //TELL3(__ImgBase_inst_count, __LINE__, m_mem.size())
  }

             template<class A>
  ImgBase(ImgBase<T,A> const& _ImgBase)
  {
    this->construct(_ImgBase);
  }

             //template<class T, class A>
  ImgBase(ImgBase&& _ImgBase)
  {
    constructMove( std::move(_ImgBase) );
  }

             template<class A>
  ImgBase& operator=(const ImgBase<T,A>& _ImgBase)
  {
    construct(_ImgBase);
    return *this;
  }

             //template<class T, class A>
  ImgBase& operator=(ImgBase&& _ImgBase)
  {
    constructMove( std::move(_ImgBase) );
    return *this;
  }

  //ImgBase(ImgBase<T,_Alloc>&& _ImgBase)
  //{
  //  constructMove( std::move(_ImgBase) );
  //}

  ~ImgBase()
  { 
    //if(m_mem.size() > 0) {
    //  __ImgBase_inst_count.fetch_sub(1);
    //  TELL3(__ImgBase_inst_count, __LINE__, m_mem.size())
    //}
  }


  template<class Archive> void 
  save(Archive& archive) const
  {
    using namespace std;
    
    vector<uint8_t> compressed;
    uint64_t sz  =  m_mem.size() * sizeof(Vec::value_type);
    compressed   =  vector<uint8_t>(sz);
    int resz     =  LZ4_compress_default((const char*)m_mem.data(), 
                                                (char*)compressed.data(), 
                                                (int)sz,
                                                (int)sz);
    compressed.resize(resz);
    compressed.shrink_to_fit();

    archive(m_chans, m_width, m_height, m_chanSz, compressed);
  }
  template<class Archive> void 
  load(Archive& archive)
  {
    using namespace std;

    vector<uint8_t> compressed;
    archive(m_chans, m_width, m_height, m_chanSz, compressed);
    
    sim_assert(m_chanSz == m_width*m_height, m_chanSz, m_width*m_height)

    m_mem            =  Vec(m_chans*m_chanSz, 0.f);
    uint64_t compSz  =  compressed.size();
    uint64_t sz      =  m_mem.size() * sizeof(Vec::value_type);

    //Println( (void*)compressed.data()," ",(void*)m_mem.data()," ",(int)compSz," ", (int)sz);

    if(compSz > 0) {
      int    resz    =  LZ4_decompress_safe((const char*)compressed.data(), 
                                                  (char*)m_mem.data(),
                                                  (int)compSz,
                                                  (int)sz);
      sim_assert(resz == sz, resz, sz)
    }
  }

  inline T&         operator()(i64 x, i64 y, i64 c)
  { 
    #ifdef FAST_DEBUG
        return m_mem.data()[ index(x,y,c) ];
    #else
      #ifdef _DEBUG    
        sim_assert( c >= 0 && c < m_chans,  c, m_chans  );
        sim_assert( x >= 0 && x < m_width,  x, m_width  );
        sim_assert( y >= 0 && y < m_height, y, m_height );
        return m_mem[ index(x,y,c) ];
      #else
        return m_mem[ index(x,y,c) ];
      #endif
    #endif
  }
  inline const T&   operator()(i64 x, i64 y, i64 c) const 
  { 
    #ifdef FAST_DEBUG
        return m_mem.data()[ index(x,y,c) ];
    #else
      #ifdef _DEBUG    
        sim_assert( c >= 0 && c < m_chans,  c, m_chans  );
        sim_assert( x >= 0 && x < m_width,  x, m_width  );
        sim_assert( y >= 0 && y < m_height, y, m_height );
        return m_mem[ index(x,y,c) ];
      #else
        return m_mem[ index(x,y,c) ];
      #endif
    #endif
  }  
  //inline T*         operator()(i64 x, i64 y)
  //{
  //  return m_mem.data() + index()
  //}
  inline T&         operator[](i64 _offset)
  {
    #ifdef FAST_DEBUG
      return m_mem.data()[_offset];
    #else
      #ifdef _DEBUG    
        sim_assert( _offset >= 0 && _offset < m_mem.size(), _offset, m_mem.size() ); 
        return m_mem[_offset];
      #else
        return m_mem[_offset];   // fast path
      #endif
    #endif
  }
  inline T const&   operator[](i64 _offset) const
  {
    #ifdef FAST_DEBUG
      return m_mem.data()[_offset];
    #else
      #ifdef _DEBUG    
        sim_assert( _offset >= 0 && _offset < m_mem.size(), _offset, m_mem.size() ); 
        return m_mem[_offset];
      #else
        return m_mem[_offset];   // fast path
      #endif
    #endif
  }

  inline ImgBase&           reset()
  {
    this->m_mem     =  Vec();
    this->m_chans   =  0;
    this->m_width   =  0;
    this->m_height  =  0;
    this->m_chanSz  =  0;

    return *this;
  }

  inline T*             chanPtr(i64 c)     
  {
    //sim_assert( c >= 0 && c < m_chans, c, m_chans ); 
    return m_mem.data()  +  c*m_chanSz;
  }
  inline const T*       chanPtr(i64 c)  const     
  {
    //sim_assert( c >= 0 && c < m_chans, c, m_chans ); 
    return m_mem.data()  +  c*m_chanSz;
  }
  inline i64            width()         const  
  {
    return m_width;
  }
  inline i64            height()        const 
  {
    return m_height;
  }
  inline i64            chans()         const 
  {
    return m_chans;
  }
  inline i64            size()          const
  { 
    return (int)m_mem.size();
  }
  inline i64            chanSize()      const
  {
    return m_chanSz;
  }
  inline i64            chanSizeBytes() const
  {
    return m_chanSz * sizeof(type);
  }
  inline i64            sizeBytes()     const
  {
    return chans() * chanSizeBytes();
  }
  inline Iter           chanIter(i64 c)
  {
    //sim_assert( c >= 0 && c < m_chans, c, m_chans ); 
    return m_mem.begin()  +  c*m_chanSz;
  }  
  inline CIter          chanIter(i64 c) const
  {
    //sim_assert( c >= 0 && c < m_chans, c, m_chans ); 
    return m_mem.begin()  +  c*m_chanSz;
  }  

  inline Iter           begin()               {  return m_mem.begin();  }
  inline Iter           end()                 {  return m_mem.end();    }
  inline CIter          begin()       const   {  return m_mem.begin();  }
  inline CIter          end()         const   {  return m_mem.end();    }
  inline Iter           begin(i64 c)          {  return m_mem.begin() + c*m_chanSz;     }
  inline Iter           end(i64   c)          {  return m_mem.begin() + (c+1)*m_chanSz; }
  inline CIter          begin(i64 c)  const   {  return m_mem.begin() + c*m_chanSz;     }
  inline CIter          end(i64   c)  const   {  return m_mem.begin() + (c+1)*m_chanSz; }

  //inline Mtx       mtx(int c) const
  //{ 
  //  using namespace std;
  //  
  //  Mtx retMtx(m_width, m_height);
  //
  //  auto memSt   =  cbegin(m_mem)  +  c*m_chanSz;
  //  auto memEnd  =  memSt          +  m_chanSz; 
  //  std::copy( memSt, memEnd, retMtx.begin() );
  //  
  //  return retMtx;
  //}

  void      fill(T val)
  {
    std::fill( ALL(m_mem), val );
  }

            template<class A>
  void      copy(ImgBase<T,A> const& src)
  {
    m_chans   =  src.m_chans;
    m_width   =  src.m_width;
    m_height  =  src.m_height;
    m_chanSz  =  src.m_chanSz;

    auto sz = src.size();
    m_mem.resize( sz );

    memcpy( m_mem.data(), (void*)src.m_mem.data(), sz * sizeof(T) );
  }
  void      copy(ImgBase const& src, i64 destSt, i64 srcSt = 0, i64 chanCount = 1)
  {
    i64 len = chanSizeBytes();
    //sim_assert(len==src.chanSizeBytes(),len,src.chanSizeBytes())

    TO(chanCount, c)
      memcpy(chanPtr(destSt+c), src.chanPtr(srcSt+c), len);
  }
  inline void      mult(ImgBase const& b, i64 c)
  {
    //sim_assert(chans()==b.chans(), chans(), b.chans())

    auto ptr = b.chanPtr(c);
    TO(chanSize(),i) (*this)[i] *= ptr[i];
  }
  inline ImgBase&  mult(ImgBase const& b, i64 ac = 0, i64 bc = 0)
  {
    using namespace std;

    auto ach  =  chanPtr(ac);
    auto bch  =  b.chanPtr(bc);
    TO(chanSize(),i) ach[i] = ach[i] * bch[i];

    return *this;
  }
  inline ImgBase&  add(const ImgBase& _ImgBase)
  { // this shit is more trouble than it is worth
    using namespace std;

    auto chns  =  min( this->chans(), _ImgBase.chans() );
    auto   en  =  this->chanPtr(chns);
    transform( this->chanPtr(0), en, _ImgBase.begin(), this->begin(), plus<>() );
    return *this;
  }
  inline ImgBase&  add(ImgBase const& b, i64 ac, i64 bc)
  {
    using namespace std;

    auto ach  =  chanPtr(ac);
    auto bch  =  b.chanPtr(bc);
    TO(chanSize(),i) ach[i] += bch[i];

    return *this;
  }
  inline ImgBase&  div(ImgBase const& b, i64 ac = 0, i64 bc = 0)
  {
    using namespace std;

    auto ach  =  chanPtr(ac);
    auto bch  =  b.chanPtr(bc);
    TO(chanSize(),i) ach[i] = divZero(ach[i], bch[i]);

    return *this;
  }

      template<typename F> inline ImgBase& 
  apply(F f)
  { 
    TO(size(),i) 
      (*this)[i] = f( (*this)[i] );
    return *this;
  }

      template<typename F> inline ImgBase& 
  apply(F f, int chan)
  { 
    auto ch = chanPtr(chan);
    TO(chanSize(),i) ch[i] = f(ch[i]);
    return *this;
  }

      template <typename _BinaryOp> inline ImgBase&  
  operator()(const ImgBase& _ImgBase, _BinaryOp op, int _threads = 0)
  {
    using namespace std;

    auto threads  =  _threads > 0 ? _threads : thread::hardware_concurrency();
    auto chans    =  this->chans();
    auto stride   =  _ImgBase.chans();

    TO(chans, c)
    {
      const auto cc  =  c % stride;
      mt_transform( this->begin(c), this->end(c), _ImgBase.begin(cc), this->begin(c), op, threads );
    }

    return *this;
  }


      template <typename _BinaryOp> inline ImgBase&  
  operator()(const ImgBase& _ImgBase, int chan, _BinaryOp op, int _threads = 0)
  {
    using namespace std;

    int threads  =  _threads > 0 ? _threads : thread::hardware_concurrency();

    TO(this->chans(), c)
    {
      mt_transform(  this->begin(c),    this->end(c), 
                     _ImgBase.begin(chan),  this->begin(c),  op, threads);
    }
   
    return *this;
  }

      template <typename _BinaryOp> inline ImgBase&  
  operator()(const ImgBase& _ImgBase, _BinaryOp op, int _st, int _end, int _threads = 0)
  {
    using namespace std;

    int  threads   =  _threads > 0 ? _threads : thread::hardware_concurrency();

    auto startPtr  =  this->chanPtr(_st);
    auto endPtr    =  this->chanPtr(_end);
    mt_transform( startPtr, endPtr, _ImgBase.begin(), startPtr, op, threads);
    return *this;
  }

};


using  Img = ImgBase<float>;

//inline Img&   Norm(Img*, int skip = -1);

          template<class T,class A>
ImgBase<T,A>&   Norm(ImgBase<T,A>* _ImgBase, int skip = -1)
{
  auto& ImgBase  =  *_ImgBase;
  auto  chans    =  ImgBase.chans();
  TO(chans,c)
  {
    if(c != skip) {
      TO(ImgBase.height(),y) TO(ImgBase.width(),x) {
        auto& px = ImgBase(x,y,c);
         px = divZero(px, ImgBase(x,y,chans-1));
      }
    }
  }
  return ImgBase;
}


#endif













//__ImgBase_inst_count.fetch_add(1);
//TELL3(__ImgBase_inst_count, __LINE__, m_mem.size())

//__ImgBase_inst_count.fetch_add(1);
//TELL3(__ImgBase_inst_count, __LINE__, m_mem.size())

//const float EPS = 0.15625f;
//const float EPS = 1.5625f;

//template <class T = float, class _Alloc = std::allocator<T>, typename _BinaryOp> 
//inline ImgBase&
//ImgBase::operator()(const ImgBase& _ImgBase, _BinaryOp op, int _threads)
//{
//  using namespace std;
//
//  int threads  =  _threads > 0 ? _threads : thread::hardware_concurrency();
//  int chans    =  this->chans();
//  int stride   =  _ImgBase.chans();
//
//  TO(chans, c)
//  {
//    const int cc  =  c % stride;
//    mt_transform( this->begin(c), this->end(c), _ImgBase.begin(cc), this->begin(c), op, threads );
//  }
//
//  return *this;
//}
//
//template <typename _BinaryOp> inline ImgBase&
//ImgBase::operator()(const ImgBase& _ImgBase, int chan, _BinaryOp op, int _threads)
//{
//  using namespace std;
//
//  int threads  =  _threads > 0 ? _threads : thread::hardware_concurrency();
//
//  TO(this->chans(), c)
//  {
//    mt_transform(  this->begin(c),    this->end(c), 
//                   _ImgBase.begin(chan),  this->begin(c),  op, threads);
//  }
//   
//  return *this;
//}
//
//template <typename _BinaryOp> inline ImgBase&  
//ImgBase::operator()(const ImgBase& _ImgBase, _BinaryOp op, int _st, int _end, int _threads)
//{
//  using namespace std;
//
//  int  threads   =  _threads > 0 ? _threads : thread::hardware_concurrency();
//
//  auto startPtr  =  this->chanPtr(_st);
//  auto endPtr    =  this->chanPtr(_end);
//  mt_transform( startPtr, endPtr, _ImgBase.begin(), startPtr, op, threads);
//  return *this;
//}

// int strides  =  chans / stride;
// ImgBase out(in.width(), in.height(), stride);
//TO(stride, s)  TO(strides, ss)

//sim_assert( _offset >= 0 && _offset < m_mem.size(), _offset, m_mem.size() ); 
//return m_mem[_offset];

//sim_assert( c >= 0 && c < m_chans,  c, m_chans  );
//sim_assert( x >= 0 && x < m_width,  x, m_width  );
//sim_assert( y >= 0 && y < m_height, y, m_height );
//return m_mem[ index(x,y,c) ];

////template<class Archive> void 
////serialize(Archive& archive)
////{
////  archive(m_chans, m_width, m_height, m_chanSz, m_mem, wgt);
////}
//// time for fancy shit
//template<class Archive> void 
//save(Archive& archive) const
//{
//  using namespace std;
//  
//  //std::fill(ALL(m_mem), 0.f);
//  //uint64_t sz = m_mem.size() * sizeof(float);
//
//  vector<uint8_t> compressed;
//
//  //Vec wgt(m_width*m_height, 0.f);
//  //memcpy(wgt.data(), chanPtr(m_chans-1), m_chanSz);
//  //
//  //if(m_chans > 1)
//  //{
//  //  Img cpy      =  *this;
//  //  //Norm(&cpy);
//  //
//  //  TO(m_chans-1,c) TO(m_chanSz,i) {
//  //    if(wgt[i] > EPS) cpy.chanPtr(c)[i] / wgt[i];
//  //  }
//  //  TO(m_chanSz,i) cpy.chanPtr(m_chans-1)[i] = 1.0f;
//
//    //uint64_t sz  =  cpy.m_mem.size() * sizeof(Vec::value_type);
//    uint64_t sz  =  m_mem.size() * sizeof(Vec::value_type);
//    compressed   =  vector<uint8_t>(sz);
//    int resz     =  LZ4_compress_default((const char*)m_mem.data(), 
//                                               (char*)compressed.data(), 
//                                               sz,sz);
//    compressed.resize(resz);
//    compressed.shrink_to_fit();
//  //}
//
//  //int resz  =  LZ4_compress_HC((const char*)m_mem.data(),
//  //                                   (char*)compressed.data(), 
//  //                                     sz,sz,16);
//  //memcpy(compressed.data(), m_mem.data(), sz);
//
//  archive(m_chans, m_width, m_height, m_chanSz, compressed); // wgt);
//}
//template<class Archive> void 
//load(Archive& archive)
//{
//  using namespace std;
//
//  vector<uint8_t> compressed;
//  //Vec             wgt;
//  archive(m_chans, m_width, m_height, m_chanSz, compressed); // wgt);
//  
//  sim_assert(m_chanSz == m_width*m_height, m_chanSz, m_width*m_height)
//
//  m_mem              =  Vec(m_chans*m_chanSz, 0.f);
//  //if(m_chans > 1)
//  //{
//    uint64_t compSz  =  compressed.size();
//    uint64_t sz      =  m_mem.size() * sizeof(Vec::value_type);
//    int      resz    =  LZ4_decompress_safe((const char*)compressed.data(), 
//                                                  (char*)m_mem.data(),
//                                                  compSz,sz);
//    sim_assert(resz == sz, resz, sz)
//  //}
//
//  //TO(m_chans,c) TO(m_chanSz,i) {
//  //  if(wgt[i] > EPS) chanPtr(c)[i]  *=  wgt[i];
//  //}
//
//  //this->copy(wgt, m_chans-1, 0, 1);
//  //
//  //uint64_t sz      =  m_mem.size()      * sizeof(float);
//  //uint64_t compSz  =  compressed.size() * sizeof(uint8_t);
//  //memcpy(m_mem.data(), compressed.data(), sz);
//
//}

//template <typename _BinaryOp> inline Img&
//Img::operator()(_BinaryOp op, int _stride, int _threads)
//{
//  using namespace std;
//
//  int threads  =  _threads > 0 ?  _threads : thread::hardware_concurrency();
//  int stride   =  _stride  < 0 ?  this->chans()  : _stride;
//  int chans    =  this->chans() - (this->chans() % stride);
//  #pragma omp parallel for default(shared) num_threads(threads)
//  TO(chans, c)
//  {
//    int  sc    =  (c+1) % stride;      // sub chan
//    int  wc    =  c + (stride - sc);   // weight chan
//    if( sc != 0 )
//      mt_transform( this->begin(c),   this->end(c),
//                    this->begin(wc),  this->begin(c), op, threads);
//  }
//
//  return *this;
//}

//template <typename _BinaryOp> inline Img&
//  operator()(_BinaryOp op, int _stride = -1, int _threads = 0);  // shadows access operator()

//inline Img&           operator()(const Img& _img, float(*op)(float a, float b) )
//{
//  using namespace std;
//  transform( ALL(*this), _img.begin(), this->begin(), op );
//  return *this;
//}

//template <typename T> Img(T&& _img)
//{
//  this->construct( std::forward<T>(_img) );
//}
//template <typename T> Img& operator=(T&& _img)
//{
//  this->construct( std::forward<T>(_img) );
//  return *this;
//}

// Img(float** _ptr, int _chans, int _width, int _height) 
//  : m_chans(_chans), m_width(_width), m_height(_height)  
//{
//  // todo: need to copy from the dual pointers
//}

//Img(const Img& _img)
//{}
//Img& operator=(const Img& _img)
//{}

//template <typename T> class ObsMtx
//{
//  int   m_width;
//  int   m_height;
//  T*    m_ptr;
//  
//public:
//  ObsMtx(T* _ptr, int _width, int _height)
//    : m_width(_width), m_height(_height), m_ptr(_ptr)  {}
//
//  inline T&        operator()(int x, int y)       { return m_ptr[y*m_width + x]; }
//  inline const T&  operator()(int x, int y) const { return m_ptr[y*m_width + x]; }
//};
//
// #include <SIM_GeneralTemplateUtil.hpp>
// #include <SIM_GeneralUtil.h>


