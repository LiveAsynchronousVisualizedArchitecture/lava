
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "../../no_rt_util.h"
#include "../../tbl.hpp"
#include "../LavaFlow.hpp"

enum Slots
{
  IN_FILE_PATH  = 0,
  OUT_IMAGE_IV  = 0
};

extern "C"
{
  const char* description = "Loads an image given the input file path";
  const char*  InTypes[]  = {"ASCII",                 nullptr};
  const char*  InNames[]  = {"File Path",             nullptr};
  const char* OutTypes[]  = {"IdxVerts",              nullptr};
  const char* OutNames[]  = {"Image Visualization",   nullptr};

  void ImageLoader_construct(){ }
  void ImageLoader_destruct(){ }

  uint64_t ImageLoader(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {
    using namespace std;

    if(in->slotMask[IN_FILE_PATH]){
      tbl pthTbl = LavaTblFromPckt(lp, in, IN_FILE_PATH);
      str    pth( pthTbl.data<char>() );

      int w=0,h=0,chans=0;
      stbi_uc* img = stbi_load( pth.c_str(), &w, &h, &chans, 0);

      u64 sz = w * h * chans;
      tbl it = LavaMakeTbl(lp, sz, 1.f);    // it is image tbl
      it("width")    = (u64)w;
      it("height")   = (u64)h;
      it("channels") = (u64)chans;
      TO(sz,i){
        it[i] = (f32)(img[i]) / 255.f;
      }
      stbi_image_free(img);

      
      tbl     one = LavaMakeTbl(lp, 4, 1.f);
      tbl indices = LavaMakeTbl(lp, 4, (u32)0);
      tbl      px = LavaMakeTbl(lp, 4, 0.f);
      tbl      py = LavaMakeTbl(lp, 4, 0.f);
      
      px[1] = 1.f;
      px[2] = 1.f;

      py[2] = 1.f;
      py[3] = 1.f;

      TO(4,i){
        indices[i] = (u32)i;
      }

      tbl iv = LavaMakeTbl(lp, 0, (i8)0);    // iv is IdxVerts
      //iv("mode") = (u32)7;                   // GL_QUADS
      iv("mode") = (u32)0;                   // GL_POINTS
      iv("type") = tbl::StrToInt("IdxVerts");
      
      iv("positions x")  = &px;
      iv("positions y")  = &py;
      iv("texture coordinates x") = &px;
      iv("texture coordinates y") = &py;
      iv("colors red")   = &one;
      iv("colors green") = &one;
      iv("colors blue")  = &one;
      iv("colors alpha") = &one;
      iv("indices")      = &indices;
      iv("image")        = &it;
      iv.flatten();

      out->push( LavaTblToOut(move(iv), OUT_IMAGE_IV) );
    }

    return 1;
  }

  LavaNode LavaNodes[] =
  {
    {
      ImageLoader,                                      // function
      ImageLoader_construct,                            // constructor - this can be set to nullptr if not needed
      ImageLoader_destruct,                             // destructor  - this can also be set to nullptr 
      LavaNode::FLOW,                                   // node_type   - this should be eighther LavaNode::MSG (will be run even without input packets) or LavaNode::FLOW (will be run only when at least one packet is available for input)
      "ImageLoader",                                    // name
      InTypes,                                          // in_types    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      InNames,                                          // in_names    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      OutTypes,                                         // out_types   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      OutNames,                                         // out_names   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr
      description,                                      // description
      0                                                 // version 
    },                                             

    LavaNodeListEnd                                  // This is a constant that has all the members of the LavaNode struct set to 0 or nullptr - it is used as a special value that ends the static list of LavaNodes. This negates the need for a separate static variable that gives the size of the list, which would be have to be kept in sync and therefore be error prone.
  };

  __declspec(dllexport) LavaNode* GetLavaNodes()
  {
    return (LavaNode*)LavaNodes;
  }
}



//u32 i=0;
//while( LavaNxtPckt(in, &i) )
//{
//  tbl inputTbl( (void*)(in->packets[i-1].val.value) );
//
//  for(auto& kv : inputTbl){  // this loop demonstrates how to iterate through non-empty map elements
//  }	
//
//  // out->push( LavaTblToOut(outputTbl, SLOT_OUT) );      // this demonstrates how to output a tbl into the first output slot
//}
