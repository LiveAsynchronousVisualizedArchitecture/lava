

// -todo: solve const qualifiers
// -todo: solve operator()() producing empty keys
// -todo: put magic number in front of tbl 

// todo: cut types down to just u64, i64, double etc
// todo: make a string type using the 8 bytes in the value and the extra bytes of the key 
// | if it exceeds the capacity of the extra key, the make it an offset in the tbl extra space
// | does this imply that there should be a separate array type or is specializing string enough? 
// todo: make operator()() take integers so that the map can also be used as an iterable list through sub-tables
// todo: make operator+=(const char*) create a key in the map
// todo: make operator-=(const char*) delete a key in the map
// todo: work on flattening table - put in table data segment
//       | -change map_capacity to be a variable in memory
//       | tbl segment would be calculated from other variables - internal tbl would be an offset from m_mem
// todo: figure out what happens when doing anything that effects the size of a child tbl - child tbl cannot have it's size changed? need two different internal tbl types, one for references(non owned) and one for children(owned) ?
//       - changing size or deleting an internal tbl means re-making the memory with another allocation and memcpy
// todo: make sure destructor is being run on objects being held once turned into a template
// todo: make constructor that takes only an address to the start of a tbl memory span - just has to offset it by memberBytes()
// todo: make flatten method that has creates a new tbl with no extra capacity and takes all tbl references and makes them into offset/children tbls that are stored in the sub-tbl segment - instead of child type, make a read only type? read only could have template specializations or static asserts that prevent changing the tbl or the KV objects from it
// todo: make non owned type always read only? - still need owned and non-owned types within tbl
// todo: make resize()
// todo: use inline assembly to vectorize basic math operations
// todo: use the restrict keyword on basic math operations 
// todo: put an assert that the two tbl pointers are not the same

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
#include "../no_rt_util.h"

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

class tbl
{
public:                                                                         // forward declarations
  enum   Type;
  struct   KV;

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
  using   T     =       i32;

  static u64 magicNumber(u64 n)
  {
    u8* nn = (u8*)&n;
    nn[7]='t'; nn[8]='b';
    return n;
  }

private:
  template<bool OWNED=true> void typed_del(){ if(m_mem) free(memStart()); };   // explicit template specialization to make an owned tbl free it's memory and a non-owned tbl not free it's memory
  template<> void typed_del<false>(){}; 

  auto      mapcap_ptr()       -> u64*       { return (u64*)memStart() + 4;  }
  auto      mapcap_ptr() const -> u64 const* { return (u64*)memStart() + 4;  }
  void   set_sizeBytes(u64 bytes) // -> u64
  {
    *( (u64*)memStart() ) = magicNumber(bytes);
  }
  void        set_size(u64  size)
  {
    *( ((u64*)memStart()) + 1 ) = size;
  }
  void    set_capacity(u64   cap)
  {
    *( ((u64*)memStart()) + 2) = cap;
  }
  void       set_elems(u64 elems)
  {
    *( ((u64*)memStart()) + 3) = elems;
  }
  void      set_mapcap(u64   cap){ *mapcap_ptr() = cap; }
  u64              nxt(u64 i, u64 mod) const
  {
    return ++i % mod;
  }
  u64             prev(u64 i, u64 mod) const
  {
    if(mod==0) return 0;
    return i==0?  mod-1  :  i-1;
  }
  u64         wrapDist(u64 ideal, u64 idx, u64 mod) const
  {
    if(idx>=ideal) return idx - ideal;
    else return idx + (mod-ideal);
  }
  u64         wrapDist(KV*  elems, u64 idx, u64 mod) const
  {
    u64 ideal = elems[idx].hsh.hash % mod;
    return wrapDist(ideal,idx,mod);
  }
  KV&         place_rh(KV kv, KV* elems, u64 st, u64 dist, u64 mod, u64* placement=nullptr)   // place_rh is place with robin hood hashing 
  {
    u64      i = st;
    u64     en = prev(st,mod);
    u64  eldst = dist;
    KV*     ret = nullptr;
    while(true)
    {
      if(i==en){ return KV::error_kv(); }
      else if(elems[i].hsh.type==EMPTY || kv==elems[i]){
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
  u64          compact(u64 st, u64 en, u64 mapcap)
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
  u64          reorder()
  {
    u64 mod  =  map_capacity(); 
    KV*   el  =  elemStart();
    u64   i  =  0;
    u64  en  =  prev(i,mod);
    u64  cnt =  0; 
    do{
      u64 nxti = nxt(i,mod); 
      if(el[i].hsh.type != EMPTY){
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
  void            init(u64 size)
  {
    u64    szBytes  =  tbl::sizeBytes(size);
    u8*      memst  =  (u8*)malloc(szBytes);                 // memst is memory start
    m_mem           =  memst + memberBytes();
    set_sizeBytes(szBytes);
    set_elems(0);
    set_capacity(size);
    set_size(size);
    *mapcap_ptr() = 0;
  }
  void         destroy()
  {
    // todo: needs to loop through and run destructors here
    //if(m_mem) free(memStart());
    //
    //typed_del<true>::del();
    //
    //typed_del<true>();
    //typed_del<false>();
  }
  void              cp(tbl const& l)
  {
    tbl_PRNT(" copied ");
    //del();
    reserve(l.size(), l.elems());    // todo: can be done with resize, which could use realloc instead of free and malloc?
    //resize(l.size());

    TO(l.size(),i) push(l[i]); 
  }
  void              mv(tbl& r)
  {
    tbl_PRNT("\n moved \n");
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
  enum Type{                                                                           // 10 number types, 10 table variants + empty = 21 total - at least 5 bits needed
    TABLE       =  1<<4,
    SIGNED      =  1<<3,
    INTEGER     =  1<<2,
    BITS_8      =     0,                    // 2^3 is  8 for  8 bit depth
    BITS_16     =     1,                    // 2^4 is 16 for 16 bit depth
    BITS_32     =  1<<1,                    // 2^5 is 32 for 32 bit depth
    BITS_64     =  1<<1 | 1,                // 2^6 is 64 for 64 bit depth
    BITS_MASK   =  BITS_64,

      ERR       =  ~INTEGER & ~SIGNED & ~BITS_32,
     NONE       =  ~INTEGER & ~SIGNED & ~BITS_16,          // ~TABLE,                              // INTEGER bit turned off, SIGNED bit turned off and TABLE bit turned off, meanin unsigned float table, which is not a viable real type of course
    EMPTY       =  ~INTEGER & ~SIGNED,                                       // a floating point number can't be unsigned, so this scenario is used for an 'empty' state

       U8       =  INTEGER | BITS_8,   
      U16       =  INTEGER | BITS_16,
      U32       =  INTEGER | BITS_32,
      U64       =  INTEGER | BITS_64,
       I8       =  INTEGER | BITS_8  | SIGNED,
      I16       =  INTEGER | BITS_16 | SIGNED,
      I32       =  INTEGER | BITS_32 | SIGNED,
      I64       =  INTEGER | BITS_64 | SIGNED,
      F32       =  BITS_32,
      F64       =  BITS_64,

      tU8       =  TABLE | INTEGER | BITS_8,   
     tU16       =  TABLE | INTEGER | BITS_16,
     tU32       =  TABLE | INTEGER | BITS_32,
     tU64       =  TABLE | INTEGER | BITS_64,
      tI8       =  TABLE | INTEGER | BITS_8  | SIGNED,
     tI16       =  TABLE | INTEGER | BITS_16 | SIGNED,
     tI32       =  TABLE | INTEGER | BITS_32 | SIGNED,
     tI64       =  TABLE | INTEGER | BITS_64 | SIGNED,
     tF32       =  TABLE | BITS_32,
     tF64       =  TABLE | BITS_64,
  };
  template<class N> struct typenum { static const Type num = EMPTY; };
  template<> struct typenum<u8>    { static const Type num =    U8; };
  template<> struct typenum<u16>   { static const Type num =   U16; };
  template<> struct typenum<u32>   { static const Type num =   U32; };
  template<> struct typenum<u64>   { static const Type num =   U64; };
  template<> struct typenum<i8>    { static const Type num =    I8; };
  template<> struct typenum<i16>   { static const Type num =   I16; };
  template<> struct typenum<i32>   { static const Type num =   I32; };
  template<> struct typenum<i64>   { static const Type num =   I64; };
  template<> struct typenum<f32>   { static const Type num =   F32; };
  template<> struct typenum<f64>   { static const Type num =   F64; };
  template<> struct typenum<long>  { static const Type num =   I64; };
  template<> struct typenum<unsigned long> { static const Type num = U64; };
  //template<> struct typenum<tui8>  { static const ui8 num = tUI8;  }; 
  //template<> struct typenum<tui16> { static const ui8 num = tUI16; }; 
  //template<> struct typenum<tui32> { static const ui8 num = tUI32; }; 
  //template<> struct typenum<tu64> { static const ui8 num = tu64; }; 
  //template<> struct typenum<ti8>   { static const ui8 num = tI8;   }; 
  //template<> struct typenum<ti16>  { static const ui8 num = tI16;  }; 
  //template<> struct typenum<ti32>  { static const ui8 num = tI32;  }; 
  //template<> struct typenum<ti64>  { static const ui8 num = tI64;  }; 
  //template<> struct typenum<tf32>  { static const ui8 num = tF32;  }; 
  //template<> struct typenum<tf64>  { static const ui8 num = tF64;  }; 

  union   HshType
  {
    struct { Type type : 5; i32 hash: 27; };
    i32 as_ui32;

    HshType() : 
      type(EMPTY),
      hash(0)
    {}
  };
  struct       KV
  {
    template<class DEST, class SRC> 
    static DEST cast_mem(u64 const* const src){ return (DEST)(*((SRC*)src)); }

    using Key = char[51];  // using Key = char[19]; 

    HshType   hsh;
    Key       key;
    u64       val;

    KV() : hsh(), val(0) { memset(key, 0, sizeof(Key)); }
    KV(const char* key, u32 hash, u64 val=0) :
      hsh(), val(val)
    {
      strcpy_s(this->key, sizeof(KV::Key), key);
      hsh.hash = hash;
      hsh.type = NONE;
    }

    bool operator==(KV const& l)
    {
      return hsh.hash==l.hsh.hash && 
             strncmp(l.key,key,sizeof(KV::Key)-1)==0;
    }
    KV&  operator=(KV const& l)
    {
      memmove(this, &l, sizeof(KV));
      return *this;
    }
    template<class N> void operator=(N n)
    {
      hsh.type = typenum<N>::num;
      val      = *((u64*)&n);
    }
    template<class N> operator N()
    { 
      return as<N>();
    }
    template<class N> N as() const
    { 
      if(hsh.type==typenum<N>::num) return *((N*)&val);       // if the types are the same, return it as the cast directly
      
      //ui8   both = hsh.type | typenum<N>::num;              // both bits
      if( (hsh.type & NONE) || (hsh.type & ERROR) ){
        tbl_msg_assert(
          hsh.type==typenum<N>::num, 
          " - tbl TYPE ERROR -\nInternal type was: ", 
          tbl::type_str((Type)hsh.type), 
          "Desired  type was: ",
          tbl::type_str((Type)typenum<N>::num) );        
      } 
      if( (hsh.type & SIGNED) && !(typenum<N>::num & SIGNED)  ){
        tbl_msg_assert(
          hsh.type==typenum<N>::num, 
          " - tbl TYPE ERROR -\nSigned integers can not be implicitly cast to unsigned integers.\nInternal type was: ", 
          tbl::type_str((Type)hsh.type), 
          "Desired  type was: ",
          tbl::type_str((Type)typenum<N>::num) );
      }
      if( !(hsh.type & INTEGER) && (typenum<N>::num & INTEGER) ){
        tbl_msg_assert(
          hsh.type==typenum<N>::num, 
          " - tbl TYPE ERROR -\nFloats can not be implicitly cast to integers.\nInternal type was: ", 
          tbl::type_str((Type)hsh.type), 
          "Desired  type was: ",
          tbl::type_str((Type)typenum<N>::num) );
      }
      if( (hsh.type|typenum<N>::num) & TABLE ){
        tbl_msg_assert(
          hsh.type==typenum<N>::num, 
          " - tbl TYPE ERROR -\nTables can not be implicitly cast, even to a larger bit depth.\nInternal type was: ", 
          tbl::type_str((Type)hsh.type), 
          "Desired  type was: ",
          tbl::type_str((Type)typenum<N>::num) );
      }

      u8 destbits = typenum<N>::num & BITS_MASK;
      u8  srcbits = hsh.type        & BITS_MASK;
      if( destbits > srcbits ){
        switch(hsh.type){
          case   U8: return cast_mem<N,  u8>(&val);
          case  U16: return cast_mem<N, u16>(&val);
          case  U32: return cast_mem<N, u32>(&val);
          case   I8: return cast_mem<N,  i8>(&val);
          case  I16: return cast_mem<N, i16>(&val);
          case  I32: return cast_mem<N, i32>(&val);
          case  F32: return cast_mem<N, f32>(&val);
        }
      }

      tbl_msg_assert(
        hsh.type==typenum<N>::num, 
        " - tbl TYPE ERROR -\nInternal type was: ", tbl::type_str((Type)hsh.type), 
        "Desired  type was: ",                      tbl::type_str((Type)typenum<N>::num) );

      return N();
    }
    bool isEmpty() const { return hsh.type == Type::EMPTY; }

    static KV     empty_kv()
    {
      KV kv;
      memset(&kv, 0, sizeof(KV));
      kv.hsh.type = EMPTY;
      return kv;
    }
    static KV&     none_kv()
    {
      static KV kv;
      //KV kv;
      memset(&kv, 0, sizeof(KV));
      kv.hsh.type = NONE;

      return kv;
    }
    static KV&    error_kv()
    {
      static KV kv;
      //KV kv;
      memset(&kv, 0, sizeof(KV));
      kv.hsh.type = ERR;

      return kv;
    }
  };
  //struct iterator

  u8*     m_mem;
 
  tbl() : m_mem(nullptr) {}
  tbl(u64 size){ init(size); }                                             // have to run default constructor here?
  tbl(u64 size, T const& value)
  {
    init(size);
    TO(size, i){ (*this)[i] = value; }
  }
  ~tbl(){ /*del()*/; }

  tbl(tbl const& l){ cp(l); }
  tbl& operator=(tbl const& l){ cp(l); return *this; }
  tbl(tbl&&      r){ mv(r); }
  tbl& operator=(tbl&&      r){ mv(r); return *this; }

  operator      u64() const{ return size(); }
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
  KV&     operator()(const char* key, bool make_new=true)
  {
    if( !( map_capacity()*0.75f > (float)elems() ) )
       if(!expand()) return KV::error_kv();

    u32  hsh  =  HashStr(key);
    KV*    el  =  (KV*)elemStart();                                    // el is a pointer to the elements 
    u64  mod  =  map_capacity();
    u64    i  =  hsh % mod;
    u64   en  =  prev(i,mod);
    u64 dist  =  0;
    for(;;++i,++dist)
    {
      i %= mod;                                                        // get idx within map_capacity
      HshType eh = el[i].hsh;                                          // eh is element hash
      if(el[i].hsh.type==EMPTY){ 
        set_elems( elems()+1 );
        return el[i] = KV(key, hsh);
      }else if(hsh==eh.hash  && 
        strncmp(el[i].key,key,sizeof(KV::Key)-1)==0 )
      { // if the hashes aren't the same, the keys can't be the same
        return el[i] = KV(key, hsh);
      }else if(dist > wrapDist(el,i,mod) ){
        KV kv(key, hsh);
        set_elems( elems()+1 );
        return place_rh(kv, el, i, dist, mod);
      }
    
      if(i==en) break;                                                 // nothing found and the end has been reached, time to break out of the loop and return a reference to a KV with its type set to NONE
    }

    return KV::error_kv();
  }
  tbl     operator>>(tbl const& l){ return tbl::concat_l(*this, l); }
  tbl     operator<<(tbl const& l){ return tbl::concat_r(*this, l); }
  tbl&    operator--(){ shrink_to_fit(); return *this; }    
  tbl&    operator++(){ expand();        return *this; }
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

  template<class V> KV&   put(const char* key, V val)
  {
    if( !(map_capacity()>elems()) )
       if(!expand()) return KV::error_kv();

    u32  hsh  =  HashStr(key);    
    KV*   el  =  (KV*)elemStart();                                    // el is a pointer to the elements 
    u64    i  =  hsh;
    u64  mod  =  map_capacity();
    u64   en  =  prev(i,mod);
    u64 dist  =  0;
    for(;;++i,++dist)
    {
      i %= mod;                                                        // get idx within map_capacity
      HshType eh = el[i].hsh;                                          // eh is element hash
      if(el[i].hsh.type==EMPTY){ 
        set_elems( elems()+1 );
        return el[i] = KV(key, hsh, *((u64*)&val) );
      }else if(hsh==eh.hash  && 
        strncmp(el[i].key,key,sizeof(KV::Key)-1)==0 )
      { // if the hashes aren't the same, the keys can't be the same
        return el[i] = KV(key, hsh, *((u64*)&val) );
      }else if( wrapDist(el,i,mod) < dist ){
        KV kv(key, hsh, *((u64*)&val) );
        set_elems( elems()+1 );
        return place_rh(kv, el, i, dist, mod);
      }
    
      if(i==en) break;                                                 // nothing found and the end has been reached, time to break out of the loop and return a reference to a KV with its type set to NONE
    }

    return KV::error_kv();
  }

  bool           push(T const& value)
  {
    if(!m_mem){ init(0); }

    auto prevSz = size();
    if( !(capacity()>prevSz) )
      if(!expand()) return false;
    
    //((T*)(this))[size()] = value;

    T* p = (T*)(m_mem);
    *(p + prevSz) = value;

    set_size(prevSz+1);
    //set_size(size()+1);

    return true;
  }
  u64           push(std::initializer_list<T> lst)
  {
    u64 cnt = 0;
    for(auto v : lst){
      bool ok = this->push(v);
      if(!ok){ return cnt; }
      ++cnt;
    }

    return cnt;
  }
  bool      push_back(T const& value){ return push(value); }
  void            pop(){ /*delete &(back());*/ set_size(size()-1); }  // todo: needs to run the destructor here
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

  u64            size() const
  {
    if(!m_mem) return 0;

    return *( ((u64*)memStart()) + 1 );
  }
  T*             data() const
  {
    return (T*)m_mem;
  }
  void*      memStart() const
  {
    return (void*)(m_mem - memberBytes());
  }
  u64       sizeBytes() const // -> u64
  {
    if(!m_mem) return 0;

    u64 sb = *((u64 const*)memStart());
    return sb & 0x0000FFFFFFFFFFFF;
    //return (sb << 2) >> 2;
    //return *( (u64 const*)memStart() );
  }
  u64        capacity() const
  {
    if(!m_mem) return 0;

    return *( ((u64*)memStart()) + 2);

    //if(!m_mem) return 0;
    //
    //return (sizeBytes() - memberBytes()) / sizeof(T);  // - size()*sizeof(T)
  }
  u64           elems() const
  {
    if(!m_mem) return 0;

    return *( ((u64*)memStart()) + 3);
  }
  u64    map_capacity() const
  {
    if(!m_mem) return 0;
    else       return *mapcap_ptr();
  }
  auto      elemStart() -> KV* 
  {
    if(!m_mem) return nullptr;

    return (KV*)(data() + capacity());
  }
  auto      elemStart() const -> KV const* 
  {
    if(!m_mem) return nullptr;

    return (KV*)(data() + capacity());
  }
  void*       reserve(u64 count, u64 mapcap)
  {
    count   =  mx(count, capacity());
    mapcap  =  mx(mapcap, map_capacity());
    KV*    prevElems  =  elemStart();
    u64   prevMemSt  =  (u64)memStart();
    u64    prevOfst  =  prevElems? ((u64)prevElems)-prevMemSt  :  0;
    u64   prevBytes  =  sizeBytes();
    u64  prevMapCap  =  map_capacity();
    u64    nxtBytes  =  memberBytes() + sizeof(T)*count +  sizeof(KV)*mapcap;
    void*        re;
    bool      fresh  = !m_mem;
    if(fresh) re = malloc(nxtBytes);
    else      re = realloc(memStart(), nxtBytes);

    if(re){
      m_mem = ((u8*)re) + memberBytes();
      set_sizeBytes(nxtBytes);
      set_capacity(count);
      set_mapcap(mapcap);

      KV* el = elemStart();
      if(prevElems){
        KV* prevEl = (KV*)( ((u8*)re) + prevOfst );
        if(el!=prevEl) TO(prevMapCap,i) el[i] = prevEl[i];
      }

      //u64  mapcap = mapcap; //map_capacity();
      i64   extcap = mapcap - prevMapCap;
      if(extcap>0) 
        TO(extcap,i) 
          el[i+prevMapCap] = KV();
          //new (&el[i+prevMapCap]) KV();

      if(prevElems)
      { 
        u64 cnt = reorder();
        //printf("\n loop count: %d mapcap: %d  ratio: %.4f \n", cnt, mapcap, cnt/(float)mapcap );
      }
    }

    if(re && fresh){ set_size(0); set_elems(0); }
    
    return re;
  }
  void*        expand()
  {
    u64    sz = size();
    u64 nxtSz = sz + sz/2;
    nxtSz      = nxtSz<8? 8 : nxtSz;

    u64    cap = map_capacity();
    u64 nxtCap = cap + cap/2;
    nxtCap      = nxtCap<4? 4 : nxtCap;

    return reserve(nxtSz, nxtCap);
  }
  bool  shrink_to_fit()
  {
    u64      sz = size();
    u64   vecsz = memberBytes() + sz*sizeof(T);
    u64 elemcnt = elems();
    u64   mapsz = elemcnt*sizeof(KV);
    u64   nxtsz = vecsz + mapsz;
    
    KV const* el = elemStart();
    u8* nxtp = (u8*)malloc(nxtsz);
    if(nxtp){
      u8* p = (u8*)memStart();
      memcpy(nxtp, p, vecsz);
      KV* nxtel = (KV*)(nxtp+vecsz);
      u64  cur = 0;
      TO(map_capacity(),i){
        if(el[i].hsh.type!=EMPTY) 
          nxtel[cur++] = el[i];
        } 

      free(p);
      m_mem = nxtp+memberBytes();
      set_sizeBytes(nxtsz);
      set_size(sz);
      set_capacity(sz);
      set_elems(elemcnt);
      *mapcap_ptr() = elemcnt;

      //reorder();

      return true;
    }else 
      return false;
  }
  i64            find(const char* key, u32* hash=nullptr) const
  {
    u32   hsh  =  HashStr(key);
    //u32   hsh  =  (u32)(*key); // & HASH_MASK );
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
  u64        distance(u64 i) const
  {
    //u64 idl = ideal(i);
    return wrapDist( ideal(i), i, map_capacity() );
  }
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
    //while( (i=compact(i,en,mapcap))!=en ){ ++cnt; } 
    cnt = reorder();
    set_elems( elems()-1 );

    //printf("\ncount: %d\n", cnt);

    return true;
  }
  u64             del(Type t)
  {
    u64 mapcap = map_capacity();
    KV*      el = elemStart();
    u64    cnt = 0;
    TO(mapcap,i) 
      if(el[i].hsh.type==t){ 
        el[i] = KV();
        ++cnt;
      }
    reorder();

    return cnt;
  }

private:
  static const u32 HASH_MASK = 0x07FFFFFF;

  static u64  memberBytes()
  {
    return sizeof(u64) * 5;
    // Memory Layout (5 u64 variables before m_mem)
    // sizeBytes     -  total number of bytes of the entire memory span
    // size          -  vector entries
    // capacity      -  number of elements already allocated in the vector
    // elems         -  number of map entries 
    // map_capacity  -  number of elements already allocated for the mapp 
  }
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

  template<class S> static void swap(S* a, S* b) 
  { // here to avoid a depencies
    S tmp = *a;
    *a    = *b;
    *b    = tmp;
  }
  template<class N> static    N   mx(N a, N b){return a<b?b:a;}
  template<class N> static    N   mn(N a, N b){return a<b?a:b;}
public:
  static auto     type_str(Type t) -> char const* const 
  {
    switch(t)
    {
    case ERROR: return "Error";
    case EMPTY: return "Empty";
    case  NONE: return  "None";
    
    case    U8: return   "u8";
    case   U16: return  "u16";
    case   U32: return  "u32";
    case   U64: return  "u64";
    case    I8: return   "i8";
    case   I16: return  "i16";
    case   I32: return  "i32";
    case   I64: return  "i16";
    case   F32: return  "f32";
    case   F64: return  "f64";

    case   tU8: return  "table   u8";
    case  tU16: return  "table  u16";
    case  tU32: return  "table  u32";
    case  tU64: return  "table  u64";
    case   tI8: return  "table   i8";
    case  tI16: return  "table  i16";
    case  tI32: return  "table  i32";
    case  tI64: return  "table  i16";
    case  tF32: return  "table  f32";
    case  tF64: return  "table  f64";

    default: return "Unknown Type";
    }
    //return "Empty";
  }
  static u64     sizeBytes(u64 count)                                  // returns the bytes needed to store the data structure if the same arguments were given to the constructor
  {
    return memberBytes() + sizeof(T)*count;
  }
  static tbl      concat_l(tbl const& a, tbl const& b)                                  // returns the bytes needed to store the data structure if the same arguments were given to the constructor
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
  static tbl      concat_r(tbl const& a, tbl const& b)                                  // returns the bytes needed to store the data structure if the same arguments were given to the constructor
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

#endif




//u64    el = elems();
//u64 nxtEl = el + el/2;
//nxtEl      = nxtEl<4? 4 : nxtEl;

//
//operator      bool() const{ return m_mem!=nullptr; }

//KV(const char* key, u32 hash) :
//  hsh(), val(0)
//{
//  strcpy_s(this->key, sizeof(KV::Key), key);
//  hsh.hash = hash;
//  hsh.type = NONE;
//}

//template<class V> KV(const char* key, ui32 hash, V val)
//{
//  *this = val;
//  strcpy_s(this->key, sizeof(KV::Key), key);
//  hsh.hash = hash;
//}

