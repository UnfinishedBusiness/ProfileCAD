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
  cadAppend(cadEntity{CAD_LINE, cadColorAttribute, cadEntityLine{ start, end }, cadEntityArc{ start, end } });
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
        glLineWidth(1);
        glBegin(GL_LINE_LOOP);
        glVertex3f((GLfloat) cadEntityArray[i].Line.start.x, cadEntityArray[i].Line.start.y, cadEntityArray[i].Line.start.z);
        glVertex3f((GLfloat) cadEntityArray[i].Line.end.x, cadEntityArray[i].Line.end.y, cadEntityArray[i].Line.end.z);
        glEnd();
      }
  }
}
void cadRedraw()
{
  glutPostRedisplay();
}
