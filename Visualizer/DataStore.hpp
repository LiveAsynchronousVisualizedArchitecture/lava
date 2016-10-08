
#ifndef __DATASTORE_HEADER_GUARD__
#define __DATASTORE_HEADER_GUARD__

#if !defined(NDEBUG)
  #define BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING
  #define BOOST_MULTI_INDEX_ENABLE_SAFE_MODE
#endif

#include <SIM/Types.h>
#include "Visualizer.h"
#include "VisualizerTypes.h"
#include "SerializationUtil.hpp"
#include "NthMap.hpp"

using  DataBytes   =  vec<ui8>;

class  DataServer;
class  ServerConnection;
class  ClientConnection;

struct DataTypePtr
{
  void* p;
};


class  DataStore
{
public:
  enum
  {
    SET_EXTENSIONS    =  0x00,
    SET_DIRECTORIES   =  0x01,
    AVAILABLE_TYPES   =  0x02,
    DATA_OBJ          =  0x03,
    UPDATE_OBJ        =  0x04,
    IS_VISIBLE        =  0x05
  };

  using  DataBytes   =  vec<ui8>;
  using  TypeStr     =  string;
  using  TypeStrs    =  vec<string>;
  //using  Storage     =  map<i64, MetaData>;
  using  Storage     =  hashmap<i64, MetaData>;
  using  NameIndex   =  map<string, i64>;
  using  NTHs        =  vec<NameTypeHash>;  // NameTypeHashs

private:
  Ai64       m_curId;
  Storage    m_storage;
  NameIndex  m_nameIdx;

  void TypeStrFromString(TypeStr& ts, string const& s) // todo: get rid of this  // todo: put this in TypeStr
  {
    ts = s;
    //auto len = TypeStr::LEN;
    //strncpy_s(ts.str, s.c_str(), len);
    //ts.str[len - 1] = '\0';
  }

public:
  DataStore() : m_curId(0) {}

  TypeStrs          getTypes()
  {
    TypeStrs ret;
    ret.resize( m_storage.size() );

    int i = 0;
    for(auto& kv : m_storage) {      
      ret[i] = kv.second.name;
      ++i;
    }

    return ret;
  }
  NTHs              getNameTypeHashs()
  {
    NTHs ret;

    int i = 0;
    for(auto& kv : m_storage)
    {      
      NameTypeHash nth;
      MetaData& md  =  kv.second;
      nth.name      =  md.name;
      nth.type      =  md.type;
      nth.hsh       =  HashStringAndBytes(md.name, md.bytes.data(), md.bytes.size() );
      ret.push_back( move(nth) );
      ++i;
    }
    return ret;
  }
  DataBytes         getTypesSerialized()
  {    
    TypeStrs t  =  getTypes();
    return SerializeToBytes<ArOut,TypeStrs>(t);  
  }
  DataBytes         getNameTypesSerialized()
  {    
    auto nths  =  getNameTypeHashs();
    return SerializeToBytes<ArOut,NTHs>(nths);
  }
  DataBytes         getMetaFromSerialName(ui8* bytes, size_t len)
  {
    string name;
    UnSerialize<ArIn,string>(&name, bytes, len);

    if(m_nameIdx.count(name) < 1)
      return DataBytes();

    MetaData& md  =  m_storage[m_nameIdx[name]];       // md is MetaData
    return SerializeToBytes<ArOut,MetaData>( md );
  }
  bool              insert(i64 id, string const& name, string const& type="", DataBytes data = DataBytes())
  {
    MetaData md(id,name,type,move(data));
    
    bool newKey    =  m_storage.count(id) > 0;
    m_storage[id]  =  move(md);

    //auto newKey      =  m_storage.emplace(piecewise_construct,
    //                                      forward_as_tuple(id), 
    //                                      forward_as_tuple(id,name,type,move(data)) ).second;
    m_nameIdx[name]  =  id;
    return newKey;
  }
  i64               id(string const& name)
  {
    //if( m_nameIdx.count(name) > 0 )
    auto iter = m_nameIdx.find(name); 
    if( iter != end(m_nameIdx)  )
      return iter->second;
    else
      return -1;
  }
  DataBytes         getData(i64 id)
  {
    if( m_storage.count(id) < 1 )
      return DataBytes();
    else
     return m_storage.at(id).bytes;
  }

  static NTHs       unserializeTypeStrs(void* data, size_t len)
  {
    NTHs ret;
    UnSerialize<ArIn,NTHs>(&ret, data, len);
    return ret;
  }
  static MetaData   unserializeMetaData(void* data, size_t len)
  {
    MetaData ret;
    UnSerialize<ArIn,MetaData>(&ret, data, len);
    return ret;
  }
    template<class T>
  static T unserialize(void* data, size_t len)
  {
    T ret;
    UnSerialize<ArIn,T>(&ret, data, len);
    return ret;
  }
};
class  ServerConnection : public InterprocessConnection 
{
public:
  using Mem = MemoryBlock;

  const int  port     =  0xDADA; // 56026
  const int  timeout  =  1000;

  DataServer*  dataServer;

  ServerConnection() : InterprocessConnection(true) {}

  bool connect();
  void connectionMade() override;
  void connectionLost() override;
  void messageReceived(MemoryBlock const& message) override;
  bool sendMessage(ui8 msgType, DataStore::DataBytes const& bytes);
  DataStore& dataStore();
};
class  ClientConnection : public InterprocessConnection 
{
public:
  using Func  =  std::function< void(MemoryBlock) >;

  const int  port     =  0xDADA; // 56026
  const int  timeout  =  1000;

  Func msg_callback;

  ClientConnection() : InterprocessConnection(true) {}
  ClientConnection(Func _func) : 
    InterprocessConnection(),
    msg_callback(_func)
  {}

  bool connect();
  void connectionMade() override;
  void connectionLost() override;
  void messageReceived(MemoryBlock const& message) override;
  bool sendMessage(ui8 msgType, DataStore::DataBytes const& bytes);
  void setCallback(Func callback);
};
class  DataServer : public InterprocessConnectionServer
{
private:
  using Connection   =  unique_ptr<ServerConnection>;
  using Connections  =  set< Connection >;

  const int port     =  56026;

  DataStore    m_ds;
  Connections  m_connections;

public:
  bool start();
  InterprocessConnection* createConnectionObject();
  DataStore&              data();
  DataStore::DataBytes&   operator[](string const& key);
  bool                    insert(i64 id, string const& name, string const& type="", DataBytes data = DataBytes());
  i64                     id(string const& name);
};

// ClientConnection Implementation
inline bool ClientConnection::connect()
{
  return InterprocessConnection::connectToSocket("127.0.0.1",port,timeout);
}
inline void ClientConnection::connectionMade()
{
}
inline void ClientConnection::connectionLost() 
{
}
inline void ClientConnection::messageReceived(MemoryBlock const& message)
{
  this->msg_callback(message);
}
inline void ClientConnection::setCallback(Func callback)
{
  msg_callback = callback;
}
inline bool ClientConnection::sendMessage(ui8 msgType, DataStore::DataBytes const& bytes)
{
  MemoryBlock msg(bytes.size()+1,true);
  msg[0] = msgType;
  msg.copyFrom(bytes.data(), 1, bytes.size());

  return InterprocessConnection::sendMessage(msg);
}

// ServerConnection Implementation
inline bool ServerConnection::connect()
{
  return InterprocessConnection::connectToSocket("localhost",port,timeout);
}
inline void ServerConnection::connectionMade()
{
}
inline void ServerConnection::connectionLost() 
{
}
inline void ServerConnection::messageReceived(MemoryBlock const& msg) 
{
  //auto len    =  msg.getSize();
  //if(len<=0) return;
  ui8*   bytes  =  (ui8*)msg.getData();
  void*  ptr    =  ((ui8*)msg.getData()) + 1;
  auto   len    =  msg.getSize() - 1;
  if(msg.getSize() <= 0) return;
  
  switch(bytes[0])
  {
  case DataStore::AVAILABLE_TYPES:
  {
    auto byteBuf = dataServer->data().getNameTypesSerialized();
    sendMessage( (ui8)DataStore::AVAILABLE_TYPES, byteBuf);
    break;
  }
  case DataStore::DATA_OBJ:
  case DataStore::IS_VISIBLE:
  {
    //auto metadataBytes = dataStore().getMetaFromSerialName(bytes+1, len-1);
    auto metadataBytes = dataStore().getMetaFromSerialName( (ui8*)ptr, len);
    sendMessage( (ui8)DataStore::DATA_OBJ, metadataBytes );
    break;
  }
  default:
    ;
  }
}
inline bool ServerConnection::sendMessage(ui8 msgType, DataStore::DataBytes const& bytes)
{
  MemoryBlock msg(bytes.size()+1, true);
  msg.copyFrom(bytes.data(), 1, bytes.size());
  msg[0] = msgType;
  return InterprocessConnection::sendMessage(msg);
}
inline DataStore& ServerConnection::dataStore()
{
  return dataServer->data();
}

// DataServer Implementation  
inline bool DataServer::start()
{
  return InterprocessConnectionServer::beginWaitingForSocket(port);
}
inline InterprocessConnection* DataServer::createConnectionObject()
{
  Connection con(new ServerConnection());
  auto p         =  con.get();
  p->dataServer  =  this;
  //p->connect();
  m_connections.insert( move(con) );
    
  return p;
}
inline bool DataServer::insert(i64 id, string const& name, string const& type, DataBytes data)
{
// right now this sends a new mesage if the binary was updated
// what needs to happen is sending a new message on any insert if the binary is being used in the visualizer
// probably a notification that the visualizer can use to request the data again would be best
  bool newKey = m_ds.insert(id, name, type, data);
  //if(!newKey && m_connections.size()>0)
  //{
    MetaData md(id, move(name), move(type), move(data) );
    auto bytes = SerializeToBytes<ArOut,MetaData>(md);

    //auto nameBytes = SerializeToBytes<ArOut,string>(name);    
    for(auto& con : m_connections) {
      con->sendMessage( DataStore::UPDATE_OBJ, bytes );
      //con->sendMessage( DataStore::IS_VISIBLE, nameBytes );
    }
  //}
  return newKey;
}
inline i64  DataServer::id(string const& name)
{
  return m_ds.id(name);
}
inline DataStore& DataServer::data()
{
  return m_ds;
}



#endif















//i64 id = m_curId.fetch_add(1);
//m_storage[id]    =  MetaData{ id, TypeStr(name), TypeStr(type), data };
//
//auto storIter  =  m_storage.find(id);
//bool update    =  storIter != m_storage.end();
//if(update) m_storage.emplace_hint(storIter, MetaData{id,name,type,move(data)} );
//else 
//m_storage[id]    =  MetaData{id,name,type,data};

//using  ArOut        =  cereal::PortableBinaryOutputArchive;
//using  ArIn         =  cereal::PortableBinaryInputArchive;
//using  ArOut        =  cereal::BinaryOutputArchive;
//using  ArIn         =  cereal::BinaryInputArchive;
//using  OutAr        =  cereal::PortableBinaryOutputArchive;
//using  InAr         =  cereal::PortableBinaryInputArchive;

//if(msgType == DataStore::GET_AVAILABLE_TYPES)
//{
  //msg[0] = (ui8)DataStore::AVAILABLE_TYPES;
  //MemoryBlock msg(bytes.data(), bytes.size());
//}

//using  Storage      =  hashmap<i64, MetaData>;
//using  NameIndex    =  hashmap<string, i64>;

//GET_AVAILABLE_TYPES = 0xFF  // -1 gets all types in the data store, other numbers are indices? 
//GET_AVAILABLE_TYPES  =  0x00,  // -1 gets all types in the data store, other numbers are indices? 

//if(bytes[0]==DataStore::GET_AVAILABLE_TYPES)
//{
//  //auto byteBuf = dataServer->data().getTypesSerialized();
//}
//else if(bytes[0])
//{}

//ret.resize( m_storage.size() );
//ret[i].name = kv.second.name;
//ret[i].type = kv.second.type;
// {kv.second.name, kv.second.type} );

//struct TypeStr
//{
//  static const size_t LEN = 128;
//
//  char str[LEN];
//
//  void cp(TypeStr const& lval)
//  {
//    memcpy(str, lval.str, LEN);
//  }
//
//  TypeStr() { memset(str,0,LEN); }
//  TypeStr(string const& s)
//  {
//    strncpy_s(str, s.c_str(), LEN);
//    str[LEN - 1] = '\0';
//  }
//  TypeStr(TypeStr const& lval) { cp(lval); }
//  TypeStr& operator=(TypeStr const& lval) { cp(lval); return *this; }
//
//  template<class ARCHIVE>
//  void serialize(ARCHIVE& ar)
//  {
//    ar(str);
//  }
//};

//return SerializeToBytes<ARCHIVE>(t);
//
//using namespace std;
//using namespace cereal;
//
//const i64 BUF_SZ = 1024;
//
//Types t  =  getTypes();
////Types t;
////vec<int> t  = { 1, 2, 3};
//
//stringstream ss;
////ostringstream ss;
////stringstream ss(ios::binary | ios::in | ios::out);
//{    
//  PortableBinaryOutputArchive oarchive(ss);
//  //oarchive(string("skidoosh"));
//  oarchive(t);
//}  // flush on scope exit
////ss << "squidoosh";
//
//DataBytes ret;
//i64 bytesRead = 0;
////ss.seekg(0);
////ss.seekp(0);
////bytesRead  =  ss.readsome(buf, BUF_SZ);
//do { 
//  char buf[BUF_SZ];
//  ss.read(buf, BUF_SZ);
//  bytesRead   =  ss.gcount();
//  auto len    =  min(BUF_SZ, bytesRead);
//  
//  TO(len,i) ret.push_back( buf[i] );
//} while(bytesRead > 0);
//
//return ret;

//DataStore::DataBytes& DataServer::operator[](string const& key)
//{
//  return m_ds[key];
//}

//using Storage    =  hashmap<string, DataBytes>;
//DataBytes&  operator[](string const& key)
//{
//  m_storage[m_curId.fetch_add(1)] = ;
//}

//TypeStr tkey;
//strncpy_s(tkey, key.c_str(), TypeStrLen);
//tkey[TypeStrLen-1] = '\0';

//using Mem   =  MemoryBlock;
//function<void(*)(void)>  msg_callback;
//function<void(*)(void)>

//static const size_t TypeStrLen = 128;
//using TypeStr    =  char[TypeStrLen];

// todo: make message recieved capture the data, queue it, and update a count?
// todo: use a lockless queue?
//int sz = byteBuf.size();
//MemoryBlock out(sz,true);
//out.copyFrom(byteBuf.data(), 0, sz);


