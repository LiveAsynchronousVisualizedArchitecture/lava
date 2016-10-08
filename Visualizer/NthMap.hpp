
#ifndef __NTHMAP_HEADER_GUARD__
#define __NTHMAP_HEADER_GUARD__

#if !defined(NDEBUG)
  #define BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING
  #define BOOST_MULTI_INDEX_ENABLE_SAFE_MODE
#endif

#include "SerializationUtil.hpp"
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/hashed_index.hpp>

class NameTypeHash
{
public:
  ui64    id    =   0;
  ui64    hsh   =   0;
  string  name;
  string  type;
  string  source;
  string  xform;

  void mv(NameTypeHash&&      rval)
  {
    id      =  rval.id;
    hsh     =  rval.hsh;
    name    =  move(rval.name);
    type    =  move(rval.type);
    source  =  move(rval.source);
    xform   =  move(rval.xform);
  }
  void cp(NameTypeHash const& lval)
  {
    id      =  lval.id;
    hsh     =  lval.hsh;
    name    =  lval.name;
    type    =  lval.type;
    source  =  lval.source;
    xform   =  lval.xform;
  }

  //NameTypeHash(){}
public:
  NameTypeHash(ui64   _id      =  0,
               string _name    = "",
               string _type    = "",
               string _source  = "",
               string _xform   = ""
               ) :
    id(_id),
    hsh(0),
    name(  move(_name)  ),
    type(  move(_type)  ),
    source(move(_source)),
    xform( move(_xform) )
  {}
  NameTypeHash           (NameTypeHash&& rval)
  { mv(move(rval)); }
  NameTypeHash& operator=(NameTypeHash&& rval)
  { mv(move(rval)); return *this; }

  NameTypeHash           (NameTypeHash const& lval)
  { cp(lval); }
  NameTypeHash& operator=(NameTypeHash const& lval)
  { cp(lval); return *this; }

    template<class Ar>
  void save(Ar& ar) const
  {
    ar(name, type, hsh);
  }
    template<class Ar>
  void load(Ar& ar)
  {
    ar(name, type, hsh);
  }

  ui64 hash() const { return hsh; }
};

struct id{};
struct hsh{};
struct name{};
struct type{};
struct source{};
struct xform{};

using boost::multi_index_container;
using namespace boost::multi_index;
using NthMap = multi_index_container< NameTypeHash,
  indexed_by<
    hashed_non_unique< tag<id>,     BOOST_MULTI_INDEX_MEMBER(NameTypeHash,ui64,id)>,
    hashed_non_unique< tag<name>,   BOOST_MULTI_INDEX_MEMBER(NameTypeHash,string,name)>,
    hashed_non_unique< tag<source>, BOOST_MULTI_INDEX_MEMBER(NameTypeHash,string,source)>,
    hashed_non_unique< tag<xform>,  BOOST_MULTI_INDEX_MEMBER(NameTypeHash,string,xform)>
  > // index_by
>; // mult_index_container


#endif










//using NthMap = multi_index_container< NameTypeHash,
//  indexed_by<
//    //random_access<>,
//    hashed_non_unique< tag<id>, BOOST_MULTI_INDEX_MEMBER(NameTypeHash,ui64,id) >,
//    //random_access<>,
//    //ordered_non_unique< tag<hsh>,    BOOST_MULTI_INDEX_MEMBER(NameTypeHash,ui64,hsh)>,
//    //random_access<>,
//    //ordered_non_unique< tag<name>,   BOOST_MULTI_INDEX_MEMBER(NameTypeHash,string,name)>,
//    //random_access<>,
//    //ordered_non_unique< tag<type>,   BOOST_MULTI_INDEX_MEMBER(NameTypeHash,string,type)>
//    //random_access<>,
//    hashed_non_unique< tag<source>, BOOST_MULTI_INDEX_MEMBER(NameTypeHash,string,source)>,
//    //ordered_non_unique< tag<source>, BOOST_MULTI_INDEX_MEMBER(NameTypeHash,string,source)>,
//    //random_access<>,
//    hashed_non_unique< tag<xform>,  BOOST_MULTI_INDEX_MEMBER(NameTypeHash,string,xform)>
//    //ordered_non_unique< tag<xform>,  BOOST_MULTI_INDEX_MEMBER(NameTypeHash,string,xform)>
//  >/*index_by*/ 
//>;/*mult_index*/

//using NthMap = multi_index_container<
//                 NameTypeHash, 
//                  indexed_by<
//                   ordered_unique<
//                    tag<name>, BOOST_MULTI_INDEX_MEMBER(NameTypeHash,string,name)>,
//                   ordered_non_unique<
//                    tag<type>, BOOST_MULTI_INDEX_MEMBER(NameTypeHash,string,type)>,
//                   ordered_non_unique<
//                    tag<hsh>,  BOOST_MULTI_INDEX_MEMBER(NameTypeHash,ui64,hsh)>/*order*/ 
//                  >/*index_by*/ 
//                >;/*mult_index*/


