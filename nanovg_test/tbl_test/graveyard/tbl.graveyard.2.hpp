
// using Key = char[19];
//template<class DEST, class SRC> static DEST cast_ptr(u64 src){ return (DEST*)src; }

//case    U8: return   "u8";
//case   U16: return  "u16";
//case   U32: return  "u32";
//
//case    I8: return   "i8";
//case   I16: return  "i16";
//case   I32: return  "i32";
//
//case   F32: return  "f32";
//
//case   tU8: return  "table   u8";
//case  tU16: return  "table  u16";
//case  tU32: return  "table  u32";
//
//case   tI8: return  "table   i8";
//case  tI16: return  "table  i16";
//case  tI32: return  "table  i32";
//case  tF32: return  "table  f32";
//return "Empty";

//u64 mapcap = map_capacity();
//if( !kv && (mapcap==0 || mapcap*8 < elems()*10) )
//  if(!expand(false, true)) return KVOfst();

//
// bool make_new=true) // make_new); // bool make_new=true)

//  }
//  }else if(hsh==kv->hsh.hash  &&                                                // if the hashes aren't the same, the keys can't be the same
//    strncmp(kv->key,key,sizeof(KV::Key)-1)==0 )
//  {
//    auto type = kv->hsh.type;
//    if( (type&HshType::TABLE) && (type&HshType::CHILD) )
//      return KVOfst(kv, (void*)memStart());
//    else
//      return KVOfst(kv);
//  }else if(dist > wrapDist(el,i,mod) ){
//    KV nxtkv(key, hsh);
//    elems( elems()+1 );
//    return KVOfst( &(place_rh(nxtkv, el, i, dist, mod)) );
//  }
//}else return KVOfst(kv)                                                    // if the key wasn't found, kv will be a nullptr which is the same as an error KVOfst that will evaluate to false when cast to a boolean      

//bool array_destruct=true)
//
//if(array_destruct){
//}

//u64 mapcap = map_capacity();
//KV*      e = elemStart();
//TO(mapcap,i)
//  if( (e[i].hsh.type&HshType::TABLE) && !(e[i].hsh.type&HshType::CHILD) ){
//    // this assumes that the array doesn't hold any tbls, which at the moment it can't, because arrays of child tables can only hold numeric types, no compound types, no structs 
//    ((tbl<T>*)e[i].val)->destroy(false);  // destroy the held table, but don't run the destructors on the array because we don't know the type and they should be intrinsic numerical types anyway
//  }

//tbl<T> tmp = e[i];
//
//if( (e[i].hsh.type&HshType::TABLE) && (e[i].hsh.type&HshType::CHILD) ){
//  tbl<T> tmp = e[i];      // this will get a reference to the non-owned table
//  tmp.destroy(false);     // destroy/destruct without actually freeing the memory - this will call the destructors on the children recursivly
//  // is destructing the child memory even needed since the child tables will contain only intrinsic types and children of intrinsic types ? 
//  // won't only non-children tables need to be destructed ? 
//
//  //tmp.owned(true);      // this will set the owned flag in the child memory - because of this the 
//}
//if(free_mem){ free(memStart()); }

// l.elems());    
// todo: can be done with resize, which could use realloc instead of free and malloc? - this was the copy constructor - cp()

//if(&l==nullptr) || l.data()==nullptr)
//reserve(l.size(), l.map_capacity() ); // l.elems());    // todo: can be done with resize, which could use realloc instead of free and malloc?

//KVOfst  operator()(const char* key, bool make_new=true)
//{
//  if(!owned()){ 
//    if(has(key))
//  }
//
//  u64 mapcap = map_capacity();
//  if( mapcap==0 || mapcap*8 < elems()*10 )
//    if(!expand(false, true)) return &KV::error_kv();
//
//  HshType hh;
//  hh.hash   =  HashStr(key);
//  u32 hsh   =  hh.hash;                                                 // make sure that the hash is being squeezed into the same bit depth as the hash in HshType
//  KV*   el  =  (KV*)elemStart();                                        // el is a pointer to the elements 
//  u64  mod  =  map_capacity();
//  u64    i  =  hsh % mod;
//  u64   en  =  prev(i,mod);
//  u64 dist  =  0;
//  for(;;++i,++dist)
//  {
//    i %= mod;                                                        // get idx within map_capacity
//    HshType eh = el[i].hsh;                                          // eh is element hash
//    if(el[i].hsh.type == HshType::EMPTY){ 
//      elems( elems()+1 );
//      new (&el[i]) KV(key, hsh);
//      el[i].hsh.type = HshType::NONE;
//      return KVOfst( &(el[i]) );
//    }else if(hsh==eh.hash  &&                                                // if the hashes aren't the same, the keys can't be the same
//      strncmp(el[i].key,key,sizeof(KV::Key)-1)==0 )
//    { 
//      auto type = el[i].hsh.type;
//      if( (type&HshType::TABLE) && (type&HshType::CHILD) ){
//        return KVOfst( &el[i], (void*)(memStart()) );
//      }else
//        return KVOfst( &(el[i]) ); //el[i];   //  = KV(key, hsh);
//    }else if(dist > wrapDist(el,i,mod) ){
//      KV kv(key, hsh);
//      elems( elems()+1 );
//      return KVOfst( &(place_rh(kv, el, i, dist, mod)) );
//    }
//  
//    if(i==en) break;                                                 // nothing found and the end has been reached, time to break out of the loop and return a reference to a KV with its type set to NONE
//  }
//
//  return &KV::error_kv();
//}

//tbl<N>* tp = &n;
//n.childData()

//if(kv)
//{
//
//}else return KVOfst();                                                   // if(kv)
//}else{                                                                     // if(owned())
//  KV* kv = get(key);
//}

//if( !( map_capacity()*0.75f > (float)elems() ) )
//   if(!expand(false, true)) return &KV::error_kv();
//
//if(!m_mem){ return KVOfst(); }

//template<bool OWNED=true> void typed_del(){ if(m_mem) free(memStart()); };   // explicit template specialization to make an owned tbl free it's memory and a non-owned tbl not free it's memory
//template<> void typed_del<false>(){}; 

//tbl(tbl&&      r) = delete;
//tbl(tbl&&      r){ mv(std::move(r)); }
//tbl& operator=(tbl&&      r) = delete;
//operator       u64() const{ return size(); }

//KV&         kv;
//
//KVOfst(KV&& _kv, void* _base=nullptr) :
//  kv(std::move(_kv)), base(_base)
//{}
//
//operator tu64();

//
//u32  hsh  =  HashStr(key);

//u64 chldst = (u64)f + f->capacity*8 + f->mapcap*sizeof(KV) + sizeof(f);
//u64     cd = (u64)t.childData();

//KVOfst kvo( &el[i], (void*)(memStart()) );           
//auto f = (fields*)(el[i].val + (u64)childData());

//t.m_mem = (u8*)base;
//t.m_mem = (u8*)base;
//auto f = t.memStart();
//
//tu64 ret;
//ret.m_mem = (u8*)( chldst + kv->val + tu64::memberBytes() ); 
//auto ff = ret.memStart();

//set_capacity(count);
//set_mapcap(mapcap);

//if(!m_mem) return 0;
//u64 sb = *((u64 const*)memStart());
//sb >>= 16;
//return sb;

//if(!m_mem) return 0;
//return *( ((u64*)memStart()) + 2);

//if(!m_mem) return 0;
//
//return (sizeBytes() - memberBytes()) / sizeof(T);  // - size()*sizeof(T)

//
//{ if(!m_mem) return 0; return memStart()->size; }

//free(p); // from shrink_to_fit
//reorder();

// *( ((u64*)memStart()) + 1 );
//return *( ((u64*)memStart()) + 1 );

//set_elems( elems()+1 );
//memStart()->elems += 1;
//
//return el[i];

//KV   kv       =  el[i];
//kv.val       +=  (u64)memStart();
//kv.hsh.type  &=  ~HshType::CHILD;
//
//KVOfst kvo;
//kvo.kv   = el[i];
//kvo.base = (void*)(this);
//return kvo;
//
//KVOfst kvo( &el[i], (void*)(memStart()) );
//tu64 t;
//t.m_mem = (u8*)( (u64)kvo.base + kvo.kv->val + memberBytes() );
//tu64::fields*  f = t.memStart();

//m_mem           =  memst + memberBytes();
//set_sizeBytes(szBytes);
//set_elems(0);
//set_capacity(size);
//set_size(size);
//*mapcap_ptr() = 0;

//sizeBytes(szBytes);
//elems(0);
//capacity(size);
//this->size(size);
//mapcap(0);

//auto f = (tbl<u64>::fields*)((u64)base+kv->val);
//
//auto ff = tp.memStart();
//
//tf64*  tp = (tf64*)((u64)base+kv.val+tbl<tf64>::memberBytes() ); 
//
//else     tp.m_mem = ((tu64*)kv->val)->m_mem;
//
//return tp;

//static u64   magicNumber(u64 n)
//{
//  n <<= 16;
//  u8* nn = (u8*)&n;
//  //nn[7]='t'; nn[8]='b';
//  nn[0]='t'; nn[1]='b';
//  return n;
//}

//KVOfst(KV const& _kv, void* _base=nullptr) :
//  kv(_kv), base(_base)
//{}
//
//KVOfst& operator=(KV const& _kv){ kv = _kv; base=nullptr; }
//
//KVOfst(KV&& _kv, void* _base) :
//  kv(_kv), 
//  base(_base)
//{}
//
//template<class T> KVOfst(KVOfst&& kvo) : 
//
//template<class T> KVOfst(T&& _kv, void* _base) :
//  kv(std::forward<T>(_kv)), 
//  base(_base)
//{
//}
//
//operator tf64*();
//operator tf64&();

//KVOfst::operator tf64*(){ 
//  tf64*  t = (tf64*)((u64)base+kv.val);
//  //t->m_mem = 
//  return t; }
//
//u64 taddr = (tf64*)((u64)base+kv.val); 
//tf64 ret( *tp );
//((tf64*)taddr);

//void*      memStart() const
//{
//  return (void*)(m_mem - memberBytes());
//}

//struct       KV
//{
//  using  Type   =  HshType::Type;
//
//  template<class N> struct typenum { static const Type num = HshType::EMPTY; };
//  template<> struct typenum<u64>   { static const Type num = HshType::U64;   };
//  template<> struct typenum<i64>   { static const Type num = HshType::I64;   };
//  template<> struct typenum<f64>   { static const Type num = HshType::F64;   };
//  template<> struct typenum<tu64>  { static const Type num = HshType::tU64;  }; 
//  template<> struct typenum<ti64>  { static const Type num = HshType::tI64;  }; 
//  template<> struct typenum<tf64>  { static const Type num = HshType::tF64;  };
//  template<> struct typenum<long>  { static const Type num = HshType::I64;   };
//  template<> struct typenum<unsigned long> { static const Type num = HshType::U64; };
//
//  template<class C> struct typecast { using type = C;   };                               // cast types
//  template<> struct typecast<i8>    { using type = i64; };
//  template<> struct typecast<i16>   { using type = i64; };
//  template<> struct typecast<i32>   { using type = i64; };
//  template<> struct typecast<u8>    { using type = u64; };
//  template<> struct typecast<u16>   { using type = u64; };
//  template<> struct typecast<u32>   { using type = u64; };
//  template<> struct typecast<f32>   { using type = f64; };
//
//  template<class DEST, class SRC> 
//  static DEST cast_mem(u64 const* const src)
//  { return (DEST)(*((SRC*)src)); }
//
//  using Key = char[43];    // using Key = char[19]; 
//
//  HshType   hsh;
//  Key       key;
//  u64       val;
//  u64      base;
//
//  KV() : hsh(), val(0), base(0) { 
//    memset(key, 0, sizeof(Key));
//    //hsh.type = Type::NONE;
//    hsh.type = HshType::EMPTY;
//  }
//  KV(const char* key, u32 hash, u64 _val=0, u64 _base=0) :
//    hsh(), val(_val), base(_base)
//  {
//    memcpy(this->key, key, sizeof(KV::Key) );
//    hsh.hash = hash;
//    hsh.type = HshType::EMPTY;
//
//    //strcpy_s(this->key, sizeof(KV::Key), key);
//    //hsh.type = NONE;
//  }
//  KV(KV const& l){ cp(l); }
//  KV(KV&&      r){ cp(r); }
//
//  KV& cp(KV const& l)
//  {
//    hsh  = l.hsh;
//    val  = l.val;
//    base = l.base;
//    memmove(key, l.key, sizeof(Key) );
//    return *this;
//  }
//
//  bool operator==(KV const& l)
//  {
//    return hsh.hash==l.hsh.hash && 
//      strncmp(l.key,key,sizeof(KV::Key)-1)==0;
//  }
//  KV&  operator=(KV const& l){ return cp(l); }
//  KV&  operator=(KV&& r){      return cp(r); }
//  template<class N> KV& operator=(N const& n)
//  {
//    hsh.type     = typenum< typecast<N>::type >::num;
//    auto castVal = (typecast<N>::type)n;
//    val          = *((u64*)&castVal);
//    return *this;
//  }
//  template<class N> KV& operator=(tbl<N> const& n)
//  {
//    hsh.type  =  typenum< tbl<N> >::num;
//    val       =  (u64)&n;
//    return *this;
//  }
//
//  operator tf64*()
//  { 
//    //tf64 t;
//    //*((u64*)&t) = val;
//    //return t;
//    tf64* t = (tf64*)val;
//    return t;
//  }
//  operator tf64&()
//  { 
//    return *((tf64*)val);
//  }
//  template<class N> operator N()
//  { 
//    return as<N>();
//  }
//  template<class N> N as() const
//  { 
//    if(hsh.type==typenum<N>::num) return *((N*)&val);       // if the types are the same, return it as the cast directly
//
//                                                            //ui8   both = hsh.type | typenum<N>::num;              // both bits
//    if( (hsh.type & HshType::NONE) || (hsh.type & ERROR) ){
//      tbl_msg_assert(
//        hsh.type==typenum<N>::num, 
//        " - tbl TYPE ERROR -\nInternal type was: ", 
//        tbl::type_str((Type)hsh.type), 
//        "Desired  type was: ",
//        tbl::type_str((Type)typenum<N>::num) );        
//    } 
//    if( (hsh.type & SIGNED) && !(typenum<N>::num & SIGNED)  ){
//      tbl_msg_assert(
//        hsh.type==typenum<N>::num, 
//        " - tbl TYPE ERROR -\nSigned integers can not be implicitly cast to unsigned integers.\nInternal type was: ", 
//        tbl::type_str((Type)hsh.type), 
//        "Desired  type was: ",
//        tbl::type_str((Type)typenum<N>::num) );
//    }
//    if( !(hsh.type & INTEGER) && (typenum<N>::num & INTEGER) ){
//      tbl_msg_assert(
//        hsh.type==typenum<N>::num, 
//        " - tbl TYPE ERROR -\nFloats can not be implicitly cast to integers.\nInternal type was: ", 
//        tbl::type_str((Type)hsh.type), 
//        "Desired  type was: ",
//        tbl::type_str((Type)typenum<N>::num) );
//    }
//    if( (hsh.type|typenum<N>::num) & TABLE ){
//      tbl_msg_assert(
//        hsh.type==typenum<N>::num, 
//        " - tbl TYPE ERROR -\nTables can not be implicitly cast, even to a larger bit depth.\nInternal type was: ", 
//        tbl::type_str((Type)hsh.type), 
//        "Desired  type was: ",
//        tbl::type_str((Type)typenum<N>::num) );
//    }
//
//    u8 destbits = typenum<N>::num & BITS_MASK;
//    u8  srcbits = hsh.type        & BITS_MASK;
//    if( destbits > srcbits ){
//      switch(hsh.type){
//      case   U8: return cast_mem<N,  u8>(&val);
//      case  U16: return cast_mem<N, u16>(&val);
//      case  U32: return cast_mem<N, u32>(&val);
//      case   I8: return cast_mem<N,  i8>(&val);
//      case  I16: return cast_mem<N, i16>(&val);
//      case  I32: return cast_mem<N, i32>(&val);
//      case  F32: return cast_mem<N, f32>(&val);
//
//      case  tF64: return cast_mem<N, tf64>(&val);
//      case  tU64: return cast_mem<N, tu64>(&val);
//      case  tI64: return cast_mem<N, ti64>(&val);
//
//      }
//    }
//
//    tbl_msg_assert(
//      hsh.type==typenum<N>::num, 
//      " - tbl TYPE ERROR -\nInternal type was: ", tbl::type_str((Type)hsh.type), 
//      "Desired  type was: ",                      tbl::type_str((Type)typenum<N>::num) );
//
//    return N();
//  }
//
//  bool isEmpty()           const {
//    return hsh.type==HshType::NONE || hsh.type==HshType::EMPTY;
//  }
//
//  static KV&    empty_kv()
//  {
//    static KV kv;
//    kv.hsh.type = HshType::EMPTY;
//    return kv;
//  }
//  static KV&     none_kv()
//  {
//    static KV kv;
//    kv.hsh.type = HshType::NONE;
//    return kv;
//  }
//  static KV&    error_kv()
//  {
//    static KV kv;
//    kv.hsh.type = HshType::ERR;
//    return kv;
//  }
//  static auto   type_str(Type t) -> char const* const 
//  {
//    switch(t)
//    {
//    case HshType::ERR:   return "Error";
//    case HshType::EMPTY: return "Empty";
//    case HshType::NONE:  return  "None";
//
//    //case    U8: return   "u8";
//    //case   U16: return  "u16";
//    //case   U32: return  "u32";
//    case   HshType::U64: return  "u64";
//    //case    I8: return   "i8";
//    //case   I16: return  "i16";
//    //case   I32: return  "i32";
//    case   HshType::I64: return  "i16";
//    //case   F32: return  "f32";
//    case   HshType::F64: return  "f64";
//
//    //case   tU8: return  "table   u8";
//    //case  tU16: return  "table  u16";
//    //case  tU32: return  "table  u32";
//    case  HshType::tU64: return  "table  u64";
//    //case   tI8: return  "table   i8";
//    //case  tI16: return  "table  i16";
//    //case  tI32: return  "table  i32";
//    case  HshType::tI64: return  "table  i16";
//    //case  tF32: return  "table  f32";
//    case  HshType::tF64: return  "table  f64";
//
//    default: return "Unknown Type";
//    }
//    //return "Empty";
//  }
//};

//auto      mapcap_ptr()       -> u64*       { return (u64*)memStart() + 4;  }
//auto      mapcap_ptr() const -> u64 const* { return (u64*)memStart() + 4;  }
//void   set_sizeBytes(u64 bytes) // -> u64
//{
//  *( (u64*)memStart() ) = magicNumber(bytes);
//}
//
//void        set_size(u64  size)
//{
//  *( ((u64*)memStart()) + 1 ) = size;
//}
//void    set_capacity(u64   cap)
//{
//  *( ((u64*)memStart()) + 2) = cap;
//}
//void       set_elems(u64 elems)
//{
//  *( ((u64*)memStart()) + 3) = elems;
//}
//void      set_mapcap(u64   cap){ *mapcap_ptr() = cap; }
//
//*( (u64*)memStart() ) = magicNumber(bytes);

//{ return *((tf64*)kv.val + offset);  }
//
//{ 
//  tf64* t = (tf64*)(kv.val + offset);
//  return t;
//}
//
//operator tf64&(KVOfst const& l){ return *((tf64*)l.kv.val + l.offset);  }

//  HshType     hsh;
//  char        key[51];
//  HshType::u64         val;
//};

//KV      operator()(const char* key, bool make_new=true) const
//{
//  if( !( map_capacity()*0.75f > (float)elems() ) )
//    if(!expand(false, true)) return KV::error_kv();
//
//  HshType hh;
//  hh.hash   =  HashStr(key);
//  u32 hsh   =  hh.hash;                                                 // make sure that the hash is being squeezed into the same bit depth as the hash in HshType
//                                                                        //u32  hsh  =  HashStr(key);
//  KV*   el  =  (KV*)elemStart();                                     // el is a pointer to the elements 
//  u64  mod  =  map_capacity();
//  u64    i  =  hsh % mod;
//  u64   en  =  prev(i,mod);
//  u64 dist  =  0;
//  for(;;++i,++dist)
//  {
//    i %= mod;                                                        // get idx within map_capacity
//    HshType eh = el[i].hsh;                                          // eh is element hash
//    if(el[i].hsh.type == HshType::EMPTY){ 
//      set_elems( elems()+1 );
//      new (&el[i]) KV(key, hsh);
//      el[i].hsh.type = HshType::NONE;
//      return el[i];
//    }else if(hsh==eh.hash  &&                                                // if the hashes aren't the same, the keys can't be the same
//      strncmp(el[i].key,key,sizeof(KV::Key)-1)==0 )
//    { 
//      auto type = el[i].hsh.type;
//      if( (type&HshType::TABLE) && (type&HshType::CHILD) ){
//        KV   kv       =  el[i];
//        kv.val       +=  (u64)memStart();
//        kv.hsh.type  &=  ~HshType::CHILD;
//        return kv;
//      }else
//        return el[i];   //  = KV(key, hsh);
//    }else if(dist > wrapDist(el,i,mod) ){
//      KV kv(key, hsh);
//      set_elems( elems()+1 );
//      return place_rh(kv, el, i, dist, mod);
//    }
//
//    if(i==en) break;                                                 // nothing found and the end has been reached, time to break out of the loop and return a reference to a KV with its type set to NONE
//  }
//
//  return KV::error_kv();
//
//  //return el[i] = KV(key, hsh);
//}

// forward declarations
//enum   Type;
//struct   KV;
//using HshType = KV::HshType;

//auto       p    =  (u64*)(e[i].val);
//auto szbytes    =  *p >> 16;
//
//tbl<u8> t;
//*((u64*)(&t))  =  e[i].val;
//auto szbytes   =  t.sizeBytes();
//auto       p    =  (tbl<u8>*)(e[i].val);
//auto szbytes    =  p->sizeBytes();
//
//HshType ht;
//int chld = HshType::CLEAR | HshType::CHILD;
//ht.type  = chld; // | e[i].hsh.type;
//u32 type  =  e[i].hsh.type;
//type               |=  HshType::CHILD;
//e[i].hsh.type       = ht.type; // (HshType::Type)( HshType::CLEAR | HshType::CHILD | e[i].hsh.type );
//e[i].hsh.type   =  (HshType::Type)(e[i].hsh.type | HshType::CHILD);

//auto p  =  (tbl<u8>*)(e[i].val);
//cap    +=  *p >> 16;
//cap    +=  p->sizeBytes();
//
//tbl<u8> t;
//*((u64*)(&t))  =   e[i].val;

//val       =  (u64)n.memStart();
//val       =  (u64)n.data();

//union   HshType
//{
//  struct { Type type : 6; u32 hash: 26; };
//  u32 as_u32;
//
//  HshType() : 
//    type(HshType::EMPTY),
//    hash(0)
//  {}
//};

//template<,class SRC> static tu64 cast_mem(u64 const* const src)
//{ 
//  tu64 t;
//  *((u64*)&t) = *src;
//  return t;
//}

//memmove(this, &l, sizeof(KV));
//memcpy(this, &l, sizeof(KV));

//KV kv;
//memset(&kv, 0, sizeof(KV));
//
//KV kv;
//memset(&kv, 0, sizeof(KV));
//KV kv;
//
//memset(&kv, 0, sizeof(KV));
//KV kv;

//u64             del(KV::Type t)
//{
//  u64 mapcap = map_capacity();
//  KV*      el = elemStart();
//  u64    cnt = 0;
//  TO(mapcap,i) 
//    if(el[i].hsh.type==t){ 
//      el[i] = KV();
//      ++cnt;
//    }
//  reorder();
//
//  return cnt;
//}

//struct { Type type : 5; i32 hash: 27; };
//i32 as_ui32;

// // U8       =  INTEGER | BITS_8,   
// //U16       =  INTEGER | BITS_16,
// //U32       =  INTEGER | BITS_32,
// U64       =  INTEGER | BITS_64,
// // I8       =  INTEGER | BITS_8  | SIGNED,
// //I16       =  INTEGER | BITS_16 | SIGNED,
// //I32       =  INTEGER | BITS_32 | SIGNED,
// I64       =  INTEGER | BITS_64 | SIGNED,
// //F32       =  BITS_32,
// F64       =  BITS_64,
//
//// tU8       =  TABLE | INTEGER | BITS_8,   
////tU16       =  TABLE | INTEGER | BITS_16,
////tU32       =  TABLE | INTEGER | BITS_32,
//tU64       =  TABLE | INTEGER | BITS_64,
//// tI8       =  TABLE | INTEGER | BITS_8  | SIGNED,
////tI16       =  TABLE | INTEGER | BITS_16 | SIGNED,
////tI32       =  TABLE | INTEGER | BITS_32 | SIGNED,
//tI64       =  TABLE | INTEGER | BITS_64 | SIGNED,
////tF32       =  TABLE | BITS_32,
//tF64       =  TABLE | BITS_64,

//template<class N> struct typenum { static const Type num = EMPTY; };
////template<> struct typenum<u8>    { static const Type num =    U8; };
////template<> struct typenum<u16>   { static const Type num =   U16; };
////template<> struct typenum<u32>   { static const Type num =   U32; };
//template<> struct typenum<u64>   { static const Type num =   U64; };
////template<> struct typenum<i8>    { static const Type num =    I8; };
////template<> struct typenum<i16>   { static const Type num =   I16; };
////template<> struct typenum<i32>   { static const Type num =   I32; };
//template<> struct typenum<i64>   { static const Type num =   I64; };
////template<> struct typenum<f32>   { static const Type num =   F32; };
//template<> struct typenum<f64>   { static const Type num =   F64; };
//template<> struct typenum<long>  { static const Type num =   I64; };
//template<> struct typenum<unsigned long> { static const Type num = U64; };
//
////template<> struct typenum<tui8>  { static const ui8 num = tUI8;  }; 
////template<> struct typenum<tui16> { static const ui8 num = tUI16; }; 
////template<> struct typenum<tui32> { static const ui8 num = tUI32; }; 
//template<> struct typenum<tU64> { static const ui8 num = tU64; }; 
////template<> struct typenum<ti8>   { static const ui8 num = tI8;   }; 
////template<> struct typenum<ti16>  { static const ui8 num = tI16;  }; 
////template<> struct typenum<ti32>  { static const ui8 num = tI32;  }; 
//template<> struct typenum<tI64>  { static const ui8 num = tI64;  }; 
////template<> struct typenum<tf32>  { static const ui8 num = tF32;  }; 
//template<> struct typenum<tF64>  { static const ui8 num = tF64;  }; 

//if(!m_mem){ init(0); }
//
//auto prevSz = size();
//if( !(capacity()>prevSz) )
//  if(!expand(true, false)){ return false; }
//
////((T*)(this))[size()] = value;
//
//T* p = (T*)(m_mem);
////*(p + prevSz) = value;
//new (p+prevSz) T(value);
//
//set_size(prevSz+1);
////set_size(size()+1);
//
//return true;

//
// /*delete &(back());*/ todo: needs to run the destructor here

//auto tmp = &sb;
//u8* sbb = (u8*)&sb;
//
//return sb & 0xFFFFFFFFFFFF0000;
//
//u64 sb = *((u64 const*)memStart());
//sb &= 0x0000FFFFFFFFFFFF;
//
//sbb[0] = sbb[1] = 0;
//
//return (sb << 2) >> 2;
//return *( (u64 const*)memStart() );

//tbl_PRNT(" copied ");
//del();
//resize(l.size());
//
// todo: use memcpy instead of copying elements 1 by 1 - can't, need to use copy constructor
//destroy();
//u64 szBytes = l.sizeBytes();
//m_mem = (u8*)malloc( l.sizeBytes() );
//memcpy(m_mem, l.memStart(), szBytes);

//if( !(map_capacity()>elems()) )
//   if(!expand(false, true)){ return KV::error_kv(); }
//
//u32  hsh  =  HashStr(key);    
//KV*   el  =  (KV*)elemStart();                                    // el is a pointer to the elements 
//u64    i  =  hsh;
//u64  mod  =  map_capacity();
//u64   en  =  prev(i,mod);
//u64 dist  =  0;
//for(;;++i,++dist)
//{
//  i %= mod;                                                        // get idx within map_capacity
//  HshType eh = el[i].hsh;                                          // eh is element hash
//  if(el[i].hsh.type==EMPTY){ 
//    set_elems( elems()+1 );
//    return el[i] = KV(key, hsh, *((u64*)&val) );
//  }else if(hsh==eh.hash && strncmp(el[i].key,key,sizeof(KV::Key)-1)==0 ){   // if the hashes aren't the same, the keys can't be the same
//    return el[i] = KV(key, hsh, *((u64*)&val) );
//  }else if( wrapDist(el,i,mod) < dist ){
//    KV kv(key, hsh, *((u64*)&val) );
//    kv.hsh.type = typenum<V>::num;
//    set_elems( elems()+1 );
//    return place_rh(kv, el, i, dist, mod);
//  }
//
//  if(i==en) break;                                                 // nothing found and the end has been reached, time to break out of the loop and return a reference to a KV with its type set to NONE
//}
//
//return KV::error_kv();

// todo: needs to loop through and run destructors here
//if(m_mem) free(memStart());
//
//typed_del<true>::del();
//
//typed_del<true>();
//typed_del<false>();

//u64  mapcap = mapcap; //map_capacity();
//el[i+prevMapCap] = KV();   // KV::empty_kv(); // KV();
//new (&el[i+prevMapCap]) KV();

//KV* ell = elemStart();
//TO(mapcap,i){ 
//  assert(ell[i].hsh.type != 0);
//}

//(u8*)elemStart();
//if(prevElems){
//  KV* prevEl = (KV*)( ((u8*)re) + prevOfst );
//  if(el!=prevEl)TO(prevMapCap,i){
//      KV kv = prevEl[i];
//      el[i] = kv;
//    }
//}

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



