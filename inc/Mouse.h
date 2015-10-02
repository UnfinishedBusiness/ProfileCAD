#ifndef MOUSE_
#define MOUSE_
#include <application.h>

struct mouse_t{
  int x,y;
};

void mouseInit();

void mouseToggleLineEndpointSnap();
void mouseToggleArcEndpointSnap();
void mouseToggleCenterSnap();
void mouseToggleMidpointSnap();
void mouseToggleVectorSnap();
void mouseToggleIntersectionSnap();
std::vector<std::string> mouseUISnapIndicator();

point_t mouseCadGetCurrentPos();
point_t mouseCadLastClickPos();
point_t mouseCadLastSnapClick();
void mouseCallback(int, int, int, int);
void mouseMotionCallback(int, int);
void mousePassiveMotionCallback(int , int);

void mouseLiveShow(std::string);
void mouseLiveClear();

#endif
