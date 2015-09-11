#include <CAD.h>

using namespace std;
#define CAD_ARC 0x00
#define CAD_LINE 0x01

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
  D printf("(cadDrawLine) Drawing line at (%.6f, %.6f, %.6f) ===== (%.6f, %.6f, %.6f)\n", start.x, start.y, start.z, end.x, end.y, end.z);
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
void cadRender()
{
  for (int i = 0; i < cadEntityArrayIndex; i++)
  {
      if (cadEntityArray[i].Type == CAD_LINE && !cadEntityArray[i].Removed) //Where a line and its not been removed
      {
        if (cadEntityArray[i].Selected)
        {
          sceneColor(WHITE);
        }
        else
        {
          sceneColor(cadEntityArray[i].Color);
        }
        cadRenderLine(cadEntityArray[i].Line);
      }
      if (cadEntityArray[i].Type == CAD_ARC && !cadEntityArray[i].Removed && cadEntityArray[i].Arc.radius > 0)
      {
        D printf("(cadRender) Found arc!\n");
        if (cadEntityArray[i].Selected)
        {
          sceneColor(WHITE);
        }
        else
        {
          sceneColor(cadEntityArray[i].Color);
        }
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
}
void cadRenderArc(arc_t a)
{
  float start_angle;
  float end_angle;
  float inc; //Increment angle

  if (a.direction == ARC_CW)
  {
    start_angle = 6.28318531;
    end_angle = 0;
    inc = -0.001;
  }
  else
  {
    start_angle = 0;
    end_angle = 6.28318531;
    inc = 0.001; //Increment angle
  }
  cout << KRED << "Start angle: " << geoGetArcStartAngle(a) << "\n" << KNORMAL;
  float r = 5;
  point_t p;
	glBegin(GL_LINE_STRIP);//since the arc is not a closed curve, this is a strip now
  bool found_start = false;
  bool found_end = false;
  int count = 0;

  auto op = [](arc_t a, float i, float end_angle) {
    if (a.direction == ARC_CW)
    {
      return i > end_angle;
    }
    else
    {
      return i < end_angle;
    }
  };

  for(float i = start_angle; op(a, i, end_angle) ; i=i + inc)
  {
    p.x = a.center.x + cos(i) * r;
    p.y = a.center.y + sin(i) * r;
    if (count > 0 && found_start == false && isSimilar(a.start.x, p.x) && isSimilar(a.start.y, p.y))
    {
      cout << KRED << ">> Found Start at angle: " << i << "\n" << KNORMAL;
      found_start = true;
    }
    if (count > 0 && found_start == true && isSimilar(a.end.x, p.x) && isSimilar(a.end.y, p.y))
    {
      cout << KRED << ">> Found End!\n" << KNORMAL;
      found_end = true;
    }
    //cout << KGREEN << ">> " << p.x << ", " << p.y << "\n" << KNORMAL;
    if (found_start && !found_end)
    {
      glVertex3f(p.x, p.y, 0);
    }
    count ++;
  }
	glEnd();
}
void cadRedraw()
{
  glutPostRedisplay();
}
