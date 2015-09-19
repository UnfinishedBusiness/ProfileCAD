#ifndef MOUSE_
#define MOUSE_
#include <application.h>

struct mouse_t{
  int x,y;
};

void mouseInit();
point_t mouseCadLastClickPos();
point_t mouseCadLastSnapClick();
void mouseCallback(int, int, int, int);
void mouseMotionCallback(int, int);
void mousePassiveMotionCallback(int , int);
#endif
