#include <UI.h>

using namespace std;

vector<uiEntity> uiEntityArray;
int uiEntityArrayIndex;

void uiInit()
{
  uiEntityArrayIndex = 0;
  uiAppend(uiEntity{UI_TEXT, WHITE, ">", point_t{5, 10, 0}}); //CmdInput text - index 0
}
void uiAppend(uiEntity e)
{
  uiEntityArray.push_back(uiEntity());
  uiEntityArray[uiEntityArrayIndex] = e;
  uiEntityArrayIndex++;
  glutPostRedisplay();
}
void uiEdit(int x, uiEntity e)
{
  if (uiEntityArrayIndex > x) //Make sure entity exists
  {
    uiEntityArray[x] = e;
  }
  glutPostRedisplay();
}
void uiDrawText(void * font, char *s, float x, float y, float z)
{
  D printf("(windowDrawText) s = %s\n", s);
  unsigned int i;
  glRasterPos3f(x, y, z);
  for (i = 0; i < strlen (s); i++)
  {
    glutBitmapCharacter (font, s[i]);
  }
}
void uiRender()
{
  D printf("(uiRender)\n");
  for (int i = 0; i < uiEntityArrayIndex; i++)
  {
      if (uiEntityArray[i].Type == UI_TEXT)
      {
        //glScalef(5, 5, 0);
        sceneColor(uiEntityArray[i].Color);
        uiDrawText(GLUT_BITMAP_HELVETICA_12, (char *)uiEntityArray[i].Text.c_str(), uiEntityArray[i].Position.x * .001, uiEntityArray[i].Position.y * .001, uiEntityArray[i].Position.z * .001);
      }

  }
}
