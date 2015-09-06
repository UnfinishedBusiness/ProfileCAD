#ifndef APPLICATION_
#define  APPLICATION_

#define DEBUG

#ifdef DEBUG
 #define D
#else
 #define D for(;0;)
#endif

struct point_t{
  float x,y,z;
};

#endif
