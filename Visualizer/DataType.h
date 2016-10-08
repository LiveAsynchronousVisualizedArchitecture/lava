

#ifndef __DATATYPE_UTIL_HEADER_GUARD__
#define __DATATYPE_UTIL_HEADER_GUARD__

#include <cstdarg>
#include <cstdint>

extern "C"
{

struct DataType;
struct DataBuffer
{
  void*    p;
  size_t   byteLen;
};

using  DataCreate      =  void*(*)(uint32_t constructor, uint32_t arglen, ...);
using  DataLoad        =  void*(*)(void* bytesPtr,   size_t   byteLen);
using  DataSave        =  void(*)(void*  objPtr,     void*  bytesPtrAddr, size_t*  byteLen);
using  DataDestroy     =  void(*)(void*  objPtr);
using  GetDataTypes_t  =  DataType*(*)();

struct DataType
{
  const char*     type;
  DataCreate      create;
  DataLoad        load;
  DataSave        save;
  DataDestroy     destroy;
  size_t          byteLen;
  size_t          version;
  size_t          id;
};

inline void* DataTypeLoadIfNotNull   (DataType* dt,               void* bytesPtr, size_t  byteLen)
{
  if(dt && dt->load)
    return dt->load(bytesPtr, byteLen);
  else
    return nullptr;
}
inline void  DataTypeSaveIfNotNull   (DataType* dt, void* objPtr, void* bytesPtr, size_t* byteLen)
{
  if(dt && dt->save)
    dt->save(objPtr, bytesPtr, byteLen);
}
inline void  DataTypeDestroyIfNotNull(DataType* dt, void* dataTypePtr)
{
  if(dt && dt->destroy)
    dt->destroy(dataTypePtr);
}

};


#endif





//using  DataLoad        =  void(*)(void** objPtrAddr, uint8_t*  bytesPtr,     size_t   byteLen);
// Convenience functions
//void DataTypeCreateIfNotNull   (DataType* dt, void** objPtrAddr,  uint8_t*  bytesPtr,     size_t  byteLen)
//{
//
//}

//
//dt->data_create(dataTypePtr, db);

//const char*     name;
//const char*     extension;
