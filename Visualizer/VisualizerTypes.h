


#ifndef __VISUALIZER_TYPES_HEADER_GUARD__
#define __VISUALIZER_TYPES_HEADER_GUARD__

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>

using  ArOut  =  cereal::PortableBinaryOutputArchive;
using  ArIn   =  cereal::PortableBinaryInputArchive;

#endif
