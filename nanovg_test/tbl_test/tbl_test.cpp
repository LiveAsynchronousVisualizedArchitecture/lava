
#include <iostream>
#include <cstdio>

#include "tbl.hpp"

int main()
{
  using namespace std;
  
  //printf("\n variant size: %d \n\n\n", sizeof(tbl::Var) );
  printf("\n");
  printf("kv size: %d \n", sizeof(tbl::kv) );
  printf("\n");

  tbl::kv num;
  num    =  85.0f;  // 85.0f;
  //ui32 n = num; // assert fails
  f32 n = num;
  printf("int:   %d   \n",  n);
  printf("float: %.5f \n",  n);
  printf("\n");

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

  tbl t;
  TO(5,i) t.push( (int)i );
  //TO(t,i){ printf(" %d \n", t.back() ); t.pop(); } 

  tbl t2;
  TO(5,i) t2.push( (int)(i*i) );
  tbl t3 = t >> t2;
  TO(t3,i){ printf(" %d \n", t3.back() ); t3.pop(); } 

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


