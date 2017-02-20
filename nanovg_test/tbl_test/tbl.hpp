
// -todo: make push and push_back
// -todo: make pop() and pop_back()

// todo: fix realloc to be resize based on element count
// todo: operator >> for concatenation with left keys as priority
// todo: operator << for concatenation with right keys as priority
// todo: make emplace and emplace_back()
// todo: make resize()
// todo: make shrink_to_fit()
// todo: make reserve()
// todo: keep track of capacity and make capacity() const function
// todo: make front() and back()
// todo: make variant structure
// todo: make hash union
// todo: make key value pair be 4 bytes for hash, 18 + 1 for c_str(), and 8 + 1 for variant + type
// todo: make operator~ return just the vector
// todo: make different unary operator return just the map?
// todo: try template constructor that returns a tbl<type> with a default value set?
// todo: use binary bit operators to make tbl act like a set
// todo: make operator-- be shrink_to_fit() and ++ be resize() ?
// todo: robin hood hashing


#ifndef __TBL_HEADERGUARD_H__
#define __TBL_HEADERGUARD_H__

#include <cstdlib>
#include "../no_rt_util.h"

class tbl
{
private:
  static ui64 memberBytes()
  {
    return sizeof(ui64) * 2;
  }

  void   set_sizeBytes(ui64 bytes) const // -> ui64
  {
    *( (ui64*)memStart() ) = bytes;
  }
  void        set_size(ui64  size)
  {
    *( ((ui64*)memStart()) + 1 ) = size;
  }
  void*       memStart()
  {
    return (void*)(m_mem - memberBytes());
  }
  void              cp(tbl const& l)
  {
  }
  void              mv(tbl&& r)
  {
  }

public:
  static ui64 sizeBytes(ui64 count)                                  // returns the bytes needed to store the data structure if the same arguments were given to the constructor
  {
    return memberBytes() + sizeof(T)*count;
  }
  static tbl     concat(tbl const& a, tbl const& b)                                  // returns the bytes needed to store the data structure if the same arguments were given to the constructor
  {
    auto sz = a.size();
    tbl ret(sz + b.size());
    
    TO(sz,i) ret[i]    = a[i];
    TO(b, i) ret[sz+i] = b[i];

    return ret;
  }
    
  union  hsh
  {
    struct { ui32 type : 4; ui32 : 28; };
    ui32 as_ui32;
  };
  struct Var{};        // todo: future variant type

  using T    =  int;
  using var  =  Var;

  i8*     m_mem;
 
  tbl() : 
    m_mem(nullptr)
  {
  }
  tbl(ui64 count) 
  {
    //m_size(count),
    //ui64  cntBytes  =  count*sizeof(T);
    //ui64   szBytes  =  cntBytes + memberBytes();             // sizeof(ui64)*2;
    
    ui64   szBytes  =  tbl::sizeBytes(count);
    i8*      memst  =  (i8*)malloc(szBytes);                 // memst is memory start
    m_mem           =  memst + memberBytes();
    set_sizeBytes(szBytes);
    set_size(count);
  }

  operator    ui64() const
  {
    return size();
  }
  T&    operator[](ui64 i)
  {
    return ((T*)m_mem)[i];
  }
  auto  operator[](ui64 i) const -> T const&
  {
    return ((T*)m_mem)[i];
  }
  var&  operator()(const char*)                             // todo: future hash map interface
  {
    Var nonsense;
    return nonsense;
  }
  tbl   operator>>(tbl const& l)
  {
    return tbl::concat(*this, l);
  }
  tbl   operator<<(tbl const& l)
  {
    return tbl::concat(*this, l);
  }

  bool        push(T const& value)
  {
    if( !(capacity()>size()) )
      if(!expand()) return false;
    
    (*this)[size()] = value;

    set_size(size()+1);

    return true;
  }
  bool   push_back(T const& value){ return push(value); }
  void         pop(){ set_size(size()-1); }
  void    pop_back(){ pop(); }
  T&         front(){ return (*this)[0]; }
  T&          back(){ return (*this)[size()-1]; }

  ui64        size() const
  {
    if(!m_mem) return 0;

    return *( ((ui64*)memStart()) + 1 );
  }
  T*          data() const
  {
    return (T*)m_mem;
  }
  void*   memStart() const
  {
    return (void*)(m_mem - memberBytes());
  }
  ui64   sizeBytes() const // -> ui64
  {
    if(!m_mem) return 0;

    return *( (ui64 const*)memStart() );
  }
  ui64    capacity() const
  {
    if(!m_mem) return 0;

    return (sizeBytes() - memberBytes()) / sizeof(T);  // - size()*sizeof(T)
  }
  ui64       count() const
  {
    // todo: make this the number of elements in the map
    //return *( ((ui64*)memStart()) - 1 );
  }
  void*    reserve(ui64 size)
  {
    auto  nxtBytes  =  memberBytes() + sizeof(T)*size;
    void*       re;
    bool     fresh  = !m_mem;
    if(fresh) re = malloc(nxtBytes);
    else      re = realloc(memStart(), nxtBytes);

    if(re){
      m_mem = ((i8*)re) + memberBytes();
      set_sizeBytes(nxtBytes);
    }    

    if(fresh) set_size(0);
    
    return re;
    
    //set_sizeBytes(nxtBytes);
  }
  void*     expand()
  {
    ui64    sz = size();
    ui64 nxtSz = (sz/2)? sz+sz/2 : sz+1;
    return reserve(nxtSz);
  }
};


#endif



//if(m_mem==nullptr){                            // can just use realloc here
//  auto szBytes  =  tbl::sizeBytes(1);
//  i8*    memst  =  (i8*)malloc(szBytes);                 // memst is memory start
//  m_mem         =  memst + memberBytes();
//  //m_mem         = (i8*)malloc( szBytes );
//  set_sizeBytes( szBytes );
//  set_size(0);
//  //if(mem) m_mem = mem;
//  //new (this) tbl(1);
//  return m_mem;
//}
//
//auto szBytes  =  sizeBytes();
//auto   extra  =    szBytes/2;
//extra         =  extra>sizeof(T)? extra : sizeof(T);
//auto    nxtSz =  szBytes + extra;
//void*     re  =  realloc(memStart(), nxtSz);
//if(re){
//  m_mem = ((i8*)re) + memberBytes();
//  set_sizeBytes(nxtSz);
//}
//
//return re;

//auto sz = size();
//tbl ret(sz + l.size());
//
//TO(sz,i) ret[i]    = (*this)[i];
//TO(l, i) ret[sz+i] = l[i];
//
//return ret;
