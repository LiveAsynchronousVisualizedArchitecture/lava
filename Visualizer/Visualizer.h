

#ifndef __VISUALIZER_HEADER_GUARD__
#define __VISUALIZER_HEADER_GUARD__

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
//#include "MemStore.hpp"
#include <SIM/MemStore.hpp>

using  ArOut  =  cereal::PortableBinaryOutputArchive;
using  ArIn   =  cereal::PortableBinaryInputArchive;

struct MetaData
{
  using  DataBytes  =  vec<ui8>;

  i64        id;
  string     name;
  string     type;
  DataBytes  bytes;
  void*      dataType;
     
  void mv(MetaData&&      rval)
  {
    id        =  rval.id;
    name      =  move(rval.name);
    type      =  move(rval.type);
    bytes     =  move(rval.bytes);
    dataType  =  rval.dataType;
  }
  void cp(MetaData const& lval)
  {
    id        =  lval.id;
    name      =  lval.name;
    type      =  lval.type;
    bytes     =  lval.bytes;
    dataType  =  lval.dataType;
  }

  MetaData(){}
  MetaData(i64 _id, string const& _name, string const& _type, DataBytes _bytes) :
    id(_id),
    name(_name),
    type(_type),
    bytes(_bytes)
  {}
  MetaData           (MetaData&& rval)
  { mv(move(rval)); }
  MetaData& operator=(MetaData&& rval)
  { mv(move(rval)); return *this; }

  MetaData           (MetaData const& lval)
  { cp(lval); }
  MetaData& operator=(MetaData const& lval)
  { cp(lval); return *this; }

    template<class Ar>
  void save(Ar& ar) const
  {
    ar(id, name, type, bytes);
  }
    template<class Ar>
  void load(Ar& ar)
  {
    ar(id, name, type, bytes);
    dataType = nullptr;
  }

  //  template<class Archive>
  //void serialize(Archive& ar)
  //{
  //  ar(id, name, type, data);
  //}
};
using MD = MetaData;

extern MemStore    bindb;
extern MemStore   namedb;
extern MemStore   typedb;
extern MemStore    srcdb;
extern MemStore  xformdb;
extern MemStore   timedb;

inline i64       Put(MemStore::BB const& bytes, cstr name, cstr type, cstr src)
{
  auto binId = bindb.nextId();
  bindb.put(  binId, bytes );
  namedb.put( binId,  name );
  typedb.put( binId,  type );
  srcdb.put(  binId,   src );

  return binId;
}
inline void  PutTime(i64 key, i64 time)
{
  ui8*   t     =  (ui8*)&time;
  timedb.put(key, { t[0],t[1],t[2],t[3],t[4],t[5],t[6],t[7] } );
}
inline MD        Get(i64 key)
{
  MD ret(key, string(namedb.getPtr(key)), string(typedb.getPtr(key)), bindb.get(key) );
  return ret;
}

#endif

