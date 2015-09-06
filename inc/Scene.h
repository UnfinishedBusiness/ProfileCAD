#ifndef SCENE_
#define SCENE_
#include "application.h"
static int stencilOn = 1;

static GLfloat cube[8][3] =
{0.0, 0.0, 0.0,
  1.0, 0.0, 0.0,
  1.0, 0.0, 1.0,
  0.0, 0.0, 1.0,
  1.0, 1.0, 0.0,
  1.0, 1.0, 1.0,
  0.0, 1.0, 1.0,
  0.0, 1.0, 0.0};

static int faceIndex[6][4] =
{0, 1, 2, 3,
  1, 4, 5, 2,
  4, 7, 6, 5,
  7, 0, 3, 6,
  3, 2, 5, 6,
  7, 4, 1, 0};
void sceneInit();
void sceneDraw(void);
void sceneDrawWireframe(int);
void sceneDrawFilled(int);
void sceneSetViewAngle(float, float, float);
void sceneIncViewAngle(float, float, float);
point_t sceneGetViewAngle();

#endif
