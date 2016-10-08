

#include "JuceHeader.h"
#include <filesystem>
#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <moodycamel/concurrentqueue.h>
#include <cpplinq/cpplinq.hpp>
#include <SIM/Types.h>
#include <SIM/ColorSamples.hpp>
#include <SIM/SIM_GeneralTemplateUtil.hpp>
#include <SIM/SIM_Concurrency.hpp>
#include "DataType.h"
#include "VisualizerTypes.h"
#include "Shape.hpp"
#include "FileUtil.hpp"
#include "JuceUtil.hpp"
//#include "../IndexedVerts_datatype/IndexedVerts_datatype.h"






