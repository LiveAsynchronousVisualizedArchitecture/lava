

#ifndef __LAVA_NODE_HEADER_GUARD__
#define __LAVA_NODE_HEADER_GUARD__

#include <cstdint>

extern "C"
{

enum   LavaTypes
{
  LAVA_EMPTY     =  0,
  LAVA_IDX_VRTS,
  LAVA_STRING, 
  LAVA_TBL  
};

struct LavaNode;
struct  LavaArg                                       // idx is an array of indices and len is the number of indices - the data store will have the version number and length of each individual index
{
  uint64_t* idx;
  uint64_t  len;
};
struct LavaData
{
  uint64_t  pack :  1;
  uint64_t  type : 17;                             // 131 thousand types - use a type map of strings to indices ?
  uint64_t   idx : 46;
};

//struct LavaArg;
//using  LavaFunc        =  void*(*)(void* in);      // void** out);

using  LavaFunc        =  LavaArg*(*)(LavaArg* in);  // void** out);
using  GetLavaNodes_t  =  LavaNode*(*)();            // A function that returns a pointer to a LavaNode list. The last node is null, which makes the list similiar to a C string. Looping through is easy, though random access is not possible without first looping through to get the length.

struct LavaNode
{
  LavaFunc             func;
  const char*          name;
  uint8_t            inputs;
  const char**     in_types;
  uint8_t           outputs;
  const char**    out_types;
  uint64_t          version;
  uint64_t               id;
};

}

inline LavaArg* RunIfNotNull(LavaNode* n, LavaArg* in) // void** out)
{
  if(n && n->func)
    return n->func(in);   //out);
}


#endif 


//struct LavaArgs
//{
//  void*  in; 
//  void* out;
//  uint64_t in_len, out_len;
//};

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
