

#ifndef __LAVA_NODE_HEADER_GUARD__
#define __LAVA_NODE_HEADER_GUARD__

#include <cstdint>

extern "C"
{

enum LavaTypes
{
  LAVA_EMPTY     =  0,
  LAVA_IDX_VRTS,
  LAVA_STRING, 
  LAVA_TBL  
};

struct LavaNode;
using  DoTransform     =  void*(*)(void* in);      // void** out);
using  GetLavaNodes_t  =  LavaNode*(*)();          // A function that returns a pointer to a LavaNode list. The last node is null, which makes the list similiar to a C string. Looping through is easy, though random access is not possible without first looping through to get the length.

struct LavaArgs
{
  void*  in; 
  void* out;
  uint64_t in_len, out_len;
};

union LavaData
{
  uint64_t  pack :  1;
  uint64_t  type : 17;                             // 131 thousand types - use a type map of strings to indices ?
  uint64_t   idx : 46;
};


struct LavaNode
{
  const char*       in_type;
  const char*      out_type;
  const char*          name;
  DoTransform  do_transform;
  size_t            version;
  size_t                 id;
};

}

inline void* DoTransformIfNotNull(LavaNode* t, void* in) // void** out)
{
  if(t && t->do_transform)
    return t->do_transform(in); //out);
}


#endif 



//union LavaData                                     // length not needed because it is stored in the database? should be 128 bits total? 
//{
//struct{ uint8_t bytes[16]; };
//
//struct{ uint64_t idx : 48; };
//
//struct{ uint8_t _pad[6]; uint64_t len : 48; uint32_t type; };
//
////struct{
////  uint64_t  idx : 48;
////  uint64_t  len : 48;
////  uint64_t type : 32;
////};
//
//};
