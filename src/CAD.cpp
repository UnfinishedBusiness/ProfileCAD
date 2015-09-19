#include <CAD.h>

using namespace std;

#define CAD_ARC 0x00
#define CAD_LINE 0x01

cadSelectionBox_t cadSelectionBox;

vector<cadEntity> cadEntityArray;
int cadEntityArrayIndex;
color_t cadColorAttribute = GREEN;

void cadInit()
{
  cadEntityArrayIndex = 0;
}
void cadAppend(cadEntity e)
{
  e.Selected = false;
  e.Removed = false;
  e.Index = cadEntityArrayIndex;
  cadEntityArray.push_back(cadEntity());
  cadEntityArray[cadEntityArrayIndex] = e;
  cadEntityArrayIndex++;
  glutPostRedisplay();
}
void cadEdit(int i, cadEntity e)
{
  if (cadEntityArrayIndex > i)
  {
    cadEntityArray[i] = e;
  }
  glutPostRedisplay();
}
void cadDrawLine(line_t l)
{
  cadDrawLine(l.start, l.end);
}
void cadDrawLine(point_t start, point_t end)
{
  //D printf("(cadDrawLine) Drawing line at (%.6f, %.6f, %.6f) ===== (%.6f, %.6f, %.6f)\n", start.x, start.y, start.z, end.x, end.y, end.z);
  cadAppend(cadEntity{CAD_LINE, cadColorAttribute, line_t{ start, end }, arc_t{} });
}
void cadDrawArc(arc_t a)
{
  cadAppend(cadEntity{CAD_ARC, cadColorAttribute, line_t{}, a });
}
void cadSetColor(color_t c)
{
  cadColorAttribute = c;
}
cadEntity cadGetEntityArray(int x)
{
  if (cadEntityArrayIndex > x)
  {
    return cadEntityArray[x];
  }
}
int cadGetEntityArrayIndex()
{
  return cadEntityArrayIndex;
}
void cadRemoveSelected()
{
  for (int i = 0; i < cadEntityArrayIndex; i++)
  {
      if (cadEntityArray[i].Selected && !cadEntityArray[i].Removed) //Make sure were selected and not removed
      {
        cadEntityArray[i].Removed = true;
      }
  }
}
std::vector<cadEntity> cadGetSelected()
{
  std::vector<cadEntity> e;
  for (int i = 0; i < cadEntityArrayIndex; i++)
  {
      if (cadEntityArray[i].Selected && !cadEntityArray[i].Removed) //Make sure were selected and not removed
      {
        e.push_back(cadEntity());
        e[e.size()-1] = cadEntityArray[i];
      }
  }
  return e;
}
int cadCountSelection()
{
  int r = 0;
  for (int i = 0; i < cadEntityArrayIndex; i++)
  {
      if (cadEntityArray[i].Selected && !cadEntityArray[i].Removed) //Make sure were selected and not removed
      {
        r++;
      }
  }
  return r;
}
void cadShowSelectionBox(point_t p)
{
  //line_t geoGetPerpendicularLine(line_t, point_t, float);
  //line_t geoGetParallelLine(line_t, point_t, float);
  cadSelectionBox.a.radius = 0.015 / sceneGetScale();
  cadSelectionBox.a.center = p;
  cadSelectionBox.a.start = p;
  cadSelectionBox.a.start.x += cadSelectionBox.a.radius;
  cadSelectionBox.a.end = p;
  cadSelectionBox.a.end.x += cadSelectionBox.a.radius;
  cadSelectionBox.visable = true;
}
void cadHideSelectionBox()
{
  cadSelectionBox.visable = false;
}
void cadRender()
{
  if (cadSelectionBox.visable == true)
  {
    //printf("Rendering selection box!\n");
    sceneColor(DARKGREY);
    cadRenderArc(cadSelectionBox.a);
  }
  for (int i = 0; i < cadEntityArrayIndex; i++)
  {
      if (cadEntityArray[i].Type == CAD_LINE && !cadEntityArray[i].Removed) //Where a line and its not been removed
      {
        if (cadEntityArray[i].Selected || cadEntityArray[i].MouseOver)
        {
          sceneColor(WHITE);
        }
        else
        {
          sceneColor(cadEntityArray[i].Color);
        }
        cadEntityArray[i].Line.parentIndex = i;
        cadRenderLine(cadEntityArray[i].Line);
      }
      if (cadEntityArray[i].Type == CAD_ARC && !cadEntityArray[i].Removed && cadEntityArray[i].Arc.radius > 0)
      {
        //D printf("(cadRender) Found arc!\n");
        if (cadEntityArray[i].Selected || cadEntityArray[i].MouseOver)
        {
          sceneColor(WHITE);
        }
        else
        {
          sceneColor(cadEntityArray[i].Color);
        }
        cadEntityArray[i].Arc.parentIndex = i;
        cadRenderArc(cadEntityArray[i].Arc);
      }
  }
}
void cadRenderLine(line_t l)
{
  glLineWidth(1);
  glBegin(GL_LINE_LOOP);
  glVertex3f((GLfloat) l.start.x, l.start.y, l.start.z);
  glVertex3f((GLfloat) l.end.x, l.end.y, l.end.z);
  glEnd();

  //glBegin(GL_POINTS);
  cadEntityArray[l.parentIndex].Vector.clear();
  cadEntityArray[l.parentIndex].Vector = geoGetPointsOfLine(l);
  /*for (int x = 0; x < cadEntityArray[l.parentIndex].Vector.size(); x++)
  {
      glVertex3f((GLfloat) cadEntityArray[l.parentIndex].Vector[x].x, (GLfloat) cadEntityArray[l.parentIndex].Vector[x].y, 0);
  }
  glEnd();*/
}
void cadRenderArc(arc_t a)
{
  float includedAngle = geoGetIncludedAngle(a);
  line_t l = line_t{ point_t{a.center.x, a.center.y,} , point_t{a.start.x, a.start.y} };
  glBegin(GL_LINE_STRIP);
  if (a.start.x != a.end.x && a.start.y != a.end.y)
  {
    glVertex3f(a.start.x, a.start.y, 0);
  }
  int steps;
  if (a.start == a.end)
  {
    //Were a circle
    steps = 361; //Go the full 360 to close the gap
  }
  else
  {
    steps = geoGetIncludedAngle(a);
  }
  float inc_angle = 1; //Degrees
  for (int x=0; x < steps; x++)
  {
    if (a.direction == ARC_CW)
    {
      l = geoRotateLine(l, a.center, -inc_angle);
    }
    else
    {
      l = geoRotateLine(l, a.center, inc_angle);
    }
    glVertex3f(l.end.x, l.end.y, 0);
    if (cadEntityArray[a.parentIndex].Vector.size() != steps) //Populate vector if it hasnt already been
    {
      cadEntityArray[a.parentIndex].Vector.push_back(point_t());
      cadEntityArray[a.parentIndex].Vector[x] = l.end;
    }
  }
  if (a.start.x != a.end.x && a.start.y != a.end.y)
  {
    glVertex3f(a.end.x, a.end.y, 0);
  }

  glEnd();
}
void cadRedraw()
{
  glutPostRedisplay();
}
point_t cadScreenCordToCadCord(int x, int y)
{
  GLint viewport[4]; //var to hold the viewport info
  GLdouble modelview[16]; //var to hold the modelview info
  GLdouble projection[16]; //var to hold the projection matrix info
  GLfloat winX, winY, winZ; //variables to hold screen x,y,z coordinates
  GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates
  glGetDoublev( GL_MODELVIEW_MATRIX, modelview ); //get the modelview info
  glGetDoublev( GL_PROJECTION_MATRIX, projection ); //get the projection matrix info
  glGetIntegerv( GL_VIEWPORT, viewport ); //get the viewport info
  winX = (float)x;
  winY = (float)viewport[3] - (float)y;
  winZ = 0;
  //get the world coordinates from the screen coordinates
  gluUnProject( winX, winY, winZ, modelview, projection, viewport, &worldX, &worldY, &worldZ);
  //printf("tx: %.6f, ty: %.6f\n", sceneGetPanOffset().x, sceneGetPanOffset().y);
  worldX = (worldX/sceneGetScale());
  worldY = (worldY/sceneGetScale());
  point_t panOffset = sceneGetPanOffset();
  return point_t{(float)worldX - (panOffset.x / sceneGetScale()), (float)worldY - (panOffset.y / sceneGetScale()), 0};
}
