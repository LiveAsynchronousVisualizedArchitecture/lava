
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
// -todo: make signed to unsigned debug catch for implicit casts
// -todo: make constructor with default value
// -todo: specialize del() function for being owned or not - template specialization of functions seems to work well
// -todo: make sure an owned tbl from a non-owned tbl copies, but a non-owned from a non-owned does not? - forget this and always copy unless there is a move?
// -todo: make operator+= 
// -todo: test operator+ with mismatched sizes
// -todo: fill out other numeric and binary operators
// -todo: make numeric operators that take a single right side number
// -todo: put in assert to check for bounds errors on debug
// -todo: make bounds checks during debug
// -todo: specialize offset type somehow?  tbl inside a tbl is given by being an offset/child and owned type. the tbl then gives up a reference/non-owned type that is not a child/offset type  - not neccesary with owned and non-owned tables
// -todo: make operator[] and operator() call template specialized functions that jump by an offset if they are a child type? - shouldn't be neccesary, because acessing a child tbl can just construct a reference type each time, using the held offset to the tbl children segment (!) - not neccesary with owned and non-owned tables
// -todo: make reserve rehash and reinsert all elements - works?
// -todo: factor in wrap aroun
// -todo: debug expand()  /  reserve with more vector capacity - not vector capacity, but mapcap with 6 instead of 4 causing problems?
// -todo: test putting more than 8 elements into map - seems broken - debug 5th element insertion - is it reserve or the collision that is the problem? - expand is the problem? it enlarges the capacity AND map_cap?
// -todo: make error messages for ERROR and NONE
// -todo: fix comapact_all removing elements - empty index wasn't being set to the current index when moving an element
// -todo: resolve 'two' conflict with duplicate keys - have to make sure each element has not empty space between its ideal position and its final position
// -todo: split operator() into a find function that returns the index of the element with the given key
// -todo: make a compact function that searches backwards to move and element back as far as possible?
// -todo: debug duplicate entries - possibly because of expand() being run
// -todo: make comapct function increment empty index instead of setting it to the moved index? - instead, make move element as far back as possible
// -todo: make a del(const char* key) function that sets the type to empty, then moves back elements until either an empty space or an element aleady in its ideal position is found  
// -todo: make function to detect holes between a key and its empty position
// -todo: try using operator() instead of place_kv
// -todo: with robin-hood hashing, would it be possible to re-insert elements, then deal with them in spans? - no because all movement creates holes
// -todo: make placement loop reset the end point and always loop to the end index
// -todo: make function to move an index forward or backward? - not needed with rolling placement reordering
// -todo: make a function to remove any holes between an element and its ideal position
// -todo: make function to find spans of non-empty blocks, sort them and move them - not needed
// -todo: simplify test case and debug duplicates in concatenation - should reserve use a del() function combined with reinsertion? 
// -todo: revisit concactenation to make sure map elems are copied
// -todo: try reserve again - without recursion? - if curKV's ideal idx is the same, loop on the next idx. - problem, robin hood hashing would mean swapping if curKV's ideal is larger than the element at i, but if an element hasn't been moved, its distance to ideal could be large, meaning it wouldn't move even though it should 
//       | solution: don't do robbin hood hasing at first, just make sure all the elements that hash to the same bucket are linearly packed
//       |   problem: are there scenarios that will trigger infinite recursion? - yes, if only comparing ideal indices
//       |   problem: moving an element creates a hole - does that mean the elements in front should be moved back if it would make them closer to their ideal index?
//       |     solution: make a delete function that removes a key and checks elements forward to see if moving them one back would get them closer to their ideal position
//       |     solution: make a compact function that scans through looking for empty spaces and moves elements backwards if it would be more ideal - make the function start at an index and go until it hits an element already in its ideal position, returning the number of elements moved
// -todo: robin hood hashing
// -todo: make put into a template

// todo: change place_rh to return reference
// todo: change operator() to use put
// todo: change concatenation to use put()
// todo: make a KV constructor that takes a key, hash and a value
// todo: make rolling reshuffle and put into delete function
// todo: make KVRef struct with a *this ptr and an index - make internal and external KV structs, with the external having a this ptr that can modify the table?
// todo: test KV with key string above character length  
// todo: give KV a default constructor and copy constructor so that the c_str key is copied?
// todo: make resize()
// todo: make copy use resize? - should it just be a memcpy?
// todo: make sure destructor is being run on objects being held once turned into a template
// todo: make shrink_to_fit()
// todo: make begin and end iterators to go with C++11 for loops - loop through keys value pairs?
// todo: make visualization for table as a tree that shows arrays, key-values and their types, then sub tables - make various visualizations for arrays - histogram, graph, ???
// todo: try template constructor that returns a tbl<type> with a default value set? - can't have a constructor that reserves elems with this? would need a reserve_elems() function?
// todo: make constructor that takes only an address to the start of a tbl memory span - just has to offset it by memberBytes()
// todo: figure out what happens when doing anything that effects the size of a child tbl - child tbl cannot have it's size changed? need two different internal tbl types, one for references(non owned) and one for children(owned) ?
// todo: make flatten method that has creates a new tbl with no extra capacity and takes all tbl references and makes them into offset/children tbls that are stored in the sub-tbl segment - instead of child type, make a read only type? read only could have template specializations or static asserts that prevent changing the tbl or the KV objects from it
// todo: make non owned type always read only? - still need owned and non-owned types within tbl
// todo: make function to delete keys with NONE type? set NONE to empty?
// todo: fold HashType into KV? - would have to make an internal struct/union? - just change HashType hsh to HashType ht?
// todo: keep the max distance through every loop and use that to short-circuit the reorder loop? - have place_kv return the the distance from ideal in a separate variable, keep track of the index... is there a way to take the max distance back down? does it need to go back down? does the end point only need to be prevMapcap elements forward? - no because there could still be holes?

// todo: is an optimization possible where the index of the largest distance from ideal position is kept so the loop doesn't have to go all the way back around? - no because moving the largest distance means the next largest would have to be found again 
// todo: is it possible to deal with spans of keys that go to the same bucket? would it be more efficient?
// todo: make reserve_elems() function or reserve_map() function? - if reserve 
// todo: use binary bit operators to make tbl act like a set?
// todo: make operator~ return just the vector
// todo: make different unary operator return just the map?
// todo: make operator-- be shrink_to_fit() and ++ be expand() ?
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

  using T    =  int;

private:
  template<bool OWNED=true> void typed_del(){ if(m_mem) free(memStart()); };   // explicit template specialization to make an owned tbl free it's memory and a non-owned tbl not free it's memory
  template<> void typed_del<false>(){}; 

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
  ui64             nxt(ui64 i, ui64 mod)
  {
    return ++i % mod;
  }
  ui64            prev(ui64 i, ui64 mod)
  {
    if(mod==0) return 0;
    return i==0?  mod-1  :  i-1;
  }
  ui64        wrapDist(ui64 ideal, ui64 idx, ui64 mod)
  {
    if(idx>=ideal) return idx - ideal;
    else return idx + (mod-ideal);
  }
  ui64        wrapDist(KV*  elems, ui64 idx, ui64 mod)
  {
    ui64 ideal = elems[idx].hsh.hash % mod;
    return wrapDist(ideal,idx,mod);
  }
  i64            place(KV kv, KV* elems, ui64 mod)
  {
    ui64  i = kv.hsh.hash % mod;
    ui64 en = prev(i,mod);
    while(elems[i].hsh.type!=EMPTY){
      if(i==en) return -1;
      i = nxt(i,mod);
    }

    elems[i] = kv;
    return i;
  }
  KV&         place_rh(KV kv, KV* elems, ui64 st, ui64 dist, ui64 mod)   // place_rh is place with robin hood hashing 
  {
    ui64     i = st;
    ui64    en = prev(st,mod);
    ui64 eldst = dist;
    while(true)
    {
      if(i==en){ return KV::error_kv(); }
      else if(elems[i].hsh.type==EMPTY){
        return elems[i] = kv;
        //return i;
      }else if( (eldst=wrapDist(elems,i,mod)) < dist){
        swap( &kv, &elems[i] );
        dist = eldst;
      }

      i = nxt(i,mod);
      ++dist;
    }
  
    return KV::error_kv();
  }
  void            init(ui64 size)
  {
    ui64   szBytes  =  tbl::sizeBytes(size);
    i8*      memst  =  (i8*)malloc(szBytes);                 // memst is memory start
    m_mem           =  memst + memberBytes();
    set_sizeBytes(szBytes);
    set_elems(0);
    set_capacity(size);
    set_size(size);
  }
  bool             del(const char* key)
  { 
    ui64 i = find(key);
    KV* el = elemStart();
    if(el[i].hsh.type==EMPTY) return false;

    el[i] = KV();
    ui64 mapcap = map_capacity();
    ui64     en = prev(i, mapcap);
    //compact(i, en, mapcap);  
    // todo: need rolling reshuffle here

    return true;
  }
  bool             del(ui64 i)
  { 
    //ui64 i = find(key);
    KV* el = elemStart();
    if(el[i].hsh.type==EMPTY) return false;

    el[i] = KV();
    ui64 mapcap = map_capacity();
    ui64     en = prev(i, mapcap);
    //compact(i, en, mapcap);

    return true;
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

    TO(l,i) push(l[i]); 
  }
  void              mv(tbl& r)
  {
    tbl_PRNT("\n moved \n");
    m_mem   = r.m_mem;
    r.m_mem = nullptr;
  }
  template<class OP> void op_asn(tbl const& l, OP op)
  {
    ui64     mx_sz = size();
    ui64     mn_sz = l.size();
    if(mx_sz<mn_sz){ auto tmp=mx_sz; mx_sz=mn_sz; mn_sz=tmp; }
    TO(mn_sz,i) op( (*this)[i], l[i] );
  }
  template<class OP> tbl  bin_op(tbl const& l, OP op)
  {     
    ui64     mx_sz = size();
    ui64     mn_sz = l.size();
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
  template<class OP> tbl  bin_op(T   const& l, OP op)
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

    KV() : hsh(), val(0) { memset(key, 0, sizeof(Key)); }
    template<class V> KV(const char* key, ui32 hash, V val)
    {
      *this = val;
      strcpy_s(this->key, sizeof(KV::Key), key);
      hsh.hash = hash;
    }
    KV(const char* key, ui32 hash)
    {
      strcpy_s(this->key, sizeof(KV::Key), key);
      hsh.hash = hash;
    }

    KV& operator=(KV const& l)
    {
      memmove(this, &l, sizeof(KV));
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

  i8*     m_mem;
 
  tbl() : 
    m_mem(nullptr)
  {
  }
  tbl(ui64 size)                                 // have to run default constructor here?
  {    
    init(size);
  }
  tbl(ui64 size, T const& value)
  {
    init(size);
    TO(size, i) (*this)[i] = value;
  }
  ~tbl(){ /*del()*/; }

  tbl(tbl const& l){ cp(l); }
  tbl& operator=(tbl const& l){ cp(l); return *this; }
  tbl(tbl&&      r){ mv(r); }
  tbl& operator=(tbl&&      r){ mv(r); return *this; }

  operator      ui64() const { return size(); }
  operator      bool() const { return m_mem!=nullptr; }
  T&      operator[](ui64 i)
  {
    tbl_msg_assert(i < size(), "\n\nTbl index out of range\n----------------------\nIndex:  ", i, "Size:   ", size())
    return ((T*)m_mem)[i];
  }
  auto    operator[](ui64 i) const -> T const& 
  {
    tbl_msg_assert(i < size(), "\n\nTbl index out of range\n----------------------\nIndex:  ", i, "Size:   ", size())
    return ((T*)m_mem)[i];
  }
  KV&     operator()(const char* key, bool make_new=true)
  {
    if( !(map_capacity()>elems()) )
       if(!expand()) return KV::error_kv();

    ui32  hsh  =  HashStr(key);    
    KV*    el  =  (KV*)elemStart();                                    // el is a pointer to the elements 
    ui64  mod  =  map_capacity();
    ui64    i  =  hsh % mod;
    ui64   en  =  prev(i,mod);
    ui64 dist  =  0;
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
  //KV&     operator()(const char* key, bool make_new=true)              // todo: just needs to return a kv reference since the kv can be set with operator=() ?
  //{
  //  if( !(map_capacity()>elems()) )
  //    if(!expand()) return KV::error_kv();
  //
  //  KV*    el  =  (KV*)elemStart();                                     // el is a pointer to the elements 
  //  ui32 hash  =  0;
  //  i64     i  =  find(key, &hash);
  //  if(i<0) return KV::error_kv();
  //
  //  if(el[i].hsh.type==EMPTY){ 
  //    if(make_new){
  //      strcpy_s(el[i].key, sizeof(KV::Key), key);
  //      el[i].hsh.hash = hash;
  //      el[i].hsh.type = NONE;                                         // this makes this slot no longer empty. When an assignment occurs it will overwrite this type.
  //      set_elems( elems()+1 );
  //    }
  //    return el[i];
  //  }
  //    
  //  if(hash == el[i].hsh.hash){                                        // if the hashes aren't the same, the keys can't be the same
  //    auto cmp = strncmp(el[i].key, key, sizeof(KV::Key)-1);           // check if the keys are the same 
  //    if(cmp==0) return el[i];
  //  }
  //
  //  //ui32   hsh  =  HashStr(key);
  //  ////ui32   hsh  =  (ui32)(*key); // & HASH_MASK );
  //  //
  //  //KV*     el  =  (KV*)elemStart();                                   // el is a pointer to the elements 
  //  //ui64   cap  =  map_capacity();  
  //  //ui64     i  =  hsh;
  //  //ui64  wrap  =  hsh % cap - 1;
  //  //ui64    en  =  wrap<(cap-1)? wrap : cap-1;                         // clamp to cap-1 for the case that hash==0, which will result in an unsigned integer wrap 
  //  //for(;;++i)
  //  //{
  //  //  i %= cap;                                                        // get idx within map_capacity
  //  //  HshType eh = el[i].hsh;                                          // eh is element hash
  //  //  if(el[i].hsh.type==EMPTY){ 
  //  //    if(make_new){
  //  //      strcpy_s(el[i].key, sizeof(KV::Key), key);
  //  //      el[i].hsh.hash = hsh;
  //  //      el[i].hsh.type = NONE;                                       // this makes this slot no longer empty. When an assignment occurs it will overwrite this type.
  //  //      set_elems( elems()+1 );
  //  //    }
  //  //    return el[i];
  //  //  }else if(hsh == eh.hash){                                        // if the hashes aren't the same, the keys can't be the same
  //  //    auto cmp = strncmp(el[i].key, key, sizeof(KV::Key)-1);         // check if the keys are the same 
  //  //    if(cmp==0) return el[i];
  //  //  }
  //  //
  //  //  if(i==en) break;                                                 // nothing found and the end has been reached, time to break out of the loop and return a reference to a KV with its type set to NONE
  //  //}
  //  
  //  return KV::error_kv();
  //}
  tbl     operator>>(tbl const& l){ return tbl::concat_l(*this, l); }
  tbl     operator<<(tbl const& l){ return tbl::concat_r(*this, l); }
  void    operator+=(tbl const& l){ op_asn(l, [](T& a, T const& b){ a += b; } ); }
  void    operator-=(tbl const& l){ op_asn(l, [](T& a, T const& b){ a -= b; } ); }
  void    operator*=(tbl const& l){ op_asn(l, [](T& a, T const& b){ a *= b; } ); }
  void    operator/=(tbl const& l){ op_asn(l, [](T& a, T const& b){ a /= b; } ); }
  void    operator%=(tbl const& l){ op_asn(l, [](T& a, T const& b){ a %= b; } ); }
  tbl     operator+ (tbl const& l){ return bin_op(l,[](T const& a, T const& b){return a + b;}); }
  tbl     operator- (tbl const& l){ return bin_op(l,[](T const& a, T const& b){return a - b;}); }
  tbl     operator* (tbl const& l){ return bin_op(l,[](T const& a, T const& b){return a * b;}); }
  tbl     operator/ (tbl const& l){ return bin_op(l,[](T const& a, T const& b){return a / b;}); }
  tbl     operator% (tbl const& l){ return bin_op(l,[](T const& a, T const& b){return a % b;}); }
  void    operator+=(T   const& l){ op_asn(l, [](T& a, T const& b){ a += b; } ); }
  void    operator-=(T   const& l){ op_asn(l, [](T& a, T const& b){ a -= b; } ); }
  void    operator*=(T   const& l){ op_asn(l, [](T& a, T const& b){ a *= b; } ); }
  void    operator/=(T   const& l){ op_asn(l, [](T& a, T const& b){ a /= b; } ); }
  void    operator%=(T   const& l){ op_asn(l, [](T& a, T const& b){ a %= b; } ); }
  tbl     operator+ (T   const& l){ return bin_op(l,[](T const& a, T const& b){return a + b;}); }
  tbl     operator- (T   const& l){ return bin_op(l,[](T const& a, T const& b){return a - b;}); }
  tbl     operator* (T   const& l){ return bin_op(l,[](T const& a, T const& b){return a * b;}); }
  tbl     operator/ (T   const& l){ return bin_op(l,[](T const& a, T const& b){return a / b;}); }
  tbl     operator% (T   const& l){ return bin_op(l,[](T const& a, T const& b){return a % b;}); }

  template<class V> KV&   put(const char* key, V val)
  {
    if( !(map_capacity()>elems()) )
       if(!expand()) return KV::error_kv();

    ui32  hsh  =  HashStr(key);    
    KV*    el  =  (KV*)elemStart();                                    // el is a pointer to the elements 
    ui64    i  =  hsh;
    ui64  mod  =  map_capacity();
    ui64   en  =  prev(i,mod);
    ui64 dist  =  0;
    for(;;++i,++dist)
    {
      i %= mod;                                                        // get idx within map_capacity
      HshType eh = el[i].hsh;                                          // eh is element hash
      if(el[i].hsh.type==EMPTY){ 
        set_elems( elems()+1 );
        return el[i] = KV(key, hsh, val);
      }else if(hsh==eh.hash  && 
        strncmp(el[i].key,key,sizeof(KV::Key)-1)==0 )
      { // if the hashes aren't the same, the keys can't be the same
        return el[i] = KV(key, hsh, val);
      }else if( wrapDist(el,i,mod) < dist ){
        KV kv(key, hsh, val);
        set_elems( elems()+1 );
        return place_rh(kv, el, i, dist, mod);
      }
    
      if(i==en) break;                                                 // nothing found and the end has been reached, time to break out of the loop and return a reference to a KV with its type set to NONE
    }

    return KV::error_kv();
  }

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

    auto    sz = sizeBytes();
    auto membr = memberBytes();
    auto   cap = capacity();
    auto   szT = sizeof(T);
    auto  szKV = sizeof(KV);
    return (sz - membr - cap*szT) / szKV;
  }
  auto     elemStart() -> KV* 
  {
    if(!m_mem) return nullptr;

    return (KV*)(data() + capacity());
  }
  auto     elemStart() const -> KV const* 
  {
    if(!m_mem) return nullptr;

    return (KV*)(data() + capacity());
  }
  void*      reserve(ui64 count, ui64 mapcount)
  {
    count    = mx(count, capacity());
    mapcount = mx(mapcount, map_capacity());
    KV*    prevElems  =  elemStart();
    ui64   prevMemSt  =  (ui64)memStart();
    ui64    prevOfst  =  prevElems? ((ui64)prevElems)-prevMemSt  :  0;
    ui64   prevBytes  =  sizeBytes();
    ui64  prevMapCap  =  map_capacity();
    ui64    nxtBytes  =  memberBytes() + sizeof(T)*count +  sizeof(KV)*mapcount;
    void*        re;
    bool      fresh  = !m_mem;
    if(fresh) re = malloc(nxtBytes);
    else      re = realloc(memStart(), nxtBytes);

    //if(prevBytes<nxtBytes) 
    //  memset(re, 0, nxtBytes);
      //memset( (i8*)re + prevBytes, 0, nxtBytes-prevBytes );

    if(re){
      m_mem = ((i8*)re) + memberBytes();
      set_sizeBytes(nxtBytes);
      set_capacity(count);
      //set_elems(0);

      KV* el = elemStart();
      if(prevElems){
        KV* prevEl = (KV*)( ((i8*)re) + prevOfst );
        //ui64   len = prevMapCap*sizeof(KV);
        if(el!=prevEl) TO(prevMapCap,i) el[i] = prevEl[i];
        //memmove(el, prevEl, len);
      }

      //TO(map_capacity(),i) new (&el[i]) KV();

      ui64  mapcap = map_capacity();
      i64   extcap = mapcap - prevMapCap;
      if(extcap>0) 
        TO(extcap,i) 
          new (&el[i+prevMapCap]) KV();

      if(prevElems)
      { 
        ui64  i  = 0;
        ui64 en  = prev(i,mapcap);
        ui64 cnt = 0; 
        do{
          ui64 nxti = nxt(i,mapcap); 
          if(el[i].hsh.type != EMPTY){
            KV kv  = el[i];
            el[i]  = KV();
            ui64 p = place(kv,el,mapcap);
            if(p!=i) en = i;
          }
          i = nxti;
          ++cnt;
        }while(i!=en);
        printf("\n loop count: %d mapcap: %d  ratio: %.4f \n", cnt, mapcap, cnt/(float)mapcap );

        //for(ui64 mvcnt=1; mvcnt>0;)
        //{
        //  mvcnt=0;
        //  TO(mapcap, i) if(el[i].hsh.type != EMPTY){
        //    KV kv = el[i];
        //    el[i] = KV();
        //    ui64 p = place(kv,el,mapcap);
        //    if(p!=i) ++mvcnt;
        //  }
        //  printf("\n move count: %d \n", mvcnt);
        //}

        //TO(mapcap, i) if(el[i].hsh.type != EMPTY){
        //  KV kv = el[i];
        //  el[i] = KV();
        //  place(kv,el,mapcap);
        //}

        //TO(prevMapCap, i) if(el[i].hsh.type != EMPTY){
        //  KV kv = el[i];
        //  el[i] = KV();
        //  (*this)(kv.key) = kv;                    
        //}

        //TO(prevMapCap, i) if(el[i].hsh.type != EMPTY){
        //  ui64 h = holeOfst(i);
        //  if(h>0) 
        //}
      }

      //if(prevElems)
      //{
      //  ui64    mod = mapcap; // - 1;
      //  TO(prevMapCap, i){
      //    KV kv = el[i];
      //    if(kv.hsh.type != EMPTY){
      //      ui64 nxtIdx = kv.hsh.hash % mod;
      //      if(nxtIdx!=i){
      //        //el[i] = KV();
      //        //compact_back( nxt(i,mod), mod);
      //        del(i);
      //        place_kv(kv, el, nxtIdx, mod);
      //      }
      //    }
      //  }
      //   
      //  compact_all( prev(prevMapCap,mapcap), mapcap);                      // start where the loop above left off
      //}
    }

    if(re && fresh){ set_size(0); set_elems(0); }
    
    return re;
  }
  void*       expand()
  {
    ui64    sz = size();
    ui64 nxtSz = sz + sz/2;
    nxtSz      = nxtSz<8? 8 : nxtSz;

    ui64    el = elems();
    ui64 nxtEl = el + el/2;
    nxtEl      = nxtEl<8? 8 : nxtEl;

    return reserve(nxtSz, nxtEl);
  }
  i64           find(const char* key, ui32* hash=nullptr)
  {
    ui32   hsh  =  HashStr(key);
    //ui32   hsh  =  (ui32)(*key); // & HASH_MASK );
    if(hash) *hash = hsh;

    KV*     el  =  (KV*)elemStart();                                   // el is a pointer to the elements 
    ui64   cap  =  map_capacity();  
    ui64     i  =  hsh;
    ui64  wrap  =  hsh % cap - 1;
    ui64    en  =  wrap<(cap-1)? wrap : cap-1;                         // clamp to cap-1 for the case that hash==0, which will result in an unsigned integer wrap 
    //ui64  dist  =  0;
    for(;;++i/*,++dist*/)
    {
      i %= cap;                                                        // get idx within map_capacity
      HshType eh = el[i].hsh;                                          // eh is element hash
      if(el[i].hsh.type==EMPTY){ 
        return i;
      //}else if(ideal(i)<dist){
      //  return i;
      }else if(hsh == eh.hash){                                        // if the hashes aren't the same, the keys can't be the same
        auto cmp = strncmp(el[i].key, key, sizeof(KV::Key)-1);         // check if the keys are the same 
        if(cmp==0) return i;
      }

      if(i==en) break;                                                 // nothing found and the end has been reached, time to break out of the loop and return a reference to a KV with its type set to NONE
    }
    
    return -1;
  }
  ui64         ideal(ui64 i)
  {
    auto el = elemStart();
    if(el[i].hsh.type==EMPTY) return i;
     
    return el[i].hsh.hash % map_capacity();
  }
  ui64      distance(ui64 i)
  {
    //ui64 idl = ideal(i);
    return wrapDist( ideal(i), i, map_capacity() );
  }
  i64       holeOfst(ui64 i)
  { // finds the closes hole from an element, but not the furthest hole
    KV*   el = elemStart();
    ui64 mod = map_capacity();
    //if(el[i].hsh.type==EMPTY) return -1;
    
    i64    h = -1;
    //ui64 idl = ideal(i);
    ui64 dst = distance(i);
    ui64 cnt = 0;
    //while(el[i].hsh.type!=EMPTY && cnt!=dst){
    while(dst >= cnt){ // count can equal distance
      if(el[i].hsh.type==EMPTY) h = i;
      i = prev(i,mod);
      ++cnt;
    }
    return h;
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
  static tbl      concat_l(tbl const& a, tbl const& b)                                  // returns the bytes needed to store the data structure if the same arguments were given to the constructor
  {
    auto sz = a.size();
    tbl ret(sz + b.size());
    
    TO(sz,i) ret[i]    = a[i];
    TO(b, i) ret[sz+i] = b[i];

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
    
    TO(sz,i) ret[i]    = a[i];
    TO(b, i) ret[sz+i] = b[i];

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







//strcpy_s(el[i].key, sizeof(KV::Key), key);
//el[i].hsh.hash = hsh;
//el[i] = val;                                                 // this makes this slot no longer empty. When an assignment occurs it will overwrite this type.
//el[i]       =  kv;
//strcpy_s(kv.key, sizeof(KV::Key), key);
//kv.hsh.hash = hsh;
//
//KV kv;
//strcpy_s(kv.key, sizeof(KV::Key), key);
//kv.hsh.hash = hsh;
//kv = val;
//strcpy_s(kv.key, sizeof(KV::Key), key);
//kv.hsh.hash = hsh;
//el[i]       =  kv;
//return el[i] = kv;
//
//KV kv;
//kv = val;
//strcpy_s(kv.key, sizeof(KV::Key), key);
//kv.hsh.hash = hsh;

//ui32   hsh  =  (ui32)(*key); // & HASH_MASK );
//ui64   cap  =  map_capacity();  
//ui64  wrap  =  hsh % cap - 1;
//ui64    en  =  wrap<(cap-1)? wrap : cap-1;                       // clamp to cap-1 for the case that hash==0, which will result in an unsigned integer wrap 

//
//ui64 nxtSz = (sz/2)? sz+sz/2 : sz+1;

//void    compact_back(ui64 st, ui64 mod)
//{
//  //ui64   cnt = 0;                                   // cnt is count which counts the number of loops, making sure the entire map capacity (mapcap) has not bene exceeded
//  KV*      el = elemStart(); 
//  ui64  mvIdx = st;
//  ui64  elDst = wrapDist(el, st, mod);
//  if(elDst==0) return;
//
//  ui64      i = prev(st, mod);
//  ui64     en = st; //nxt(st, mapcap);
//  ui64  mvDst = 0;
//  while(el[i].hsh.type==EMPTY && mvDst<elDst && i!=en){ 
//    ++mvDst;
//    i = prev(i,mod);
//  }
//  i = nxt(i,mod);
//  el[i]  = el[st];
//  el[st] = KV();
//  //TO(mvDst,ii) mvIdx=prev(mvIdx,mod);      // todo: do this with a function that doesn't loop
//}
//ui64         compact(ui64 st, ui64 en, ui64 mapcap) //ui64 mapcap)
//{
//  ui64   cnt = 0;                                   // cnt is count which counts the number of loops, making sure the entire map capacity (mapcap) has not bene exceeded
//  KV*     el = elemStart(); 
//  ui64     i = st;
//  ui64 empty;
//  while(el[i].hsh.type!=EMPTY && i!=en){ 
//    i = nxt(i,mapcap);
//    ++cnt;
//  } // finds the first empty slot
//  empty = i;
//  while(el[i].hsh.type==EMPTY && i!=en){
//    i = nxt(i,mapcap);
//    ++cnt;
//  } // find the first non-empty slot after finding an empty slot
//  while(el[i].hsh.type!=EMPTY && i!=en){
//    ui64 elemDst = wrapDist(el,i,mapcap);
//    if( elemDst>0 ){ 
//      ui64 emptyDst = wrapDist(empty,i,mapcap);           // emptyDst is empty distance - the distance to the empty slot - this can probably be optimized to just be an increment
//      ui64    mnDst = mn(elemDst, emptyDst);               // mnDst is the minimum distances
//      ui64    mvIdx = i;
//      TO(mnDst,ii) mvIdx=prev(mvIdx,mapcap);
//      el[mvIdx] = el[i];
//      el[i]       = KV();
//      empty       = i;                                     //++empty; // empty = i;  if empty was just the current index or if there are lots of empty elements in between, empty can be incremented by one
//    }
//    i = nxt(i,mapcap);
//    ++cnt;
//  } // moves elements backwards if they would end up closer to their ideal position
//
//  return i;
//
//  //ui64  mved = 0;                                 // mved is moved which is the number of elements that have been moved
//  //ui64    en = st;                                  // en is ending index which is the index that the loops end up on - it is one after the last index looked at
//  //
//  // while(el[i].hsh.type!=EMPTY && cnt<mapcap){ 
//  // while(el[i].hsh.type==EMPTY && cnt<mapcap){
//  // while(el[i].hsh.type!=EMPTY && cnt<mapcap){
//}
//void     compact_all(ui64 st, ui64 mapcap)
//{
//  //ui64  wrap  =  st - 1;
//  //ui64    en  =  wrap<(mapcap-1)? wrap : mapcap-1;                         // clamp to cap-1 for the case that hash==0, which will result in an unsigned integer wrap 
//
//  ui64    en = prev(st,mapcap);
//  ui64 cmpct = st;
//  do{
//    cmpct = compact(cmpct,en,mapcap);
//  }while( cmpct != en );
//  
//}

//i64            place(KV kv, KV* elems, ui64 mod)
//ui64     i = idx % mod;
//ui64 elidx = i;
//do{
//  //i = i % mod;
//  if(elems[i].hsh.type==EMPTY){ elems[i]=kv; break; }
//  elidx = elems[i].hsh.hash % mod;
//    i64  eldist = wrapDist(elidx,i,mod);
//    i64    dist = wrapDist(idx,i,mod);
//    if(dist>eldist)
//      swap( &(elems[i]), &kv );                     //  robin hood hashing?
//  i = nxt(i,mod);
//  //++i;
//}while(true);

//ui64           mod0(ui64 n, ui64 mod)
//{
//  n==mod? (n%mod - 1) : n;
//}
    
//place_kv(kv, elems, elidx, mod);
//    }
//
//}else{
//  swap( &(elems[i]), &kv );                     //  robin hood hashing?
//  place_kv(kv, elems, elidx, mod);
//}

//
//void        place_kv(KV kv, ui64 kvidx, KV* elems, ui64 i, ui64 mod)

//if(elems[i].hsh.type==EMPTY){ elems[i] = kv; return; }
//i64  idist = 0;

//swap( &(elems[i]), &kv );
//if(kv.hsh.type!=EMPTY)
//place_kv(kv, elems, elidx, mod);

//if(elidx!=i){
//
//}
//if( elems[elidx].hsh.hash%mod == i ){ ++i; continue; }

//do{
//  ui64 elidx = elems[i].hsh.hash % mod;
//  if(elidx!=i) break;
//  ++i;
//}while(true);

//while(i-kvidx < i-elidx  && elems[i].hsh.type==NONE){
//  swap( &(elems[i]), &kv );
//  ++i;
//  ui64 elidx = elems[i].hsh.hash % mod;
//}

//if( i-kvidx < i-elidx){
//}

//if(tmp.hsh.type==NONE){ tmp = el[i]; tmpidx = tmp.hsh.hash%mod; continue; }
//ui64  elidx = el[i].hsh.hash % mod;
//if(i-tmpidx < elidx-i) swap( &(el[i]), &tmp );
//
// todo: take this out so that hashes aren't wiped
//KV* el = elemStart();
//TO(elems, i) el[i] = KV();
//    
//set_sizeBytes(nxtBytes);

// todo: need to move the element memory the previous range to the new range
//(KV*)( ((i8*)re)+prevOfst+prevMapCap*sizeof(KV) ) 

//KV*   curEnd = el + mapcap;
//KV*  prevEnd = prevElems?  (KV*)( ((i8*)re)+prevSzBytes ) :  el;
//for(KV* kv=prevEnd; kv!=curEnd; ++kv){                                  // excap is extra capacity
//  *kv = KV();
//}
//for(auto i=prevMapCap; i<elems; ++i){
//  el[i] = 
//}
//TO(elems-prevMapCap,i) 

//ui64 curOfst = (i8*)el - (i8*)re;
//memcpy(el, (i8*)re + prevOfst, prevMapCap*sizeof(KV) );
//
// rehash, make sure that the differences in indexes include a linear search that would wrap around
//KV      tmp = KV();
//ui64 tmpidx = 0;

//  KV tmp2 = el[i];
//  el[i] = tmp;
//  tmp = tmp2;
//} 
//
//if(el[i].hsh.type != NONE){

//void    operator+=(tbl const& l)
//{
//  ui64     mx_sz = size();
//  ui64     mn_sz = l.size();
//  if(mx_sz<mn_sz){ auto tmp=mx_sz; mx_sz=mn_sz; mn_sz=tmp; }
//  TO(mn_sz,i) (*this)[i] += l[i];
//}

//ui64     mx_sz = size();
//ui64     mn_sz = l.size();
//tbl const* lrg = this;
//if(mx_sz<mn_sz){ auto tmp=mx_sz; mx_sz=mn_sz; mn_sz=tmp; lrg = &l; }        // swap mx_sz, mn_sz, and the pointer to the larger tbl if mismatched
//tbl ret(mx_sz); 
//TO(mn_sz, i) ret[i] = (*this)[i] + l[i];
//TO(mx_sz-mn_sz, i) ret[i+mn_sz] = (*lrg)[i+mn_sz];
//
//return ret;

//
//template<bool OWNED=true> struct typed_del{ static void del(){ if(m_mem) free(memStart()); } }; 

//
//mx_sz = lsz>mx_sz? lsz : mx_sz;

//ui64   szBytes  =  tbl::sizeBytes(size);
//i8*      memst  =  (i8*)malloc(szBytes);                 // memst is memory start
//m_mem           =  memst + memberBytes();
//set_sizeBytes(szBytes);
//set_elems(0);
//set_capacity(size);
//set_size(size);

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
