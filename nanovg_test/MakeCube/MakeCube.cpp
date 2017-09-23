

#include <thread>
#include "../no_rt_util.h"
#include "../LavaFlow.hpp"
#include "../tbl.hpp"
#include "../str_util.hpp"

struct vert { f32 p[3],n[3],c[4],tx[2]; };
using IvTbl = tbl<vert>;

void PrintVert(vert const& v)
{
  Print("p: ",    v.p[0],  " ", v.p[1], " ", v.p[2]);
  Print("  n: ",  v.n[0],  " ", v.n[1], " ", v.n[2]);
  Print("  c: ",  v.c[0],  " ", v.c[1], " ", v.c[2], " ", v.c[3]);
  Print("  tx: ", v.tx[0], " ", v.tx[1]);
  Println();
}

void PrintIvTbl(IvTbl const& iv)
{
  Println();
  i8* memst = (i8*)iv.memStart();
  Println("tbl check: ", memst[0]," ",memst[1]); 
  Println("owned: ",            iv.owned(),
          " sizeBytes: ",       iv.sizeBytes(),
          " capacity: ",        iv.capacity(), 
          " size: ",            iv.size(),  "\n",
          " map_capacity: ",    iv.map_capacity(),
          " elems: ",           iv.elems(), "\n",
          " child_capacity: ",  iv.child_capacity(),
          " childData: ",       iv.childData() );

  Println();
  Println("-map-");
  auto e = iv.elemStart();
  TO(iv.map_capacity(),i) if( !e[i].isEmpty() ){
    Println("k: ", e[i].key, "   v: ", e[i].val, "  type: ", e[i].hsh.type, "  hash: ", e[i].hsh.hash);
  }
  Println();

  Println("-array-");
  TO(iv.size(),i){
    PrintVert(iv[i]);
  }
  Println();
}

extern "C"
{
  const char* OutNames[]  = {"Cube Indexed Verts", nullptr};
  const char* OutTypes[]  = {"IdxVerts",           nullptr};

  uint64_t MakeCube(LavaParams* inout_lp, LavaVal* in, LavaOut* out)
  {
    using namespace std;

    printf("\n MakeCube run \n");

    //tbl<f32> idxVerts = tbl<f32>::make_borrowed(inout_lp->mem_alloc, 
    
    // todo:  this will allocate using malloc and realloc - need to change tbl to use allocation, reallocation, and free functions in the type
    IvTbl lftTri = {              // array of vert structs
      {{-1.0, -1.0f, 0.0f},       //pos
      {0.0f, 0.0f, -1.0f},        //norm
      {1.0f, 1.0f, 1.0f, 1.0f},   //color
      {0.0f, 0.0f}},              //texCoord

      {{-0.17f, -1.0f, 0.0f},     //pos
      {0.0f, 0.0f, -1.0f},        //norm
      {1.0f, 1.0f, 1.0f, 1.0f},   //color
      {0.0f, 0.0f}},              //texCoord

      {{-0.58f, 1.0f, 0.0f},      //pos
      {0.0f, 0.0f, -1.0f},        //norm
      {1.0f, 1.0f, 1.0f, 1.0f},   //color
      {0.0f, 0.0f}}               //texCoord
    };

    PrintIvTbl(lftTri);

    auto typenum    =  "IdxVerts";
    lftTri("type")  =  *((u64*)typenum);
    lftTri("mode")  =  (u64)0x0004; // (u64)GL_TRIANGLES;  // #define GL_TRIANGLES 0x0004
    tu32 ind        =  {0, 1, 2};
    lftTri("IND")   =  &ind; 
    lftTri.flatten();

    PrintIvTbl(lftTri);

    inout_lp->outputs = 1;
    out[0] = LavaTblToOut(inout_lp, lftTri);
    out[0].key.slot = 0;

    //void* outmem = inout_lp->mem_alloc(lftTri.sizeBytes());
    //memcpy(outmem, lftTri.memStart(), lftTri.sizeBytes());
    //
    //inout_lp->outputs = 1;
    //out[0].value = (u64)outmem;
    //out[0].type  = LavaArgType::MEMORY;

    this_thread::sleep_for( chrono::milliseconds(500) );

    return 1;
  }

  LavaNode LavaNodes[] =
  {
    {
      MakeCube,                                      // function
      (uint64_t)LavaNode::MSG,                       // node_type  
      "MakeCube",                                    // name
      nullptr,                                       // in_names
      OutNames,                                      // out_names
      nullptr,                                       // in_types 
      OutTypes,                                      // out_types 
      0                                              // version 
    },                                             

    {nullptr, (uint64_t)LavaNode::NONE, nullptr, nullptr, nullptr, nullptr, nullptr, 0}
  };

  __declspec(dllexport) LavaNode* GetLavaNodes()
  {
    return (LavaNode*)LavaNodes;
  }
}








//printf("\n lava heap: %llu \n", (u64)lava_thread_heap);
//LavaHeapInit();
//printf("\n lava heap: %llu \n", (u64)lava_thread_heap);

//tbl<u8> filePath(256);
//memset(filePath.data(), 0, 256);
//const char* pthstr = "H:\\projects\\lava\\lava\\tst.txt";
//auto        pthlen = strlen(pthstr);
//memcpy(filePath.data(), pthstr, pthlen+1);

//printf("\n pthstr: %s \n  pthlen: %d  \n filePath.data(): %s \n\n", pthstr, (int)pthlen, filePath.data());
//
//void* outmem = inout_lp->mem_alloc(filePath.sizeBytes());
//memcpy(outmem, filePath.memStart(), filePath.sizeBytes());
//
//((tbl<u8>)outmem).owned(false);

// tbl_str filePathStr = "C:\\projects\\lava\\";
// auto dbid = simdb.put(nodeId, filePathStr);
// LavaArg out[0] = dbid;
//
//this_thread::sleep_for( chrono::milliseconds(1000) );

//const char* MakeCubeTypes[] = {"tbl_str"};
//  
//const char* InNames[]   = {"File Path", nullptr};
//const char* InTypes[]   = {"tbl_str",   nullptr};

//LavaNode LavaNodes[] =
//{
//  {MakeCube, (uint64_t)LavaNode::MSG,         // function, node_type  
//  0, 1, "MakeCube",                           // inputs, outputs, name                                   
//  nullptr, MakeCubeTypes,                     // in_types, out_types 
//  0, 0},                                         // version, id
//
//  {nullptr, (uint64_t)LavaNode::NONE, 0,0, nullptr, nullptr,nullptr, 0, 0}
//};

//((tbl<u8>*)out[0].value)->owned(false);
//
//free( (void*)out[0].value );
//
//this_thread::sleep_for( chrono::milliseconds(3000) );
//
//free( (void*)out[0].value );
