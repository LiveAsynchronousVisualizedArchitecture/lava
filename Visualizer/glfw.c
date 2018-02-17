

// sbassett - this file compiles glfw from source into its own compilation unit

// glfw_3.2.1.h is glfw condensed into a single header file library
// currently this will only work with windows, but could be made to work universally with a little more work
// it allows compiling from source easily, which means:
// - extra flexibility 
// - less build complexity
// - less depedencies since the glfw .dll is no longer neccesary 
#define GLFW_IMPL
#include "glfw_3.2.1.h"   
