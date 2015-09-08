#ifndef MOUSE_
#define MOUSE_
#include <application.h>

struct mouse_t{
  int x,y;
};

void mouseInit();
point_t mouseCadLastClickPos();
void mouseCallback(int, int, int, int);
void mouseMotionCallback(int, int);
#endif
