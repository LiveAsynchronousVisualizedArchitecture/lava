
#include <vector>
#include <algorithm>

class lava_priq
{
public:
  using veci = std::vector<int>;

  const int MX = std::numeric_limits<int>::max(); 

  veci  q;
  int lst;

  lava_priq(int sz) :
    q(sz,MX),
    lst(0)
  {}

  void put(int n)
  {
    using namespace std;
    
    //if(n>q[0]){
    //  swap(n, q[0]);
    //}
    //
    //int   i = 1;
    ////int nxt = i << 1;
    //while(i<q.size() && n>q[i]){
    //  swap(n,q[i]);
    //  i = i << 1;
    //  //nxt = i << 1;
    //}

    int i   = lst++;
    //if(i==0) return;
    /*par>0 && */

    q[i]    = n;
    int par = i/2;
    while(q[par]>q[i]){
      swap(q[par],q[i]);
      if(i==0) return; 
      i    = par;
      par /= 2;
    }
  }

  int take()
  {
    using namespace std;

    int ret = q[0];
    if(lst==0){ q[lst]=MX; return ret; }
    q[0]    = q[--lst];
    q[lst]  = MX;

    int i = 0;
    do{
      int   l = i*2+1;
      int   r = l+1;
      if( r>=q.size() ) break;

      int mni = q[l]<q[r]? l : r;
      if(q[mni]<q[i]){
        swap(q[mni], q[i]);
        i = mni;
      }else 
        break;
    }while(true);

    return ret;
  }

};

