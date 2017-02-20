
#include <iostream>
#include <cstdio>

#include "tbl.hpp"

int main()
{
  using namespace std;
  
  //void* wat =  malloc(100);
  //void*  re = realloc(wat, (ui64)1 << 9);
  //
  //printf("wat: %p \nre : %p \n", wat, re);
  
  //tbl t(10);
  //
  //for(ui64 i=0; i<t; ++i) t[i] = (int)i;

  tbl t;
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

  TO(10,i) t.push( (int)i );
  TO(t,i){ printf(" %d \n", t.back() ); t.pop(); } 

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


