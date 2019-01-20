#include "data.h"
#include <arpa/inet.h>

float htonf(float x)
{
   int i = htonl(*(int*)&x);
   return *(float*)(&i);
}

{
   union FU {
      float xf;
      int   xi;
   } xu;
   xu.xf = x;
   xu.xi = htonl(xu.xi);
   return xu.xf;
}

float ntohf(float x)
{
   union FU {
      float xf;
      int   xi;
   } xu;
   xu.xf = x;
   xu.xi = ntohl(xu.xi);
   return xu.xf;
}
