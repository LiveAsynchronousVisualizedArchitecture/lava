
#include <cstdio>
#include "../no_rt_util.h"
#include "lava_priq.hpp"

int  main()
{
  using namespace std;

  lava_priq pq(12);
  pq.put(55);
  pq.put(44);
  pq.put(22);
  pq.put(88);
  pq.put(99);
  pq.put(33);
  pq.put(11);
  pq.put(66);
  pq.put(77);

  ui64 exp = 0;
  TO(pq.q.size(),i){
    printf(" %d ", pq.q[i]);
  }
  printf("\n\n\n");

  TO(pq.q.size(),i){
    printf(" %d ", pq.take());
  }
  printf("\n\n\n");

  TO(pq.q.size(),i){
    printf(" %d ", pq.q[i]);
  }
  printf("\n\n\n");

  scanf_s("%d");
  return 0;
}




//if(i==0){
//  printf("\n");
//  exp = 1;
//}else if(i>>exp){
//  exp = exp << exp;
//  printf("\n");
//}
//if( i%(1ul<<exp) == 0){
//  ++exp;
//  printf("\n");
//}
