
#include <iostream>
#include <cstdio>

#include "tbl.hpp"

int main()
{
  using namespace std;
  
  //printf("\n variant size: %d \n\n\n", sizeof(tbl::Var) );
  //printf("\n");
  //printf("kv size: %d \n", sizeof(tbl::kv) );
  
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
  printf("\n 1<<: %d \n\n", 1<<4 );

  //TO(1000000, leak)
  {
    tbl t;
    t("wat") = 185;
    int wat = t("wat");
    printf("\n wat: %d elems: %d \n\n", wat, t.elems() );

    //SECTION(vector testing)
    //{
    //  TO(5,i) t.push( (int)i );
    //  //TO(t,i){ printf(" %d \n", t[i] ); } 
    //  TO(t-2,i){
    //    printf(" %d \n", t.back() );
    //    t.pop();
    //  } 
    //  printf("\n", t.back() );
    //
    //  tbl t2;
    //  TO(5,i) t2.push( (int)(i*i) );
    //  tbl t3 = t >> t2;
    //  auto t4 = t >> t2;
    //
    //  TO(t3,i){ printf(" %d \n", t3.back() ); t3.pop(); } 
    //}
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


