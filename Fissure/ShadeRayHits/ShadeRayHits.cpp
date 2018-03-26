

#include <cmath>
#include "../../no_rt_util.h"
#include "../shared/vec.hpp"
#include "../../tbl.hpp"
#include "../LavaFlow.hpp"

#include "../shared/math/affinespace.h"
#include "../shared/math/linearspace3.h"

enum        Slots
{
  // This is an example enumeration that is meant to be helpful, though is not strictly neccesary. Referencing slots by a name will generally be less error prone than using their index and remembering what each index is for
  SLOT_IN  = 0,        
  SLOT_OUT = 0
};

using embree::Vec3f;

struct BrdfResult
{
  Vec3f  m_spectrum;
  Vec3f  m_L;
  float  m_pdf;

  BrdfResult() : m_spectrum(0.f), m_L(0.f), m_pdf(0.f) {  }
  BrdfResult(Vec3f spectrum, Vec3f L, float pdf)
    : m_spectrum(spectrum), m_L(L), m_pdf(pdf) {}
  bool isZero()
  {
    if (m_spectrum.x != 0.f) return false;
    if (m_spectrum.y != 0.f) return false;
    if (m_spectrum.z != 0.f) return false;
    return true;
  }
};

template<class T> T sqr(T x){ return x*x; };

Vec3f    TransformFromNormal(const Vec3f& from, const Vec3f& to, const Vec3f& P)
{
  using namespace embree;

  Vec3f nV1 = normalize(from);
  Vec3f nV2 = normalize(to);

  float cos_theta = dot(nV1, nV2);
  if( cos_theta > 0.999999f )
    return P;
  if( cos_theta < -0.999999f )
    return P * -1.0f;
  float theta = embree::acos(cos_theta);
  float sin_theta = embree::sin(theta);

  // vector to rotate about
  Vec3f a = normalize(embree::cross(nV1, nV2));

  float m00 = a.x * a.x + (1.0 - a.x *a.x)  * cos_theta;
  float m01 = a.x * a.y * (1.0 - cos_theta) - a.z * sin_theta;
  float m02 = a.x * a.z * (1.0 - cos_theta) + a.y * sin_theta;

  float m10 = a.x * a.y * (1.0 - cos_theta) + a.z * sin_theta;
  float m11 = a.y * a.y + (1.0 - a.y * a.y) * cos_theta;
  float m12 = a.y * a.z * (1.0 - cos_theta) - a.x * sin_theta;

  float m20 = a.x * a.z * (1.0 - cos_theta) - a.y * sin_theta;
  float m21 = a.y * a.z * (1.0 - cos_theta) + a.x * sin_theta;
  float m22 = a.z * a.z + (1.0 - a.z * a.z) * cos_theta;

  LinearSpace3<Vec3f> mtx(   m00, m01, m02,
    m10, m11, m12,
    m20, m21, m22  );

  // should be neccesary, matrix constructor says row major
  return xfmPoint(mtx, P);
}
Vec3f    UniformSampleSphere(float u1, float u2)
{
  float y    =  1.0f - 2.0f * u1;
  float r    =  std::sqrt(std::max(0.0f, 1.0f - y * y));
  float phi  =  2.0f * PIf * u2;
  float z    =  r * cosf(phi);
  float x    =  r * sinf(phi);
  return Vec3f(x, y, z);
}
void VNDFSamplerGGX_sample11(
  const double theta_i,               // input   // incident direction
  double U1, double U2,                          // random numbers
  double& slope_x, double& slope_y    // output  // slopes
)
{
  // special case (normal incidence)
  if(theta_i < 0.0001)
  {
    const double r = sqrt(U1/(1-U1));
    const double phi = 6.28318530718 * U2;
    slope_x = r * cos(phi);
    slope_y = r * sin(phi);
    return;
  }

  // precomputations
  const double tan_theta_i = tan(theta_i);
  const double a = 1 / (tan_theta_i);
  const double G1 = 2 / (1 + sqrt(1.0+1.0/(a*a)));

  // sample slope_x
  const double A = 2.0*U1/G1 - 1.0;
  const double tmp = 1.0 / (A*A-1.0);
  const double B = tan_theta_i;
  const double D = sqrt(B*B*tmp*tmp - (A*A-B*B)*tmp);
  const double slope_x_1 = B*tmp - D;
  const double slope_x_2 = B*tmp + D;
  slope_x = (A < 0 || slope_x_2 > 1.0/tan_theta_i) ? slope_x_1 : slope_x_2;

  // sample slope_y
  double S;
  if(U2 > 0.5){
    S = 1.0;
    U2 = 2.0*(U2-0.5);
  }else{
    S = -1.0;
    U2 = 2.0*(0.5-U2);
  }

  const double z = (U2*(U2*(U2*0.27385-0.73369)+0.46341)) / (U2*(U2*(U2*0.093073+0.309420)-1.000000)+0.597999);
  slope_y = S * z * sqrt(1.0+slope_x*slope_x);
}
void VNDFSampler_sample(
  const double omega_i[3],                       // input  // incident direction
  const double alpha_x, const double alpha_y,              // anisotropic roughness
  const double U1, const double U2,                        // random numbers
  double omega_m[3])                            // output // micronormal
{
  // 1. stretch omega_i
  double omega_i_[3];
  omega_i_[0] = alpha_x * omega_i[0];
  omega_i_[1] = alpha_y * omega_i[1];
  omega_i_[2] = omega_i[2];

  // normalize
  double inv_omega_i = 1.0 / sqrt(omega_i_[0]*omega_i_[0] + omega_i_[1]*omega_i_[1] + omega_i_[2]*omega_i_[2]);
  omega_i_[0] *= inv_omega_i;
  omega_i_[1] *= inv_omega_i;
  omega_i_[2] *= inv_omega_i;

  // get polar coordinates of omega_i_
  double theta_ = 0.0;
  double phi_ = 0.0;
  if(omega_i_[2] < 0.99999)
  {
    theta_ = acos(omega_i_[2]);
    phi_ = atan2(omega_i_[1], omega_i_[0]);
  }

  // 2. sample P22_{ omega_i }( x_slope, y_slope, 1, 1)
  double slope_x, slope_y;
  VNDFSamplerGGX_sample11( theta_, U1, U2, slope_x, slope_y);

  // 3. rotate
  double tmp = cos(phi_)*slope_x - sin(phi_)*slope_y;
  slope_y = sin(phi_)*slope_x + cos(phi_)*slope_y;
  slope_x = tmp;

  // 4. unstretch
  slope_x = alpha_x * slope_x;
  slope_y = alpha_y * slope_y;

  // 5. compute normal
  double inv_omega_m = sqrt(slope_x*slope_x + slope_y*slope_y + 1.0);
  omega_m[0] = -slope_x/inv_omega_m;
  omega_m[1] = -slope_y/inv_omega_m;
  omega_m[2] = 1.0/inv_omega_m;
}
float       D_GGX(float alpha, float cosThetaM)
{
  using namespace std;
  
  float CosSquared = cosThetaM*cosThetaM;
  float TanSquared = (1-CosSquared)/CosSquared;
  return (1.0/PIf) * sqr(alpha/(CosSquared * (alpha*alpha + TanSquared)));
}
float       BRDF_D_GGX(Vec3f L, Vec3f V, Vec3f N, float alpha)
{
  Vec3f H = normalize( L + V );
  float D = D_GGX(alpha, dot(N,H));
  return D;
}
float       BrdfGGX(Vec3f L, Vec3f V, Vec3f N, float roughness)
{
  float  alpha  =  sqr(roughness);
  Vec3f  H      =  normalize( L + V );
  float  D      =  D_GGX(alpha, dot(N,H));
  return D;
}
BrdfResult  BrdfSampleGGX(float u1, float u2, Vec3f V, Vec3f N, float roughness)
{
  using namespace std;

  float alpha = sqr(roughness);

  Vec3f    from(0, 0, 1.f);
  Vec3f    wi = TransformFromNormal(N, from, V);

  double aryWi[3]  =  { wi.x, wi.y, wi.z };
  double aryH[3];
  VNDFSampler_sample(aryWi, alpha, alpha, u1, u2, aryH);

  Vec3f H      =   Vec3f(aryH[0], aryH[1], aryH[2]);
  Vec3f tH     =   TransformFromNormal(from, N, H);

  Vec3f L      =   2  *  dot(V, tH) * tH - V;
  float LdotN  =   dot(L, N);
  float D      =   BRDF_D_GGX(L, V, N, alpha);
  float pdf    =   D;     // / sqr(LdotN); // * 2.f * PI; // * (PI/2.f);

  return BrdfResult( Vec3f(D), L, pdf); // 0.f );
}
float       BrdfLambertian(v3 L, v3 V, v3 N)
{
  return dot(L,N);
}
BrdfResult  BrdfSampleLambertian(float u1, float u2, Vec3f V, Vec3f N)
{
  using namespace embree;

  auto       L   =  UniformSampleSphere(u1, u2);
  auto   LdotN   =  dot(L, N);
  auto   cLdotN  =  LdotN<0?  0  :  LdotN;
  auto   pdf     =  1.f / (4.f * PIf);

  return BrdfResult( Vec3f(LdotN), L, pdf); // 0.f );
}


extern "C"
{
  const char*  InTypes[]  = {"IdxVerts",           nullptr};            // This array contains the type that each slot of the same index will accept as input.
  const char*  InNames[]  = {"ShadeRayHits Slot In",   nullptr};            // This array contains the names of each input slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.
  const char* OutTypes[]  = {"IdxVerts",           nullptr};            // This array contains the types that are output in each slot of the same index
  const char* OutNames[]  = {"ShadeRayHits Slot Out",  nullptr};            // This array contains the names of each output slot as a string that can be used by the GUI.  It will show up as a label to each slot and be used when visualizing.

  void ShadeRayHits_construct(){ }
  void ShadeRayHits_destruct(){ }

  uint64_t ShadeRayHits(LavaParams const* lp, LavaFrame const* in, lava_threadQ* out) noexcept
  {
    using namespace std;

    u32 i=0;
    while( LavaNxtPckt(in, &i) )
    {
      tbl inputTbl( (void*)(in->packets[i-1].val.value) );

      for(auto& kv : inputTbl){  // this loop demonstrates how to iterate through non-empty map elements
      }	

      // out->push( LavaTblToOut(outputTbl, SLOT_OUT) );      // this demonstrates how to output a tbl into the first output slot
    }

    return 1;
  }

  LavaNode LavaNodes[] =
  {
    {
      ShadeRayHits,                                      // function
      ShadeRayHits_construct,                            // constructor - this can be set to nullptr if not needed
      ShadeRayHits_destruct,                             // destructor  - this can also be set to nullptr 
      LavaNode::FLOW,                                // node_type   - this should be eighther LavaNode::MSG (will be run even without input packets) or LavaNode::FLOW (will be run only when at least one packet is available for input)
      "ShadeRayHits",                                    // name
      InTypes,                                       // in_types    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      InNames,                                       // in_names    - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      OutTypes,                                      // out_types   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr 
      OutNames,                                      // out_names   - this can be set to nullptr instead of pointing to a list that has the first item as nullptr
      nullptr,                                       // description
      0                                              // version 
    },                                             

    LavaNodeListEnd                                  // This is a constant that has all the members of the LavaNode struct set to 0 or nullptr - it is used as a special value that ends the static list of LavaNodes. This negates the need for a separate static variable that gives the size of the list, which would be have to be kept in sync and therefore be error prone.
  };

  __declspec(dllexport) LavaNode* GetLavaNodes()
  {
    return (LavaNode*)LavaNodes;
  }
}






//
//auto   cLdotN  =  ClampLo(dot(L,N), 0.f);

//float z    =  1.0f - 2.0f * u1;
//float r    =  std::sqrt(std::max(0.0f, 1.0f - z * z));
//float phi  =  2.0f * PI * u2;
//float x    =  r * cosf(phi);
//float y    =  r * sinf(phi);
//return Vec3f(x, y, z);

//LinearSpace3f transMtx = mtx.transposed();
//Vector3f wat = P;

//float       D_GGX(float alpha, float cosThetaM)
//{
//  float CosSquared = cosThetaM*cosThetaM;
//  float TanSquared = (1-CosSquared)/CosSquared;
//  return (1.0/PI) * sqr(alpha/(CosSquared * (alpha*alpha + TanSquared)));
//}

//auto  theta    =  u2 * 2.f * PI;
//auto  uu2      =  embree::asin(theta) / 2.f * PI;
//
//auto   hemi    =  UniformSampleHemisphere(u1, u2);
//
//auto   hemi    =  CosineSampleHemisphere(u1, u2);
//Vec3f  L       =  TransformFromNormal(Vec3f(0, 1.0f, 0), N, hemi);
//
//auto   pdf     =  LdotN / PI;                    // cos theta
//auto   LdotN  =  dot(L, N);
//
//return BrdfResult( Vec3f(LdotN), L, 1.0/(2.f * PI), 0.f );

// Vec3f X, Vec3f Y)
//
//return Vec3f(D);
