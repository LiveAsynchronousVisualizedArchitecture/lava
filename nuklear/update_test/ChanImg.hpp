


#if defined(_WIN32)
  #pragma once
#endif

#ifndef __CHANIMG_HEADER_GUARD_H__
#define __CHANIMG_HEADER_GUARD_H__

#include <functional>
#include <vector>
//#include "SIM_MacroUtil.h"
//#include "Img.hpp"

      template<class T = float, class _Alloc = std::allocator<T> >
class ChanImgBase
{
public:
  using  type   =  T;
  using  Vec    =  std::vector<T,_Alloc>;
  using  Iter   =  decltype(Vec().begin());   // std::vector<T,_Alloc>::iterator;
  using  CIter  =  decltype(Vec().cbegin());  // std::vector<T,_Alloc>::const_iterator;

private:
  i64      m_chans;
  i64      m_width;
  i64      m_height;
  i64      m_scanlineSz;    // cache of width * chans
  Vec      m_mem;

  i64   index(i64 x, i64 y, i64 c) const
  {
    return y*m_scanlineSz + x*m_chans + c;
  }
  void  construct(const ChanImgBase<T,_Alloc>& _ChanImgBase)
  {
    m_chans       =  _ChanImgBase.m_chans;
    m_width       =  _ChanImgBase.m_width;
    m_height      =  _ChanImgBase.m_height;
    m_scanlineSz  =  _ChanImgBase.m_scanlineSz;
    m_mem         =  _ChanImgBase.m_mem;

  }
  void  construct(ChanImgBase<T,_Alloc>&& _ChanImgBase)
  {
    using namespace std;
    
    m_chans       =  _ChanImgBase.m_chans;
    m_width       =  _ChanImgBase.m_width;
    m_height      =  _ChanImgBase.m_height;
    m_scanlineSz  =  _ChanImgBase.m_scanlineSz;
    m_mem         =  move(_ChanImgBase.m_mem);
    
    _ChanImgBase.m_chans       =  0;
    _ChanImgBase.m_width       =  0;
    _ChanImgBase.m_height      =  0;
    _ChanImgBase.m_scanlineSz  =  0;
  }
  
public:
  //ChanImgBase(const Img& _img)
  //  : m_chans(_img.chans()), m_width(_img.width()), m_height(_img.height()), 
  //    m_scanlineSz(m_width * m_chans), m_mem(m_height * m_scanlineSz)
  //{
  //  TO(_img.chans(), c) TO(_img.height(), y) TO(_img.width(), x)
  //  {
  //    (*this)(x,y,c)  =  _img(x,y,c);
  //  }
  //}
  //ChanImgBase(Img&& _img)
  //  : m_chans(_img.chans()), m_width(_img.width()), m_height(_img.height()), 
  //    m_scanlineSz(m_width * m_chans), m_mem(m_height * m_scanlineSz)
  //{
  //  TO(_img.chans(), c) TO(_img.height(), y) TO(_img.width(), x)
  //  {
  //    (*this)(x,y,c)  =  _img(x,y,c);
  //  }
  //}
  //ChanImgBase(const ImgBase<T,_Alloc>& lval)
  //{
  //  //this->construct(_ChanImgBase);
  //  m_chans       =  lval.m_chans;
  //  m_width       =  lval.m_width;
  //  m_height      =  lval.m_height;
  //}

  ChanImgBase() 
    : m_chans(0), m_width(0), m_height(0), m_scanlineSz(0), m_mem() { }  
  ChanImgBase(i64 _width, i64 _height, i64 _chans) 
    : m_chans(_chans), m_width(_width), m_height(_height), 
      m_scanlineSz(_width * _chans), m_mem(m_height * m_scanlineSz)
  {  }
  ChanImgBase(i64 _width, i64 _height, i64 _chans, T val) 
    : m_chans(_chans), m_width(_width), m_height(_height), 
      m_scanlineSz(_width * _chans), m_mem(m_height * m_scanlineSz, val)
  {  }
  ChanImgBase(const ChanImgBase<T,_Alloc>& _ChanImgBase, T val)
    : m_chans(_ChanImgBase.chans()), m_width(_ChanImgBase.width()), m_height(_ChanImgBase.height()), 
      m_scanlineSz(m_width * m_chans), m_mem(m_height * m_scanlineSz, val)
  {
  }
  ChanImgBase(const ChanImgBase<T,_Alloc>& _ChanImgBase)
  {
    this->construct(_ChanImgBase);
  }

  ChanImgBase(ChanImgBase<T,_Alloc>&& _ChanImgBase)
  {
    construct( std::move(_ChanImgBase) );
  }
  ChanImgBase& operator=(const ChanImgBase<T,_Alloc>& _ChanImgBase)
  {
    this->construct(_ChanImgBase);
    return *this;
  }
  ChanImgBase& operator=(ChanImgBase<T,_Alloc>&& _ChanImgBase)
  {
    //this->construct( std::forward<ChanImgBase>(_ChanImgBase) );
    construct( std::move(_ChanImgBase) );    
    return *this;
  }

  inline T&         operator()(i64 x, i64 y, i64 c)
  { 
    //sim_assert( c >= 0 && c < m_chans,  c, m_chans  );
    //sim_assert( x >= 0 && x < m_width,  x, m_width  );
    //sim_assert( y >= 0 && y < m_height, y, m_height );
    return m_mem[ index(x,y,c) ];
  }
  inline const T&   operator()(i64 x, i64 y, i64 c) const 
  { 
    //sim_assert( c >= 0 && c < m_chans,  c, m_chans  );
    //sim_assert( x >= 0 && x < m_width,  x, m_width  );
    //sim_assert( y >= 0 && y < m_height, y, m_height );
    return m_mem[ index(x,y,c) ];
  }  
  inline T*         operator()(i64 x, i64 y)
  {
    return m_mem.data()  +  y*m_scanlineSz + x*m_chans;
  }
  inline const T&   operator[](i64 _offset) const
  {
    return m_mem[_offset];
  }
  inline T&         operator[](i64 _offset)
  {
    return m_mem[_offset];
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
  inline size_t         size()          const
  { 
    return m_mem.size();
  }
  inline int64_t        sizeBytes()     const
  {
    return size()*sizeof(type) + sizeof(int)*4 + sizeof(Vec);
  }
  inline Iter           begin()               {  return m_mem.begin();  }
  inline Iter           end()                 {  return m_mem.end();    }
  inline CIter          begin()       const   {  return m_mem.begin();  }
  inline CIter          end()         const   {  return m_mem.end();    }
  inline void          fill(T val)
  {
    std::fill( ALL(m_mem), val );
  }
  //inline Img          toImg() 
  //{ 
  //  Img ret( width(), height(), chans() );
  //  TO(chans(), c) TO(height(), y) TO(width(), x)
  //  {
  //    ret(x,y,c) = (*this)(x,y,c);
  //  }
  //  return ret;
  //}

};


using ChanImg = ChanImgBase<float>;


#endif







//using  Iter   =  std::vector<T,_Alloc>::iterator;
//using  CIter  =  std::vector<T,_Alloc>::const_iterator;

//static std::atomic<int> count;
//struct Counter { Counter(){count.fetch_add(1);} ~Counter(){count.fetch_sub(1);} };
//Counter  m_counter;
//
//ChanImg(const Img& _img)
//  : m_chans(_img.chans()), m_width(_img.width()), m_height(_img.height()), 
//    m_scanlineSz(m_width * m_chans), m_mem(m_height * m_scanlineSz)
//{
//  TO(_img.chans(), c) TO(_img.height(), y) TO(_img.width(), x)
//  {
//    (*this)(x,y,c)  =  _img(x,y,c);
//  }
//}

//inline ChanImg&           operator()(const ChanImg& _ChanImg, float(*op)(float a, float b) )
  //{
  //  using namespace std;
  //  transform( ALL(*this), _ChanImg.begin(), this->begin(), op );
  //  return *this;
  //}

//inline ChanImg&           add(const ChanImg& _ChanImg)
  //{
  //  using namespace std;
  //  transform( ALL(*this), _ChanImg.begin(), this->begin(), plus<>() );
  //  return *this;
  //  //[](float a, float b)
  //  //{
  //  //  return a + b
  //  //});
  //}
  //
  //inline float*         chanPtr(int c)     
  //{
  //  //sim_assert( c >= 0 && c < m_chans, c, m_chans ); 
  //  return m_mem.data()  +  c*m_chanSz;
  //}
  //inline const float*   chanPtr(int c)  const     
  //{
  //  //sim_assert( c >= 0 && c < m_chans, c, m_chans ); 
  //  return m_mem.data()  +  c*m_chanSz;
  //}

//inline int            chanSize()      const
  //{
  //  return m_chanSz;
  //}
  //inline Iter           chanIter(int c)
  //{
  //  //sim_assert( c >= 0 && c < m_chans, c, m_chans ); 
  //  return m_mem.begin()  +  c*m_chanSz;
  //}  
  //inline CIter          chanIter(int c) const
  //{
  //  //sim_assert( c >= 0 && c < m_chans, c, m_chans ); 
  //  return m_mem.begin()  +  c*m_chanSz;
  //}  

//inline Iter           begin(int c)          {  return m_mem.begin() + c*m_chanSz;     }
  //inline Iter           end(int c)            {  return m_mem.begin() + (c+1)*m_chanSz; }
  //inline CIter          begin(int c)  const   {  return m_mem.begin() + c*m_chanSz;     }
  //inline CIter          end(int c)    const   {  return m_mem.begin() + (c+1)*m_chanSz; }
  //
  //inline Mtx  mtx(int c) const
  //{ 
  //  using namespace std;
  //  
  //  Mtx retMtx(m_width, m_height);
  //
  //  auto memSt   =  cbegin(m_mem)  +  c*m_chanSz;
  //  auto memEnd  =  memSt          +  m_chanSz; 
  //  copy( memSt, memEnd, retMtx.begin() );
  //  
  //  return retMtx;
  //}

//template <typename _BinaryOp> inline ChanImg&  
  //  operator()(const ChanImg& _ChanImg, _BinaryOp op, int _threads = 0);
  //
  //template <typename _BinaryOp> inline ChanImg&  
  //  operator()(const ChanImg& _ChanImg, int chan, _BinaryOp op, int _threads = 0);
  //
  //template <typename _BinaryOp> inline ChanImg&  
  //  operator()(const ChanImg& _ChanImg, _BinaryOp op, int _st, int _end, int _threads = 0);
  //
  //template <typename _BinaryOp> inline ChanImg&
  //  operator()(_BinaryOp op, int _stride = -1, int _threads = 0);

//
//sim_assert( _offset >= 0 && _offset < m_mem.size(), _offset, m_mem.size() ); 

//template <typename _BinaryOp> inline ChanImg&
//ChanImg::operator()(const ChanImg& _ChanImg, _BinaryOp op, int _threads)
//{
//  using namespace std;
//
//  int threads  =  _threads > 0 ? _threads : thread::hardware_concurrency();
//  int chans    =  this->chans();
//  int stride   =  _ChanImg.chans();
//
//  // int strides  =  chans / stride;
//  // ChanImg out(in.width(), in.height(), stride);
//  //TO(stride, s)  TO(strides, ss)
//
//  TO(chans, c)
//  {
//    const int cc  =  c % stride;
//    mt_transform( this->begin(c), this->end(c), _ChanImg.begin(cc), this->begin(c), op, threads );
//  }
//
//  return *this;
//}
//
//template <typename _BinaryOp> inline ChanImg&
//ChanImg::operator()(const ChanImg& _ChanImg, int chan, _BinaryOp op, int _threads)
//{
//  using namespace std;
//
//  int threads  =  _threads > 0 ? _threads : thread::hardware_concurrency();
//
//  TO(this->chans(), c)
//  {
//    mt_transform(  this->begin(c),    this->end(c), 
//                   _ChanImg.begin(chan),  this->begin(c),  op, threads);
//  }
//   
//  return *this;
//}
//
//template <typename _BinaryOp> inline ChanImg&  
//ChanImg::operator()(const ChanImg& _ChanImg, _BinaryOp op, int _st, int _end, int _threads)
//{
//  using namespace std;
//
//  int  threads   =  _threads > 0 ? _threads : thread::hardware_concurrency();
//
//  auto startPtr  =  this->chanPtr(_st);
//  auto endPtr    =  this->chanPtr(_end);
//  mt_transform( startPtr, endPtr, _ChanImg.begin(), startPtr, op, threads);
//  return *this;
//}
//
//template <typename _BinaryOp> inline ChanImg&
//ChanImgm::operator()(_BinaryOp op, int _stride, int _threads)
//{
//  using namespace std;
//
//  int threads  =  _threads > 0 ?  _threads : thread::hardware_concurrency();
//  int stride   =  _stride  < 0 ?  this->chans() : _stride;
//  
//  int chans    =  this->chans();
//  //#pragma omp parallel for default(shared) num_threads(threads)
//  TO(chans, c)
//  {
//    int  sc    =  (c+1) % stride;      // sub chan
//    int  wc    =  c + (stride - sc);   // weight chan
//    if( sc != 0 )
//      transform( this->begin(c),   this->end(c),
//                 this->begin(wc),  this->begin(c), op );
//  }
//
//  return *this;
//}
