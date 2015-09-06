#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include <iostream>
#include <Scene.h>

using namespace std;
float ax, ay, az;       /* angles for animation */
void sceneInit()
{
  glutDisplayFunc(sceneDraw);
  sceneSetViewAngle(10, -10, 0);
}
void sceneSetViewAngle(float x, float y, float z)
{
  ax = x;
  ay = y;
  az = z;
}
void sceneDrawFilled(int face)
{
  int i;
  glBegin(GL_POLYGON);
  for (i = 0; i < 4; i++)
    glVertex3fv((GLfloat *) cube[faceIndex[face][i]]);
  glEnd();
}
void sceneDrawWireframe(int face)
{
  int i;
  glBegin(GL_LINE_LOOP);
  for (i = 0; i < 4; i++)
    glVertex3fv((GLfloat *) cube[faceIndex[face][i]]);
  glEnd();
}
void sceneDraw(void)
{
  int i;
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
  glRotatef(ax, 1.0, 0.0, 0.0);
  glRotatef(-ay, 0.0, 1.0, 0.0);

  /* all the good stuff follows */

  if (stencilOn)
  {
    glEnable(GL_STENCIL_TEST);
    glClear(GL_STENCIL_BUFFER_BIT);
    glStencilMask(1);
    glStencilFunc(GL_ALWAYS, 0, 1);
    glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
  }
  glColor3f(1.0, 1.0, 0.0);
  for (i = 0; i < 6; i++)
  {
    sceneDrawWireframe(i);
    if (stencilOn) {
      glStencilFunc(GL_EQUAL, 0, 1);
      glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    }
    glColor3f(0.0, 0.0, 0.0);
    sceneDrawFilled(i);

    glColor3f(1.0, 1.0, 0.0);
    if (stencilOn) {
      glStencilFunc(GL_ALWAYS, 0, 1);
      glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
    }
    glColor3f(1.0, 1.0, 0.0);
    sceneDrawWireframe(i);
  }
  glPopMatrix();

  if (stencilOn)
    glDisable(GL_STENCIL_TEST);

  /* end of good stuff */

  glutSwapBuffers();
}
