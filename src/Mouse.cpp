#include <Mouse.h>

using namespace std;

float mouseClose = 0.010;
float mouseTolerance()
{
  return mouseClose / sceneGetScale();
}
point_t mouseVectorIntersection;
vector<cadEntity> mouseLive;

bool mouseVector(cadEntity e, point_t p)
{
  if (e.Type == CAD_LINE)
  {
    line_t floater = geoGetParallelLine(e.Line, p, geoGetPerpendicularDistance(e.Line , p));
    float distance_from_midpoint = geoGetLineLength(line_t{geoGetLineMidpoint(floater), p});
    if (distance_from_midpoint > (geoGetLineLength(floater) / 2))
    {
      return false; //Mouse is outside of segment box
    }
    float floater_distance = geoGetLineLength(line_t{geoGetLineMidpoint(floater), geoGetLineMidpoint(e.Line)});
    if (floater_distance > mouseTolerance() + mouseClose)
    {
      return false; //Floater is two far away from Line
    }
    if ( floater_distance < mouseTolerance())
    {
      line_t intersection_line = line_t{p, p};
      intersection_line = geoExtendLineAngle(p, geoGetLineAngle(floater) + geoDegreesToRadians(90), 1);
      mouseVectorIntersection = geoGetIntersection(intersection_line, e.Line);
      return true; //Mouse is within Selection tolerance
    }
  }
  if (e.Type == CAD_ARC)
  {
    arc_t floater = e.Arc;
    floater.radius = geoGetLineLength(line_t{floater.center, p});

    //floater.direction = !floater.direction; //This is the vodo arc bug i still havnt traced
    floater.start = geoGetArcPoint(floater, geoRadiansToDegrees(geoGetLineAngle(line_t{floater.center, floater.start})) + 180);
    floater.end = geoGetArcPoint(floater, geoRadiansToDegrees(geoGetLineAngle(line_t{floater.center, floater.end})) + 180);

    //float start_angle = geoRadiansToDegrees(geoGetLineAngle(line_t{floater.center, floater.start})) - 90;
    //float end_angle = geoRadiansToDegrees(geoGetLineAngle(line_t{floater.center, floater.end})) - 90;
    float start_angle = geoRadiansToDegrees(geoGetArcStartAngle(floater));
    float end_angle = geoRadiansToDegrees(geoGetArcEndAngle(floater));
    float current_angle = 360 - fabs(geoRadiansToDegrees(geoGetLineAngle(line_t{floater.center, p})) -180);


    /*
    mouseLive.clear();
    e.Arc = floater;
    mouseLive.push_back(e);
    e.Type = CAD_LINE;
    e.Line = line_t{floater.center, p};
    mouseLive.push_back(e);
    e.Line = line_t{floater.center, floater.start};
    mouseLive.push_back(e);

    e.Line = line_t{floater.center, floater.end};
    mouseLive.push_back(e);

    e.Type = CAD_NOTE;
    e.Note.size = 12;
    e.Note.text = to_string(current_angle);
    e.Note.pos = geoGetLineMidpoint(line_t{floater.center, p});
    e.Note.pos.x += 0.010;
    e.Note.pos.y += 0.010;
    mouseLive.push_back(e);

    line_t midpoint_intersector = line_t{geoGetLineMidpoint(line_t{floater.center, floater.start}),  geoGetLineMidpoint({floater.center, floater.end})};

    e.Type = CAD_LINE;
    e.Line = midpoint_intersector;
    mouseLive.push_back(e);

    if (geoDoLinesIntersect(midpoint_intersector, line_t{floater.center, p}))
    {
      cout << "+";
      fflush(stdout);
    }
    cadShowLiveEntity(mouseLive);
    */

    //if (current_angle >= start_angle && current_angle <= end_angle || floater.start == floater.end )
    line_t midpoint_intersector = line_t{geoGetLineMidpoint(line_t{floater.center, floater.start}),  geoGetLineMidpoint({floater.center, floater.end})};
    if (geoDoLinesIntersect(midpoint_intersector, line_t{floater.center, p}) || current_angle <= end_angle || floater.start == floater.end)
    {
      if (floater.radius < e.Arc.radius + mouseTolerance() && floater.radius > e.Arc.radius - mouseTolerance() )
      {
        //cout << "+";
        //fflush(stdout);
        //e.Arc = floater;
        //cadShowLiveEntity(e);
        return true;
      }
      else
      {
        return false;
      }
    }
    //return true;

  }
  return false;
}
bool snapArcCenter = true;
bool snapArcEndpoints = true;
bool snapLineMidpoint = true;
bool snapLineEndpoints = true;
bool snapVector = true;
bool snapIntersection = true;

int mod;
int button;
point_t mouseCurrent;
mouse_t mouseLast;
point_t mouseLastSnapClick;
point_t mouseLastMouseOverPoint;
cadEntity mouseLastMouseOverEntity;

std::vector<string> mouseUISnapIndicator()
{
  std::vector<string> r;
  if (snapArcCenter)
  {
    r.push_back(string("Arc Center - T"));
  }
  else
  {
    r.push_back(string("Arc Center - F"));
  }
  if (snapArcEndpoints)
  {
    r.push_back(string("Arc Endpoints - T"));
  }
  else
  {
    r.push_back(string("Arc Endpoints - F"));
  }
  if (snapLineMidpoint)
  {
    r.push_back(string("Line Midpoint - T"));
  }
  else
  {
    r.push_back(string("Line Midpoint - F"));
  }
  if (snapLineEndpoints)
  {
    r.push_back(string("Line Endpoints - T"));
  }
  else
  {
    r.push_back(string("Line Endpoints - F"));
  }
  if (snapVector)
  {
    r.push_back(string("Snap Vector - T"));
  }
  else
  {
    r.push_back(string("Snap Vector - F"));
  }
  if (snapIntersection)
  {
    r.push_back(string("Intersection - T"));
  }
  else
  {
    r.push_back(string("Intersection - F"));
  }
  r.push_back(string("Entitys - " + to_string(cadCountEntitys())));
  r.push_back(string("Selected - " + to_string(cadGetSelected().size())));
  return r;
}
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
void mouseToggleIntersectionSnap()
{
  snapIntersection = !snapIntersection;
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
    //V printf("Scroll %s At %d %d\n", (btn == 3) ? "Up" : "Down", x, y);
    if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN && mod == GLUT_ACTIVE_CTRL)
    {
        //V printf("Left + Ctrl button at X: %d, Y: %d\n", x, y);
    }
    if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
    {
      if (cadGetEntityArrayIndex() > 0 && cadGetEntityArray(mouseLastMouseOverEntity.Index).MouseOver == true)
      {
        if (mod == GLUT_ACTIVE_CTRL)
        {
          mouseLastMouseOverEntity.Selected = false;
        }
        else
        {
          mouseLastMouseOverEntity.Selected = true;
          mouseLastMouseOverEntity.SelectionIndex = cadCountSelection() + 1;
          //debugDumpArcStructure(e.Arc);
        }
        cadEdit(mouseLastMouseOverEntity.Index, mouseLastMouseOverEntity);
      }
      if (cadGetSelectionBox().visable == true)
      {
        mouseLastSnapClick = cadGetSelectionBox().a.center;
        mouseLastMouseOverEntity.SelectedAt = mouseLastSnapClick;
        cadEdit(mouseLastMouseOverEntity.Index, mouseLastMouseOverEntity);
      }
    }
    if(btn==GLUT_RIGHT_BUTTON && state==GLUT_DOWN)
    {
        //V printf("Right button at X: %d, Y: %d\n", x, y);
    }
}
void mouseMotionCallback(int x, int y)
{
    point_t pos = cadScreenCordToCadCord(x, y);
    mouseCurrent = pos;
    if (button == 1) //Middle mouse button
    {
      mouse_t inc{x - mouseLast.x , y - mouseLast.y};
      //V printf("\rPan %d, %d", inc.x, inc.y);
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
      //V printf("\rOrbit %d, %d", inc.x, inc.y);
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
      //V printf("\rMouse %d, %d", x, y);
      //D fflush(stdout);
    }
    mouseLast.x = x;
    mouseLast.y = y;
}
void mousePassiveMotionCallback(int x, int y)
{
  point_t pos = cadScreenCordToCadCord(x, y);
  mouseCurrent = pos;
  //V printf("%sX: %.6f, Y: %.6f, Z: %.6f%s\r", KGREEN, pos.x, pos.y, pos.z, KNORMAL);
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

  std::vector<cadEntity> MouseOver = cadGetMouseOver();
  //V cout << KRED << "(mousePassiveMotionCallback) MouseOver Size => " << KGREEN << MouseOver.size() << KNORMAL << endl;
  if (MouseOver.size() == 2 && snapIntersection)
  {
    //V cout << KRED << "(mousePassiveMotionCallback) MouseOver => " << KGREEN << "Calculating intersection point!" << KNORMAL << endl;
    point_t intersection = geoGetIntersection(MouseOver[0].Line, MouseOver[1].Line);
    if (geoInTolerance(pos.x, intersection.x, mouseTolerance()) && geoInTolerance(pos.y, intersection.y, mouseTolerance()))
    {
      cadShowSelectionBox(intersection);
      mouseLastMouseOverPoint = intersection;
      return;
    }
    else
    {
      cadHideSelectionBox();
    }
  }

  cadEntity e;
  for (int a = 0; a < cadGetEntityArrayIndex(); a++)
  {
    e = cadGetEntityArray(a);
    if (geoInTolerance(pos.x, e.Arc.start.x, mouseTolerance()) && geoInTolerance(pos.y, e.Arc.start.y, mouseTolerance()) && !e.Removed && e.Type == CAD_ARC && snapArcEndpoints)
    {
      //uiEdit(0, uiEntity{UI_TEXT, RED, "Start point", UI_MENU_POSITION});
      cadShowSelectionBox(e.Arc.start);
      mouseLastMouseOverPoint = e.Arc.start;
      mouseLastMouseOverEntity = e;
      e.MouseOver = true;
      cadEdit(a, e);
      return;
    }
    else if (geoInTolerance(pos.x, e.Arc.end.x, mouseTolerance()) && geoInTolerance(pos.y, e.Arc.end.y, mouseTolerance()) && !e.Removed && e.Type == CAD_ARC && snapArcEndpoints)
    {
      //uiEdit(0, uiEntity{UI_TEXT, RED, "End point", UI_MENU_POSITION});
      cadShowSelectionBox(e.Arc.end);
      mouseLastMouseOverPoint = e.Arc.end;
      mouseLastMouseOverEntity = e;
      e.MouseOver = true;
      cadEdit(a, e);
      return;
    }
    else if (geoInTolerance(pos.x, e.Arc.center.x, mouseTolerance()) && geoInTolerance(pos.y, e.Arc.center.y, mouseTolerance()) && !e.Removed && e.Type == CAD_ARC && snapArcCenter)
    {
      //uiEdit(0, uiEntity{UI_TEXT, RED, "Center point", UI_MENU_POSITION});
      cadShowSelectionBox(e.Arc.center);
      mouseLastMouseOverPoint = e.Arc.center;
      mouseLastMouseOverEntity = e;
      e.MouseOver = true;
      cadEdit(a, e);
      return;
    }
    else if (geoInTolerance(pos.x, e.Line.start.x, mouseTolerance()) && geoInTolerance(pos.y, e.Line.start.y, mouseTolerance()) && !e.Removed && e.Type == CAD_LINE && snapArcCenter)
    {
      cadShowSelectionBox(e.Line.start);
      mouseLastMouseOverPoint = e.Line.start;
      mouseLastMouseOverEntity = e;
      e.MouseOver = true;
      cadEdit(a, e);
      return;
    }
    else if (geoInTolerance(pos.x, e.Line.end.x, mouseTolerance()) && geoInTolerance(pos.y, e.Line.end.y, mouseTolerance()) && !e.Removed && e.Type == CAD_LINE && snapLineEndpoints)
    {
      cadShowSelectionBox(e.Line.end);
      mouseLastMouseOverPoint = e.Line.end;
      mouseLastMouseOverEntity = e;
      e.MouseOver = true;
      cadEdit(a, e);
      return;
    }
    else if (geoInTolerance(pos.x, geoGetLineMidpoint(e.Line).x, mouseTolerance()) && geoInTolerance(pos.y, geoGetLineMidpoint(e.Line).y, mouseTolerance()) && !e.Removed && e.Type == CAD_LINE && snapLineMidpoint)
    {
      cadShowSelectionBox(geoGetLineMidpoint(e.Line));
      mouseLastMouseOverPoint = geoGetLineMidpoint(e.Line);
      mouseLastMouseOverEntity = e;
      e.MouseOver = true;
      cadEdit(a, e);
      return; //Only highlight one entity
    }
    else if (mouseVector(e, pos) && snapVector)
    {
      mouseLastMouseOverPoint = mouseVectorIntersection;
      mouseLastMouseOverEntity = e;
      e.MouseOver = true;
      cadEdit(a, e);

      //line_t floater = geoGetParallelLine(e.Line, pos, geoGetPerpendicularDistance(e.Line , pos));
      //line_t floater = line_t{pos, mouseVectorIntersection};
      //e.Line = floater;
      //cadShowLiveEntity(e);
      cadRedraw();
      break;
    }
    else
    {
      e.MouseOver = false;
      cadEdit(a, e);
      cadHideSelectionBox();
      cadHideLiveEntity();
      cadRedraw();
    }
  }
  //V fflush(stdout);
}
