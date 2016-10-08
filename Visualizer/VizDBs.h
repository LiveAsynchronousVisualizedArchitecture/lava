
//#define VIZDBS_COMPILE

#ifndef VIZDBS_COMPILE

  #if defined(_WIN32)
    #pragma once
  #endif

  #ifndef __VIZDBS_HEADER_GUARD__
  #define __VIZDBS_HEADER_GUARD__

    #include <SIM/MemStore.hpp>

    extern MemStore     bindb;
    extern MemStore    namedb;
    extern MemStore    typedb;
    extern MemStore     srcdb;
    extern MemStore   xformdb;

  #endif 

#else  // End VIZDBS_HEADER

  #ifndef __VIZDBS_COMPILE_GUARD__
  #define __VIZDBS_COMPILE_GUARD__

    #define __VIZDBS_HEADER_GUARD__
    #include <SIM/MemStore.hpp>

    MemStore     bindb;
    MemStore    namedb;
    MemStore    typedb;
    MemStore     srcdb;
    MemStore   xformdb;

    struct VizDBsInit
    {
      VizDBsInit()
      {
          bindb.init(   "bin",  MemStore::GB * 3);
         namedb.init(  "name",  MemStore::GB * 3);
         typedb.init(  "type",  MemStore::GB * 3);
          srcdb.init(   "src",  MemStore::GB * 3);
        xformdb.init( "xform",  MemStore::GB * 3);
      }
    };

    VizDBsInit __vizDBsInit;

    #undef VIZDBS_COMPILE

  #endif
#endif

