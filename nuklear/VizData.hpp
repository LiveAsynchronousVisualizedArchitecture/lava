
/*
  This file contains static data meant to go into the executable
*/

#ifdef _MSC_VER
  #pragma once
#endif

#ifndef __VIZ_DATA_HEADERGUARD_HPP__
#define __VIZ_DATA_HEADERGUARD_HPP__

#include "../DataStore/ConcurrentMap/simdb.hpp"
#include "VizDecl.h"

extern "C" unsigned char Roboto_Regular_ttf[];
extern "C" unsigned int  Roboto_Regular_ttf_len;

extern "C" unsigned char Roboto_Bold_ttf[];
extern "C" unsigned int  Roboto_Bold_ttf_len;

extern "C" unsigned char entypo_ttf[];
extern "C" unsigned int  entypo_ttf_len;

static const char* hotkeyGuideH = 
" h    -> Home : Returns camera to default position.";
static const char* hotkeyGuideF =
" f    -> Fit  :  Fits  view to currently visible geometry.";
static const char* hotkeyGuidePage =
" 'Page Up' and 'Page Down' :  Particle and line size.";

static const char*  vertShader   = 
"#version 330 core\n"
"\n"
"uniform mat4 transform; \n"
"\n"
"layout(location = 0) in vec3  P; \n"
"layout(location = 1) in vec3  N; \n"
"layout(location = 2) in vec4  C; \n"
"layout(location = 3) in vec2 UV; \n"
"\n"
"out vec3  fragN; \
out vec4  fragC; \
out vec2 fragUV; \
\
void main(){ \
  gl_Position = transform * vec4(P, 1.0f);  \
  fragN  =  N; \
  fragC  =  C; \
  fragUV = UV; \
}";
static const char*  fragShader   = 
"#version 330 core\n"
"\
in vec3 fragN;  \
in vec4 fragC;  \
in vec2 fragUV; \
\
out vec4 Ci; \
\
uniform sampler2D tex0; \n \
\
void main(){ \
  Ci = fragC; \n \
  vec4 tClr =  texture(tex0, fragUV); \n \
  //Ci        =  vec4(tClr.xyz*tClr.a,tClr.a) + ( (1-tClr.a)*fragC ); \n \
  //Ci        =  vec4(tClr.rgb*tClr.a,tClr.a) + ( (1-tClr.a)*fragC ); \n \
  Ci        =  tClr + ( (1-tClr.a)*fragC ); \n \
  \
  Ci = vec4(1,1,1,1); \
}";

//static const char*  fragShader = 
//R"frag(
//#version 330 core\n
//
//in vec3 fragN; 
//in vec4 fragC; 
//in vec2 fragUV;
//
//out vec4 Ci;
//
//uniform sampler2D tex0; \n
//
//void main()
//{
//  Ci = fragC; \n
//  vec4 tClr =  texture(tex0, fragUV); \n
//  //Ci        =  vec4(tClr.xyz*tClr.a,tClr.a) + ( (1-tClr.a)*fragC ); \n
//  //Ci        =  vec4(tClr.rgb*tClr.a,tClr.a) + ( (1-tClr.a)*fragC ); \n
//  Ci        =  tClr + ( (1-tClr.a)*fragC ); \n
//
//  Ci = vec4(1,1,1,1);
//} 
//)frag";

static simdb    db;
static VizData  vd;

#endif

