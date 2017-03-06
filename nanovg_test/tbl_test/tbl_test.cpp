
#include <iostream>
#include <cstdio>

#include "tbl.hpp"

void prnt_elems(tbl& t)
{
  tbl::KV* kv = t.elemStart();
  TO(t.map_capacity(), i) printf(" %s:%d:%d:%d ", kv[i].key, (int)(kv[i].val), kv[i].hsh.type, t.distance(i) );
  printf("\n\n");
}

int main()
{
  using namespace std;
  
  //printf("\n variant size: %d \n\n\n", sizeof(tbl::Var) );
  //printf("\n");
  printf("KV size: %d \n", sizeof(tbl::KV) );
  
  printf("\n");

  //SECTION(assigning a double and extracting a float fails)
  //{
  //  tbl::kv num;
  //  num    =  85.0;
  //  //ui32 n = num; // assert fails
  //  //f32 n = (f32)(num.as<f32>());
  //  f32 n  =  num.as<f32>();
  //  printf("int:   %d   \n",  n);
  //  printf("float: %.5f \n",  n);
  //  printf("\n");
  //}

  //SECTION(assigning a ui16 and extrating a i32 fails for now)
  //{
  //  tbl::kv num;
  //  num     =  (ui16)85;
  //  ui32 n  =  num;
  //  printf("int:   %d   \n",  n);
  //  printf("\n");
  //}

  //SECTION(same types)
  //{
  //  tbl::KV num;
  //  num    =  85;
  //  i32 n  =  num;
  //  printf("int:   %d   \n",  n);
  //  printf("\n");
  //}


  //void* wat =  malloc(100);
  //void*  re = realloc(wat, (ui64)1 << 9);
  //
  //printf("wat: %p \nre : %p \n", wat, re);
  
  //tbl t(10);
  //
  //for(ui64 i=0; i<t; ++i) t[i] = (int)i;

  //printf(" %d %lld %lld %lld \n", 0, t.size(), t.capacity(), t.sizeBytes() );
  //t.push( (int)85 );
  //printf(" %d %lld %lld %lld \n", t[0], t.size(), t.capacity(), t.sizeBytes() );
  //t.expand();
  //printf(" %d %lld %lld %lld \n", t[0], t.size(), t.capacity(), t.sizeBytes() );
  //t.push( (int)86 );

  //TO(6,i){
  //  t.push( (int)i );
  //  if(i==1) t.expand();
  //  printf("%d %lld %lld %lld \n", t[i], t.size(), t.capacity(), t.sizeBytes() );
  //  TO(t,j) printf("     %d %lld %lld %lld \n", t[j], t.size(), t.capacity(), t.sizeBytes() );
  //}

  //tbl nul;
  //if(nul) printf("\n bool: %d \n\n", (bool)nul );
  //int bin = 0b10000;
  //printf("\n 1<<: %d \n\n", 1<<4 );

  //SECTION(test map and vector inserts)
  ////TO(1000000, leak)
  //{
  //  tbl t;
  //  printf("\n has wat: %d \n\n", t.has("wat") );
  //  //t("wat") = 185;
  //  t.insert("wat", 185);
  //  int wat = t("wat");
  //  printf("\n wat: %d elems: %d \n\n", wat, t.elems() );
  //  printf("\n has wat: %d \n\n", t.has("wat") );
  //  //printf("\n EMPTY: %d NONE: %d ERROR: %d \n\n", tbl::EMPTY, tbl::NONE, tbl::ERROR );
  //
  //  //SECTION(vector testing)
  //  //{
  //  //  TO(5,i) t.push( (int)i );
  //  //  //TO(t,i){ printf(" %d \n", t[i] ); } 
  //  //  TO(t-2,i){
  //  //    printf(" %d \n", t.back() );
  //  //    t.pop();
  //  //  } 
  //  //  printf("\n", t.back() );
  //  //
  //  //  tbl t2;
  //  //  TO(5,i) t2.push( (int)(i*i) );
  //  //  tbl t3 = t >> t2;
  //  //  auto t4 = t >> t2;
  //  //
  //  //  TO(t3,i){ printf(" %d \n", t3.back() ); t3.pop(); } 
  //  //}
  //}

  //SECTION(test implicit casting to higher bit depths)
  //{
  //  tbl t;
  //
  //  t("a") = (ui8)85;
  //  int a = t("a");
  //  printf("\n a: %d \n\n", a);
  //
  //  t("b") = (ui16)85;
  //  int b = t("b");
  //  printf("\n b: %d \n\n", b);
  //
  //  t("c") = (ui32)85;
  //  i64 c  = t("c");
  //  printf("\n c: %d \n\n", c);
  //
  //  t("e") = 85.0f;
  //  double e = t("e");
  //  printf("\n e: %.3f \n\n", e);
  //
  //  //t("f") = 85.0;                      // fails in debug mode, prints 0 in release mode
  //  //i64 f = t("f");
  //  //printf("\n f: %d \n\n", f);
  //
  //  //t("g") = -85;                         // fails in debug mode, prints a huge unsigned number in release mode
  //  //ui64 g = t("g");
  //  //printf("\n g: %ull \n\n", g);
  //}

  //SECTION(test default value for vector)
  //{
  //  TO(100000000, leak)
  //  {
  //    tbl t(10, 85);
  //    //TO(t,i) printf(" %d ", t[i]);
  //  }
  //}

  //SECTION(test math operators across vector)
  //{
  //  tbl a(7); TO(a,i) a[i] = (int)(i+1);
  //  tbl b(5); TO(b,i) b[i] = (int)((i+1)*(i+1));
  //  TO(a,i) printf(" %d ", a[i]);
  //  printf("\n\n");
  //  TO(b,i) printf(" %d ", b[i]);
  //  printf("\n\n");
  //
  //  tbl c = b % a;
  //
  //  a %= b;
  //  TO(a,i) printf(" %d ", a[i]);
  //  printf("\n\n");
  //
  //  TO(c,i) printf(" %d ", c[i]);
  //  printf("\n\n");
  //}

  //SECTION(test math operators with single left side number)
  //{
  //  tbl a(7); TO(a,i) a[i] = (int)(i+1);
  //  tbl b(5); TO(b,i) b[i] = (int)((i+1)*(i+1));
  //  TO(a,i) printf(" %d ", a[i]);
  //  printf("\n\n");
  //  TO(b,i) printf(" %d ", b[i]);
  //  printf("\n\n");
  //
  //  tbl c = b % 10;
  //
  //  a %= 100;
  //  TO(a,i) printf(" %d ", a[i]);
  //  printf("\n\n");
  //
  //  TO(c,i) printf(" %d ", c[i]);
  //  printf("\n\n");
  //}

  //SECTION(test out of range debug assertion)
  //{
  //  tbl t(5);
  //  TO(t+1,i) printf(" %d ", t[i]);
  //}

  SECTION(test reserve placement)
  {
    tbl::KV* kv;
    
    tbl t;
    t("0") = 85;
    t("1") = 85;
    t("2") = 85;
    t("3") = 85;
    //t("zero")  = 85;
    //t("one")   = 85;
    //t("two")   = 85;
    //t("three") = 85;
    prnt_elems(t);
    //kv = t.elemStart();
    //TO(t.map_capacity(), i) printf(" %s:%d:%d:%d ", kv[i].key, (int)(kv[i].val), kv[i].hsh.type, t.distance(i) );
    //printf("\n\n");

    //t.reserve(0,5);
    t.expand();
    //////t.expand();
    //////t("4") = 4;
    //////t("5") = 5;
    prnt_elems(t);
    //kv = t.elemStart();
    //TO(t.map_capacity(), i) printf(" %s:%d:%d ", kv[i].key, (int)(kv[i].val), kv[i].hsh.type );
    //printf("\n\n");

    tbl t2;
    //t2("0")   = 0;
    //t2("1")   = 1;
    t2("2")   = 2;
    //t2("3")   = 3;
    //t2("zero")  = 0;
    //t2("one")   = 1;
    //t2("two")   = 2;
    ////t2("2")   = 2;
    //t2("three") = 3;
    //t2("four")  = 4;
    //t2("five")  = 5;
    //t2("six")   = 6;
    //kv = t2.elemStart();
    //TO(t2.map_capacity(), i) printf(" %s:%d:%d ", kv[i].key, (int)(kv[i].val), kv[i].hsh.type );
    //printf("\n\n");

    tbl t3 = t << t2;

    prnt_elems(t3);
    //kv = t3.elemStart();
    //TO(t3.map_capacity(), i) printf(" %s:%d:%d:%d ", kv[i].key, (int)(kv[i].val), kv[i].hsh.type, t3.distance(i)  );
    //printf("\n\n");

    //printf(" %d ", (int)t("one hundred") );   // for testing out an error on NONE type

    //int _1 = t("1");
    //printf(" mapcap: %d  %d  %d  %d ", t.map_capacity(), _1, (int)t("2"), (int)t("3") );

    //kv = t.elemStart();
    //TO(t.map_capacity(), i) printf(" %s:%d ", kv[i].key, (int)(kv[i].val) );
    //printf("\n\n");

    //t.reserve(0,23);
    //kv = t.elemStart();
    //TO(t.map_capacity(), i) printf(" %s:%d ", kv[i].key, (int)(kv[i].val) );
    //printf("\n\n");

    //int _1 = t("1");
    //printf(" mapcap: %d  %d  %d  %d ", t.map_capacity(), _1, (int)t("2"), (int)t("3") );
  }

  //float*  pos = t("pos").data();
  //float* norm = t("normals").data();

  int a; cin >> a;
  return 0;
}






//return m_size;
//return count(); //*(((ui64*)m_mem)-1);

//ui64  mbrBytes  = sizeof(ui64) * 2;                     // mbrBytes is member variable bytes
//
//*((ui64*)memst) = szBytes;
//*(((ui64*)memst)+1) = count;

//ui64   m_size;
//
//m_size(0),


