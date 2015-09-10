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
        glLineWidth(1);
        glBegin(GL_LINE_LOOP);
        glVertex3f((GLfloat) cadEntityArray[i].Line.start.x, cadEntityArray[i].Line.start.y, cadEntityArray[i].Line.start.z);
        glVertex3f((GLfloat) cadEntityArray[i].Line.end.x, cadEntityArray[i].Line.end.y, cadEntityArray[i].Line.end.z);
        glEnd();
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
        glLineWidth(1);
        point_t center = point_t{0, 0, 0};
    		float Radius = cadEntityArray[i].Arc.radius;
    		float ypos, xpos;
    		float two_pi=6.283f;
    		float angle_inc=0.1f/Radius;
        //glBegin(GL_LINE_LOOP);
        //glVertex3f((GLfloat) cadEntityArray[i].Arc.start.x, (GLfloat) cadEntityArray[i].Arc.start.y, 0);
        float found_start = true;
        for(float angle=0.0f; angle<= two_pi;angle+=angle_inc)
    		{
            //D printf("arc> (%.6f, %.6f) \n", xpos, ypos);

            if (isSimilar(xpos, cadEntityArray[i].Arc.end.x) && isSimilar(ypos, cadEntityArray[i].Arc.end.y) && found_start)
            {
              //break;
            }
            if (isSimilar(xpos, cadEntityArray[i].Arc.start.x) && isSimilar(ypos, cadEntityArray[i].Arc.start.y))
            {
              found_start = true;
            }

            if (found_start)
            {
              glBegin(GL_POINT);
              xpos=center.x+Radius*cos(angle);
      		    ypos=center.y+Radius*sin(angle);
              glVertex3f((GLfloat) xpos, (GLfloat) ypos, 0);
              glEnd();
              //angle+=angle_inc;
              //xpos=center.x+Radius*cos(angle);
      		    //ypos=center.y+Radius*sin(angle);
              //glVertex3f((GLfloat) xpos, (GLfloat) ypos, 0);
              //glEnd();
            }
    		}
        //glEnd();
      }
  }
}
void cadRedraw()
{
  glutPostRedisplay();
}
