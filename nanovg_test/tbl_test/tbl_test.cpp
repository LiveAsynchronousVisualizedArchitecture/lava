
#include <iostream>

#include <cstdlib>
#include <cstdio>
#include "../no_rt_util.h"

class tbl
{
public:
  using T = int;

  //ui64   m_size;
  i8*     m_mem;
 
  tbl() : 
    //m_size(0),
    m_mem(nullptr)
  {
  }
  tbl(ui64 count) //: 
    //m_size(count),
  {
    ui64  cntBytes = count*sizeof(T);
    ui64  mbrBytes = sizeof(ui64) * 2;                     // mbrBytes is member variable bytes
    ui64 sizeBytes = cntBytes + sizeof(ui64)*2;
    i8*      memst = (i8*)malloc(sizeBytes);               // memst is memory start
    *((ui64*)memst) = sizeBytes;
    *(((ui64*)memst)+1) = count;
    m_mem = memst + mbrBytes;
  }

  operator ui64() const
  {
    return size();
  }
  T& operator[](ui64 i)
  {
    return ((T*)m_mem)[i];
  }
  ui64 size() const
  {
    //return m_size;
    return *(((ui64*)m_mem)-1);
  }
};

int main()
{
  using namespace std;
  
  void* wat =  malloc(100);
  void*  re = realloc(wat, (ui64)1 << 12);

  printf("wat: %p \nre : %p \n", wat, re);

  tbl t(10);
  TO(t,i) t[i] = (int)i;
  //for(ui64 i=0; i<t; ++i) t[i] = (int)i;
  TO(t,i) printf(" %d \n", t[i]);


  int a; cin >> a;
  return 0;
}

