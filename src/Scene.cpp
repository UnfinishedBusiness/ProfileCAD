#include <Scene.h>

using namespace std;
float ax, ay, az;       /* angles for orbit */
float tx, ty, tz;       /* factors for paning */
float scale = 0.1;          /* factor for scaling */
D int s = 0;

point_t sceneLastZoomToMouse;

color_t BackgroundColor = BACKGROUND;
void sceneInit()
{
  tx, ty, tz = 0;
  ////glutDisplayFunc(sceneDraw);
  //sceneSetViewAngle(10, -10, 0);
}
void sceneIncViewAngle(float x, float y, float z)
{
  ax = ax + x;
  ay = ay + y;
  az = az + z;
  PostRedisplay();
}
void sceneSetViewAngle(float x, float y, float z)
{
  ax = x;
  ay = y;
  az = z;
  PostRedisplay();
}
point_t sceneGetViewAngle()
{
  return point_t{ax, ay, az};
}
void sceneIncZoom(float inc)
{
  scale += inc;
  if (scale <= 0.001)
  {
    scale = 0.001;
  }
  //scale = scale + inc;
  //cout << "Scale: " << scale << endl;
  //V cout << KRED << "(sceneIncZoom) scale = " <<  KGREEN << scale << KNORMAL << "\r";
  //V fflush(stdout);
  PostRedisplay();
}
void sceneZoomToMouse(float inc, point_t pos)
{
  sceneIncZoom(inc);
  tx = (pos.x * -1);
  ty = (pos.y * -1);
  point_t m = cadCadCordToScreenCord(point_t{0, 0, 0});
  //glutWarpPointer(m.x, m.y);
  //cout << "pos.x: " << pos.x << " pos.y: " << pos.y << endl;
  PostRedisplay();
}
void sceneIncPan(float x, float y, float z)
{
  tx = tx + x;
  ty = ty + y;
  tz = tz + z;
  PostRedisplay();
}
void sceneColor(color_t c)
{
  glColor3f(c.r, c.g, c.b);
}
float sceneGetScale()
{
  return scale;
}
point_t sceneGetPanOffset()
{
  return point_t{tx, ty, tz};
}
void sceneDraw(void)
{
  glClearColor(BackgroundColor.r, BackgroundColor.g, BackgroundColor.b, BackgroundColor.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //glMatrixMode(GL_MODELVIEW);

  glTranslatef(tx, ty, tz);

  glRotatef(ax, 1.0f, 0.0f, 0.0f);
  glRotatef(-ay, 0.0f, 1.0f, 0.0f);
  glScalef(scale, scale, scale);
  //glTranslatef(tx, ty, tz);

  //cadRender();



  glLineWidth(10);
  glColor3f(1.0, 1.0, 0.0);
  glBegin(GL_LINES);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(500, 0, 0);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(500, 500, 0);
  glEnd();
  //glPopMatrix();


}
