
// -todo: make push and push_back
// -todo: make pop() and pop_back()
// -todo: fix realloc to be resize based on element count
// -todo: operator >> for concatenation with left keys as priority
// -todo: operator << for concatenation with right keys as priority
// -todo: make reserve()
// -todo: keep track of capacity and make capacity() const function
// -todo: make front() and back()
// -todo: make key value pair be 4 bytes for hash, 18 + 1 for c_str(), and 8 + 1 for variant + type - should be 32 bytes - with 8 bytes for hash and type -> 36 bytes + 28 more bytes for key str = 64 bytes? - this_is18_char_key | hsh+type -> 8 bytes, 
// -todo: make tbl_tell that uses printf - can't because it won't detect the types like iostream will
// -todo: make tbl_assert
// -todo: make assignment operator
// -todo: test single kv implicit cast
// -todo: make template specialization strings to print out types - possible but opted for a switch jump table at run time since it should only be used during debugging and run time type strings will be needed for hsh.type anyway
// -todo: make tbl_assert a function - can't make it a function because of expression printing and line numbers
// -todo: make tbl assert have error message
// -todo: make tbl_assert back into a macro
// -todo: fill out the rest of the type nums
// -todo: fill out the rest of the type strings
// -todo: make as<>() template function
// -todo: make and test destructor
// -todo: make move constructor
// -todo: make hash union
// -todo: try in release mode - find and fix release mode crash - first try at destructors didn't work
// -todo: make copy constructor and assignment operator
// -todo: test and fix destructor crash - reversed boolean of m_mem
// -todo: make bool implicit cast that evaluates pointer

// todo: make 1 bit always indicate signed, 1 bit always indicate table, 1 bit indicate integer, and 2 bits indicate the bit depth as 3,4,5, or 6 - same 5 bits as discreet 21 types if unsigned & not integer is used for empty!
// todo: make switch statement to have flexible number casts (a ui8 can be cast without error to a ui32)
// todo: make kv have implicit casts to the different number types
// todo: make variant structure
// todo: make enum with number types and table-number types
// todo: make a table hold any type, but a map hold only numbers and table-number types
// todo: make copy use resize
// todo: make sure destructor is being run on objects being held once turned into a template
// todo: make resize()
// todo: make begin and end iterators to go with C++11 for loops - loop through keys value pairs?
// todo: make emplace and emplace_back()
// todo: make shrink_to_fit()
// todo: make constructor with default value
// todo: make operator~ return just the vector
// todo: make different unary operator return just the map?
// todo: try template constructor that returns a tbl<type> with a default value set?
// todo: use binary bit operators to make tbl act like a set
// todo: make operator-- be shrink_to_fit() and ++ be resize() ?
// todo: robin hood hashing
// todo: make visualization for table as a tree that shows arrays, key-values and their types, then sub tables - make various visualizations for arrays - histogram, graph, ???


#ifndef __TBL_HEADERGUARD_H__
#define __TBL_HEADERGUARD_H__

#include <cstdlib>
#include <cassert>
#include "../no_rt_util.h"

#if defined(_MSC_VER) && defined(_DEBUG)
  #include <iostream>
  #include <iomanip>
  #define tbl_TELL(var) ::std::cout \
                        << ::std::setprecision(4) \
                        << #var ## ": " \
                        << ::std::endl \
                        << var<<::std::endl \
                        << ::std::endl; ::std::cout.flush();
  #define tbl_PRNT(msg) ::std::cerr << (msg);

  // this must be a macro and not a function so that the expression and line number are both printed correctly 
  #define tbl_msg_assert(exp, msgA, varA, msgB, varB) \
    if((exp)==false){ \
      tbl_PRNT((msgA)) tbl_PRNT((varA)) tbl_PRNT("\n") \
      tbl_PRNT((msgB)) tbl_PRNT((varB)) tbl_PRNT("\n\n") \
      assert( (exp) ); \
    }  
#else
  //#define tbl_assert(exp, varA, varB) ;
  #define tbl_msg_assert(exp, msgA, varA, msgB, varB)
  #define tbl_PRNT(msg)
#endif

//namespace wat
//{
//  enum Type
//  {                                                           // 10 number types, 10 table variants + empty = 21 total - at least 5 bits needed
//    EMPTY = 0,
//    UI8,   I8,  UI16,  I16,  UI32,  I32,  UI64,  I64,  F32,  F64,
//    tUI8, tI8, tUI16, tI16, tUI32, tI32, tUI64, tI64, tF32, tF64
//  };
//}

//static const char EMPTYs[] = "EMPTY";
//static const char   F32s[] =   "f32";
//static const ui64 namestr;
//char const* const typenum<f32>::namestr = "f32";

class tbl
{
public:                                                                       // forward declarations
  enum Type;

private:

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
  void             del()
  { 
    // todo: needs to loop through and run destructors here
    if(m_mem) free(memStart());
  }
  void              cp(tbl const& l)
  {
    tbl_PRNT(" copied ");
    del();
    reserve(l.size());    // todo: can be done with resize, which could use realloc instead of free and malloc?
    //resize(l.size());

    TO(l,i) push(l[i]); 
  }
  void              mv(tbl& r)
  {
    tbl_PRNT(" moved ");
    m_mem   = r.m_mem;
    r.m_mem = nullptr;
  }

public:  
  enum Type
  {                                                                           // 10 number types, 10 table variants + empty = 21 total - at least 5 bits needed
    TABLE     =  1<<4,
    SIGNED    =  1<<3,
    INTEGER   =  1<<2,
    POWEROF2  =  1<<1 | 1,
    EMPTY     =  ~INTEGER & ~SIGNED,                                         // a floating point number can't be unsigned, so this scenario is used for an 'empty' state
    UI8,   I8,  UI16,  I16,  UI32,  I32,  UI64,  I64,  F32,  F64,
    tUI8, tI8, tUI16, tI16, tUI32, tI32, tUI64, tI64, tF32, tF64
  };
  template<class N> struct typenum { static const ui8 num = EMPTY; };
  template<> struct typenum<ui8>   { static const ui8 num = UI8;   }; 
  template<> struct typenum<ui16>  { static const ui8 num = UI16;  }; 
  template<> struct typenum<ui32>  { static const ui8 num = UI32;  }; 
  template<> struct typenum<ui64>  { static const ui8 num = UI64;  }; 
  template<> struct typenum<i8>    { static const ui8 num = I8;    }; 
  template<> struct typenum<i16>   { static const ui8 num = I16;   }; 
  template<> struct typenum<i32>   { static const ui8 num = I32;   }; 
  template<> struct typenum<i64>   { static const ui8 num = I64;   }; 
  template<> struct typenum<f32>   { static const ui8 num = F32;   }; 
  template<> struct typenum<f64>   { static const ui8 num = F64;   }; 
  //template<> struct typenum<tui8>  { static const ui8 num = tUI8;  }; 
  //template<> struct typenum<tui16> { static const ui8 num = tUI16; }; 
  //template<> struct typenum<tui32> { static const ui8 num = tUI32; }; 
  //template<> struct typenum<tui64> { static const ui8 num = tUI64; }; 
  //template<> struct typenum<ti8>   { static const ui8 num = tI8;   }; 
  //template<> struct typenum<ti16>  { static const ui8 num = tI16;  }; 
  //template<> struct typenum<ti32>  { static const ui8 num = tI32;  }; 
  //template<> struct typenum<ti64>  { static const ui8 num = tI64;  }; 
  //template<> struct typenum<tf32>  { static const ui8 num = tF32;  }; 
  //template<> struct typenum<tf64>  { static const ui8 num = tF64;  }; 

  union   HshType
  {
    struct { ui32 type : 5; ui32 : 27; };
    ui32 as_ui32;
  };
  struct       kv
  {
    HshType   hsh;
    char      key[19];
    ui64      val;

    template<class N> void operator=(N n)
    {
      hsh.type = typenum<N>::num;
      val      = *((ui64*)&n);
    }
    template<class N> operator N()
    { 
      tbl_msg_assert(
        hsh.type==typenum<N>::num, 
        " - tbl TYPE ERROR -\nInternal type was: ", tbl::type_str((Type)hsh.type), 
        "Desired  type was: ",                      tbl::type_str((Type)typenum<N>::num) );
      return *((N*)&val);
    }
    template<class T> T as(){ return (T)(*this); }
  };
  struct      Var
  {
    ui8   type;
    ui64  data; 
  };

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
  ~tbl(){ del(); }

  tbl(tbl const& l){ cp(l); }
  tbl& operator=(tbl const& l){ cp(l); return *this; }
  tbl(tbl&& r){ mv(r); }
  tbl& operator=(tbl&& r){ mv(r); return *this; }

  operator    ui64() const { return size(); }
  operator    bool() const { return m_mem!=nullptr; }
  T&    operator[](ui64 i){ return ((T*)m_mem)[i]; }
  auto  operator[](ui64 i) const -> T const& { return ((T*)m_mem)[i]; }
  //var&  operator()(const char*)                             // todo: future hash map interface
  //{
  //  Var nonsense;
  //  return nonsense;
  //}
  tbl   operator>>(tbl const& l){ return tbl::concat(*this, l); }
  tbl   operator<<(tbl const& l){ return tbl::concat(*this, l); }

  bool        push(T const& value)
  {
    if( !(capacity()>size()) )
      if(!expand()) return false;
    
    (*this)[size()] = value;

    set_size(size()+1);

    return true;
  }
  bool   push_back(T const& value){ return push(value); }
  void         pop(){ /*delete &(back());*/ set_size(size()-1); }  // todo: needs to run the destructor here
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


private:
  static ui64 memberBytes()
  {
    return sizeof(ui64) * 2;
  }
public:
  static char const* const type_str(Type t)
  {
    switch(t)
    {
    case EMPTY: return "Empty";
    
    case   UI8: return  "ui8";
    case  UI16: return "ui16";
    case  UI32: return "ui32";
    case  UI64: return "ui64";
    case    I8: return   "i8";
    case   I16: return  "i16";
    case   I32: return  "i32";
    case   I64: return  "i16";
    case   F32: return  "f32";
    case   F64: return  "f64";

    case  tUI8: return  "table ui8";
    case tUI16: return "table ui16";
    case tUI32: return "table ui32";
    case tUI64: return "table ui64";
    case   tI8: return   "table i8";
    case  tI16: return  "table i16";
    case  tI32: return  "table i32";
    case  tI64: return  "table i16";
    case  tF32: return  "table f32";
    case  tF64: return  "table f64";

    default: return "Unknown Type";
    }
    //return "Empty";
  }
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
};


#endif






//template<> struct typenum<ui8>   { static const ui8 num =   UI8; static const char* namestr =   "ui8"; };
//
//template<class N> union type
//{
//  struct<
//};

//case   UI8: return "";
//case  UI16: return "";
//case  UI32: return "";
//case  UI64: return "";
//case    I8: return "";
//case   I16: return "";
//case   I32: return "i32";
//case   I64: return "";
//case   F32: return "f32";
//case   F64: return "";
//case  tUI8: return "";
//case tUI16: return "";
//case tUI32: return "";
//case tUI64: return "";
//case   tI8: return "";
//case  tI16: return "";
//case  tI32: return "";
//case  tI64: return "";
//case  tF32: return "";
//case  tF64: return "";

//tbl_msg_assert(hsh.type == typenum<N>::num, "Internal type was: ", "", "Desired type was: ", "");
//tbl_msg_assert(hsh.type == typenum<N>::num, "Internal type was: ");

//#define tbl_msg_assert(exp, msgA, varA, msgB, varB) if((exp)==false){ tbl_PRNT((msgA)) tbl_PRNT((varA)) tbl_PRNT((msgA)) tbl_PRNT((varA)) assert( (exp) ); }  
//
//void tbl_msg_assert(bool exp, const char* msgA, const char* varA, const char* msgB, const char* varB)
//{     
//  if((exp)==false)
//  { 
//    ::std::cerr 
//    << msgA << varA << ::std::endl 
//    << msgB << varB << ::std::endl << ::std::endl;
//
//    assert( (exp) );
//  }
//}

//#define tbl_TELL(var) fprintf(stderr, "\n    ::std::cout<<::std::endl<<::std::setprecision(4)<<#var ## ": "<<::std::endl<<var<<::std::endl;::std::cout.flush();
//#ifdef _DEBUG
//#include <cstdio>
//#define tbl_assert(exp, varA, varB) if( (exp) == false ) { tbl_TELL(varA) tbl_TELL(varB) assert( (exp) ); }
//#define tbl_assert(exp, varA, varB) if( (exp) == false ) { assert( (exp) && (varA) && (varB) ); assert( (varA)?false:false );  }
//#define func_assert(exp, FUNC) if( (exp) == false ) { FUNC assert( (exp) );  }
//
//#define func_assert(exp, FUNC) ;
//#endif

//#include <sstream>
//#include <ostream>
//#include <string>

//static char const* const namestr() { return   "f32"; }
//char const* const namestr() const { return "Empty"; }

//static char const* const type_string(Type t);
//
//static ui64 memberBytes()
//{
//  return sizeof(ui64) * 2;
//}

//operator ui32()
//{
//  return *((ui32*)&val);
//}
//operator float()
//{
//  return *((float*)&val);
//}

//kv& operator=(float v)
//{
//  val = *((ui64*)&v);
//
//  return *this;
//}

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
