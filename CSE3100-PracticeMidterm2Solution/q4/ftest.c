#include "data.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
   float f0 = 3.141592;
   float f1 = htonf(f0);

   float f2 = ntohf(f1);
   unsigned char* ba1 = (unsigned char*)&f0;
   unsigned char* ba2 = (unsigned char*)&f1;
   unsigned char* ba3 = (unsigned char*)&f2;

   printf("Converting back and forth from f0 -> f1 -> f2\n");
   printf("With: f0 : a float in host order (little endian)\n");
   printf("      f1 : a float in network order (big endian)\n");
   printf("      f2 : a float in host order (little endian)\n");
   printf("f0    as byte array: [%d,%d,%d,%d]\n",ba1[0],ba1[1],ba1[2],ba1[3]);
   printf("f1    as byte array: [%d,%d,%d,%d]\n",ba2[0],ba2[1],ba2[2],ba2[3]);
   printf("f2    as byte array: [%d,%d,%d,%d]\n",ba3[0],ba3[1],ba3[2],ba3[3]);
   printf("Converted back and forth: f0=%f  f1=%f  f2=%f\n",f0,f1,f2);
   return 0;
}
