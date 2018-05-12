


#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #define WIN32_EXTRA_LEAN
  #define NOMINMAX
  #include <winsock2.h>
#endif // WIN32

#include "happyhttp.h"

#include "../../no_rt_util.h"
#include "../../tbl.hpp"
#include "../LavaFlow.hpp"

enum   Slots
{
  // This is an example enumeration that is meant to be helpful, though is not strictly neccesary. Referencing slots by a name will generally be less error prone than using their index and remembering what each index is for
  IN_URL  = 0,        
  //SLOT_OUT = 0
};

struct SocketInit
{
  SocketInit()
  {
  #ifdef _WIN32
    WSAData wsaData;
    int code = WSAStartup(MAKEWORD(1, 1), &wsaData);
    if( code != 0 )
    {
      fprintf(stderr, "shite. %d\n",code);
      //return 0;
    }
  #endif //WIN32
  }
  ~SocketInit()
  {
  #ifdef _WIN32
    WSACleanup();
  #endif // WIN32
  }
};

static int count = 0;
void OnBegin( const happyhttp::Response* r, void* userdata )
{
  printf( "BEGIN (%d %s)\n", r->getstatus(), r->getreason() );
  count = 0;
}
void OnData( const happyhttp::Response* r, void* userdata, const unsigned char* data, int n )
{
  fwrite( data,1,n, stdout );
  count += n;
}
void OnComplete( const happyhttp::Response* r, void* userdata )
{
  printf( "COMPLETE (%d bytes)\n", count );
}

const char* headers[] = 
{
  "Connection",   "close",
  "Content-type", "application/x-www-form-urlencoded",
  "Accept", "text/plain",
  nullptr
};

extern "C"
{
  const char*  InTypes[]  = {        nullptr};            // This array contains the type that each slot of the same index will accept as input.
  const char*  InNames[]  = {"URL",  nullptr};            // This array contains the names of each input slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  const char* OutTypes[]  = {        nullptr};            // This array contains the types that are output in each slot of the same index
  const char* OutNames[]  = {        nullptr};            // This array contains the names of each output slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.

  void http_construct(){ }
  void http_destruct(){ }

  uint64_t http(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {
    using namespace std;
    using namespace happyhttp;

    SocketInit si;                                                               // inits windows sockets and on destruction cleans them up

    //tbl    url = LavaMakeTbl(lp);
    //str urlStr = url.data<const char>();

    //str urlStr = "216.58.216.110";
    str urlStr = "www.google.com";
    str pgeStr = "/";
    
    Connection cnct(urlStr.c_str(), 80);
    cnct.connect();
    cnct.setcallbacks(OnBegin, OnData, OnComplete, nullptr);
    cnct.request("GET", pgeStr.c_str(), 0, 0, 0);

    //cnct.request( "POST",
    //  "/happyhttp/test.php",
    //  headers,
    //  (const unsigned char*)body,
    //  strlen(body) );

    while( cnct.outstanding() ){
      printf("\n\n\n\n");
      cnct.pump();
    }

    //tbl pageTxt;
    //out->push( LavaTblToOut(pageTxt, IN_URL) );         // this demonstrates how to output a tbl into the first output slot

    return 1;
  }

  LavaNode LavaNodes[] =
  {
    {
      http,                                      // function
      http_construct,                            // constructor - this can be set to nullptr if not needed
      http_destruct,                             // destructor  - this can also be set to nullptr 
      LavaNode::FLOW,                            // node_type   - this should be eighther LavaNode::MSG (will be run even without input packets) or LavaNode::FLOW (will be run only when at least one packet is available for input)
      "http",                                    // name
      InTypes,                                   // in_types    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      InNames,                                   // in_names    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      OutTypes,                                  // out_types   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      OutNames,                                  // out_names   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr
      nullptr,                                   // description
      0                                          // version 
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
//  //for(auto& kv : inputTbl){  // this loop demonstrates how to iterate through non-empty map elements
//  //}	
//}
