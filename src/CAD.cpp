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
void cadRenderArcDecip1(arc_t a)
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
  start_angle = geoGetArcStartAngle(a);
  D cout << KGREEN << "Start angle: " << start_angle << "\n" << KNORMAL;
  //D cout << KGREEN << "End angle: " << end_angle << "\n" << KNORMAL;
  length = geoGetArchLength(a);
  D cout << KGREEN << "Length: " << length << "\n" << KNORMAL;
  if (a.direction == ARC_CW)
  {
    D cout << KGREEN << "\tArc CW\n" << KNORMAL;
  }
  else
  {
    D cout << KGREEN << "\tArc CCW\n" << KNORMAL;
  }
  for(float i = start_angle; i > length; i=i - step) arc_loop(a, i, r);
	glEnd();
  fflush(stdout);
}
//void drawArc(int x, int y, float startAngle, float endAngle, uint32_t radiusX, uint32_t radiusY,)
void cadRenderArcDecip(arc_t a) //Fuck me!!!!
{
    if (a.radius <= 0) return;
    float step;
    float startAngle = geoGetArcStartAngle(a);
    float endAngle = geoGetArcEndAngle(a);
    float arcLength = geoGetArchLength(a);
    float stepAngle;
    float includedAngle = geoRadiansToDegrees(geoGetIncludedAngle(a));
    arc_t gen;
    //if (startAngle < endAngle)
    gen.radius = a.radius;
    gen.center = a.center;
    gen.direction = a.direction;
    step = arcLength / includedAngle;
    gen.start = a.start;
    gen.end = a.start;
    printf("Include arc angle: %.6f\n", geoRadiansToDegrees(geoGetIncludedAngle(a)));
    //return;
    printf("Step: %.6f\nstartAngle: %.6f\nendAngle: %.6f\narcLength: %.6f\n", step, startAngle, endAngle, arcLength);
    //printf("Test: %.6f\n", geoRadiansToDegrees(atan2(-5, 0)));
    //return;
    glBegin(GL_LINE_STRIP);
    int over_run = abs(7 / step);
    float x = 0;
    float y = 0;
    stepAngle = startAngle;
    while(includedAngle > x)
    {
      gen.end.x = a.center.x + cos(stepAngle) * gen.radius;
      gen.end.y = a.center.y + sin(stepAngle) * gen.radius;
      glVertex3f(gen.end.x, gen.end.y, 0);
      cout << KYELLOW << "Step " << x << " / " << includedAngle << "\n" << KNORMAL;
      cout << KYELLOW << "\t Current radian: " << stepAngle << "\n" << KNORMAL;
      if (gen.direction == ARC_CCW)
      {
          stepAngle += geoDegreesToRadians(step);
      }
      else
      {
        stepAngle -= geoDegreesToRadians(step);
      }
      x += step;
    }
    glEnd();
}
void cadRenderArc(arc_t a)
{
  float includedAngle = geoRadiansToDegrees(geoGetIncludedAngle(a));
  line_t l = line_t{ point_t{a.start.x, a.start.y} , point_t{a.end.x, a.end.y} };
  glBegin(GL_LINE_STRIP);
  //Draw start line
  glVertex3f(a.center.x, a.start.y, 0);
  glVertex3f(a.start.x, a.start.y, 0);

  l = geoRotateLine(l, a.center, 5);
  glVertex3f(l.start.x, l.start.y, 0);
  glVertex3f(l.end.x, l.end.y, 0);

  //Draw end line
  glVertex3f(a.center.x, a.start.y, 0);
  glVertex3f(a.end.x, a.end.y, 0);
  glEnd();
}
void cadRedraw()
{
  glutPostRedisplay();
}
