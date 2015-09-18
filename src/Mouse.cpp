#include <Mouse.h>

using namespace std;

int mod;
int button;
mouse_t mouseLast;
point_t mouseCadLastClickPos()
{
  return cadScreenCordToCadCord(mouseLast.x, mouseLast.y);
}
void mouseInit()
{
  glutMouseFunc(mouseCallback);
  glutMotionFunc(mouseMotionCallback);
  glutPassiveMotionFunc(mousePassiveMotionCallback);
}

void mouseCallback(int btn, int state, int x, int y)
{
    point_t pos = cadScreenCordToCadCord(x, y);
    button = btn;
    mod = glutGetModifiers();
    if ((btn == 3) || (btn == 4)) // It's a wheel event
    {
      if (btn == 3) //Zoom in
      {
        sceneIncZoom(+0.1);
      }
      else //Zoom out
      {
        sceneIncZoom(-0.1);
      }
      //D printf("Scroll %s At %d %d\n", (btn == 3) ? "Up" : "Down", x, y);
    }
    if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN && mod == GLUT_ACTIVE_CTRL)
    {
        //D printf("Left + Ctrl button at X: %d, Y: %d\n", x, y);
    }
    if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
    {
        //D printf("Left button at X: %d, Y: %d\n", x, y);
        mouseLast.x = x;
        mouseLast.y = y;
        cadEntity e;
        for (int a = 0; a < cadGetEntityArrayIndex(); a++)
        {
          e = cadGetEntityArray(a);
          if (isSimilar(pos.x, e.Arc.start.x) && isSimilar(pos.y, e.Arc.start.y) && !e.Removed && e.Type == CAD_ARC)
          {
            if (mod == GLUT_ACTIVE_CTRL)
            {
              e.Selected = false;
            }
            else
            {
              e.Selected = true;
              e.SelectionIndex = cadCountSelection() + 1;
              debugDumpArcStructure(e.Arc);
            }
            e.SelectedAt = e.Arc.start;
            cadEdit(a, e);
            return;
          }
          if (isSimilar(pos.x, e.Arc.end.x) && isSimilar(pos.y, e.Arc.end.y) && !e.Removed && e.Type == CAD_ARC)
          {
            if (mod == GLUT_ACTIVE_CTRL)
            {
              e.Selected = false;
            }
            else
            {
              e.Selected = true;
              e.SelectionIndex = cadCountSelection() + 1;
              debugDumpArcStructure(e.Arc);
            }
            e.SelectedAt = e.Arc.end;
            cadEdit(a, e);
            return;
          }
          if (isSimilar(pos.x, e.Arc.center.x) && isSimilar(pos.y, e.Arc.center.y) && !e.Removed && e.Type == CAD_ARC)
          {
            if (mod == GLUT_ACTIVE_CTRL)
            {
              e.Selected = false;
            }
            else
            {
              e.Selected = true;
              e.SelectionIndex = cadCountSelection() + 1;
              debugDumpArcStructure(e.Arc);
            }
            e.SelectedAt = e.Arc.center;
            cadEdit(a, e);
            return;
          }
          if (isSimilar(pos.x, e.Line.start.x) && isSimilar(pos.y, e.Line.start.y) && !e.Removed && e.Type == CAD_LINE)
          {
            //D printf("\t%s Entity #%d Start point Clicked!%s\n", KGREEN, a, KNORMAL);
            if (mod == GLUT_ACTIVE_CTRL)
            {
              e.Selected = false;
            }
            else
            {
              e.Selected = true;
              e.SelectionIndex = cadCountSelection() + 1;
            }
            e.SelectedAt = e.Line.start;
            cadEdit(a, e);
            return;
          }
          if (isSimilar(pos.x, e.Line.end.x) && isSimilar(pos.y, e.Line.end.y) && !e.Removed && e.Type == CAD_LINE)
          {
            //D printf("\t%s Entity #%d End point Clicked!%s\n", KGREEN, a, KNORMAL);
            if (mod == GLUT_ACTIVE_CTRL)
            {
              e.Selected = false;
            }
            else
            {
              e.Selected = true;
              e.SelectionIndex = cadCountSelection() + 1;
            }
            e.SelectedAt = e.Line.end;
            cadEdit(a, e);
            return;
          }
          if (isSimilar(pos.x, geoGetLineMidpoint(e.Line).x) && isSimilar(pos.y, geoGetLineMidpoint(e.Line).y) && !e.Removed && e.Type == CAD_LINE)
          {
            if (mod == GLUT_ACTIVE_CTRL)
            {
              e.Selected = false;
            }
            else
            {
              e.Selected = true;
              e.SelectionIndex = cadCountSelection() + 1;
            }
            e.SelectedAt = geoGetLineMidpoint(e.Line);
            cadEdit(a, e);
            return;
          }
        }
    }
    if(btn==GLUT_RIGHT_BUTTON && state==GLUT_DOWN)
    {
        D printf("Right button at X: %d, Y: %d\n", x, y);
    }
}
void mouseMotionCallback(int x, int y)
{
    if (button == 1) //Middle mouse button
    {
      mouse_t inc{x - mouseLast.x , y - mouseLast.y};
      //D printf("\rPan %d, %d", inc.x, inc.y);
      //D fflush(stdout);
      if (abs(inc.x) < 10  && abs(inc.y) < 10)
      {
        sceneIncPan(inc.x * 0.01, (-1 * inc.y) * 0.01 , 0);
      }
    }
    else if (mod == GLUT_ACTIVE_CTRL)
    {
      mouse_t inc{x - mouseLast.x , y - mouseLast.y};
      //D printf("\rOrbit %d, %d", inc.x, inc.y);
      //D fflush(stdout);
      if (abs(inc.x) < 10  && abs(inc.y) < 10)
      {
        sceneIncViewAngle(inc.y, -1 * inc.x, 0);
      }
    }
    else
    {
      D printf("\rMouse %d, %d", x, y);
      D fflush(stdout);
    }
    mouseLast.x = x;
    mouseLast.y = y;
}
void mousePassiveMotionCallback(int x, int y)
{
  point_t pos = cadScreenCordToCadCord(x, y);
  //D printf("%sX: %.6f, Y: %.6f, Z: %.6f%s\r", KGREEN, pos.x, pos.y, pos.z, KNORMAL);
  cadEntity e;
  for (int a = 0; a < cadGetEntityArrayIndex(); a++)
  {
    e = cadGetEntityArray(a);
    if (isSimilar(pos.x, e.Arc.start.x) && isSimilar(pos.y, e.Arc.start.y) && !e.Removed && e.Type == CAD_ARC)
    {
      uiEdit(0, uiEntity{UI_TEXT, RED, "Start point", UI_MENU_POSITION});
      cadShowSelectionBox(e.Arc.start);
      e.MouseOver = true;
      cadEdit(a, e);
      return;
    }
    else if (isSimilar(pos.x, e.Arc.end.x) && isSimilar(pos.y, e.Arc.end.y) && !e.Removed && e.Type == CAD_ARC)
    {
      uiEdit(0, uiEntity{UI_TEXT, RED, "End point", UI_MENU_POSITION});
      cadShowSelectionBox(e.Arc.end);
      e.MouseOver = true;
      cadEdit(a, e);
      return;
    }
    else if (isSimilar(pos.x, e.Arc.center.x) && isSimilar(pos.y, e.Arc.center.y) && !e.Removed && e.Type == CAD_ARC)
    {
      uiEdit(0, uiEntity{UI_TEXT, RED, "Center point", UI_MENU_POSITION});
      cadShowSelectionBox(e.Arc.center);
      e.MouseOver = true;
      cadEdit(a, e);
      return;
    }
    else if (isSimilar(pos.x, e.Line.start.x) && isSimilar(pos.y, e.Line.start.y) && !e.Removed && e.Type == CAD_LINE)
    {
      cadShowSelectionBox(e.Line.start);
      e.MouseOver = true;
      cadEdit(a, e);
      return;
    }
    else if (isSimilar(pos.x, e.Line.end.x) && isSimilar(pos.y, e.Line.end.y) && !e.Removed && e.Type == CAD_LINE)
    {
      cadShowSelectionBox(e.Line.end);
      e.MouseOver = true;
      cadEdit(a, e);
      return;
    }
    else if (isSimilar(pos.x, geoGetLineMidpoint(e.Line).x) && isSimilar(pos.y, geoGetLineMidpoint(e.Line).y) && !e.Removed && e.Type == CAD_LINE)
    {
      cadShowSelectionBox(geoGetLineMidpoint(e.Line));
      e.MouseOver = true;
      cadEdit(a, e);
      return; //Only highlight one entity
    }
    else
    {
      e.MouseOver = false;
      cadEdit(a, e);
      cadHideSelectionBox();
      cadRedraw();
    }
  }
  fflush(stdout);
}
