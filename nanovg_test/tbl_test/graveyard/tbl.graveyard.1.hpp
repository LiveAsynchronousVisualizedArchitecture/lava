

//if(prevBytes<nxtBytes) 
//  memset(re, 0, nxtBytes);
  //memset( (i8*)re + prevBytes, 0, nxtBytes-prevBytes );
//
  //set_elems(0);
    //ui64   len = prevMapCap*sizeof(KV);
    //memmove(el, prevEl, len);
//
//TO(map_capacity(),i) new (&el[i]) KV();

//ui64  i  = 0;
//ui64 en  = prev(i,mapcap);
//ui64 cnt = 0; 
//do{
//  ui64 nxti = nxt(i,mapcap); 
//  if(el[i].hsh.type != EMPTY){
//    KV kv  = el[i];
//    el[i]  = KV();
//    //ui64 p = place(kv,el,mapcap);
//    ui64 st = kv.hsh.hash%mapcap;
//    ui64  p = i; 
//    place_rh(kv,el,st,0,mapcap,&p);
//    if(p!=i) en = i;
//  }
//  i = nxti;
//  ++cnt;
//}while(i!=en);

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

//if(!m_mem) return 0;
//
//auto    sz = sizeBytes();
//auto membr = memberBytes();
//auto   cap = capacity();
//auto   szT = sizeof(T);
//auto  szKV = sizeof(KV);
//return (sz - membr - cap*szT) / szKV;

//
//*( ((ui64*)memStart()) + 4) = cap;

//if(el[i].hsh.type==EMPTY) return -1;
//ui64 idl = ideal(i);
//while(el[i].hsh.type!=EMPTY && cnt!=dst){

//ui64  dist  =  0;
// ,++dist
//
//}else if(ideal(i)<dist){
//  return i;

//bool             del(ui64 i)
//{ 
//  //ui64 i = find(key);
//  KV* el = elemStart();
//  if(el[i].hsh.type==EMPTY) return false;
//
//  el[i] = KV();
//  ui64 mapcap = map_capacity();
//  ui64     en = prev(i, mapcap);
//  //compact(i, en, mapcap);
//
//  return true;
//}

//void* el = elemStart();
//memcpy(nxtp+vecsz, el, mapsz);

//i64            place(KV kv, KV* elems, ui64 mod)
//{
//  ui64  i = kv.hsh.hash % mod;
//  ui64 en = prev(i,mod);
//  while(elems[i].hsh.type!=EMPTY){
//    if(i==en) return -1;
//    i = nxt(i,mod);
//  }
//
//  elems[i] = kv;
//  return i;
//}

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



