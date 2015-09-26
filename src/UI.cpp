#include <UI.h>

using namespace std;

vector<uiEntity> uiEntityArray;
int uiEntityArrayIndex;

void uiInit()
{
  uiEntityArrayIndex = 0;
  uiAppend(uiEntity{UI_TEXT, WHITE, "", UI_MENU_POSITION}); //Msg text - index 0
  uiAppend(uiEntity{UI_TEXT, WHITE, "", UI_INPUT_POSITION}); //Msg text - index 1
  uiAppend(uiEntity{UI_TEXT, WHITE, "", UI_HINT_POSITION}); //Mouse Position - index 2
  uiAppend(uiEntity{UI_TEXT, WHITE, "", UI_MOUSE_POSITION}); //Mouse Position - index 3
  uiAppend(uiEntity{UI_TEXT, WHITE, "", UI_SNAP_POSITION}); //Snap Position - index 4
  uiSnapIndicator();
  cliMenu();
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
  //D printf("(uiDrawText) s = %s\n", s);
  unsigned int i;
  glRasterPos3f(x, y, z);
  for (i = 0; i < strlen (s); i++)
  {
    glutBitmapCharacter (font, s[i]);
  }
}
void uiSnapIndicator()
{
  std::vector<std::string> snap = mouseUISnapIndicator();
  std::string s = "";
  if (snap.size() > 0)
  {
    s = snap[0];
    for (int x = 1; x < snap.size(); x++)
    {
      s.append(" | ");
      s.append(snap[x]);
      //cout << snap[x] << endl;
    }
  }
  V cout << KRED << "(uiSnapIndicator) => " << KGREEN << s << KNORMAL << endl;
  uiEdit(4, uiEntity{UI_TEXT, RED, s , UI_SNAP_POSITION});
}
void uiRender()
{
  //D printf("(uiRender)\n");
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
