#include <Mouse.h>

using namespace std;

bool snapArcCenter = true;
bool snapArcEndpoints = true;
bool snapLineMidpoint = true;
bool snapLineEndpoints = true;
bool snapVector = true;
bool snapIntersection = true; //Not yet implemented

int mod;
int button;
point_t mouseCurrent;
mouse_t mouseLast;
point_t mouseLastSnapClick;
point_t mouseLastMouseOver;
point_t mouseCadGetCurrentPos()
{
  return mouseCurrent;
}
point_t mouseCadLastClickPos()
{
  return cadScreenCordToCadCord(mouseLast.x, mouseLast.y);
}
point_t mouseCadLastSnapClick()
{
  return mouseLastSnapClick;
}
void mouseInit()
{
  glutMouseFunc(mouseCallback);
  glutMotionFunc(mouseMotionCallback);
  glutPassiveMotionFunc(mousePassiveMotionCallback);
}
void mouseToggleLineEndpointSnap()
{
  snapLineEndpoints = !snapLineEndpoints;
}
void mouseToggleArcEndpointSnap()
{
  snapArcEndpoints = !snapArcEndpoints;
}
void mouseToggleCenterSnap()
{
  snapArcCenter = !snapArcCenter;
}
void mouseToggleMidpointSnap()
{
  snapLineMidpoint = !snapLineMidpoint;
}
void mouseToggleVectorSnap()
{
  snapVector = !snapVector;
}
void mouseCallback(int btn, int state, int x, int y)
{
    point_t pos = cadScreenCordToCadCord(x, y);
    mouseCurrent = pos;
    button = btn;
    mod = glutGetModifiers();
    if (state==GLUT_UP) // It's a wheel event
    {
        if ((btn == 3))
        {
          sceneZoomToMouse(+(0.05 * sceneGetScale()), cadScreenCordToCadCord(x, y));
        }
        if ((btn == 4))
        {
          sceneZoomToMouse(-(0.05 * sceneGetScale()), cadScreenCordToCadCord(x, y));
        }

    }
    //D printf("Scroll %s At %d %d\n", (btn == 3) ? "Up" : "Down", x, y);
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
          if (isSimilar(pos.x, e.Arc.start.x) && isSimilar(pos.y, e.Arc.start.y) && !e.Removed && e.Type == CAD_ARC && snapArcEndpoints)
          {
            if (mod == GLUT_ACTIVE_CTRL)
            {
              e.Selected = false;
            }
            else
            {
              e.Selected = true;
              e.SelectionIndex = cadCountSelection() + 1;
              //debugDumpArcStructure(e.Arc);
            }
            e.SelectedAt = e.Arc.start;
            mouseLastSnapClick = e.SelectedAt;
            cadEdit(a, e);
            return;
          }
          if (isSimilar(pos.x, e.Arc.end.x) && isSimilar(pos.y, e.Arc.end.y) && !e.Removed && e.Type == CAD_ARC && snapArcEndpoints)
          {
            if (mod == GLUT_ACTIVE_CTRL)
            {
              e.Selected = false;
            }
            else
            {
              e.Selected = true;
              e.SelectionIndex = cadCountSelection() + 1;
              //debugDumpArcStructure(e.Arc);
            }
            e.SelectedAt = e.Arc.end;
            mouseLastSnapClick = e.SelectedAt;
            cadEdit(a, e);
            return;
          }
          if (isSimilar(pos.x, e.Arc.center.x) && isSimilar(pos.y, e.Arc.center.y) && !e.Removed && e.Type == CAD_ARC && snapArcCenter)
          {
            if (mod == GLUT_ACTIVE_CTRL)
            {
              e.Selected = false;
            }
            else
            {
              e.Selected = true;
              e.SelectionIndex = cadCountSelection() + 1;
              //debugDumpArcStructure(e.Arc);
            }
            e.SelectedAt = e.Arc.center;
            mouseLastSnapClick = e.SelectedAt;
            cadEdit(a, e);
            return;
          }
          if (isSimilar(pos.x, e.Line.start.x) && isSimilar(pos.y, e.Line.start.y) && !e.Removed && e.Type == CAD_LINE && snapLineEndpoints)
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
            //D printf("\t%s Entity #%d Start point Clicked! X: %.6f, Y: %.6f%s\n", KGREEN, a, e.Line.start.x, e.Line.start.y, KNORMAL);
            mouseLastSnapClick = e.SelectedAt;
            cadEdit(a, e);
            return;
          }
          if (isSimilar(pos.x, e.Line.end.x) && isSimilar(pos.y, e.Line.end.y) && !e.Removed && e.Type == CAD_LINE && snapLineEndpoints)
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
            //D printf("\t%s Entity #%d End point Clicked! X: %.6f, Y: %.6f%s\n", KGREEN, a, e.Line.end.x, e.Line.end.y, KNORMAL);
            mouseLastSnapClick = e.SelectedAt;
            cadEdit(a, e);
            return;
          }
          if (isSimilar(pos.x, geoGetLineMidpoint(e.Line).x) && isSimilar(pos.y, geoGetLineMidpoint(e.Line).y) && !e.Removed && e.Type == CAD_LINE && snapLineMidpoint)
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
            mouseLastSnapClick = e.SelectedAt;
            cadEdit(a, e);
            return;
          }
          bool selected = false;
          if (snapVector)
          {
            for (x = 0; x < e.Vector.size(); x++)
            {
              if ( geoInTolerance(pos.x, e.Vector[x].x, 0.050) && geoInTolerance(pos.y, e.Vector[x].y, 0.050) )
              {
                //cout << KRED << "Vecter[" << x << "] = " << e.Vector[x].x << ", " << e.Vector[x].y << KNORMAL << endl;
                selected = true;
                //cadShowSelectionBox(e.Vector[x]);
                mouseLastMouseOver = e.Vector[x];
                e.MouseOver = true;
                cadEdit(a, e);
                cadRedraw();
                break;
              }
            }
          }
          if (selected)
          {
            e.MouseOver = false;
            if (mod == GLUT_ACTIVE_CTRL)
            {
              e.Selected = false;
            }
            else
            {
              e.Selected = true;
              e.SelectionIndex = cadCountSelection() + 1;
            }
            e.SelectedAt = mouseLastMouseOver;
            //D printf("\t%s Entity #%d Clicked!\n", KGREEN, e.Index ,KNORMAL);
            cadEdit(a, e);
            cadRedraw();
          }
        }
    }
    if(btn==GLUT_RIGHT_BUTTON && state==GLUT_DOWN)
    {
        //D printf("Right button at X: %d, Y: %d\n", x, y);
    }
}
void mouseMotionCallback(int x, int y)
{
    point_t pos = cadScreenCordToCadCord(x, y);
    mouseCurrent = pos;
    if (button == 1) //Middle mouse button
    {
      mouse_t inc{x - mouseLast.x , y - mouseLast.y};
      //D printf("\rPan %d, %d", inc.x, inc.y);
      //D fflush(stdout);
      if (abs(inc.x) < 30  && abs(inc.y) < 30)
      {
        //sceneIncPan(inc.x * 0.01 * sceneGetScale(), (-1 * inc.y) * 0.01 * sceneGetScale(), 0);
        sceneIncPan(inc.x * 0.01, (-1 * inc.y) * 0.01, 0);
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
        point_t  va = sceneGetViewAngle();
        uiEdit(2, uiEntity{UI_TEXT, RED, "Orbit > Xa: " + to_string(va.x) + " Ya: " + to_string(va.y), UI_MOUSE_POSITION});
      }
    }
    else
    {
      //D printf("\rMouse %d, %d", x, y);
      //D fflush(stdout);
    }
    mouseLast.x = x;
    mouseLast.y = y;
}
void mousePassiveMotionCallback(int x, int y)
{
  point_t pos = cadScreenCordToCadCord(x, y);
  mouseCurrent = pos;
  //D printf("%sX: %.6f, Y: %.6f, Z: %.6f%s\r", KGREEN, pos.x, pos.y, pos.z, KNORMAL);
  string m;
  if (pos.z > 0)
  {
      m = "X: " + to_string(pos.x) + " Y: " + to_string(pos.y) + " Z: " + to_string(pos.z);
  }
  else
  {
    m = "X: " + to_string(pos.x) + " Y: " + to_string(pos.y);
  }
  uiEdit(2, uiEntity{UI_TEXT, WHITE, m, UI_MOUSE_POSITION});
  cadEntity e;
  for (int a = 0; a < cadGetEntityArrayIndex(); a++)
  {
    e = cadGetEntityArray(a);
    if (isSimilar(pos.x, e.Arc.start.x) && isSimilar(pos.y, e.Arc.start.y) && !e.Removed && e.Type == CAD_ARC && snapArcEndpoints)
    {
      //uiEdit(0, uiEntity{UI_TEXT, RED, "Start point", UI_MENU_POSITION});
      cadShowSelectionBox(e.Arc.start);
      mouseLastMouseOver = e.Arc.start;
      e.MouseOver = true;
      cadEdit(a, e);
      return;
    }
    else if (isSimilar(pos.x, e.Arc.end.x) && isSimilar(pos.y, e.Arc.end.y) && !e.Removed && e.Type == CAD_ARC && snapArcEndpoints)
    {
      //uiEdit(0, uiEntity{UI_TEXT, RED, "End point", UI_MENU_POSITION});
      cadShowSelectionBox(e.Arc.end);
      mouseLastMouseOver = e.Arc.end;
      e.MouseOver = true;
      cadEdit(a, e);
      return;
    }
    else if (isSimilar(pos.x, e.Arc.center.x) && isSimilar(pos.y, e.Arc.center.y) && !e.Removed && e.Type == CAD_ARC && snapArcCenter)
    {
      //uiEdit(0, uiEntity{UI_TEXT, RED, "Center point", UI_MENU_POSITION});
      cadShowSelectionBox(e.Arc.center);
      mouseLastMouseOver = e.Arc.center;
      e.MouseOver = true;
      cadEdit(a, e);
      return;
    }
    else if (isSimilar(pos.x, e.Line.start.x) && isSimilar(pos.y, e.Line.start.y) && !e.Removed && e.Type == CAD_LINE && snapArcCenter)
    {
      cadShowSelectionBox(e.Line.start);
      mouseLastMouseOver = e.Line.start;
      e.MouseOver = true;
      cadEdit(a, e);
      return;
    }
    else if (isSimilar(pos.x, e.Line.end.x) && isSimilar(pos.y, e.Line.end.y) && !e.Removed && e.Type == CAD_LINE && snapLineEndpoints)
    {
      cadShowSelectionBox(e.Line.end);
      mouseLastMouseOver = e.Line.end;
      e.MouseOver = true;
      cadEdit(a, e);
      return;
    }
    else if (isSimilar(pos.x, geoGetLineMidpoint(e.Line).x) && isSimilar(pos.y, geoGetLineMidpoint(e.Line).y) && !e.Removed && e.Type == CAD_LINE && snapLineMidpoint)
    {
      cadShowSelectionBox(geoGetLineMidpoint(e.Line));
      mouseLastMouseOver = geoGetLineMidpoint(e.Line);
      e.MouseOver = true;
      cadEdit(a, e);
      return; //Only highlight one entity
    }
    else
    {
      bool selected = false;
      if (snapVector)
      {
        for (x = 0; x < e.Vector.size(); x++)
        {
          if ( geoInTolerance(pos.x, e.Vector[x].x, 0.050) && geoInTolerance(pos.y, e.Vector[x].y, 0.050) )
          {
            //cout << KRED << "Vecter[" << x << "] = " << e.Vector[x].x << ", " << e.Vector[x].y << KNORMAL << endl;
            selected = true;
            //cadShowSelectionBox(e.Vector[x]);
            mouseLastMouseOver = e.Vector[x];
            e.MouseOver = true;
            cadEdit(a, e);
            cadRedraw();
            break;
          }
        }
      }
      if (!selected)
      {
        e.MouseOver = false;
        cadEdit(a, e);
        cadHideSelectionBox();
        cadRedraw();
      }
    }
  }
  fflush(stdout);
}
