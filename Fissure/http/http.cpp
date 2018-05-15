


#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #define WIN32_EXTRA_LEAN
  #define NOMINMAX
  #include <winsock2.h>
#endif // WIN32

#include "happyhttp.h"
#include "tinyxml2.h"
#include "../../no_rt_util.h"
#include "../../tbl.hpp"
#include "../LavaFlow.hpp"

using namespace tinyxml2;

enum   Slots
{
       IN_HTTP_URL  =  0,  
  IN_XML_PARSE_TXT  =  0,

  OUT_HTTP_TXT      =  0,
  OUT_XML_PARSE_XML =  0
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
void OnBegin(    const happyhttp::Response* r, void* userdata )
{
  printf( "BEGIN (%d %s)\n", r->getstatus(), r->getreason() );
  count = 0;
}
void OnData(     const happyhttp::Response* r, void* userdata, const unsigned char* data, int n )
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

class Vis : public tinyxml2::XMLVisitor
{
  void printTxt(const char* label, const XMLText* txt)
  {
    printf("\n %s: %s \n", label, txt? txt->Value() : "NULL");
  }

  bool VisitEnter(const XMLDocument& doc){
    printTxt("doc text:", doc.ToText() );
    printf("doc val: %s", doc.Value()? doc.Value() : "val null");
    //printf("doc get text: %s", doc.Value() );
    return true;
  }

  bool VisitExit(const XMLDocument& doc){
    printTxt("doc text:", doc.ToText() );
    printf("doc val: %s", doc.Value()? doc.Value() : "val null");
    //printf("doc get text: %s", doc.GetText() );
    return true;
  }

  bool VisitEnter(const XMLElement& elem, const XMLAttribute* firstAttr)
  {
    //auto txt = elem.ToText();
    ////if(txt){ 
    //  printf("\n elem text: %s \n", txt? txt->Value() : "NULL");
    ////}

    printTxt("elem text:", elem.ToText());
    printf(" elem get text: %s", elem.GetText());
    printf(" elem name: %s", elem.Name() );
    //printTxt("attr text:", firstAttr.ToText() );

    if(firstAttr){
      auto txt = firstAttr->Value();
      printf("\n attr text: %s \n", txt? txt : "NULL");
    }

    return true;
  }

  bool VisitExit( const XMLElement& elem)
  {
    printTxt("elem exit:", elem.ToText());
    return true;
  }

  bool Visit(const tinyxml2::XMLText& xt)
  {
    printTxt("text txt:", xt.ToText());

    //auto txt = xt.ToText();
    //if(txt){
    //  printf("\n text: %s \n", txt? txt->Value() : "NULL");
    //}

    return true;
  }

  bool Visit(const XMLDeclaration& decl)
  {
    printTxt("declaration:", decl.ToText() ); 
    printf("declaration val: %s", decl.Value()? decl.Value() : "val null");

    //auto txt = decl.ToText();
    //printf("\n declaration: %s \n", txt? txt->Value() : "NULL");

    return true;
  }

  bool Visit(const XMLComment& com){
    printTxt("commment txt:", com.ToText() );
    return true;
  }
  bool Visit(const XMLUnknown& unknown){
    printTxt("unknown txt:", unknown.ToText() );
    return true;
  }

};

extern "C"
{
  const char*       http_InTypes[] = {"ASCII",          nullptr};            // This array contains the type that each slot of the same index will accept as input.
  const char*       http_InNames[] = {"URL",            nullptr};            // This array contains the names of each input slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  const char*      http_OutTypes[] = {"ASCII",          nullptr};            // This array contains the types that are output in each slot of the same index
  const char*      http_OutNames[] = {"txt",            nullptr};            // This array contains the names of each output slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  void            http_construct(){}
  void            http_destruct(){}
  uint64_t         http(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {
    using namespace std;
    using namespace happyhttp;

    SocketInit si;                                                               // inits windows sockets and on destruction cleans them up

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

  const char*   xml_parse_InTypes[] = {"txt",             nullptr};            // This array contains the type that each slot of the same index will accept as input.
  const char*   xml_parse_InNames[] = {"XML Text",        nullptr};            // This array contains the names of each input slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  const char*  xml_parse_OutTypes[] = {"XML",             nullptr};            // This array contains the types that are output in each slot of the same index
  const char*  xml_parse_OutNames[] = {"Parsed XML Tbl",  nullptr};            // This array contains the names of each output slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  void        xml_parse_construct(){}
  void         xml_parse_destruct(){}
  uint64_t      xml_parse(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {    
    Vis v;

    auto inTxtPckt = in->packets[IN_XML_PARSE_TXT];

    str inTxt;
    inTxt.resize( inTxtPckt.sz_bytes );
    memcpy( (void*)inTxt.data(), (void*)inTxtPckt.val.value, inTxtPckt.sz_bytes);

    printf("\n\n %s \n", inTxt.c_str() );

    tinyxml2::XMLDocument doc(true, tinyxml2::COLLAPSE_WHITESPACE);
    doc.Parse( inTxt.c_str(), inTxt.size() );

    //auto xmlElem = doc.FirstChildElement();
    //auto xmlElem = doc.RootElement();
    //auto  xmlDoc = xmlElem->GetDocument();
    //auto     e2  = xmlDoc->FirstChildElement();
    //auto   txt1  = e2->ToText();
    
    doc.Accept( &v );

    auto xmlDoc = doc.FirstChildElement();
    auto   txt1 = xmlDoc->ToText();
    printf("\n %s \n", txt1? txt1->Value() : "NULL"); 

    auto chld = doc.FirstChild();
    auto txt2 = chld->ToText();
    //auto a    = chld->Accept( &v );
    printf("\n %s \n", txt2? txt2->Value() : "NULL"); 


    //e2->NextSiblingElement();
    //auto     txt = xmlDoc->ToText();

    //printf("\n\n %s %s \n\n", xmlElem->ToText()->Value(), xmlDoc->ToText()->Value() );

    tbl xmlParse = LavaMakeTbl(lp);
    out->push( LavaTblToOut(xmlParse, OUT_XML_PARSE_XML) );

    return 0;
  }

  LavaNode LavaNodes[] =
  {
    {
      http,                                      // function
      http_construct,                            // constructor - this can be set to nullptr if not needed
      http_destruct,                             // destructor  - this can also be set to nullptr 
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

    LavaNodeListEnd                                  // This is a constant that has all the members of the LavaNode struct set to 0 or nullptr - it is used as a special value that ends the static list of LavaNodes. This negates the need for a separate static variable that gives the size of the list, which would be have to be kept in sync and therefore be error prone.
  };

  __declspec(dllexport) LavaNode* GetLavaNodes()
  {
    return (LavaNode*)LavaNodes;
  }
}






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
