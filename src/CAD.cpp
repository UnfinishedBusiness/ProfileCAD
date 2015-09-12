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
void cadRenderArcDecip(arc_t a)
{
  float step = 0.5;
  float start_angle;
  float end_angle;
  float length;
  float r = a.radius;
  auto arc_loop = [](arc_t a, float i, float r)
  {
    cout << KYELLOW << i << "," << KNORMAL;
    point_t p;
    p.x = a.center.x + cos(i) * r;
    p.y = a.center.y + sin(i) * r;
    //cout << KGREEN << ">> " << p.x << ", " << p.y << "\n" << KNORMAL;
    glVertex3f(p.x, p.y, 0);
  };
  glBegin(GL_LINE_STRIP);//since the arc is not a closed curve, this is a strip now
  /*if (a.direction == ARC_CW)
  {
    D cout << KRED << "\t> Arc CW!\n" << KNORMAL;
    end_angle = geoGetArcStartAngle(a);
    start_angle = geoGetArcEndAngle(a);

  }
  else
  {
    D cout << KRED << "\t> Arc CCW!\n" << KNORMAL;
    start_angle = geoGetArcStartAngle(a);
    end_angle = geoGetArcEndAngle(a);

  }*/
  start_angle = geoGetArcStartAngle(a);
  D cout << KGREEN << "Start angle: " << start_angle << "\n" << KNORMAL;
  //D cout << KGREEN << "End angle: " << end_angle << "\n" << KNORMAL;
  length = geoGetArchLength(a);
  D cout << KGREEN << "Length: " << length << "\n" << KNORMAL;
  if (a.direction == ARC_CW)
  {
    D cout << KGREEN << "\tArc CW\n" << KNORMAL;
    for(float i = start_angle; i < length; i=i + step) arc_loop(a, i, r);
  }
  else
  {
    D cout << KGREEN << "\tArc CCW\n" << KNORMAL;
    for(float i = start_angle; i > length; i=i - step) arc_loop(a, i, r);
  }




	glEnd();
  fflush(stdout);
}
//void drawArc(int x, int y, float startAngle, float endAngle, uint32_t radiusX, uint32_t radiusY,)
void cadRenderArc(arc_t a)
{
    if (a.radius <= 0) return;
    float step;
    float startAngle = geoGetArcStartAngle(a);
    float endAngle = geoGetArcEndAngle(a);
    point_t p;
    if (startAngle < endAngle)
    {
        step = +0.001;
        ++ endAngle;
    }
    else
    {
        step = -0.001;
        -- endAngle;
    }
    printf("Step: %.6f\n EndAngle: %.6f\n", step, endAngle);
    //return;
    glBegin(GL_LINE_STRIP);
    while (startAngle != endAngle)
    {
        cout << KYELLOW << startAngle << "," << KNORMAL;
        point_t p;
        p.x = a.center.x + cos(startAngle) * a.radius;
        p.y = a.center.y + sin(startAngle) * a.radius;
        //cout << KGREEN << ">> " << p.x << ", " << p.y << "\n" << KNORMAL;
        glVertex3f(p.x, p.y, 0);
        startAngle += step;
    }
    glEnd();
}
void cadRedraw()
{
  glutPostRedisplay();
}
