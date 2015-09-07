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
  cadEntityArray.push_back(cadEntity());
  cadEntityArray[cadEntityArrayIndex] = e;
  cadEntityArrayIndex++;
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
void cadRender()
{
  //sceneColor(WHITE);
  D printf("(cadRender)\n");
  glBegin(GL_LINE_LOOP);
  for (int i = 0; i < cadEntityArrayIndex; i++)
  {
      if (cadEntityArray[i].Type == CAD_LINE)
      {
        sceneColor(cadEntityArray[i].Color);
        glVertex3f((GLfloat) cadEntityArray[i].Line.start.x, cadEntityArray[i].Line.start.y, cadEntityArray[i].Line.start.z);
        glVertex3f((GLfloat) cadEntityArray[i].Line.end.x, cadEntityArray[i].Line.end.y, cadEntityArray[i].Line.end.z);
      }

  }
  glEnd();
}
void cadRedraw()
{
  glutPostRedisplay();
}
