
// -todo: use output to make a tbl that can be made into a const file that contains the directory to monitor and the window to refresh
// -todo: in the tbl possibly put the msg to send and the arguments in the input tbl as well
// -todo: use FindFirstChangeNotification and possibly WaitForSingleObject(m_dir, INFINITE); to monitor directory
// -todo: use FindWindow function to possibly find a web browser window

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #define WIN32_EXTRA_LEAN
  #define NOMINMAX
  #include <windows.h>
  #include <winsock2.h>
#endif // WIN32

#define NO_RT_UTIL_IMPL

#include <iostream>
#include <algorithm>
#include "happyhttp.h"
#include "tinyxml2.h"
#include "../../no_rt_util.h"
#include "../../tbl.hpp"
#include "../LavaFlow.hpp"
#include "json_fwd.hpp"
#include "json.hpp"

using namespace tinyxml2;
using namespace nlohmann;

using abool = std::atomic<bool>;
using au32  = std::atomic<uint32_t>;
using au64  = std::atomic<uint64_t>;

enum        Slots
{
          IN_HTTP_URL  =  0,  
     IN_XML_PARSE_TXT  =  0,
  IN_JSON_PARSE_ASCII  =  0,
        IN_WINMSG_MSG  =  0,
  IN_TBL_TO_STR_ASCII  =  0,

         OUT_HTTP_TXT  =  0,
    OUT_XML_PARSE_XML  =  0,
      OUT_WINMSG_STAT  =  0,
   OUT_JSON_PARSE_TBL  =  0,
   OUT_TBL_TO_STR_TXT  =  0
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

static int    count = 0;
static int runCount = 0;

void OnBegin(    const happyhttp::Response* r, void* tblPtr)
{
  printf( "BEGIN (%d %s)\n", r->getstatus(), r->getreason() );
  count = 0;

  tbl* t = (tbl*)tblPtr;
}
void OnData(     const happyhttp::Response* r, void* tblPtr, const unsigned char* data, int n )
{
  fwrite( data,1,n, stdout );
  count += n;

  tbl* t = (tbl*)tblPtr;

  t->push<i8>('\n');

  // inefficient
  TO(n,i)
    t->push<i8>( data[i] );
}
void OnComplete( const happyhttp::Response* r, void* tblPtr)
{
  printf( "COMPLETE (%d bytes)\n", count );

  tbl* t = (tbl*)tblPtr;
}

//void OnBegin(    const happyhttp::Response* r, void* userdata )
//{
//  printf( "BEGIN (%d %s)\n", r->getstatus(), r->getreason() );
//  count = 0;
//}
//void OnData(     const happyhttp::Response* r, void* userdata, const unsigned char* data, int n )
//{
//  fwrite( data,1,n, stdout );
//  count += n;
//}
//void OnComplete( const happyhttp::Response* r, void* userdata )
//{
//  printf( "COMPLETE (%d bytes)\n", count );
//}

#ifdef _WIN32 

static int     winErr = 0;
static au64   winHndl = 0;
static au32       msg = 0;
static au32    wParam = 0;
static au32    lParam = 0;
static au64  waitHndl = 0;
static abool    runCb = false;

VOID CALLBACK cb_WaitOrTimerCallback(
  _In_ PVOID   lpParameter,
  _In_ BOOLEAN TimerOrWaitFired
)
{
  if( !TimerOrWaitFired ){
    if( runCb.exchange(false) ){
      LRESULT ok = SendMessage( (HWND)(winHndl.load()), msg, wParam, lParam);
      printf("\n send message ok: %ld \n", (long)ok); 
    }
  }
}
#endif

const char* headers[] = 
{
  "Connection",   "close",
  "Content-type", "application/x-www-form-urlencoded",
  "Accept", "text/plain",
  nullptr
};

class Vis : public tinyxml2::XMLVisitor
{
  void printTxt(const char* label, const XMLText* txt)
  {
    printf("\n %s: %s \n", label, txt? txt->Value() : "NULL");
  }

  //bool VisitEnter(const XMLDocument& doc){
  //  printTxt("doc text:", doc.ToText() );
  //  printf("doc val: %s", doc.Value()? doc.Value() : "val null");
  //  //printf("doc get text: %s", doc.Value() );
  //  return true;
  //}
  //bool  VisitExit(const XMLDocument& doc){
  //  printTxt("doc text:", doc.ToText() );
  //  printf("doc val: %s", doc.Value()? doc.Value() : "val null");
  //  //printf("doc get text: %s", doc.GetText() );
  //  return true;
  //}

  bool VisitEnter(const XMLElement& elem, const XMLAttribute* firstAttr)
  {
    printf("\n%s: %s \n", elem.Name(), elem.GetText()? elem.GetText() : "");

    //auto txt = elem.ToText();
    ////if(txt){ 
    //  printf("\n elem text: %s \n", txt? txt->Value() : "NULL");
    ////}
    //
    //printTxt("elem text:", elem.ToText());
    //printf(" elem get text: %s", elem.GetText());
    //printTxt("attr text:", firstAttr.ToText() );
    //
    //if(firstAttr){
    //  auto txt = firstAttr->Value();
    //  printf("\n attr text: %s \n", txt? txt : "NULL");
    //}

    return true;
  }

  //bool VisitExit(const XMLElement&     elem)
  //{
  //  printTxt("elem exit:", elem.ToText());
  //  return true;
  //}
  //bool     Visit(const XMLText&          xt)
  //{
  //  printTxt("text txt:", xt.ToText());
  //
  //  //auto txt = xt.ToText();
  //  //if(txt){
  //  //  printf("\n text: %s \n", txt? txt->Value() : "NULL");
  //  //}
  //
  //  return true;
  //}
  //bool     Visit(const XMLDeclaration& decl)
  //{
  //  printTxt("declaration:", decl.ToText() ); 
  //  printf("declaration val: %s", decl.Value()? decl.Value() : "val null");
  //
  //  //auto txt = decl.ToText();
  //  //printf("\n declaration: %s \n", txt? txt->Value() : "NULL");
  //
  //  return true;
  //}
  //bool     Visit(const XMLComment&      com){
  //  printTxt("commment txt:", com.ToText() );
  //  return true;
  //}
  //bool     Visit(const XMLUnknown&  unknown){
  //  printTxt("unknown txt:", unknown.ToText() );
  //  return true;
  //}

};

void PrintElem(json::value_type& e)
{
  using namespace std;

  switch(e.type()){
  case json::value_t::array:
  case json::value_t::object:{
    for(auto& ee : e){
      PrintElem(ee);
    }
  }break;
  default: 
    cout << e << endl << endl << endl;
    break;
  }

  //for(auto&& e : j){
  //}
}

extern "C"
{
  const char*      TblToStr_InTypes[]  = {"ASCII",          nullptr};            // This array contains the type that each slot of the same index will accept as input.
  const char*      TblToStr_InNames[]  = {"Raw Text",       nullptr};            // This array contains the names of each input slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  const char*      TblToStr_OutTypes[] = {"Text",           nullptr};            // This array contains the types that are output in each slot of the same index
  const char*      TblToStr_OutNames[] = {"Text as Tbl",    nullptr};            // This array contains the names of each output slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  uint64_t         TblToStr(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {
    auto inTxtPckt = in->packets[IN_TBL_TO_STR_ASCII];
    tbl        txt = LavaMakeTbl(lp, inTxtPckt.sz_bytes, 0);

    memcpy(txt.data(), (void*)inTxtPckt.val.value, inTxtPckt.sz_bytes);

    out->push( LavaTblToOut(txt, OUT_TBL_TO_STR_TXT) );

    return 0;
  }

  const char*       http_InTypes[] = {"ASCII",          nullptr};            // This array contains the type that each slot of the same index will accept as input.
  const char*       http_InNames[] = {"URL",            nullptr};            // This array contains the names of each input slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  const char*      http_OutTypes[] = {"ASCII",          nullptr};            // This array contains the types that are output in each slot of the same index
  const char*      http_OutNames[] = {"txt",            nullptr};            // This array contains the names of each output slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  uint64_t         http(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {
    using namespace std;
    using namespace happyhttp;

    str url = (str)LavaTblFromPckt(lp, in, IN_HTTP_URL);

    SocketInit si;                                                                       // inits windows sockets and on destruction cleans them up
    
    auto addrSt = url.find("//") + 2;
    auto pageSt = find( url.begin()+addrSt, url.end(), '/');

    str addr(url.begin()+addrSt, pageSt);
    str page(pageSt, url.end());

    tbl response = LavaMakeTbl(lp, 0, (i8)0);
    Connection cnct(addr.c_str(), 80);
    cnct.connect();
    cnct.setcallbacks(OnBegin, OnData, OnComplete, &response);
    cnct.request("GET", page.c_str(), 0, 0, 0);

    printf("\n");
    while( cnct.outstanding() ){
      printf(".");
      cnct.pump();
      nort_sleep(10);
    }
    printf("\n");

    out->push( LavaTblToOut(response, OUT_HTTP_TXT) );

    return 1;
  }

  const char*   xml_parse_InTypes[] = {"txt",             nullptr};            // This array contains the type that each slot of the same index will accept as input.
  const char*   xml_parse_InNames[] = {"XML Text",        nullptr};            // This array contains the names of each input slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  const char*  xml_parse_OutTypes[] = {"XML",             nullptr};            // This array contains the types that are output in each slot of the same index
  const char*  xml_parse_OutNames[] = {"Parsed XML Tbl",  nullptr};            // This array contains the names of each output slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  void        xml_parse_construct(){}
  void         xml_parse_destruct(){}
  uint64_t      xml_parse(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {    
    //printf("entered");

    Vis v;

    auto inTxtPckt = in->packets[IN_XML_PARSE_TXT];

    str inTxt;
    inTxt.resize( inTxtPckt.sz_bytes );
    memcpy( (void*)inTxt.data(), (void*)inTxtPckt.val.value, inTxtPckt.sz_bytes);

    //printf("\n\n %s \n", inTxt.c_str() );

    tinyxml2::XMLDocument doc(true, tinyxml2::COLLAPSE_WHITESPACE);
    doc.Parse( inTxt.c_str(), inTxt.size() );
    
    doc.Accept( &v );

    tbl xmlParse = LavaMakeTbl(lp);
    xmlParse.resize<i8>(1,0);
    out->push( LavaTblToOut(xmlParse, OUT_XML_PARSE_XML) );

    return 0;
  }

  const char*   json_parse_InTypes[] = {"JSON",                     nullptr};  // This array contains the type that each slot of the same index will accept as input.
  const char*   json_parse_InNames[] = {"JSON to Parse",            nullptr};  // This array contains the names of each input slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  const char*  json_parse_OutTypes[] = {"TBL",                      nullptr};  // This array contains the types that are output in each slot of the same index
  const char*  json_parse_OutNames[] = {"Nest Tbls of JSON values", nullptr};  // This array contains the names of each output slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  void        json_parse_construct(){}
  void         json_parse_destruct(){}
  uint64_t      json_parse(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  { 
    using namespace std;
    using namespace nlohmann;
    
    str    s = LavaStrFromPckt(in, IN_JSON_PARSE_ASCII);
    json   j = json::parse(s);

    for(auto&& e : j){
      PrintElem(e);
    }

    tbl tmp = LavaMakeTbl(lp);
    out->push( LavaTblToOut(tmp, OUT_JSON_PARSE_TBL) );

    return 0;
  }

  const char*   winmsg_InTypes[] = {"params",         nullptr};            // This array contains the type that each slot of the same index will accept as input.
  const char*   winmsg_InNames[] = {"Watch and MSG",  nullptr};            // This array contains the names of each input slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  const char*  winmsg_OutTypes[] = {"STATS",          nullptr};            // This array contains the types that are output in each slot of the same index
  const char*  winmsg_OutNames[] = {"Status",         nullptr};            // This array contains the names of each output slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  void        winmsg_construct(){}
  void         winmsg_destruct(){}
  uint64_t      winmsg(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {    
    runCount++; 

    tbl  params = LavaTblFromPckt(lp, in, IN_WINMSG_MSG);
    str    path = (str)params;
        winHndl = (u64)params("window handle");
            msg = (u32)params("windows message");
         wParam = (u32)params("wParam");
         lParam = (u32)params("lParam");

    //u64 winHndl = params("window handle");
    //u32     msg = params("windows message");
    //u32  wParam = params("wParam");
    //u32  lParam = params("lParam");

    //WindowFromPoint() // gets the handle from a point on the screen

    #ifdef _WIN32
      printf("\n cnt: %d path: %s \n", runCount, path.c_str() );
      printf("\n hndl: %lld  msg: %d wParam: %d lParam %d \n", winHndl.load(), msg.load(), wParam.load(), lParam.load());

      //SendMessage( (HWND)0x00040A68, WM_KEYDOWN, VK_F5, 0);     // F5 key
      //SendMessage( (HWND)winHndl, msg, VK_RCONTROL, lParam);
      //SendMessage( (HWND)winHndl, msg, wParam,      lParam);
      //SendMessage( (HWND)winHndl, msg, VK_BROWSER_REFRESH, lParam);

      // 0x52 r key | 0xA3 RCONTROL | VK_BROWSER_REFRESH 0xA8 | 0x74 VK_F5
      //SendMessage( (HWND)winHndl, msg, wParam, lParam);

      //HANDLE prevHndl = (HANDLE)waitHndl.exchange(0);
      //if(prevHndl){
      //  bool ok = UnregisterWait( (HANDLE)prevHndl );
      //  printf("\n unregister ok: %d \n", ok);
      //}else{

      bool prev = false;
      //while(true)
      //{
        HANDLE dirHndl = FindFirstChangeNotification( 
          path.c_str(),                                           // directory to watch 
          FALSE,                                                  // do not watch subtree 
          FILE_NOTIFY_CHANGE_LAST_WRITE);

        if(dirHndl == INVALID_HANDLE_VALUE){
          winErr  = true;
          dirHndl = nullptr;
          //break;
        }

        WaitForSingleObject(dirHndl, INFINITE);

        LRESULT ok = SendMessage( (HWND)(winHndl.load()), msg, wParam, lParam);
        printf("\n send message ok: %ld \n", (long)ok); 

        //prev = false;
        //if( runCb.compare_exchange_strong(prev, true) )
        //{
        //  PHANDLE pHndl = (void**)&waitHndl;
        //  auto ok = RegisterWaitForSingleObject(
        //               pHndl, 
        //               dirHndl, 
        //               cb_WaitOrTimerCallback, 
        //               0, 
        //               INFINITE, 
        //               WT_EXECUTEONLYONCE | WT_EXECUTEINWAITTHREAD | WT_EXECUTEDEFAULT);
        //  printf("\n register ok: %d \n", ok);
        //}

        auto closeOk = FindCloseChangeNotification(dirHndl);

        //Sleep(20000);
      //}
      //}

      //RegisterWaitForSingleObject
      //while(true){
      //  auto waitRet = WaitForSingleObject(dirHndl, INFINITE);
      //  if(waitRet == WAIT_OBJECT_0){ 
      //    SendMessage( (HWND)winHndl, msg, wParam, lParam);
      //    break;
      //  }
      //}
    #endif

    tbl tmp = LavaMakeTbl(lp);
    tmp("window handle")   = (u64)0;
    tmp("windows message") = (u32)WM_KEYDOWN;
    tmp("wParam")          = (u32)VK_F5;
    tmp("lParam")          = (u32)0;
    tmp.resize<i8>(1024, 0);
    strncpy(tmp.data<char>(), "file to watch", 1023);
    out->push( LavaTblToOut(tmp, OUT_WINMSG_STAT) );

    return 0;
  }

  LavaNode LavaNodes[] =
  {
    {
      TblToStr,                                  // function
      nullptr,                                   // constructor - this can be set to nullptr if not needed
      nullptr,                                   // destructor  - this can also be set to nullptr 
      LavaNode::FLOW,                            // node_type   - this should be eighther LavaNode::MSG (will be run even without input packets) or LavaNode::FLOW (will be run only when at least one packet is available for input)
      "TblToStr",                                // name
      TblToStr_InTypes,                          // in_types    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      TblToStr_InNames,                          // in_names    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      TblToStr_OutTypes,                         // out_types   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      TblToStr_OutNames,                         // out_names   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr
      nullptr,                                   // description
      0                                          // version 
    },

    {
      http,                                      // function
      nullptr,                                   // constructor - this can be set to nullptr if not needed
      nullptr,                                   // destructor  - this can also be set to nullptr 
      LavaNode::FLOW,                            // node_type   - this should be eighther LavaNode::MSG (will be run even without input packets) or LavaNode::FLOW (will be run only when at least one packet is available for input)
      "http",                                    // name
      http_InTypes,                              // in_types    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      http_InNames,                              // in_names    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      http_OutTypes,                             // out_types   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      http_OutNames,                             // out_names   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr
      nullptr,                                   // description
      0                                          // version 
    },

    {
      xml_parse,                                 // function
      xml_parse_construct,                       // constructor - this can be set to nullptr if not needed
      xml_parse_destruct,                        // destructor  - this can also be set to nullptr 
      LavaNode::FLOW,                            // node_type   - this should be eighther LavaNode::MSG (will be run even without input packets) or LavaNode::FLOW (will be run only when at least one packet is available for input)
      "xml_parse",                               // name
      xml_parse_InTypes,                         // in_types    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      xml_parse_InNames,                         // in_names    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      xml_parse_OutTypes,                        // out_types   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      xml_parse_OutNames,                        // out_names   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr
      nullptr,                                   // description
      0                                          // version 
    },

    {
      json_parse,                             // function
      json_parse_construct,                   // constructor - this can be set to nullptr if not needed
      json_parse_destruct,                    // destructor  - this can also be set to nullptr 
      LavaNode::FLOW,                         // node_type   - this should be eighther LavaNode::MSG (will be run even without input packets) or LavaNode::FLOW (will be run only when at least one packet is available for input)
      "json_parse",                           // name
      json_parse_InTypes,                     // in_types    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      json_parse_InNames,                     // in_names    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      json_parse_OutTypes,                    // out_types   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      json_parse_OutNames,                    // out_names   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr
      nullptr,                                // description
      0                                       // version 
    },

    {
      winmsg,                                 // function
      winmsg_construct,                       // constructor - this can be set to nullptr if not needed
      winmsg_destruct,                        // destructor  - this can also be set to nullptr 
      LavaNode::FLOW,                         // node_type   - this should be eighther LavaNode::MSG (will be run even without input packets) or LavaNode::FLOW (will be run only when at least one packet is available for input)
      "winmsg",                               // name
      winmsg_InTypes,                         // in_types    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      winmsg_InNames,                         // in_names    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      winmsg_OutTypes,                        // out_types   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      winmsg_OutNames,                        // out_names   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr
      nullptr,                                // description
      0                                       // version 
    },

    LavaNodeListEnd                                  // This is a constant that has all the members of the LavaNode struct set to 0 or nullptr - it is used as a special value that ends the static list of LavaNodes. This negates the need for a separate static variable that gives the size of the list, which would be have to be kept in sync and therefore be error prone.
  };

  __declspec(dllexport) LavaNode* GetLavaNodes()
  {
    return (LavaNode*)LavaNodes;
  }
}










// todo: change this to raw text
//tbl pageTxt;
//out->push( LavaTblToOut(pageTxt, OUT_HTTP_TXT) );

//printf("url: %s \n\n addr: %s \n\n file: %s", url.c_str());
//
//str urlStr = "www.google.com";
//
//str pgeStr = "/";
//

//while((result = strchr(result, target)) != NULL) {
//  printf("Found '%c' starting at '%s'\n", target, result);
//  ++result; // Increment result, otherwise we'll find target at the same location
//}

//void            http_construct(){}
//void             http_destruct(){}

//http_construct,                          // constructor - this can be set to nullptr if not needed
//http_destruct,                           // destructor  - this can also be set to nullptr 

//str   s = LavaStrFromPckt(in, IN_TBL_TO_STR_ASCII);
//
//str inTxt;
//inTxt.resize( inTxtPckt.sz_bytes );
//memcpy( (void*)inTxt.data(), (void*)inTxtPckt.val.value, inTxtPckt.sz_bytes);
//
//tbl tmp = LavaMakeTbl(lp);

//cnct.request( "POST",
//  "/happyhttp/test.php",
//  headers,
//  (const unsigned char*)body,
//  strlen(body) );

//str  tst = j.dump();
//printf("\ntest string:\n%s", tst.c_str());
//printf("\n\n");
//
//cout << setw(2) << j << endl;

//cout << e << endl << endl << endl;
//switch(e.type()){
//case json::value_t::array:
//case json::value_t::object:
//  
//default: ;
//}

//const str s = (const str)LavaTblFromPckt(lp, in, IN_JSON_PARSE_ASCII);
//
//auto inTxtPckt = in->packets[IN_XML_PARSE_TXT];
//str inTxt;
//inTxt.resize( inTxtPckt.sz_bytes );
//memcpy( (void*)inTxt.data(), (void*)inTxtPckt.val.value, inTxtPckt.sz_bytes);

//tmp.resize<i8>(1024, 0);
//strncpy(tmp.data<char>(), "file to watch", 1023);

//u64 pathLen = strnlen( params.data<char>(), params.size() );
//str    path;
//path.resize(pathLen);
//strncpy( (char*)path.data(), params.data<char>(), path.size() );

//#ifdef _WIN32
//    m_dir = FindFirstChangeNotification( 
//      path,                                   // directory to watch 
//      FALSE,                                  // do not watch subtree 
//      FILE_NOTIFY_CHANGE_LAST_WRITE);
//
//    if(m_dir == INVALID_HANDLE_VALUE){
//      m_error = true;
//      m_dir   = nullptr;
//    }
//    WaitForSingleObject(m_dir, INFINITE); 
//#endif
//
//WM_LBUTTONDOWN fwKeys: 0000 xPos: 86 yPos 57 
//WM_KEYDOWN nVirtKey: VK_MEDIA_NEXT_TRACK cRepeat: 1 ScanCode: 00f Extended: 1 fAltDown: 0 fRepeat: 0 fUp: 

//auto xmlElem = doc.FirstChildElement();
//auto xmlElem = doc.RootElement();
//auto  xmlDoc = xmlElem->GetDocument();
//auto     e2  = xmlDoc->FirstChildElement();
//auto   txt1  = e2->ToText();
//
//auto xmlDoc = doc.FirstChildElement();
//auto   txt1 = xmlDoc->ToText();
//printf("\n %s \n", txt1? txt1->Value() : "NULL"); 
//
//auto chld = doc.FirstChild();
//auto txt2 = chld->ToText();
////auto a    = chld->Accept( &v );
//printf("\n %s \n", txt2? txt2->Value() : "NULL"); 
//
//e2->NextSiblingElement();
//auto     txt = xmlDoc->ToText();
//
//printf("\n\n %s %s \n\n", xmlElem->ToText()->Value(), xmlDoc->ToText()->Value() );

//tbl    url = LavaMakeTbl(lp);
//str urlStr = url.data<const char>();
//
//str urlStr = "216.58.216.110";

//u32 i=0;
//while( LavaNxtPckt(in, &i) )
//{
//  tbl inputTbl( (void*)(in->packets[i-1].val.value) );
//  //for(auto& kv : inputTbl){  // this loop demonstrates how to iterate through non-empty map elements
//  //}	
//}
