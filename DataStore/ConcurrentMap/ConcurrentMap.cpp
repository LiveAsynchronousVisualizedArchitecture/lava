

#include <stdint.h>
#include <atomic>
#include <mutex>
#include <vector>
#include <string>

#include <SIM/SIM_GeneralTemplateUtil.hpp>
#include <SIM/SIM_Concurrency.hpp>

//#include "ConcurrentMap.hpp"

#include "ConcurrentMap.hpp"

ui32       intHash(ui32    h)
{
  //h += 1;
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;
  return h;
}
ui32  nextPowerOf2(ui32    v)
{
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;

  return v;
}


int main()
{
  using namespace std;
  
  //ui32 sz = 18921703;
  //ui32 sz = 400;
  //ConcurrentMap cm(sz);

  //ScopeTime t;
  //t.start();
  //cm.init(sz);
  //t.stop("Init");

  //Println( (i64)t.stop() );
  //t.start();
  //
  //Println("sz: ", cm.size());
  //
  //TO(100,i) {
  //  Println("i: ",i," ", intHash(i) );
  //}
  //
  //TO(100,i) {
  //  Println("i: ",i," ", nextPowerOf2(i));
  //}


  //ui32 loopSz  =  (ui32)( double(cm.size()) / 1.5);

  RngInt<int> rng(1, 2);

  //vec<thread> thrds;
  //TO(5,tid)
  //{
  //  thrds.push_back( thread([&cm, &rng, loopSz, tid]()
  //  {
  //    ScopeTime t;
  //
  //    t.start();
  //    TO(loopSz, i) {
  //      auto val = i*10 + tid;
  //      ui32 pidx = cm.put(i, val);
  //      SleepMs( rng() );
  //
  //      //SleepMs( (int)pow(4-tid,2) );
  //      //cout << pidx << "  ";
  //      //Println("Put Idx: ", (i64)pidx);
  //    }
  //    t.stop("Put");
  //    //Println( t.getSeconds() );
  //
  //    //t.start();
  //    //TO(loopSz, i) {
  //    //  ui32 gidx = cm.get(i);
  //    //  cout << gidx << "  ";
  //    //  //Println("Get Idx: ", (i64)gidx);
  //    //}
  //    //t.stop("Get");
  //    //Println( t.getSeconds() );
  //  })); // .detach();
  //  //thrds.back().detach();
  //}
  ////for(auto& th : thrds) th.detach();
  //for(auto& th : thrds) th.join();

  // test getting back from the map
  //t.start();
  //TO(loopSz, i) {
  //  ui32 gidx = cm.get(i);
  //  cout << gidx << "  ";
  //  //Println("Get Idx: ", (i64)gidx);
  //}
  //Println();
  //t.stop("Get");

  //RngInt<int> rngb(0,1);
  //RngInt<int> rngl(0,loopSz-1);
  //ConcurrentList cl(loopSz);

  //// serial test of ConcurrentList
  ////t.start();
  ////TO(loopSz, i){
  ////  Print(cl.idx(),":", cl.alloc(), " ");
  ////}
  ////TO(loopSz, i){
  ////  Print(cl.idx(),":", cl.free(i), " ");
  ////}
  ////Println();
  ////auto lv = cl.list();
  ////TO(lv->size(), i){
  ////  Print( i,":",(*lv)[i], " ");
  ////}
  ////Println();
  ////t.stop("List");

  //Println("\nLinks: ", cl.lnkCnt(), " ");
  //vec<thread> thrds;
  //TO(12,tid)
  //{
  //  thrds.push_back( thread([&cl, &rngb, &rngl, loopSz, tid]()
  //  {
  //    ScopeTime t;

  //    t.start();
  //    TO(loopSz/5, i){
  //      //if(rngb()) 
  //      Print(tid,":",cl.nxt()," ");
  //      //else       Print(tid,":",cl.free(rngl()), " ");
  //      SleepMs( rngb() );
  //    }
  //    t.stop("alloc/free");
  //  }));
  //}
  //for(auto& th : thrds) th.join();

  //Println();
  //auto lv = cl.list();
  //TO(lv->size(), i){
  //  Print( i,":",(*lv)[i], " ");
  //}
  //Println();
  //Println("\nLinks: ", cl.lnkCnt(), " ");



  //i32 blkSz  = 5;
  //i32 blocks = 2;
  //vec<ui8> mem(blocks*blkSz, 0);
  //ConcurrentStore cs(mem.data(), blkSz, (ui32)(blocks) );
  //
  //Println("\n");
  //
  //TO(cs.m_cl.list()->size(), i){
  //  Println( (*cs.m_cl.list())[i] );
  //}
  //Println("\n\n");
  //
  //TO(2,i){
  //  i32  blks = 0;
  //  auto    s = "w";
  //  i32  slen = (i32)strlen(s)+1;
  //  //i32  slen = 1;
  //  auto  idx = cs.alloc(slen, &blks);   // must allocate the exact number of bytes and no more, since that number will be used to read and write
  //  cs.put(idx, (void*)s, slen);
  //
  //  vec<char> gs(slen,0);
  //  cs.get(idx, gs.data(), slen);
  //  Println(gs.data());
  //  cs.free(idx);
  //
  //  TO(cs.m_blockCount, b){
  //    Println(cs.nxtBlock(b));
  //  }
  //  Println("\n\n");
  //  TO(cs.m_cl.list()->size(), i){
  //    Println( (*cs.m_cl.list())[i] );
  //  }
  //  Println("\n\n");
  //
  //}

    struct keyval
    {
      uint64_t  readers  :   4;
      uint64_t      key  :  28;
      uint64_t      val  :  28;
    };


  ConcurrentHash ch(8);

  Println("kv size: ", sizeof(ConcurrentHash::kv) );
  Println("kv size: ", sizeof(keyval) );
  Println("ui64 size: ", sizeof(ui64) );
  TO(8,h)
  {
    Println(h,": ", ch.put(h, h*h) );
    Println(h,": ", ch.get(h) );
  }

  PAUSE

  return 0;
}
















//Aui32*  key_addr  =  (Aui32*)(&m_keys.get()[i]);
//ui32   probedKey  =  atomic_load( key_addr );                                 
//atomic_store( (Aui32*)(&m_vals.get()[i]), val);

//ui32 i  =  0;
//for(; i < m_sz; ++i)

//copy( m_keys.get(), m_keys.get()+sz, EMPTY_KEY );
//copy( m_keys.get(), m_keys.get()+sz, EMPTY_KEY );

//m_keys.get()[i]  =  EMPTY_KEY;
//m_vals.get()[i]  =  EMPTY_KEY;

//~ConcurrentMap()
//{
//  //m_mut.unlock(); // if this is already unlocked, will this crash?
//}

//using Keys      =  std::vector<ui32>;
//using Values    =  std::vector<ui32>;

//ui32     put(ui32 key, ui32 val) const
//{
//  using namespace std;
//
//  ui32 empty_ref = EMPTY_KEY;
//  
//  for(ui32 i=0; true; ++i)
//  {
//    ui32 prevKey = m_keys[i].compare_exchange_strong(empty_ref,key);   // mint_compare_exchange_strong_32_relaxed(&m_entries[idx].key, 0, key);
//    if( (prevKey==0) || (prevKey==key))
//    {
//      m_vals[i].store(val);
//      return i;
//    }
//  }
//}

