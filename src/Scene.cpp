#include <Scene.h>

using namespace std;
float ax, ay, az;       /* angles for animation */
float tx, ty, tz;
D int s = 0;
void sceneInit()
{
  tx, ty, tz = 0;
  glutDisplayFunc(sceneDraw);
  sceneSetViewAngle(10, -10, 0);
}
void sceneIncViewAngle(float x, float y, float z)
{
  ax = ax + x;
  ay = ay + y;
  az = az + z;
  glutPostRedisplay();
}
void sceneSetViewAngle(float x, float y, float z)
{
  ax = x;
  ay = y;
  az = z;
  glutPostRedisplay();
}
point_t sceneGetViewAngle()
{
  return point_t{ax, ay, az};
}
void sceneIncZoom(float inc)
{
  if (inc > 0)
  {
    inc = inc + 1;
  }
  else
  {
    inc = 1 + inc;
  }
  glScalef(inc, inc, inc);
  glutPostRedisplay();
}
void sceneIncPan(float x, float y, float z)
{
  tx = tx + x;
  ty = ty + y;
  tz = tz + z;
  glutPostRedisplay();
}
void sceneDrawWireframe(int face)
{
  glBegin(GL_LINE_LOOP);
  /*for (int i = 0; i < 8; i++)
    glVertex3fv((GLfloat *) cube[faceIndex[face][i]]);*/

  glEnd();
}
void sceneDraw(void)
{
  D printf("(sceneDraw) %d\r", s++);
  D fflush(stdout);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPushMatrix();

  glRotatef(ax, 1.0, 0.0, 0.0);
  glRotatef(-ay, 0.0, 1.0, 0.0);

  glTranslatef(tx, ty, tz);


  glEnable(GL_STENCIL_TEST);
  glClear(GL_STENCIL_BUFFER_BIT);
  glStencilMask(1);
  glStencilFunc(GL_ALWAYS, 0, 1);
  glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);

  glColor3f(1.0, 1.0, 1.0); //white
  for (int i = 0; i < 6; i++)
  {
    sceneDrawWireframe(i);
    if (stencilOn) {
      glStencilFunc(GL_EQUAL, 0, 1);
      glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    }
    glColor3f(0.0, 0.0, 0.0);

    glColor3f(1.0, 1.0, 1.0); //white
    if (stencilOn) {
      glStencilFunc(GL_ALWAYS, 0, 1);
      glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
    }
    glColor3f(1.0, 1.0, 1.0); //white
    sceneDrawWireframe(i);
  }
  glPopMatrix();
  glDisable(GL_STENCIL_TEST);

  /* end of good stuff */

  glutSwapBuffers();
}
