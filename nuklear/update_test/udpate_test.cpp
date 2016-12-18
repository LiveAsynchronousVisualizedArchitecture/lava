
#include "../IndexedVerts.cpp"               // todo: make IndexedVerts into an hpp file or a single header file style lib

#include <thread>
#include <chrono>
#include "update_test_data.hpp"
#include "update_test_generators.hpp"

namespace {

using std::thread;
using std::chrono::duration;
using std::chrono::milliseconds;
using std::this_thread::sleep_for;
using ms = duration<double, std::milli>;

}

int    main(void)
{
  new (&db) simdb("test", 1024, 64);        // inititialize the DB with placement new into the data segment
  
  ui64 rightLen, cubeLen;
  vec<ui8> rightData = makeTriangle(rightLen, false);
  vec<ui8>  cubeData = makeCube(cubeLen);

  while(true)
  {
    sleep_for( milliseconds(1) );
    db.put("shape", cubeData);

    sleep_for( milliseconds(1) );    
    db.put("shape", rightData);
  }
  
  return 0;
}




//using std::this_thread;
//
//void       genTestGeo(simdb* db)
//{
//  // Create serialized IndexedVerts
//  size_t leftLen, rightLen, cubeLen;
//  vec<ui8>  leftData = makeTriangle(leftLen,   true);
//  vec<ui8> rightData = makeTriangle(rightLen, false);
//  vec<ui8>  cubeData = makeCube(cubeLen);
//
//  // Store serialized IndexedVerts in the db
//  str  leftTriangle = "leftTriangle";
//  str rightTriangle = "rightTriangle";
//  str          cube = "cube";
//
//  db->put(leftTriangle,   leftData);
//  db->put(rightTriangle, rightData);
//  db->put(cube, cubeData);
//}

//
//genTestGeo(&db);

//#include <map>
//using std::map;
//using std::vector;

//map<int,vector<int>> m;
//for(int i=0; i<10000000; ++i){
//  new (&m) map<int,vector<int>>();
//  m[0] = vector<int>(1000);
//}
