

#ifndef __SERIALIZATION_UTIL_HEADER_GUARD__
#define __SERIALIZATION_UTIL_HEADER_GUARD__

#include <iostream>
#include <sstream>
#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <SIM/Types.h>
#include <SIM/SIM_MacroUtil.h>

  template<class CEREAL_ARCHIVE, class T> int 
CerealLoad(T& o, string filePath)
{
  using namespace std;
  using namespace std::tr2::sys;

  auto p = path(filePath);
  if( exists(p) )
  {
    ifstream ifs(p.string(), ios::binary);
    //cereal::PortableBinaryInputArchive iarchive(ifs);
    CEREAL_ARCHIVE iarchive(ifs);
    iarchive(o);
    ifs.close();
    
    return 0;
  }
  else 
    return -1;    
}

  template<class CEREAL_ARCHIVE, class T> vec<ui8>
SerializeToBytes(T data)
{
  using namespace std;
  using namespace cereal;
  
  //using Ar = cereal::PortableBinaryOutputArchive;

  using Ar =  CEREAL_ARCHIVE;

  const i64 BUF_SZ = 1024;

  stringstream ss;
  {    
    Ar oarchive(ss);
    oarchive(data);
  }  // flush on scope exit

  vec<ui8> ret;
  i64 bytesRead = 0;
  do { 
    char buf[BUF_SZ];
    ss.read(buf, BUF_SZ);
    bytesRead   =  ss.gcount();
    auto len    =  min(BUF_SZ, bytesRead);
      
    TO(len,i) ret.push_back( buf[i] );
  } while(bytesRead > 0);

  return ret;
}

  template<class CEREAL_ARCHIVE, class T> void 
UnSerialize(T* out, void* bytes, size_t len)
{
  using namespace std;
  using Ar = CEREAL_ARCHIVE;

  stringstream ss;
  ss.write( (char const*)(bytes), (streamsize)len);
  Ar iarchive(ss);
  iarchive(*out);
}

inline ui64 
HashBytes(void* bytes, size_t len)  // should be faster than hashing one byte at a time
{
  using namespace std;

  ui64 hsh       =  0;
  //auto divBy8    =  div(len, 8);
  auto eightLen  =  len / 8;  //divBy8.quot;
  auto byteLens  =  len % 8;  //divBy8.rem;

  auto p8  =  (ui64*)bytes;
  auto p1  =  (ui8*)(p8 + eightLen);

  TO(eightLen,i)
    hsh ^= std::hash<ui64>()(p8[i]);

  TO(byteLens,i)
    hsh ^= std::hash<ui8>()(p1[i]);

  return hsh;
}

inline ui64 
HashStringAndBytes(string const& s, void* bytes, size_t len)  // should be faster than hashing one byte at a time
{
  using namespace std;

  auto hsh = HashBytes(bytes, len);
  hsh ^= hash<string>()(s);

  return hsh;
}


#endif














//  template<class CEREAL_ARCHIVE, class T> vec<ui8> 
//SerializeToBytes(T data)
//template<class T> vec<ui8> 

//template<class T> void 
//using Ar = cereal::PortableBinaryInputArchive;
//cereal::load(iarchive,)
//out->load(iarchive);

//stringstream ss( ios::binary | ios::in );
//istringstream ss;
//ss << "wat skidoosh";
//ss.flush();
//ss.flush();

//ss.write( (char const*)(bytes), (streamsize)len);
//ss.flush();
//CEREAL_ARCHIVE iarchive(ss);
//iarchive(*out);

//
//ss.write(  (char const*)msg.getData(), msg.getSize());


