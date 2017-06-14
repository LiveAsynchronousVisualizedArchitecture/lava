

// -todo: add pointer casts to all tbl types
// -todo: make assignment of tbl to map slot only take pointers to a tbl for clarity
// -todo: check the type of the table cast and build in asserts just like fundamental number types - most asserts still work and apply
// -todo: make separate tbl<N>  as<>() function template?  - might have to to make cast to tbl* work 
// -todo: fix child types and table pointer types
// -todo: fix child table to non-child table pointer assert error and message
// -todo: test a smaller integer type being taken out of the tbl
// -todo: test numeric operations on arrays
// -todo: put in initializer_list as constructor 
// -todo: take out assertion on casting to a smaller bit size - want a warning if possible
// -todo: need to change types when flattening - just needed a recompile
// -todo: clean up types to no longer be in the global namespace - leave the tbl types in the global namespace
// -todo: clean out comments
// -todo: take out no_rt_util.h dependency 
// -todo: make a u8* constructor
// -todo: fix serialization issue - was assigning the start of the memory buffer directly to m_mem, needed to offset it by memStart() which was done by making a u8* constructor
// -todo: fix assert on unsigned integer casting to u64 - actually working correctly
// -todo: debug IND tbl not coming through correctly - KVOfst cast to table assumes that the table is the same type as what it is being cast to - need to make offset the address of childData() directly instead of the base of the parent table
// -todo: make as<>() passthrough in KVOfst - not neccesary? 
// -todo: make operator->() passthrough to KV - works but still gives warning

// todo: make a const version of operator()
// todo: should moving a table into a key flatten the tbl and automatically make that tbl a chld? - could work due to realloc - use a dedicated function that does its own realloc? 
// todo: make sure moving a tmp tbl into a table works - will need to be destructed on flatten AND destructor - need to make moving a tbl in work - owned can still be set - will need a tbl type that isn't a pointer and isn't a child?
// todo: make initializer list syntax possible to create std::pairs of strings and lists of values
// todo: make simdb convenience function to put in a tbl with a string key
// todo: make boolean argument to flatten() to destruct pointed to tables
// todo: make flatten() recursive 
// todo: transition indexed verts to use tbl
// todo: make a string type using the 8 bytes in the value and the extra bytes of the key
//       | can casts to c_str() using a single 0 byte after the array work? 
//       |   if there is a blank key or no map and a 0 byte at the beggining of childData() then the cast to c_str() could work 
//       | does any array of strings imply a rabbit hole of keeping track of the type of the array - would any child strings just need to be kept in the map?
//       | other formats such as one child tbl containing packed strings and another array containing offsets could also be used 
//       | if it exceeds the capacity of the extra key, the make it an offset in the tbl extra space
//       | does this imply that there should be a separate array type or is specializing string enough? 

// todo: break out memory allocation from template - keep template as a wrapper for casting a typeless tbl
// todo: make resize() - should there be a resize()? only affects array?
// todo: use inline assembly to vectorize basic math operations
// todo: use the restrict keyword on basic math operations?
// todo: put an assert that the two tbl pointers are not the same
// todo: move size to be the 8 bytes before m_mem
// todo: try template constructor that returns a tbl<type> with a default value set? - can't have a constructor that reserves elems with this? would need a reserve_elems() function? reserve_elems() not needed since adding a key will allocate map_capacity memory  - will have to try after turning into a template
// todo: make copy use resize? - should it just be a memcpy?
// todo: make begin and end iterators to go with C++11 for loops - loop through keys value pairs?
// todo: take out reorder from shrink_to_fit() and make a function to sort the elements into robin hood order without any EMPTY slots - already there with place_rh ?
// todo: fold HashType into KV? - would have to make an internal struct/union? - just change HashType hsh to HashType ht?
// todo: keep the max distance through every loop and use that to short-circuit the reorder loop? - have place_kv return the the distance from ideal in a separate variable, keep track of the index... is there a way to take the max distance back down? does it need to go back down? does the end point only need to be prevMapcap elements forward? - no because there could still be holes?
// todo: make visualization for table as a tree that shows arrays, key-values and their types, then sub tables - make various visualizations for arrays - histogram, graph, ???
// todo: is an optimization possible where the index of the largest distance from ideal position is kept so the loop doesn't have to go all the way back around? - no because moving the largest distance means the next largest would have to be found again 
// todo: is it possible to deal with spans of keys that go to the same bucket? would it be more efficient?
// todo: make reserve_elems() function or reserve_map() function? - if reserve 
// todo: use binary bit operators to make tbl act like a set?
// todo: make operator~ return just the vector
// todo: make different unary operator return just the map?
// todo: make macro to allocate memory on the stack and use it for a table
// todo: make emplace and emplace_back()
// todo: specialize cp() and mv() as well?
// todo: add ability to use a function in the template type declaration to give a custom accessor by making operator() a variadic template?
// todo: make visual studio visualization?
// todo: make TBL_USE_JSON and TBL_USE_STL to have json serialization and stl integraton?
// todo: make separate 'iota' function for a sequential tbl - maybe tbl_seq<T>()
// todo: make separate accum(tbl<T> t) function for accumulating a tbl

#ifndef __TBL_HEADERGUARD_H__
#define __TBL_HEADERGUARD_H__

#include <cstdlib>
#include <cstring>
//#include "../no_rt_util.h"

#ifndef TO
  #define TO(to,var) for(uint64_t var=0, lim=(uint64_t)to; var<lim; ++var)
#endif
#ifndef FROM
  #define FROM(from,var) for(std::remove_const<decltype(from)>::type var=from; var-- > 0; )
#endif

#if defined(_MSC_VER) && defined(_DEBUG)
  #include <iostream>
  #include <iomanip>
  #include <cassert>
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

template<class T=int8_t> class tbl;
using   tu8   =  tbl<uint8_t>;
using   tu16  =  tbl<uint16_t>;
using   tu32  =  tbl<uint32_t>;
using   tu64  =  tbl<uint64_t>;
using   ti8   =  tbl<int8_t>;
using   ti16  =  tbl<int16_t>;
using   ti32  =  tbl<int32_t>;
using   ti64  =  tbl<int64_t>;
using   tf32  =  tbl<float>;
using   tf64  =  tbl<double>;
using   Tbl   =  tbl<>;
//using   tbl   =  tbl<int8_t>;

union     HshType
{
public:
  using    u8   =   uint8_t;
  using   u16   =  uint16_t;
  using   u32   =  uint32_t;
  using   u64   =  uint64_t;
  using    i8   =    int8_t;
  using   i16   =   int16_t;
  using   i32   =   int32_t;
  using   i64   =   int64_t;
  using   f32   =     float;
  using   f64   =    double;
  using  Type   =        u8;

  static const u8   MASK       =  0x3F;
  static const u8   CHILD      =  1<<5;
  static const u8   TABLE      =  1<<4;
  static const u8   SIGNED     =  1<<3;
  static const u8   INTEGER    =  1<<2;
  static const u8   BITS_8     =     0;                    // 2^3 is  8 for  8 bit depth - 0b00
  static const u8   BITS_16    =     1;                    // 2^4 is 16 for 16 bit depth - 0b01
  static const u8   BITS_32    =  1<<1;                    // 2^5 is 32 for 32 bit depth - 0b10
  static const u8   BITS_64    =  1<<1 | 1;                // 2^6 is 64 for 64 bit depth - 0b11
  static const u8   BITS_MASK  =  BITS_64;
  static const u8   CLEAR      =  BITS_8;
  static const u8   ERR        =  ~INTEGER & ~SIGNED & ~TABLE & ~CHILD & ~BITS_32 & MASK;
  static const u8   NONE       =  ~INTEGER & ~SIGNED & ~TABLE & ~CHILD & ~BITS_16 & MASK;                                         // ~TABLE,                              // INTEGER bit turned off, SIGNED bit turned off and TABLE bit turned off, meanin unsigned float table, which is not a viable real type of course
  static const u8   EMPTY      =  ~INTEGER & ~SIGNED & ~TABLE & ~CHILD & MASK;     // 0b00111111;                                       // a floating point number can't be unsigned, so this scenario is used for an 'empty' state
  

  static const u8    U8        =  INTEGER |  BITS_8;
  static const u8    I8        =  INTEGER |  BITS_8 | SIGNED;
  static const u8   U16        =  INTEGER | BITS_16;
  static const u8   I16        =  INTEGER | BITS_16 | SIGNED;
  static const u8   U32        =  INTEGER | BITS_32;
  static const u8   I32        =  INTEGER | BITS_32 | SIGNED;
  static const u8   F32        =  BITS_32 | SIGNED;
  static const u8   U64        =  INTEGER | BITS_64;
  static const u8   I64        =  INTEGER | BITS_64 | SIGNED;
  static const u8   F64        =  BITS_64 | SIGNED;

  static const u8    tU8       =  TABLE   | INTEGER |  BITS_8;
  static const u8    tI8       =  TABLE   | INTEGER |  BITS_8 | SIGNED;
  static const u8   tU16       =  TABLE   | INTEGER | BITS_16;
  static const u8   tI16       =  TABLE   | INTEGER | BITS_16 | SIGNED;
  static const u8   tU32       =  TABLE   | INTEGER | BITS_32;
  static const u8   tI32       =  TABLE   | INTEGER | BITS_32 | SIGNED;
  static const u8   tF32       =  TABLE   | F32;
  static const u8   tU64       =  TABLE   | INTEGER | BITS_64;
  static const u8   tI64       =  TABLE   | INTEGER | BITS_64 | SIGNED;
  static const u8   tF64       =  TABLE   | F64;

  static const u8    cU8       =  CHILD   |  tU8;
  static const u8    cI8       =  CHILD   |  tI8;
  static const u8   cU16       =  CHILD   | tU16;
  static const u8   cI16       =  CHILD   | tI16;
  static const u8   cU32       =  CHILD   | tU32;
  static const u8   cI32       =  CHILD   | tI32;
  static const u8   cF32       =  CHILD   | tF32;
  static const u8   cU64       =  CHILD   | tU64;
  static const u8   cI64       =  CHILD   | tI64;
  static const u8   cF64       =  CHILD   | tF64;

  struct { u32 type : 6; u32 hash: 26; };
  u32 as_u32;

  HshType() : 
    type(EMPTY),
    hash(0)
  {}

  static auto type_str(Type t) -> char const* const;
};
struct         KV
{
  using    u8   =   uint8_t;
  using   u16   =  uint16_t;
  using   u32   =  uint32_t;
  using   u64   =  uint64_t;
  using    i8   =    int8_t;
  using   i16   =   int16_t;
  using   i32   =   int32_t;
  using   i64   =   int64_t;
  using   f32   =     float;
  using   f64   =    double;

  using    Type   =  HshType::Type;
  
  template<class N> struct typenum { static const Type num = HshType::EMPTY; };
  template<> struct typenum<u64>   { static const Type num = HshType::U64;   };
  template<> struct typenum<i64>   { static const Type num = HshType::I64;   };
  template<> struct typenum<f64>   { static const Type num = HshType::F64;   };
  template<> struct typenum<long>               { static const Type num = HshType::I64;   };
  template<> struct typenum<unsigned long>      { static const Type num = HshType::U64;   };

  template<> struct typenum<tu8*>   { static const Type num = HshType::tU8;   }; 
  template<> struct typenum<ti8*>   { static const Type num = HshType::tI8;   }; 
  template<> struct typenum<tu16*>  { static const Type num = HshType::tU16;  }; 
  template<> struct typenum<ti16*>  { static const Type num = HshType::tI16;  }; 
  template<> struct typenum<tu32*>  { static const Type num = HshType::tU32;  }; 
  template<> struct typenum<ti32*>  { static const Type num = HshType::tI32;  }; 
  template<> struct typenum<tf32*>  { static const Type num = HshType::tF32;  };
  template<> struct typenum<tu64*>  { static const Type num = HshType::tU64;  }; 
  template<> struct typenum<ti64*>  { static const Type num = HshType::tI64;  }; 
  template<> struct typenum<tf64*>  { static const Type num = HshType::tF64;  };
  template<> struct typenum<tbl<long>*>          { static const Type num = HshType::tI64;  };
  template<> struct typenum<tbl<unsigned long>*> { static const Type num = HshType::tU64;  };

  template<> struct typenum<tu8>   { static const Type num = HshType::cU8;   }; 
  template<> struct typenum<ti8>   { static const Type num = HshType::cI8;   }; 
  template<> struct typenum<tu16>  { static const Type num = HshType::cU16;  }; 
  template<> struct typenum<ti16>  { static const Type num = HshType::cI16;  }; 
  template<> struct typenum<tu32>  { static const Type num = HshType::cU32;  }; 
  template<> struct typenum<ti32>  { static const Type num = HshType::cI32;  }; 
  template<> struct typenum<tf32>  { static const Type num = HshType::cF32;  };
  template<> struct typenum<tu64>  { static const Type num = HshType::cU64;  }; 
  template<> struct typenum<ti64>  { static const Type num = HshType::cI64;  }; 
  template<> struct typenum<tf64>  { static const Type num = HshType::cF64;  };
  template<> struct typenum<tbl<long>>          { static const Type num = HshType::cI64;  };
  template<> struct typenum<tbl<unsigned long>> { static const Type num = HshType::cU64;  };

  template<class C> struct typecast { using type = C;   };                               // cast types
  template<> struct typecast<i8>    { using type = i64; };
  template<> struct typecast<i16>   { using type = i64; };
  template<> struct typecast<i32>   { using type = i64; };
  template<> struct typecast<u8>    { using type = u64; };
  template<> struct typecast<u16>   { using type = u64; };
  template<> struct typecast<u32>   { using type = u64; };
  template<> struct typecast<f32>   { using type = f64; };

  template<class DEST, class SRC> static DEST cast_mem(u64 const* const src){ return (DEST)(*((SRC*)src)); }
  
  using Key = char[43];

  HshType   hsh;
  Key       key;
  u64       val;
  u64      base;

  KV() : hsh(), val(0), base(0) { 
    memset(key, 0, sizeof(Key));
    hsh.type = HshType::EMPTY;
  }
  KV(const char* key, u32 hash, u64 _val=0, u64 _base=0) :
    hsh(), val(_val), base(_base)
  {
    memcpy(this->key, key, sizeof(KV::Key) );
    hsh.hash = hash;
    hsh.type = HshType::EMPTY;
  }
  KV(KV const& l){ cp(l); }
  KV(KV&&      r){ cp(r); }

  KV& cp(KV const& l)
  {
    hsh  = l.hsh;
    val  = l.val;
    base = l.base;
    memmove(key, l.key, sizeof(Key) );
    return *this;
  }

  bool operator==(KV const& l){ return hsh.hash==l.hsh.hash && strncmp(l.key,key,sizeof(KV::Key)-1)==0; }
  KV&  operator= (KV const& l){ return cp(l); }
  KV&  operator= (KV&&      r){ return cp(r); }
  template<class N> KV& operator=(N       const& n)
  {
    hsh.type     = typenum< typecast<N>::type >::num;
    auto castVal = (typecast<N>::type)n;
    memcpy(&val, &castVal, sizeof(u64));
    return *this;
  }
  template<class N> KV& operator=(tbl<N>  const& n) = delete;
  template<class N> KV& operator=(tbl<N>  const* const n)
  {
    hsh.type  =  typenum< tbl<N> >::num;
    val       =  (u64)n;
    return *this;
  }

  template<class N> operator N(){ return as<N>(); }
  template<class N> N as() const
  { 
    using DES = typecast<N>::type;                                                       // DES is the desired type 

    if(hsh.type==typenum<DES>::num) return *((DES*)&val);                                    // if the types are the same, return it as the cast directly
    
    if( (hsh.type==HshType::NONE) || (hsh.type==HshType::ERR) ){
      tbl_msg_assert(
        hsh.type==typenum<DES>::num, 
        "\n - tbl TYPE ERROR -\nInternal type: ", 
        HshType::type_str((Type)hsh.type), 
        "Desired type: ",
        HshType::type_str((Type)typenum< typecast<DES>::type >::num) );        
    } 
    if( (hsh.type & HshType::SIGNED) && !(typenum<DES>::num & HshType::SIGNED)  ){
      tbl_msg_assert(
        hsh.type==typenum<DES>::num, 
        "\n - tbl TYPE ERROR -\nSigned integers can not be implicitly cast to unsigned integers.\nInternal type: ", 
        HshType::type_str((Type)hsh.type), 
        "Desired type: ",
        HshType::type_str((Type)typenum<DES>::num) );
    }
    if( !(hsh.type & HshType::INTEGER) && (typenum<DES>::num & HshType::INTEGER) ){
      tbl_msg_assert(
        hsh.type==typenum<DES>::num, 
        "\n - tbl TYPE ERROR -\nFloats can not be implicitly cast to integers.\nInternal type: ", 
        HshType::type_str((Type)hsh.type), 
        "Desired type: ",
        HshType::type_str((Type)typenum<DES>::num) );
    }
    if( (hsh.type|typenum<DES>::num) & HshType::TABLE ){
      tbl_msg_assert(
        hsh.type==typenum<DES>::num, 
        "\n - tbl TYPE ERROR -\nTables can not be implicitly cast, even to a larger bit depth.\nInternal type: ", 
        HshType::type_str((Type)hsh.type), 
        "Desired type: ",
        HshType::type_str((Type)typenum<DES>::num) );
    }

    switch(hsh.type)
    {
      case   HshType::U64: return cast_mem<N,  u64>(&val);
      case   HshType::I64: return cast_mem<N,  i64>(&val);
      case   HshType::F64: return cast_mem<N,  f64>(&val);

      case  HshType::tU8:  return cast_mem<N,  tu8*>(&val);
      case  HshType::tI8:  return cast_mem<N,  ti8*>(&val);
      case  HshType::tU16: return cast_mem<N, tu16*>(&val);
      case  HshType::tI16: return cast_mem<N, ti16*>(&val);
      case  HshType::tU32: return cast_mem<N, tu32*>(&val);
      case  HshType::tI32: return cast_mem<N, ti32*>(&val);
      case  HshType::tF32: return cast_mem<N, tf32*>(&val);
      case  HshType::tU64: return cast_mem<N, tu64*>(&val);
      case  HshType::tI64: return cast_mem<N, ti64*>(&val);
      case  HshType::tF64: return cast_mem<N, tf64*>(&val);
    }

    tbl_msg_assert(
      hsh.type==typenum<N>::num, 
      " - tbl TYPE ERROR -\nInternal type was: ", HshType::type_str((Type)hsh.type), 
      "Desired  type was: ",                      HshType::type_str((Type)typenum<N>::num) );

    return N();
  }

  bool isEmpty() const { return hsh.type==HshType::NONE || hsh.type==HshType::EMPTY; }

  static KV& empty_kv(){ static KV kv; kv.hsh.type = HshType::EMPTY; return kv; }
  static KV&  none_kv(){ static KV kv; kv.hsh.type = HshType::NONE;  return kv; }
  static KV& error_kv(){ static KV kv; kv.hsh.type = HshType::ERR;   return kv; }
};
struct     KVOfst
{
  KV*         kv;
  void*     base;

  KVOfst(KVOfst&  l) : kv(l.kv), base(l.base) {}
  KVOfst(KVOfst&& r) : kv(r.kv), base(r.base) {}
  KVOfst(KV* _kv=nullptr, void* _base=nullptr) : kv(_kv), base(_base) {}

  operator bool() const { return (bool)(kv); }
  template<class N> N as() const { return kv->as<N>(); }

  template<class T> operator tbl<T> ();
  template<class T> operator tbl<T>*();

  template<class N> operator N() { return (N)(*kv); }
  template<class N> KVOfst& operator=(N const& n){ *kv = n; return *this; }

  KV* operator->(){ return kv; }
};
struct  TblFields 
{
  using u64 = uint64_t;
  
  u64         t :  8;                             // will hold the character 't'
  u64         b :  8;                             // will hold the character 'b'
  u64 sizeBytes : 48;

  u64  capacity : 42;
  u64    mapcap : 22;

  u64     owned :  1;
  u64     elems : 21;
  u64      size : 42;
};

template<class T> class tbl
{
public:                 
  using fields  =  TblFields;

  using    u8   =   uint8_t;
  using   u16   =  uint16_t;
  using   u32   =  uint32_t;
  using   u64   =  uint64_t;
  using    i8   =    int8_t;
  using   i16   =   int16_t;
  using   i32   =   int32_t;
  using   i64   =   int64_t;
  using   f32   =     float;
  using   f64   =    double;

  using   tu8   =  tbl<u8>;
  using   tu16  =  tbl<u16>;
  using   tu32  =  tbl<u32>;
  using   tu64  =  tbl<u64>;
  using   ti8   =  tbl<i8>;
  using   ti16  =  tbl<i16>;
  using   ti32  =  tbl<i32>;
  using   ti64  =  tbl<i64>;
  using   tf32  =  tbl<f32>;
  using   tf64  =  tbl<f64>;

private:
  void      sizeBytes(u64  bytes){ memStart()->sizeBytes =  bytes; }
  void           size(u64   size){ memStart()->size      =   size; }
  void       capacity(u64    cap){ memStart()->capacity  =    cap; }
  void          elems(u64  elems){ memStart()->elems     =  elems; }
  void         mapcap(u64 mapcap){ memStart()->mapcap    = mapcap; }
  bool     map_expand(bool force=false)
  {  
    u64 mapcap = map_capacity();
    if( force || (mapcap==0 || mapcap*8 < elems()*10) )
      return expand(false, true);

    return true;
  }
  u64             nxt(u64     i, u64 mod) const
  {
    return ++i % mod;
  }
  u64            prev(u64     i, u64 mod) const
  {
    if(mod==0) return 0;
    return i==0?  mod-1  :  i-1;
  }
  u64        wrapDist(u64 ideal, u64 idx, u64 mod) const
  {
    if(idx>=ideal) return idx - ideal;
    else return idx + (mod-ideal);
  }
  u64        wrapDist(KV* elems, u64 idx, u64 mod) const
  {
    u64 ideal = elems[idx].hsh.hash % mod;
    return wrapDist(ideal,idx,mod);
  }
  KV&        place_rh(KV     kv, KV* elems, u64 st, u64 dist, u64 mod, u64* placement=nullptr)   // place_rh is place with robin hood hashing 
  {
    //assert( strcmp(kv.key,"")!=0 );

    u64      i = st;
    u64     en = prev(st,mod);
    u64  eldst = dist;
    KV*     ret = nullptr;
    while(true)
    {
      if(i==en){ return KV::error_kv(); }
      else if(elems[i].hsh.type==HshType::EMPTY || kv==elems[i]){
        elems[i] = kv;
        if(placement) *placement = i;
        if(ret) return *ret;
        else    return elems[i];
      }else if( dist > (eldst=wrapDist(elems,i,mod)) ){
        swap( &kv, &elems[i] );
        dist = eldst;
        if(!ret) ret = &elems[i];
      }

      i = nxt(i,mod);
      ++dist;
    }

    if(placement) *placement = i;
    if(ret) return *ret;
    else    return KV::error_kv();
  }
  u64         compact(u64    st, u64 en, u64 mapcap)
  {
    u64   cnt = 0;                                   // cnt is count which counts the number of loops, making sure the entire map capacity (mapcap) has not bene exceeded
    KV*     el = elemStart(); 
    u64     i = st;
    u64 empty;
    while(i!=en && el[i].hsh.type!=EMPTY){ 
      i = nxt(i,mapcap);
      ++cnt;
    } // finds the first empty slot
    empty = i;
    while(i!=en && el[i].hsh.type==EMPTY){
      i = nxt(i,mapcap);
      ++cnt;
    } // find the first non-empty slot after finding an empty slot
    u64 prevElemDst = 0;
    while(i!=en && el[i].hsh.type!=EMPTY){
      u64 elemDst = wrapDist(el,i,mapcap);
      if(elemDst<prevElemDst) return i;

      prevElemDst = elemDst; // short circuiting based on robin hood hashing - as soon as the distance to the ideal position goes down, its ideal position must be further forward than the current empty position. Because no other EMPTY elements have been found, it must not be able to be moved to the current empty position, and this compact() is done
      if( elemDst>0 ){ 
        u64 emptyDst = wrapDist(empty,i,mapcap);           // emptyDst is empty distance - the distance to the empty slot - this can probably be optimized to just be an increment
        u64    mnDst = mn(elemDst, emptyDst);               // mnDst is the minimum distances
        u64    mvIdx = i;
        TO(mnDst,ii) mvIdx=prev(mvIdx,mapcap);
        el[mvIdx] = el[i];
        el[i]       = KV();
        empty       = i;                                     //++empty; // empty = i;  if empty was just the current index or if there are lots of empty elements in between, empty can be incremented by one
      }
      i = nxt(i,mapcap);
      ++cnt;
    } // moves elements backwards if they would end up closer to their ideal position

    return i;
  }
  u64         reorder()                                         // can this be done by storing chunks (16-64 etc) of KVs in an array, setting the previous slots to empty, sorting the KVs in the array by the slot they will go in, then placing them?   
  { 
    u64  mod  =  map_capacity();
    if(mod==0){ return 0; }

    KV*   el  =  elemStart();
    u64    i  =  0;
    u64   en  =  prev(i,mod);
    u64  cnt  =  0; 
    do{
      u64 nxti = nxt(i,mod); 
      if(el[i].hsh.type != HshType::EMPTY){
        KV kv  = el[i];
        el[i]  = KV();
        u64 st = kv.hsh.hash % mod;
        u64  p = i; 
        place_rh(kv,el,st,0,mod,&p);
        if(p!=i) en = i;
      }
      i = nxti;
      ++cnt;
    }while(i!=en);

    return cnt;
  }
  void           init(u64 size)
  {
    u64    szBytes  =  tbl::size_bytes(size);
    u8*      memst  =  (u8*)malloc(szBytes);                 // memst is memory start
    m_mem           =  memst + memberBytes();

    fields*    f = (fields*)memst;
    f->t         = 't';
    f->b         = 'b';
    f->sizeBytes = szBytes;
    f->elems     = 0;
    f->capacity  = size;
    f->size      = size;
    f->mapcap    = 0;
    f->owned     = 1;
  }
  void        destroy()
  { 
    if( m_mem && owned() ){
      //tbl_PRNT("\n destruction \n");

      T*    a = (T*)m_mem;
      auto sz = size();
      TO(sz,i){
        a[i].T::~T();                                                                    // run the destructors manually before freeing the memory
      }
      
      free(memStart());
      m_mem = nullptr;
    }
  }
  void             cp(tbl const& l)
  {    
    if(l.owned()){
      //tbl_PRNT("\n full copy \n");
      tbl<T> prev;
      prev.m_mem = m_mem;                                                                // make a tbl for the old memory - this will be destructed at the end of this scope, which will free up the old memory. the old memory is not freed up first, since we could be copying from a child of this very same tbl 

      m_mem = nullptr;
      reserve(l.size(), l.elems(), l.child_capacity() );                                 // this will size the current table exactly because it starts from a nullptr - it also means however that realloc is not used and malloc is called for a brand new allocation
      TO(l.size(),i) push(l[i]); 
    
      auto e = l.elemStart();
      TO(l.map_capacity(),i) if( !e[i].isEmpty() ){
        put(e[i].key, e[i].val);
      }

      memcpy(childData(), l.childData(), l.child_capacity() );                           // since this is a straight copy of the child memory, the values/offsets in the map's child table can stay the same
    }else{
      //tbl_PRNT("\n shallow copy \n");
      m_mem = l.m_mem;
    } 
  }
  void             mv(tbl&& r)
  {
    //tbl_PRNT("\n moved \n");
    m_mem   = r.m_mem;
    r.m_mem = nullptr;
  }
  template<class OP> void op_asn(tbl const& l, OP op)
  {
    u64     mx_sz = size();
    u64     mn_sz = l.size();
    if(mx_sz<mn_sz){ auto tmp=mx_sz; mx_sz=mn_sz; mn_sz=tmp; }
    TO(mn_sz,i) op( (*this)[i], l[i] );
  }
  template<class OP> tbl  bin_op(tbl const& l, OP op) const
  {     
    u64     mx_sz = size();
    u64     mn_sz = l.size();
    tbl const* lrg = this;
    if(mx_sz<mn_sz){ auto tmp=mx_sz; mx_sz=mn_sz; mn_sz=tmp; lrg = &l; }        // swap mx_sz, mn_sz, and the pointer to the larger tbl if mismatched
    tbl ret(mx_sz); 
    TO(mn_sz, i) ret[i] = op( (*this)[i], l[i] );
    TO(mx_sz-mn_sz, i) ret[i+mn_sz] = (*lrg)[i+mn_sz];

    return ret;
  }
  template<class OP> void op_asn(T   const& l, OP op)
  {
    TO(size(),i) op( (*this)[i], l);
  }
  template<class OP> tbl  bin_op(T   const& l, OP op) const
  {     
    tbl ret( size() ); 
    TO(ret, i) ret[i] = op( (*this)[i], l );

    return ret;
  }

public:  
  u8*     m_mem;                                                                         // the only member variable - everything else is a contiguous block of memory
 
  tbl() : m_mem(nullptr){}
  tbl(u8* memst) : m_mem(memst+memberBytes()){}
  tbl(u64 size){ init(size); }                                                           // have to run default constructor here?
  tbl(u64 size, T const& value)
  {
    init(size);
    TO(size, i){ (*this)[i] = value; }
  }
  tbl(std::initializer_list<T> lst)
  {
    reserve(lst.size(),0,0);
    for(auto&& n : lst){ emplace(n); }
  }
  ~tbl(){ destroy(); }

  tbl           (tbl const& l){ cp(l);                          }
  tbl& operator=(tbl const& l){ cp(l);            return *this; }
  tbl           (tbl&&      r){ mv(std::move(r));               }
  tbl& operator=(tbl&&      r){ mv(std::move(r)); return *this; }

  T&      operator[](u64 i)
  {
    tbl_msg_assert(i < size(), "\n\nTbl index out of range\n----------------------\nIndex:  ", i, "Size:   ", size())
    return ((T*)m_mem)[i];
  }
  auto    operator[](u64 i) const -> T const& 
  {
    tbl_msg_assert(i < size(), "\n\nTbl index out of range\n----------------------\nIndex:  ", i, "Size:   ", size())
    return ((T*)m_mem)[i];
  }
  KVOfst  operator()(i32 k)
  {
    char key[sizeof(k)+1];
    memcpy(key, &k, sizeof(k));
    key[sizeof(k)] = '\0';
    return operator()(key);
  }
  KVOfst  operator()(const char* key)
  {      
    KVOfst  ret;                                                                         // this will be set with placement new instead of operator= because operator= is templated and used for assigning to the KV pointed to by KVOfst::KV* -  this is so tbl("some key") = 85  can work correctly
    u32     hsh;
    KV*      kv = m_mem?  get(key, &hsh) : nullptr;
    if(owned())
    { 
      if( !map_expand(!kv) ) return KVOfst();                                            // if map_expand returns false, that means that memory expansion was tried but failed

      kv = get(key, &hsh);                                                               // if the expansion succeeded there has to be space now, but the keys will be reordered 

      auto type = kv->hsh.type;
      if(type==HshType::EMPTY)
      {
        elems( elems()+1 );
        new (kv) KV(key, hsh);
        kv->hsh.type = HshType::NONE;
        new (&ret) KVOfst(kv);
      }else if( (type&HshType::TABLE) && (type&HshType::CHILD) )
        new (&ret) KVOfst(kv, childData()); // (void*)memStart());
      else
        new (&ret) KVOfst(kv);
    }else 
      new (&ret) KVOfst(kv);                                                             // if the key wasn't found, kv will be a nullptr which is the same as an error KVOfst that will evaluate to false when cast to a boolean      

    return ret;
  }
  tbl     operator>>(tbl const& l){ return tbl::concat_l(*this, l); }
  tbl     operator<<(tbl const& l){ return tbl::concat_r(*this, l); }
  tbl&    operator--(){ shrink_to_fit();    return *this; }
  tbl&    operator++(){ expand(true,false); return *this; }
  void    operator+=(tbl const& l){ op_asn(l, [](T& a, T const& b){ a += b; } ); }
  void    operator-=(tbl const& l){ op_asn(l, [](T& a, T const& b){ a -= b; } ); }
  void    operator*=(tbl const& l){ op_asn(l, [](T& a, T const& b){ a *= b; } ); }
  void    operator/=(tbl const& l){ op_asn(l, [](T& a, T const& b){ a /= b; } ); }
  void    operator%=(tbl const& l){ op_asn(l, [](T& a, T const& b){ a %= b; } ); }
  tbl     operator+ (tbl const& l) const{ return bin_op(l,[](T const& a, T const& b){return a + b;}); }
  tbl     operator- (tbl const& l) const{ return bin_op(l,[](T const& a, T const& b){return a - b;}); }
  tbl     operator* (tbl const& l) const{ return bin_op(l,[](T const& a, T const& b){return a * b;}); }
  tbl     operator/ (tbl const& l) const{ return bin_op(l,[](T const& a, T const& b){return a / b;}); }
  tbl     operator% (tbl const& l) const{ return bin_op(l,[](T const& a, T const& b){return a % b;}); }
  void    operator+=(T   const& l){ op_asn(l, [](T& a, T const& b){ a += b; } ); }
  void    operator-=(T   const& l){ op_asn(l, [](T& a, T const& b){ a -= b; } ); }
  void    operator*=(T   const& l){ op_asn(l, [](T& a, T const& b){ a *= b; } ); }
  void    operator/=(T   const& l){ op_asn(l, [](T& a, T const& b){ a /= b; } ); }
  void    operator%=(T   const& l){ op_asn(l, [](T& a, T const& b){ a %= b; } ); }
  tbl     operator+ (T   const& l) const{ return bin_op(l,[](T const& a, T const& b){return a + b;}); }
  tbl     operator- (T   const& l) const{ return bin_op(l,[](T const& a, T const& b){return a - b;}); }
  tbl     operator* (T   const& l) const{ return bin_op(l,[](T const& a, T const& b){return a * b;}); }
  tbl     operator/ (T   const& l) const{ return bin_op(l,[](T const& a, T const& b){return a / b;}); }
  tbl     operator% (T   const& l) const{ return bin_op(l,[](T const& a, T const& b){return a % b;}); }

  template<class V> KVOfst put(const char* key, V val){ return this->operator()(key) = val; }

  template<class... V> bool emplace(V&&... val)
  {
    using namespace std;
    
    if(!m_mem){ init(0); }

    auto prevSz = size();
    if( !(capacity()>prevSz) )
      if(!expand(true, false)){ return false; }

    T* p = (T*)(m_mem);
    new (p+prevSz) T( forward<V>(val)... );
    size(prevSz+1);

    return true;
  }
  template<class... V> bool emplace_back(V&&... val){ return emplace(std::forward<V>(val)...);  }
  bool           push(T const& val){ return emplace(val); }
  u64            push(std::initializer_list<T> lst)
  {
    u64 cnt = 0;
    for(auto const& v : lst){
      bool ok = this->push(v);
      if(!ok){ return cnt; }
      ++cnt;
    }

    return cnt;
  }
  bool      push_back(T const& value){ return emplace(value); }
  void            pop(){ back()->T::~T(); set_size(size()-1); }
  void       pop_back(){ pop(); }
  T const&      front() const{ return (*this)[0]; }
  T const&       back() const{ return (*this)[size()-1]; }

  template<class N> bool insert(const char* key, N const& val)
  {
    KV& kv = (*this)(key, true);
    if(kv.hsh.type==ERROR) return false;

    kv = val;
    return true;
  }
  bool            has(const char* key)
  {
    KV const& kv = (*this)(key, false);
    return kv.hsh.type != EMPTY;
  }
  KV*             get(const char* key, u32* out_hash=nullptr)
  {
    HshType hh;
    hh.hash   =  HashStr(key);
    u32  hsh  =  hh.hash;                                                 // make sure that the hash is being squeezed into the same bit depth as the hash in HshType
    if(out_hash){ *out_hash = hsh; }
    KV*   el  =  (KV*)elemStart();                                        // el is a pointer to the elements 
    u64  mod  =  map_capacity();
    if(mod==0) return nullptr;

    u64    i  =  hsh % mod;
    u64   en  =  prev(i,mod);
    u64 dist  =  0;
    for(;;++i,++dist)
    {
      i %= mod;                                                                // get idx within map_capacity
      HshType eh = el[i].hsh;                                                  // eh is element hash
      if( eh.type == HshType::EMPTY || 
           (hsh == eh.hash &&                                  // if the hashes aren't the same, the keys can't be the same
           strncmp(el[i].key,key,sizeof(KV::Key)-1)==0) )
      { 
        return &el[i];
      }else if(dist > wrapDist(el,i,mod) ){
        KV kv(key, hsh);
        elems( elems()+1 );
        return &(place_rh(kv, el, i, dist, mod));
      }

      if(i==en) break;                                                 // nothing found and the end has been reached, time to break out of the loop and return a reference to a KV with its type set to NONE
    }

    return nullptr; // no empty slots and key was not found
  }

  u64            size() const { return m_mem? memStart()->size : 0; }
  T*             data() const {return (T*)m_mem; }
  void*     childData() const { return (void*)(elemStart() + map_capacity()); }                                                      // elemStart return a KV* so map_capacity will increment that pointer by the map_capacity * sizeof(KV)
  u64  child_capacity() const
  {
    auto szb = 0;
    if(!m_mem || (szb=sizeBytes())==0 ) return 0;

    return sizeBytes() - memberBytes() - capacity()*sizeof(T) - map_capacity()*sizeof(KV);
  }
  bool          owned() const  { return m_mem? memStart()->owned : true; }
  void          owned(bool own){ memStart()->owned = own;  }
  auto       memStart() -> fields* { return m_mem? (fields*)(m_mem - memberBytes())  :  nullptr; }
  auto       memStart() const -> fields const* { return (fields*)(m_mem - memberBytes()); }
  u64       sizeBytes() const { return m_mem? memStart()->sizeBytes : 0; }
  u64        capacity() const { return m_mem? memStart()->capacity  : 0; }
  u64           elems() const { return m_mem?  memStart()->elems    : 0; }
  u64    map_capacity() const { return m_mem?  memStart()->mapcap   : 0; }
  auto      elemStart() ->KV* { return m_mem? (KV*)(data() + capacity()) : nullptr; }
  auto      elemStart() const -> KV const* { return m_mem? (KV*)(data() + capacity()) : nullptr; }
  void*       reserve(u64 count, u64 mapcap=0, u64 childcap=0)
  {
    if( !owned() ) return m_mem;

    u64 prvChldCap = child_capacity();
    count     =  mx(count,          capacity() );
    mapcap    =  mx(mapcap,     map_capacity() );
    childcap  =  mx(childcap,       prvChldCap );
    KV*    prevElems  =  elemStart();
    u64    prevMemSt  =  (u64)memStart();
    u64     prevOfst  =  prevElems? ((u64)prevElems)-prevMemSt  :  0;
    u64    prevBytes  =  sizeBytes();
    u64   prevMapCap  =  map_capacity();
    void*    prvChld  =  childData();
    u64     nxtBytes  =  memberBytes() + sizeof(T)*count +  sizeof(KV)*mapcap + childcap;
    void*     re;
    bool      fresh  = !m_mem;
    if(fresh) re = malloc(nxtBytes);
    else      re = realloc( (void*)memStart(), nxtBytes);

    if(re){
      m_mem = ((u8*)re) + memberBytes();
      sizeBytes(nxtBytes);
      capacity(count);
      this->mapcap(mapcap);
      if(re && fresh){
        auto f = memStart();
        f->t   = 't';
        f->b   = 'b';
        size(0); elems(0);
      }
      byte_move(childData(), prvChld, prvChldCap);

      KV*  el = elemStart();                                     //  is this copying elements forward in memory? can it overwrite elements that are already there? - right now reserve only ends up expanding memory for both the array and map
      u8* elb = (u8*)el;  
      if(prevElems){
        u8* prevEl = (u8*)re + prevOfst;
        u64  prevB = prevMapCap * sizeof(KV);                    // prevB is previous map bytes
        FROM(prevB,i) elb[i] = prevEl[i];
      }

      i64 extcap = mapcap - prevMapCap;
      if(extcap>0) 
        TO(extcap,i) 
          new (&el[i+prevMapCap]) KV();

      if(prevElems){ u64 cnt = reorder(); }
    }

    return re;
  }
  void*        expand(bool ary=true, bool map=true)
  {
    u64 nxtSz = size();
    if(ary){
      u64 sz = size();
      nxtSz  = sz + sz/2;
      nxtSz  = nxtSz<4? 4 : nxtSz;
    }

    u64 nxtCap = map_capacity();
    if(map){
      u64 cap = map_capacity();
      nxtCap  = cap + cap/2;
      nxtCap  = nxtCap<4? 4 : nxtCap;
    }

    return reserve(nxtSz, nxtCap);
  }
  bool  shrink_to_fit()
  { 
    if( !owned() ){ return false; }
    
    u64       sz = size();
    u64    vecsz = memberBytes() + sz*sizeof(T);
    u64  elemcnt = elems();
    u64    mapsz = elemcnt*sizeof(KV);
    u64  chldCap = child_capacity();
    u64    nxtsz = vecsz + mapsz + chldCap;
    auto prvChld = childData();
    KV const* el = elemStart();
    u8*     nxtp = (u8*)malloc(nxtsz);
    if(nxtp){
      u8*     p = (u8*)memStart();
      memcpy(nxtp, p, vecsz);                                                            // todo: needs to actually use the copy constructor or the assignment operator 
      auto   ff = (fields*)nxtp;
      KV* nxtel = (KV*)(nxtp+vecsz);                                                         // nxtel is next element
      u64   cur = 0;
      TO(map_capacity(),i)
        if(el[i].hsh.type!= HshType::EMPTY){
          nxtel[cur++] = el[i];
        }

      auto  prvF = (fields*)(prvChld);

      tbl<T> prev;
      prev.m_mem = m_mem;                                                                 // make a table to hold the previous span of bytes
      m_mem    = nxtp+memberBytes();                                                      // now this table holds the new span of bytes

      auto f   = memStart();
      f->t     = 't';
      f->b     = 'b';
      f->owned = 1;
      sizeBytes(nxtsz);
      size(sz);
      capacity(sz);
      elems(elemcnt);
      mapcap(elemcnt);

      void* chld = childData();
      byte_move(chld, prvChld, chldCap);                                                 // shouldn't be neccesary because this is using malloc and not realloc()
      auto   fff = (fields*)(chld);
      
      prev.destroy();
      prev.m_mem = nullptr;                                                              // makes destructor early exit on destroy
      
      auto cf = (fields*)( childData() );

      return true;
    }else 
      return false;
  }
  i64            find(const char* key, u32* hash=nullptr) const
  {
    u32   hsh  =  HashStr(key);
    if(hash) *hash = hsh;

    KV*     el  =  (KV*)elemStart();                                   // el is a pointer to the elements 
    u64   cap  =  map_capacity();  
    u64     i  =  hsh;
    u64  wrap  =  hsh % cap - 1;
    u64    en  =  wrap<(cap-1)? wrap : cap-1;                         // clamp to cap-1 for the case that hash==0, which will result in an unsigned integer wrap 
    for(;;++i)
    {
      i %= cap;                                                        // get idx within map_capacity
      HshType eh = el[i].hsh;                                          // eh is element hash
      if(el[i].hsh.type==EMPTY){ 
        return i;
      }else if(hsh == eh.hash){                                        // if the hashes aren't the same, the keys can't be the same
        auto cmp = strncmp(el[i].key, key, sizeof(KV::Key)-1);         // check if the keys are the same 
        if(cmp==0) return i;
      }

      if(i==en) break;                                                 // nothing found and the end has been reached, time to break out of the loop and return a reference to a KV with its type set to NONE
    }
    
    return -1;
  }
  u64           ideal(u64 i) const
  {
    auto el = elemStart();
    if(el[i].hsh.type==EMPTY) return i;
     
    return el[i].hsh.hash % map_capacity();
  }
  u64        distance(u64 i) const { return wrapDist( ideal(i), i, map_capacity() ); }
  i64        holeOfst(u64 i) const
  { // finds the closes hole from an element, but not the furthest hole
    KV const* el = elemStart();
    u64     mod = map_capacity();
    
    i64    h = -1;
    u64 dst = distance(i);
    u64 cnt = 0;
    while(dst >= cnt){ // count can equal distance
      if(el[i].hsh.type==EMPTY) h = i;
      i = prev(i,mod);
      ++cnt;
    }
    return h;
  }
  bool            del(const char* key)
  { 
    u64 i = find(key);
    KV* el = elemStart();
    if(el[i].hsh.type==EMPTY) return false;

    el[i] = KV();
    u64 mapcap = map_capacity();
    u64     en = prev(i, mapcap);

    u64 cnt=0;
    cnt = reorder();
    set_elems( elems()-1 );

    return true;
  }
  void          clear(){ if(m_mem){ destroy(); init(0); } }
  T*            begin(){ return  (T*)m_mem;           }
  T*              end(){ return ((T*)m_mem) + size(); }
  auto        flatten() -> tbl<T>&
  {
    u64   memst = (u64)memStart();
    u64 prevCap = child_capacity();
    u64  newcap = 0;
    auto      e = elemStart();
    auto mapcap = map_capacity();
    TO(mapcap,i)
      if(  (e[i].hsh.type & HshType::TABLE) && 
          !(e[i].hsh.type & HshType::CHILD) ){                                 // if the table bit is set but the child bit is not set
        tbl<T>*  t  =  (tbl<T>*)e[i].val;
        newcap  +=  t->sizeBytes();
    }
    reserve(0,0, prevCap + newcap);
    u64   chldst = (u64)childData();
    u8* curChild = (u8*)chldst + prevCap;
    TO(mapcap,i)
      if(  (e[i].hsh.type & HshType::TABLE) && 
          !(e[i].hsh.type & HshType::CHILD) ){                                 // if the table bit is set but the child bit is not set
        tbl<T>*       t  =  (tbl<T>*)e[i].val;
        auto szbytes  =  t->sizeBytes();

        memcpy(curChild, t->memStart(), szbytes);
        auto   f = (fields*)curChild;
        f->owned = 0;

        e[i].hsh.type  |=  HshType::CHILD;
        e[i].val        =  (u64)curChild - chldst;                             // the memory start will likely have changed due to reallocation
        curChild       +=  szbytes;
      }

    shrink_to_fit();                                                           // todo: do it all at once to avoid extra copying and allocations 

    return *this;
  }

private:
  static const u32 HASH_MASK = 0x07FFFFFF;

  static u64  fnv_64a_buf(void const* const buf, u64 len)
  {
    // const u64 FNV_64_PRIME = 0x100000001b3;
    u64 hval = 0xcbf29ce484222325;    // FNV1_64_INIT;  // ((Fnv64_t)0xcbf29ce484222325ULL)
    u8*   bp = (u8*)buf;	           /* start of buffer */
    u8*   be = bp + len;		           /* beyond end of buffer */

    while(bp < be)                     // FNV-1a hash each octet of the buffer
    {
      hval ^= (u64)*bp++;             /* xor the bottom with the current octet */

      //hval *= FNV_64_PRIME; // does this do the same thing?  /* multiply by the 64 bit FNV magic prime mod 2^64 */
      hval += (hval << 1) + (hval << 4) + (hval << 5) +
              (hval << 7) + (hval << 8) + (hval << 40);
    }
    return hval;
  }
  static u32    HashBytes(const void *const buf, u32 len)
  {
    u64 hsh = fnv_64a_buf(buf, len);
    return (u32)( (hsh>>32) ^ ((u32)hsh));        // fold the 64 bit hash onto itself
  }
  static u32      HashStr(const char* s)
  {
    u32 len = (u32)strlen(s);
    u32 hsh = HashBytes(s, len);
    return hsh & HASH_MASK;
  }

  template<class S> static void swap(S* a, S* b){ S tmp=*a; *a=*b; *b=tmp; }   // here to avoid a depencies
  template<class N> static    N   mx(N a, N b){return a<b?b:a;}
  template<class N> static    N   mn(N a, N b){return a<b?a:b;}

  void byte_move(void* dest, void* src, u64 sz)
  {
    u8*  d = (u8*)dest;
    u8*  s = (u8*)src;
    if(dest==src) return;
    else if(dest < src) TO(sz,i) d[i] = s[i];
    else FROM(sz,i) d[i] = s[i];
  }

public:
  static u64    memberBytes(){ return sizeof(fields); }
  static u64     size_bytes(u64 count)                                  // returns the bytes needed to store the data structure if the same arguments were given to the constructor
  {
    return memberBytes() + sizeof(T)*count;  // todo: not correct yet, needs to factor in map and child data
  }
  static tbl       concat_l(tbl const& a, tbl const& b)                                  // returns the bytes needed to store the data structure if the same arguments were given to the constructor
  {
    auto sz = a.size();
    tbl ret(sz + b.size());
    
    TO(sz,i){        ret[i]    = a[i]; }
    TO(b.size(), i){ ret[sz+i] = b[i]; }

    KV const* el = b.elemStart();
    TO(b.map_capacity(),i) ret( el[i].key ) = el[i];

    el = a.elemStart();
    TO(a.map_capacity(),i) ret( el[i].key ) = el[i];

    return ret;
  }
  static tbl       concat_r(tbl const& a, tbl const& b)                                  // returns the bytes needed to store the data structure if the same arguments were given to the constructor
  {
    auto sz = a.size();
    tbl ret(sz + b.size());
    
    TO(sz,i){        ret[i]    = a[i]; }
    TO(b.size(), i){ ret[sz+i] = b[i]; }

    KV const* el = a.elemStart();
    TO(a.map_capacity(),i){
      if(el[i].hsh.type!=EMPTY) 
        ret( el[i].key ) = el[i];
    }

    el = b.elemStart();
    TO(b.map_capacity(),i){
      if(el[i].hsh.type!=EMPTY) 
        ret( el[i].key ) = el[i];
    }

    return ret;
  }
};


template<class T> KVOfst::operator tbl<T>() 
{   
  if(base){
    tbl<T>   t;                                                                          // type only matters so that c 
    tbl<T> ret;

    //auto  f   = (tbl<T>::fields*)base;
    //t.m_mem   = (u8*)(f+1);
    //auto  fff = (tbl<T>::fields*)(kv->val + (u64)t.childData());

    auto  fff = (tbl<T>::fields*)(kv->val + (u64)base);
    ret.m_mem = (u8*)(fff+1);
    u64     a = ((u64*)ret.m_mem)[0];
    u64     b = ((u64*)ret.m_mem)[1];
    t.m_mem   = nullptr;                                                                 // prevents the destructor running

    return ret;
  }else{
    return *((tbl<T>*)kv->val);
  }
}
template<class T> KVOfst::operator tbl<T>*()
{
  tbl_msg_assert(
    kv->hsh.type == KV::typenum< tbl<T>* >::num, 
    " - tbl TYPE ERROR -\nInternal type: ", 
    HshType::type_str((HshType::Type)kv->hsh.type), 
    "Desired type: ",
    HshType::type_str((HshType::Type)KV::typenum< tbl<T>* >::num) );        

  return (tbl<T>*)kv->val;
}

auto HshType::type_str(Type t) -> char const* const 
{
  switch(t)
  {
    case HshType::ERR:   return "Error";
    case HshType::EMPTY: return "Empty";
    case HshType::NONE:  return  "None";

    case   HshType::U64: return  "u64";
    case   HshType::I64: return  "i64";
    case   HshType::F64: return  "f64";

    case   HshType::cU8: return  "child table  u8";
    case   HshType::cI8: return  "child table  i8";
    case  HshType::cU16: return  "child table  u16";
    case  HshType::cI16: return  "child table  i16";
    case  HshType::cU32: return  "child table  u32";
    case  HshType::cI32: return  "child table  i32";
    case  HshType::cF32: return  "child table  f32";
    case  HshType::cU64: return  "child table  u64";
    case  HshType::cI64: return  "child table  i64";
    case  HshType::cF64: return  "child table  f64";

    case   HshType::tU8: return  "table pointer  u8";
    case   HshType::tI8: return  "table pointer  i8";
    case  HshType::tU16: return  "table pointer  u16";
    case  HshType::tI16: return  "table pointer  i16";
    case  HshType::tU32: return  "table pointer  u32";
    case  HshType::tI32: return  "table pointer  i32";
    case  HshType::tF32: return  "table pointer  f32";
    case  HshType::tU64: return  "table pointer  u64";
    case  HshType::tI64: return  "table pointer  i64";
    case  HshType::tF64: return  "table pointer  f64";

    default: return "Unknown Type";
  }
}


#endif




//tbl_msg_assert(
//  kv->hsh.type == KV::typenum< tbl<T> >::num, 
//  " - tbl TYPE ERROR -\nInternal type: ", 
//  HshType::type_str((HshType::Type)kv->hsh.type), 
//  "Desired type: ",
//  HshType::type_str((HshType::Type)KV::typenum< tbl<T> >::num) );        

//return sizeof(u64) * 5;
//
// Memory Layout (5 u64 variables before m_mem)
// sizeBytes     -  total number of bytes of the entire memory span
// size          -  vector entries
// capacity      -  number of elements already allocated in the vector
// elems         -  number of map entries 
// map_capacity  -  number of elements already allocated for the mapp 

//
// bool ary=true, bool map=true)

//while( (i=compact(i,en,mapcap))!=en ){ ++cnt; } 
//
//printf("\ncount: %d\n", cnt);

//
//u64 idl = ideal(i);

//
//u32   hsh  =  (u32)(*key); // & HASH_MASK );

//memmove(chld, prvChld, chldCap);
//
//auto fff = (fields*)(chld);
//
//KV& kv = el[i];
//KV* e = elemStart();                                                        // now this is the new tabel, not the old one like the call to elemStart() above
//TO(map_capacity(),i){
//  auto type = e[i].hsh.type;
//  if( (type&HshType::TABLE) && (type&HshType::CHILD) )
//    e[i].val = (u64)chld + (e[i].val - (u64)prvChld);
//}
//
//*mapcap_ptr() = elemcnt;

//{
//  if(!m_mem) return nullptr;
//
//  return (KV*)(data() + capacity());
//}

//{
//  if(!m_mem) return nullptr;
//
//  return (KV*)(data() + capacity());
//}

//printf("\n loop count: %d mapcap: %d  ratio: %.4f \n", cnt, mapcap, cnt/(float)mapcap );
//
//auto f = (fields*)prvChld;
//
//auto ff = (fields*)childData();
//
//if(re && fresh){ set_size(0); set_elems(0); }

//if(!m_mem) return 0;
//else       return *mapcap_ptr();

//if(!m_mem) return 0;
//else       return memStart()->mapcap;

//if(this==nullptr || !m_mem) return 0;
//return *( ((u64*)memStart()) + 3);

//
//KVOfst put(const char* key, KV kv){ return *(this->operator()(key).kv) = kv; }

//if(owned() && !map_expand() ) return KVOfst();                                       // not forced here, because kv can't be nullptr - short circuting owned() means that this won't run on a non-owned tbl
// todo: don't return right away, check if the map should be expanded 

//
//for(auto&& n : lst){ emplace(std::forward<T>(n)); }

//{
//  kv   = _kv;
//  base = _base;
//}
//
//template<class N> operator N() { return kv->as<N>(); }

//hsh.type = Type::NONE;
//
//strcpy_s(this->key, sizeof(KV::Key), key);
//hsh.type = NONE;
//
//val          = *((u64*)&castVal);
//
//case  HshType::tU64: return (tu64*)val;
//case  HshType::tI64: return (ti64*)val;
//case  HshType::tF64: return (tf64*)val;
//
//case  HshType::tU64: return cast_ptr<N, tu64>(val);
//case  HshType::tI64: return cast_ptr<N, ti64>(val);
//case  HshType::tF64: return cast_ptr<N, tf64>(val);

//u8 destbits = typenum<N>::num & BITS_MASK;
//u8  srcbits = hsh.type        & BITS_MASK;
//if( destbits > srcbits ){
//}

//case   U8: return cast_mem<N,  u8>(&val);
//case  U16: return cast_mem<N, u16>(&val);
//case  U32: return cast_mem<N, u32>(&val);
//case   I8: return cast_mem<N,  i8>(&val);
//case  I16: return cast_mem<N, i16>(&val);
//case  I32: return cast_mem<N, i32>(&val);
//case  F32: return cast_mem<N, f32>(&val);

//
//template<> struct typenum<tf64>  { static const Type num = HshType::tF8;   };
//template<> struct typenum<tf64>  { static const Type num = HshType::tF16;  };
//
//template<class N> using DES = typenum< typecast<N>::type;

//
//struct  KV;

//using    u8   =   uint8_t;
//using   u16   =  uint16_t;
//using   u32   =  uint32_t;
//using   u64   =  uint64_t;
//using    i8   =    int8_t;
//using   i16   =   int16_t;
//using   i32   =   int32_t;
//using   i64   =   int64_t;
//using   f32   =     float;
//using   f64   =    double;

//enum Type
//{                                                                                      // 10 number types, 10 table variants + empty = 21 total - at least 5 bits needed
//  CHILD      =  1<<5,
//  TABLE      =  1<<4,
//  SIGNED     =  1<<3,
//  INTEGER    =  1<<2,
//  BITS_8     =     0,                    // 2^3 is  8 for  8 bit depth - 0b00
//  BITS_16    =     1,                    // 2^4 is 16 for 16 bit depth - 0b01
//  BITS_32    =  1<<1,                    // 2^5 is 32 for 32 bit depth - 0b10
//  BITS_64    =  1<<1 | 1,                // 2^6 is 64 for 64 bit depth - 0b11
//  BITS_MASK  =  BITS_64,
//  CLEAR      =  BITS_8,
//
//  ERR        =  ~INTEGER & ~SIGNED & ~BITS_32,
//  NONE       =  ~INTEGER & ~SIGNED & ~BITS_16,                                         // ~TABLE,                              // INTEGER bit turned off, SIGNED bit turned off and TABLE bit turned off, meanin unsigned float table, which is not a viable real type of course
//  EMPTY      =  ~INTEGER & ~SIGNED,                                                    // a floating point number can't be unsigned, so this scenario is used for an 'empty' state
//
//  U64        =  INTEGER | BITS_64,
//  I64        =  INTEGER | BITS_64 | SIGNED,
//  F64        =  BITS_64,
//
//  tU64       =  TABLE | INTEGER | BITS_64,
//  tI64       =  TABLE | INTEGER | BITS_64 | SIGNED,
//  tF64       =  TABLE | BITS_64,
//
//  cU64       =  CHILD | tU64,
//  cI64       =  CHILD | tI64,
//  cF64       =  CHILD | tF64
//};

//if(hsh.type==typenum<N>::num) return *((N*)&val);                                    // if the types are the same, return it as the cast directly
//
//if( (hsh.type==HshType::NONE) || (hsh.type==HshType::ERR) ){
//  tbl_msg_assert(
//    hsh.type==typenum<N>::num, 
//    " - tbl TYPE ERROR -\nInternal type: ", 
//    HshType::type_str((Type)hsh.type), 
//    "Desired type: ",
//    HshType::type_str((Type)typenum< typecast<N>::type >::num) );        
//} 
//if( (hsh.type & HshType::SIGNED) && !(typenum<N>::num & HshType::SIGNED)  ){
//  tbl_msg_assert(
//    hsh.type==typenum<N>::num, 
//    " - tbl TYPE ERROR -\nSigned integers can not be implicitly cast to unsigned integers.\nInternal type: ", 
//    HshType::type_str((Type)hsh.type), 
//    "Desired type: ",
//    HshType::type_str((Type)typenum<N>::num) );
//}
//if( !(hsh.type & HshType::INTEGER) && (typenum<N>::num & HshType::INTEGER) ){
//  tbl_msg_assert(
//    hsh.type==typenum<N>::num, 
//    " - tbl TYPE ERROR -\nFloats can not be implicitly cast to integers.\nInternal type: ", 
//    HshType::type_str((Type)hsh.type), 
//    "Desired type: ",
//    HshType::type_str((Type)typenum<N>::num) );
//}
//if( (hsh.type|typenum<N>::num) & HshType::TABLE ){
//  tbl_msg_assert(
//    hsh.type==typenum<N>::num, 
//    " - tbl TYPE ERROR -\nTables can not be implicitly cast, even to a larger bit depth.\nInternal type: ", 
//    HshType::type_str((Type)hsh.type), 
//    "Desired type: ",
//    HshType::type_str((Type)typenum<N>::num) );
//}

//
//ui8   both = hsh.type | typenum<N>::num;              // both bits

////base!=nullptr |, 
//
//tbl_msg_assert(
//  hsh.type == typenum< tbl<T> >::num, 
//  " - tbl TYPE ERROR -\nInternal type was: ", 
//  HshType::type_str((Type)hsh.type), 
//  "Desired  type was: ",
//  HshType::type_str((Type)typenum<N>::num) );        

//operator tf64*()
//{ 
//  //tf64 t;
//  //*((u64*)&t) = val;
//  //return t;
//  tf64* t = (tf64*)val;
//  return t;
//}
//operator tf64&()
//{ 
//  return *((tf64*)val);
//}
