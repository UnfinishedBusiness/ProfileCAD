#ifndef MOUSE_
#define MOUSE_
#include <application.h>

struct mouse_t{
  int x,y;
};

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
void mouseClick(int, int, float, float);
void mouseMotionCallback(int, int);
void mousePassiveMotionCallback(float , float);

void mouseLiveShow(std::string);
void mouseLiveClear();

#endif
