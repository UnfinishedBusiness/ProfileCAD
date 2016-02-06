#ifndef SCENE_
#define SCENE_
#include "application.h"

void sceneInit();
void sceneDraw(void);
//void sceneDrawWireframe(int);
void sceneSetViewAngle(float, float, float);
void sceneIncViewAngle(float, float, float);
point_t sceneGetViewAngle();
void sceneIncZoom(float);
void sceneZoomToMouse(float, point_t);
void sceneIncPan(float, float, float);
point_t sceneGetPanOffset();
void sceneColor(color_t);
float sceneGetScale();
#endif
