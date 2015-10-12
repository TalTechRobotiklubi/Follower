#include "cubic_hermite.h"

/*
float cubic_hermite(float a, float b, float c, float d, float t) {
  const float al = -a * 0.5f + (3.0f * b) * 0.5f - (3.0f * c) * 0.5f + d * 0.5f;
  const float bl = a - (5.0f * b) * 0.5f + 2.0f * c - d * 0.5f;
  const float cl = -a * 0.5f + c * 0.5f;
  
  return al*t*t*t + bl*t*t + cl*t + b;
}
*/

float cubic_hermite(
   float a,float b,
   float c,float d,
   float t,
   float tension,
   float bias)
{
   float m0,m1,t2,t3;
   float a0,a1,a2,a3;

  t2 = t * t;
  t3 = t2 * t;
   m0  = (b-a)*(1+bias)*(1-tension)/2;
   m0 += (c-b)*(1-bias)*(1-tension)/2;
   m1  = (c-b)*(1+bias)*(1-tension)/2;
   m1 += (d-c)*(1-bias)*(1-tension)/2;
   a0 =  2*t3 - 3*t2 + 1;
   a1 =    t3 - 2*t2 + t;
   a2 =    t3 -   t2;
   a3 = -2*t3 + 3*t2;

   return(a0*b+a1*m0+a2*m1+a3*c);
}

