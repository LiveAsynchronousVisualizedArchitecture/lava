
// todo: try using sys_msvcrt.lib to make a small .dll binary

#include "../../no_rt_util.h"
#include "../../tbl.hpp"
#include "../LavaFlow.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

enum Slots
{  
  FILE_PATH_IN = 0,
  IDXVERTS_OUT = 0
};

extern "C"
{
  const char*  InTypes[]  = {"ASCII",              nullptr};            // This array contains the type that each slot of the same index will accept as input.
  const char*  InNames[]  = {"Path of .obj File",  nullptr};            // This array contains the names of each input slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  const char* OutTypes[]  = {"IdxVerts",           nullptr};            // This array contains the types that are output in each slot of the same index
  const char* OutNames[]  = {"Indexed Verts",      nullptr};            // This array contains the names of each output slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.

  void LoadObj_construct(){ }
  void LoadObj_destruct(){ }

  uint64_t LoadObj(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {
    using namespace std;
    using namespace tinyobj;

    u32 i=0;
    while( LavaNxtPckt(in, &i) )
    {
      attrib_t              attrib;
      vector<shape_t>       shapes;
      SECTION(use the LoadObj lib to get the attribs and shapes we need)
      {
        tbl objFile( (void*)(in->packets[i-1].val.value) );

        vector<material_t> materials;
        string                   err;
        LoadObj( &attrib, &shapes, &materials, &err, (const char*)objFile.data(), nullptr, true);
      }

      tbl ind = LavaMakeTbl(lp);
      tbl  px = LavaMakeTbl(lp);
      tbl  py = LavaMakeTbl(lp);
      tbl  pz = LavaMakeTbl(lp);
      tbl  nx = LavaMakeTbl(lp);
      tbl  ny = LavaMakeTbl(lp);
      tbl  nz = LavaMakeTbl(lp);

      SECTION(set the array types for each vertex component)
      {
        ind.setArrayType<int>();
        px.setArrayType<f32>();
        py.setArrayType<f32>();
        pz.setArrayType<f32>();
        nx.setArrayType<f32>();
        ny.setArrayType<f32>();
        nz.setArrayType<f32>();
      }
      SECTION(loop through the shapes and emit an IdxVerts packet for each one)
      {
        for(auto const& s : shapes)
        {
          SECTION(clear out the component tbls so they can be reused on each iteration)
          {
            px.clear();
            py.clear();
            pz.clear();
            nx.clear();
            ny.clear();
            nz.clear();
          }
          SECTION(loop through the vertices, assuming the floats are orders xyz, then make the ind array)
          {
            auto vrtCnt = attrib.vertices.size() / 3;
            TO(vrtCnt,i){
              auto idx = i * 3;
              px.push( attrib.vertices[idx+0] );
              py.push( attrib.vertices[idx+1] );
              pz.push( attrib.vertices[idx+2] );

              f32 x,y,z;
              x = attrib.normals[idx+0];
              y = attrib.normals[idx+1];
              z = attrib.normals[idx+2];
              f32 len = sqrtf(x*x + y*y + z*z);
              nx.push( x / len );
              ny.push( y / len );
              nz.push( z / len );
            }
            TO(s.mesh.indices.size(),i){
              ind.push( s.mesh.indices[i].vertex_index ); 
            }
          }

          tbl idxVerts = LavaMakeTbl(lp);
          SECTION(add all the component tables to the IdxVerts tbl and flatten it)
          {
            idxVerts("indices")      = &ind;
            idxVerts("positions x")  = &px;
            idxVerts("positions y")  = &py;
            idxVerts("positions z")  = &pz;
            idxVerts("normals x")    = &nx;
            idxVerts("normals y")    = &ny;
            idxVerts("normals z")    = &nz;
            idxVerts("colors red")   = &nx;
            idxVerts("colors green") = &ny;
            idxVerts("colors blue")  = &nz;
            idxVerts("mode")         = (u32)4;
            idxVerts("type")         = tbl::StrToInt("IdxVerts");
            idxVerts.flatten();
          }

          out->push( LavaTblToOut(idxVerts, IDXVERTS_OUT) );
        }
      }
    }

    return 1;
  }

  LavaNode LavaNodes[] =
  {
    {
      LoadObj,                                      // function
      LoadObj_construct,                            // constructor - this can be set to nullptr if not needed
      LoadObj_destruct,                             // destructor  - this can also be set to nullptr 
      LavaNode::FLOW,                                // node_type   - this should be eighther LavaNode::MSG (will be run even without input packets) or LavaNode::FLOW (will be run only when at least one packet is available for input)
      "LoadObj",                                    // name
      InTypes,                                       // in_types    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      InNames,                                       // in_names    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      OutTypes,                                      // out_types   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      OutNames,                                      // out_names   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr
      nullptr,                                       // description
      0                                              // version 
    },                                             

    LavaNodeListEnd                                  // This is a constant that has all the members of the LavaNode struct set to 0 or nullptr - it is used as a special value that ends the static list of LavaNodes. This negates the need for a separate static variable that gives the size of the list, which would be have to be kept in sync and therefore be error prone.
  };

  __declspec(dllexport) LavaNode* GetLavaNodes()
  {
    return (LavaNode*)LavaNodes;
  }
}

//for(auto& kv : inputTbl){  // this loop demonstrates how to iterate through non-empty map elements
//}	
//
// out->push( LavaTblToOut(outputTbl, SLOT_0) );      // this demonstrates how to output a tbl into the first output slot

//str fileStr(  );
//
//ind.reserve(1,0);
//ind.memStart()->arrayType = tbl::TblType::I32;
//
//px.reserve(1,0);
//px.memStart()->arrayType = tbl::TblType::F32;
//
//py.reserve(1,0);
//py.memStart()->arrayType = tbl::TblType::F32;
//
//pz.reserve(1,0);
//pz.memStart()->arrayType = tbl::TblType::F32;
