
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
// -todo: make vector expand to 8 elements minimum
// -todo: make 1 bit always indicate signed, 1 bit always indicate table, 1 bit indicate integer, and 2 bits indicate the bit depth as 3,4,5, or 6 - same 5 bits as discreet 21 types if unsigned & not integer is used for empty!
// -todo: re-test type assertions after structuring enum bits
// -todo: make variant structure
// -todo: make enum with number types and table-number types
// -todo: copy in hash function from simdb
// -todo: make elems() the number of elements in the map
// -todo: change reserve to use both size and elems 
// -todo: start on map
// -todo: store needed map data on heap - map elements and ????
// -todo: test operator()
// -todo: fix hash comparison check
// -todo: make operator() access the map
// -todo: check for a full loop around of a hash map search
// -todo: return a reference to a NONE KV if there was no space and the map search had to loop around to the end
// -todo: fix reference return to just copy the key when the key is empty and not create a new KV
// -todo: increase elems and mark type as a default (I32?) instead of EMPTY
// -todo: add type string for NONE
// -todo: make kv have implicit casts to the different number types
// -todo: make another enum type for ERROR?
// -todo: make has() function or STL equivilent - test has()
// -todo: make insert() function 
// -todo: turn operator() into a template - not needed because the reference to kv use the templated operator=
// -todo: turn insert() into a template
// -todo: test types to make sure EMPTY and NONE and ERROR are not duplicated
// -todo: make all elements initialize to zeroed out with EMPTY enum set - just need EMPTY to be default constructor
// -todo: make operator() return type ERROR if space isn't found 
// -todo: make a table hold any type, but a map hold only numbers and table-number types
// -todo: make key length a constant so that key functions that work on C strings can be limited in length
// -todo: make a double cast template function - template<class FROM, class TO> 
// -todo: figure out casting from integer to float but not float to integer - just check for float to integer scenario
// -todo: make switch statement to have flexible number casts (a ui8 can be cast without error to a ui32)

// todo: make constructor with default value
// todo: make reserve rehash and reinsert all elements
// todo: test putting more than 8 elements into map
// todo: test KV with key string above character length  
// todo: fold HashType into KV? - would have to make an internal struct/union?
// todo: give KV a default constructor and copy constructor so that the c_str key is copied?
// todo: make emplace and emplace_back()
// todo: make resize()
// todo: make copy use resize
// todo: revisit concactenation to make sure map elems are copied
// todo: make sure destructor is being run on objects being held once turned into a template
// todo: make shrink_to_fit()
// todo: make begin and end iterators to go with C++11 for loops - loop through keys value pairs?
// todo: robin hood hashing
// todo: make visualization for table as a tree that shows arrays, key-values and their types, then sub tables - make various visualizations for arrays - histogram, graph, ???
// todo: try template constructor that returns a tbl<type> with a default value set? - can't have a constructor that reserves elems with this? would need a reserve_elems() function?
// todo: make reserve_elems() function
// todo: add ability to use a function in the template type declaration to give a custom accessor by making operator() a variadic template?
// todo: use binary bit operators to make tbl act like a set?
// todo: make operator~ return just the vector
// todo: make different unary operator return just the map?
// todo: make operator-- be shrink_to_fit() and ++ be expand() ?


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

class tbl
{
public:                                                                       // forward declarations
  enum Type;
  struct KV;

private:
  void   set_sizeBytes(ui64 bytes) // -> ui64
  {
    *( (ui64*)memStart() ) = bytes;
  }
  void        set_size(ui64  size)
  {
    *( ((ui64*)memStart()) + 1 ) = size;
  }
  void    set_capacity(ui64   cap)
  {
    *( ((ui64*)memStart()) + 2) = cap;
  }
  void       set_elems(ui64 elems)
  {
    *( ((ui64*)memStart()) + 3) = elems;
  }

  void*       memStart()
  {
    return (void*)(m_mem - memberBytes());
  }
  KV*        elemStart()
  {
    return (KV*)(data() + capacity());
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
    reserve(l.size(), l.elems());    // todo: can be done with resize, which could use realloc instead of free and malloc?
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
  enum Type{                                                                           // 10 number types, 10 table variants + empty = 21 total - at least 5 bits needed
    TABLE       =  1<<4,
    SIGNED      =  1<<3,
    INTEGER     =  1<<2,
    BITS_8      =     0,                    // 2^3 is  8 for  8 bit depth
    BITS_16     =     1,                    // 2^4 is 16 for 16 bit depth
    BITS_32     =  1<<1,                    // 2^5 is 32 for 32 bit depth
    BITS_64     =  1<<1 | 1,                // 2^6 is 64 for 64 bit depth
    BITS_MASK   =  BITS_64,

    ERROR       =  ~INTEGER & ~SIGNED & ~BITS_32,
     NONE       =  ~INTEGER & ~SIGNED & ~BITS_16,          // ~TABLE,                              // INTEGER bit turned off, SIGNED bit turned off and TABLE bit turned off, meanin unsigned float table, which is not a viable real type of course
    EMPTY       =  ~INTEGER & ~SIGNED,                                       // a floating point number can't be unsigned, so this scenario is used for an 'empty' state

      UI8       =  INTEGER | BITS_8,   
     UI16       =  INTEGER | BITS_16,
     UI32       =  INTEGER | BITS_32,
     UI64       =  INTEGER | BITS_64,
       I8       =  INTEGER | BITS_8  | SIGNED,
      I16       =  INTEGER | BITS_16 | SIGNED,
      I32       =  INTEGER | BITS_32 | SIGNED,
      I64       =  INTEGER | BITS_64 | SIGNED,
      F32       =  BITS_32,
      F64       =  BITS_64,

     tUI8       =  TABLE | INTEGER | BITS_8,   
    tUI16       =  TABLE | INTEGER | BITS_16,
    tUI32       =  TABLE | INTEGER | BITS_32,
    tUI64       =  TABLE | INTEGER | BITS_64,
      tI8       =  TABLE | INTEGER | BITS_8  | SIGNED,
     tI16       =  TABLE | INTEGER | BITS_16 | SIGNED,
     tI32       =  TABLE | INTEGER | BITS_32 | SIGNED,
     tI64       =  TABLE | INTEGER | BITS_64 | SIGNED,
     tF32       =  TABLE | BITS_32,
     tF64       =  TABLE | BITS_64,
  };
  template<class N> struct typenum { static const Type num = EMPTY; };
  template<> struct typenum<ui8>   { static const Type num =   UI8; }; 
  template<> struct typenum<ui16>  { static const Type num =  UI16; }; 
  template<> struct typenum<ui32>  { static const Type num =  UI32; }; 
  template<> struct typenum<ui64>  { static const Type num =  UI64; }; 
  template<> struct typenum<i8>    { static const Type num =    I8; }; 
  template<> struct typenum<i16>   { static const Type num =   I16; }; 
  template<> struct typenum<i32>   { static const Type num =   I32; }; 
  template<> struct typenum<i64>   { static const Type num =   I64; }; 
  template<> struct typenum<f32>   { static const Type num =   F32; }; 
  template<> struct typenum<f64>   { static const Type num =   F64; }; 
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
    struct { Type type : 5; ui32 hash: 27; };
    ui32 as_ui32;

    HshType() : 
      type(EMPTY),
      hash(0)
    {}
  };
  struct       KV
  {
    template<class DEST, class SRC> 
    static DEST cast_mem(ui64* src){ return (DEST)(*((SRC*)src)); }

    using Key = char[19]; 

    HshType   hsh;
    Key       key;
    ui64      val;

    KV& operator=(KV const& l)
    {
      memcpy(this, &l, sizeof(KV));
      return *this;
    }
    template<class N> void operator=(N n)
    {
      hsh.type = typenum<N>::num;
      val      = *((ui64*)&n);
    }
    template<class N> operator N()
    { 
      return as<N>();
    }
    template<class N> N as()
    { 
      if(hsh.type==typenum<N>::num) return *((N*)&val);    // if the types are the same, return it as the cast directly
      
      ui8   both = hsh.type | typenum<N>::num;              // both bits

      if( (both & TABLE) ){
        tbl_msg_assert(
          hsh.type==typenum<N>::num, 
          " - tbl TYPE ERROR -\nTables can not be implicitly cast, even to a larger bit depth.\nInternal type was: ", 
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

      ui8 destbits = typenum<N>::num & BITS_MASK;
      ui8  srcbits = hsh.type        & BITS_MASK;
      if( destbits > srcbits ){
        switch(hsh.type){
          case  UI8: return cast_mem<N, ui8>(&val);
          case UI16: return cast_mem<N,ui16>(&val);
          case UI32: return cast_mem<N,ui32>(&val);
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
      kv.hsh.type = ERROR;

      return kv;
    }
  };

  using T    =  int;

  i8*     m_mem;
 
  tbl() : 
    m_mem(nullptr)
  {
  }
  tbl(ui64 size)                                 // have to run default constructor here?
  {    
    ui64   szBytes  =  tbl::sizeBytes(size);
    i8*      memst  =  (i8*)malloc(szBytes);                 // memst is memory start
    m_mem           =  memst + memberBytes();
    set_sizeBytes(szBytes);
    set_elems(0);
    set_capacity(size);
    set_size(size);
  }
  tbl(ui64 size, int const& value)
  {

  }
  ~tbl(){ del(); }

  tbl(tbl const& l){ cp(l); }
  tbl& operator=(tbl const& l){ cp(l); return *this; }
  tbl(tbl&&      r){ mv(r); }
  tbl& operator=(tbl&&      r){ mv(r); return *this; }

  operator      ui64() const { return size(); }
  operator      bool() const { return m_mem!=nullptr; }
  T&      operator[](ui64 i){ return ((T*)m_mem)[i]; }
  auto    operator[](ui64 i) const -> T const& { return ((T*)m_mem)[i]; }
  KV&     operator()(const char* key, bool make_new=true)              // todo: just needs to return a kv reference since the kv can be set with operator=() ?
  {
    if( !(map_capacity()>elems()) )
      if(!expand()) return KV::error_kv();

    ui32   hsh  =  HashStr(key);
    KV*     el  =  (KV*)elemStart();                                   // el is a pointer to the elements 
    ui64   cap  =  map_capacity();  
    ui64     i  =  hsh;
    ui64  wrap  =  hsh % cap - 1;
    ui64    en  =  wrap<(cap-1)? wrap : cap-1;                         // clamp to cap-1 for the case that hash==0, which will result in an unsigned integer wrap 
    for(;;++i)
    {
      i %= cap;                                                        // get idx within map_capacity
      HshType eh = el[i].hsh;                                          // eh is element hash
      if(el[i].hsh.type==EMPTY){ 

        if(make_new){
          strcpy_s(el[i].key, sizeof(KV::Key), key);
          el[i].hsh.hash = hsh;
          el[i].hsh.type = NONE;                                       // this makes this slot no longer empty. When an assignment occurs it will overwrite this type.
          set_elems( elems()+1 );
        }
        return el[i];
      }else if(hsh == eh.hash){                                        // if the hashes aren't the same, the keys can't be the same
        auto cmp = strncmp(el[i].key, key, sizeof(KV::Key)-1);         // check if the keys are the same 
        if(cmp==0) return el[i];
      }

      if(i==en) break;                                                 // nothing found and the end has been reached, time to break out of the loop and return a reference to a KV with its type set to NONE
    }
    
    return KV::error_kv();
  }
  tbl     operator>>(tbl const& l){ return tbl::concat(*this, l); }
  tbl     operator<<(tbl const& l){ return tbl::concat(*this, l); }

  bool          push(T const& value)
  {
    if( !(capacity()>size()) )
      if(!expand()) return false;
    
    (*this)[size()] = value;

    set_size(size()+1);

    return true;
  }
  bool     push_back(T const& value){ return push(value); }
  void           pop(){ /*delete &(back());*/ set_size(size()-1); }  // todo: needs to run the destructor here
  void      pop_back(){ pop(); }
  T&           front(){ return (*this)[0]; }
  T&            back(){ return (*this)[size()-1]; }

  template<class N> bool insert(const char* key, N const& val)
  {
    KV& kv = (*this)(key, true);
    if(kv.hsh.type==ERROR) return false;

    kv = val;
    return true;
  }
  bool           has(const char* key)
  {
    KV& kv = (*this)(key, false);
    return kv.hsh.type != EMPTY;
  }

  ui64          size() const
  {
    if(!m_mem) return 0;

    return *( ((ui64*)memStart()) + 1 );
  }
  T*            data() const
  {
    return (T*)m_mem;
  }
  void*     memStart() const
  {
    return (void*)(m_mem - memberBytes());
  }
  ui64     sizeBytes() const // -> ui64
  {
    if(!m_mem) return 0;

    return *( (ui64 const*)memStart() );
  }
  ui64      capacity() const
  {
    if(!m_mem) return 0;

    return *( ((ui64*)memStart()) + 2);

    //if(!m_mem) return 0;
    //
    //return (sizeBytes() - memberBytes()) / sizeof(T);  // - size()*sizeof(T)
  }
  ui64         elems() const
  {
    if(!m_mem) return 0;

    return *( ((ui64*)memStart()) + 3);
  }
  ui64  map_capacity() const
  {
    if(!m_mem) return 0;

    return (sizeBytes()-memberBytes()-capacity()*sizeof(T)) / sizeof(KV);
  }
  void*      reserve(ui64 size, ui64 elems)
  {
    auto  nxtBytes  =  memberBytes() + sizeof(T)*size + sizeof(KV)*elems;
    void*       re;
    bool     fresh  = !m_mem;
    if(fresh) re = malloc(nxtBytes);
    else      re = realloc(memStart(), nxtBytes);

    if(re){
      m_mem = ((i8*)re) + memberBytes();
      set_sizeBytes(nxtBytes);
      set_capacity(size);

      // todo: take this out so that hashes aren't wiped
      KV* el = elemStart();
      TO(elems, i) el[i] = KV();
    }

    if(re && fresh){ set_size(0); set_elems(0); }
    
    return re;
    
    //set_sizeBytes(nxtBytes);
  }
  void*       expand()
  {
    //ui64 nxtSz = (sz/2)? sz+sz/2 : sz+1;

    ui64    sz = size();
    ui64 nxtSz = sz + sz/2;
    nxtSz      = nxtSz<8? 8 : nxtSz;

    ui64    el = elems();
    ui64 nxtEl = el + el/2;
    nxtEl      = nxtEl<8? 8 : nxtEl;

    return reserve(nxtSz, nxtEl);
  }

private:
  static const ui32 HASH_MASK = 0x07FFFFFF;

  static ui64  memberBytes()
  {
    return sizeof(ui64) * 4;
  }
  static ui64  fnv_64a_buf(void const* const buf, ui64 len)
  {
    // const ui64 FNV_64_PRIME = 0x100000001b3;
    ui64 hval = 0xcbf29ce484222325;    // FNV1_64_INIT;  // ((Fnv64_t)0xcbf29ce484222325ULL)
    ui8*   bp = (ui8*)buf;	           /* start of buffer */
    ui8*   be = bp + len;		           /* beyond end of buffer */

    while(bp < be)                     // FNV-1a hash each octet of the buffer
    {
      hval ^= (ui64)*bp++;             /* xor the bottom with the current octet */

      //hval *= FNV_64_PRIME; // does this do the same thing?  /* multiply by the 64 bit FNV magic prime mod 2^64 */
      hval += (hval << 1) + (hval << 4) + (hval << 5) +
              (hval << 7) + (hval << 8) + (hval << 40);
    }
    return hval;
  }
  static ui32    HashBytes(const void *const buf, ui32 len)
  {
    ui64 hsh = fnv_64a_buf(buf, len);

    return (ui32)( (hsh>>32) ^ ((ui32)hsh));        // fold the 64 bit hash onto itself
  }
  static ui32      HashStr(const char* s)
  {
    ui32 len = (ui32)strlen(s);
    ui32 hsh = HashBytes(s, len);
     
    return hsh & HASH_MASK;
  }
public:
  static auto     type_str(Type t) -> char const* const 
  {
    switch(t)
    {
    case  NONE: return  "None";
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

    case  tUI8: return  "table  ui8";
    case tUI16: return  "table ui16";
    case tUI32: return  "table ui32";
    case tUI64: return  "table ui64";
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
  static ui64    sizeBytes(ui64 count)                                  // returns the bytes needed to store the data structure if the same arguments were given to the constructor
  {
    return memberBytes() + sizeof(T)*count;
  }
  static tbl        concat(tbl const& a, tbl const& b)                                  // returns the bytes needed to store the data structure if the same arguments were given to the constructor
  {
    auto sz = a.size();
    tbl ret(sz + b.size());
    
    TO(sz,i) ret[i]    = a[i];
    TO(b, i) ret[sz+i] = b[i];

    return ret;
  }
};

#endif







//tbl_msg_assert(
//  hsh.type==typenum<N>::num, 
//  " - tbl TYPE ERROR -\nInternal type was: ", tbl::type_str((Type)hsh.type), 
//  "Desired  type was: ",                      tbl::type_str((Type)typenum<N>::num) );
//
//return *((N*)&val);

//ui8 typnum = typenum<N>::num;
//ui8    shr = hsh.type & typnum;    // the shared bits
//ui8    dif = hsh.type ^ typnum;    // the different bits
//
//if(dif & TABLE)   /*error*/ ;                        // don't cast a table, just error if the types are different 
//if(dif & INTEGER) /*error*/ ;                        // 

//KV(){}
//
//static KV KV_NONE; // = KV::none_kv();
//
//tbl::KV tbl::KV_NONE = tbl::KV::none_kv();

//if( !(map_capacity()>elems()) )
//  if(!expand()) return false;
//
//HshType ht;                                   // ht is hash type
//ht.hash  =  HashStr(key);
//ht.type  =  typenum<decltype(val)>::num;      // todo: need to remove const and reference here?
//
//KV  kv;
//kv       =  val;
//kv.hsh   =   ht;
//strcpy_s(kv.key, sizeof(kv.key), key);
//
//// get start of elems
//KV* elems  =  (KV*)elemStart();
//ui64  cap  =  map_capacity();  
//ui64    i  =  ht.hash;  
//for(;;++i)
//{
//  i %= cap;                                                         // get idx within map_capacity
//  HshType eh = elems[i].hsh;                                        // eh is element hash
//  if(elems[i].hsh.type==EMPTY){ 
//    elems[i] = kv; 
//    return true;
//  }else{                                                            // check if the keys are the same 
//    if(ht.hash!=eh.hash) continue;                                  // if the hashes aren't the same, the keys can't be the same
//    
//    auto cmp = strncmp(elems[i].key, kv.key, sizeof(KV::Key)-1);
//    if(cmp!=0) continue;
//    
//    elems[i] = kv;
//    return true;
//  }
//}
//
//return true;

//HshType ht;                                   // ht is hash type
//ht.hash  =  HashStr(key);
//ht.type  =  NONE; // typenum<decltype(val)>::num;      // todo: need to remove const and reference here?

//KV  kv;
////kv       =  val;
//kv.hsh.hash  =  HashStr(key);
//strcpy_s(kv.key, sizeof(kv.key), key);

//KV kv;
//kv.hsh.type = NONE;

//struct      Var
//{
//  ui8   type;
//  ui64  data; 
//};
//using var  =  Var;

//m_size(count),
//ui64  cntBytes  =  count*sizeof(T);
//ui64   szBytes  =  cntBytes + memberBytes();             // sizeof(ui64)*2;

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

//POWEROF2_3  =     0,                    // 2^3 is  8 for  8 bit depth
//POWEROF2_4  =     1,                    // 2^4 is 16 for 16 bit depth
//POWEROF2_5  =  1<<1,                    // 2^5 is 32 for 32 bit depth
//POWEROF2_6  =  1<<1 | 1,                // 2^6 is 64 for 64 bit depth
//
//tUI8, tI8, tUI16, tI16, tUI32, tI32, tUI64, tI64, tF32, tF64

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
