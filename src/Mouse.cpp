#include <Mouse.h>

using namespace std;

bool passivePrime = false;

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
point_t mouseLastClick;
point_t mouseCurrent;
mouse_t mouseLast;
point_t mouseLastSnapClick;
point_t mouseLastMouseOverPoint;
cadEntity mouseLastMouseOverEntity;


string mouseLiveShowInstruction;
void mouseLiveShow(string s)
{
  mouseLiveShowInstruction = s;
  passivePrime = true;
  mousePassiveMotionCallback(0, 0); //Prime show
}
void mouseLiveClear()
{
  mouseLiveShowInstruction = "";
  mouseLive.clear();
  passivePrime = true;
  mousePassiveMotionCallback(0, 0); //Prime show
}

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
  //return cadScreenCordToCadCord(mouseLast.x, mouseLast.y);
  return mouseLastClick;
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
    if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
    {
      mouseLastClick = pos;
      mouseLastSnapClick = mouseLastMouseOverPoint;
      //V debugDumpPointStructure(mouseLastMouseOverPoint);
      if (mouseLiveShowInstruction == "LineVerticalOrigin")
      {
        cliPush(to_string(mouseLive[0].Line.end.y));
        cliReturn();
        mouseLiveClear();
      }
      if (mouseLiveShowInstruction == "LineHorizontalOrigin")
      {
        cliPush(to_string(mouseLive[0].Line.end.x));
        cliReturn();
        mouseLiveClear();
      }
      if (mouseLiveShowInstruction == "DimensionPoint")
      {
        cliPush(to_string(pos.x) + ", " + to_string(pos.y));
        cliReturn();
        mouseLiveClear();
      }
      if (mouseLiveShowInstruction == "LineParallel")
      {
        if (cadCountSelection() > 0 && cadGetSelected()[0].Type == CAD_LINE)
        {
          if (cadGetSelectionBox().visable == true)
          {
            cliPush(to_string(geoGetPerpendicularDistance(cadGetSelected()[0].Line, mouseLastMouseOverPoint)));
          }
          else
          {
            cliPush(to_string(geoGetPerpendicularDistance(cadGetSelected()[0].Line, pos)));
          }
          cliReturn();
          mouseLiveClear();
          cliScreenUnSelectAll();
        }
      }
      if (cadGetEntityArrayIndex() > 0)
      {
        if (cadGetEntityArray(mouseLastMouseOverEntity.Index).MouseOver == true)
        {
          if (mouseLastMouseOverEntity.Selected == true)
          {
            mouseLastMouseOverEntity.Selected = false;
          }
          else
          {
            mouseLastMouseOverEntity.Selected = true;
            mouseLastMouseOverEntity.SelectionIndex = cadCountSelection() + 1;
            mouseLastMouseOverEntity.SelectedAt = mouseLastSnapClick;
          }
          cadEdit(mouseLastMouseOverEntity.Index, mouseLastMouseOverEntity, false);
        }
        else if (cadGetSelectionBox().visable == true)
        {
          mouseLastSnapClick = cadGetSelectionBox().a.center;
          mouseLastMouseOverEntity.SelectedAt = mouseLastSnapClick;
          mouseLastMouseOverEntity.SelectionIndex = cadCountSelection() + 1;
          cadEdit(mouseLastMouseOverEntity.Index, mouseLastMouseOverEntity, false);
        }
      }
    }
    if(btn==GLUT_RIGHT_BUTTON && state==GLUT_DOWN)
    {
        //V printf("Right button at X: %d, Y: %d\n", x, y);
    }
    if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN && mod == GLUT_ACTIVE_CTRL)
    {
        //V printf("Left + Ctrl button at X: %d, Y: %d\n", x, y);
        cadSelectChain();
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

  point_t note_pos;
  cadEntity l;
  l.LineWidth = 1;
  l.Color = DARKGREY;
  if (mouseLiveShowInstruction == "CurrentContour")
  {
    contour_t CurrentContour = cadGetCurrentContour();
    for (int x = 0; x < CurrentContour.Entitys.size(); x++)
    {
      CurrentContour.Entitys[x].Color = PURPLE;
      CurrentContour.Entitys[x].LineWidth = 3;
      mouseLive.push_back(CurrentContour.Entitys[x]);
      if (CurrentContour.Entitys[x].Type == CAD_LINE)
      {
        l.Type = CAD_LINE;
        //e.Line = geoExtendLineStartpoint(geoRotateLine(CurrentContour.Entitys[x].Line, geoGetLineMidpoint(CurrentContour.Entitys[x].Line), 135), 0.050);
        l.Line = geoExtendLineAngle(geoGetLineMidpoint(CurrentContour.Entitys[x].Line), geoGetLineAngle(CurrentContour.Entitys[x].Line) + geoDegreesToRadians(45), 0.1);
        l.Color = PURPLE;
        l.LineWidth = 3;
        mouseLive.push_back(l);
        l.Line = geoExtendLineAngle(geoGetLineMidpoint(CurrentContour.Entitys[x].Line), geoGetLineAngle(CurrentContour.Entitys[x].Line) + geoDegreesToRadians(-45), 0.1);
        mouseLive.push_back(l);
        note_pos = l.Line.start;
        note_pos.x += 0.2;
        note_pos.y += 0.2;
      }
      if (CurrentContour.Entitys[x].Type == CAD_ARC)
      {
        /*l = CurrentContour.Entitys[x];
        point_t midpoint = geoGetArcMidpoint(l.Arc);
        point_t center = l.Arc.center;
        l.Type = CAD_LINE;
        line_t arrow_body;
        if (l.Arc.direction == ARC_CCW)
        {
            arrow_body = geoRotateLine(line_t{midpoint, center}, midpoint, -90);
        }
        else
        {
            arrow_body = geoRotateLine(line_t{midpoint, center}, midpoint, 90);
        }
        l.Line = geoExtendLineAngle(midpoint, geoGetLineAngle(arrow_body) + geoDegreesToRadians(45), 0.1);
        l.Color = PURPLE;
        l.LineWidth = 3;
        mouseLive.push_back(l);
        l.Line = geoExtendLineAngle(midpoint, geoGetLineAngle(arrow_body) + geoDegreesToRadians(-45), 0.1);
        mouseLive.push_back(l);
        note_pos = l.Arc.start;
        note_pos.x += 0.2;
        note_pos.y += 0.2;*/

        line_t line = line_t{CurrentContour.Entitys[x].Arc.start, CurrentContour.Entitys[x].Arc.end};
        l.Type = CAD_LINE;
        l.Line = geoExtendLineAngle(geoGetLineMidpoint(line), geoGetLineAngle(line) + geoDegreesToRadians(45), 0.1);
        mouseLive.push_back(l);
        l.Line = geoExtendLineAngle(geoGetLineMidpoint(line), geoGetLineAngle(line) + geoDegreesToRadians(-45), 0.1);
        mouseLive.push_back(l);

      }
      l.Type = CAD_NOTE;
      l.Color = PURPLE;
      l.Note.size = 12;
      sprintf(l.Note.text, "Number: %d, ID: %d", x, CurrentContour.Entitys[x].Index);
      l.Note.pos = note_pos;
      mouseLive.push_back(l);
    }
  }
  if (mouseLiveShowInstruction == "LineParallel")
  {
    if (cadCountSelection() > 0 && cadGetSelected()[0].Type == CAD_LINE)
    {
      float distance;
      if (cadGetSelectionBox().visable == true)
      {
        distance = geoGetPerpendicularDistance(cadGetSelected()[0].Line, mouseLastMouseOverPoint);
      }
      else
      {
        distance = geoGetPerpendicularDistance(cadGetSelected()[0].Line, mouseCurrent);
      }

      l.Type = CAD_LINE;
      l.Line = geoGetParallelLine(cadGetSelected()[0].Line, mouseCurrent, distance);
      mouseLive.push_back(l);
    }
  }
  if (mouseLiveShowInstruction == "LineVerticalOrigin")
  {
      //V cout << KRED << "(mouseLiveShow) LineVerticalOrigin" << KNORMAL << endl;
      l.Type = CAD_LINE;
      l.Line = line_t{ point_t{0,0}, point_t{0, mouseCurrent.y} };
      mouseLive.push_back(l);
  }
  if (mouseLiveShowInstruction == "LineHorizontalOrigin")
  {
      //V cout << KRED << "(mouseLiveShow) LineVerticalOrigin" << KNORMAL << endl;
      l.Type = CAD_LINE;
      l.Line = line_t{ point_t{0,0}, point_t{mouseCurrent.x, 0} };
      mouseLive.push_back(l);
  }
  if (mouseLiveShowInstruction == "DimensionPoint")
  {
      //V cout << KRED << "(mouseLiveShow) LineVerticalOrigin" << KNORMAL << endl;
      l.Type = CAD_DIMENSION;
      l.Dimension.Type = DIMENSION_POINT;
      l.Dimension.Point.snap_pos = mouseCadLastSnapClick();
      l.Dimension.Point.text_pos = mouseCurrent;
      mouseLive.push_back(l);

      l.Type = CAD_LINE;
      line_t leader_body = geoExtendLineEndpoint(line_t{mouseCurrent, mouseCadLastSnapClick()}, 0.050);
      leader_body = geoExtendLineEndpoint(line_t{leader_body.end, leader_body.start}, 0.050);
      l.Line = leader_body;
      mouseLive.push_back(l);

      l.Type = CAD_LINE;
      l.Line = geoExtendLineStartpoint(geoRotateLine(leader_body, leader_body.start, 135), 0.050);
      mouseLive.push_back(l);

      l.Type = CAD_LINE;
      l.Line = geoExtendLineStartpoint(geoRotateLine(leader_body, leader_body.start, -135), 0.050);
      mouseLive.push_back(l);
  }
  if (mouseLive.size() > 0)
  {
    cadShowLiveEntity(mouseLive);
  }
  else
  {
    cadHideLiveEntity();
  }
  mouseLive.clear();
  if (passivePrime)
  {
    passivePrime = false;
    return;
  }
  //V printf("%sX: %.6f, Y: %.6f, Z: %.6f%s\r", KGREEN, pos.x, pos.y, pos.z, KNORMAL);
  string m;
  if (pos.z != 0)
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
      cadEdit(a, e, false);
      return;
    }
    else if (geoInTolerance(pos.x, e.Arc.end.x, mouseTolerance()) && geoInTolerance(pos.y, e.Arc.end.y, mouseTolerance()) && !e.Removed && e.Type == CAD_ARC && snapArcEndpoints)
    {
      //uiEdit(0, uiEntity{UI_TEXT, RED, "End point", UI_MENU_POSITION});
      cadShowSelectionBox(e.Arc.end);
      mouseLastMouseOverPoint = e.Arc.end;
      mouseLastMouseOverEntity = e;
      e.MouseOver = true;
      cadEdit(a, e, false);
      return;
    }
    else if (geoInTolerance(pos.x, e.Arc.center.x, mouseTolerance()) && geoInTolerance(pos.y, e.Arc.center.y, mouseTolerance()) && !e.Removed && e.Type == CAD_ARC && snapArcCenter)
    {
      //uiEdit(0, uiEntity{UI_TEXT, RED, "Center point", UI_MENU_POSITION});
      cadShowSelectionBox(e.Arc.center);
      mouseLastMouseOverPoint = e.Arc.center;
      mouseLastMouseOverEntity = e;
      e.MouseOver = true;
      cadEdit(a, e, false);
      return;
    }
    else if (geoInTolerance(pos.x, e.Line.start.x, mouseTolerance()) && geoInTolerance(pos.y, e.Line.start.y, mouseTolerance()) && !e.Removed && e.Type == CAD_LINE && snapArcCenter)
    {
      cadShowSelectionBox(e.Line.start);
      mouseLastMouseOverPoint = e.Line.start;
      mouseLastMouseOverEntity = e;
      e.MouseOver = true;
      cadEdit(a, e, false);
      return;
    }
    else if (geoInTolerance(pos.x, e.Line.end.x, mouseTolerance()) && geoInTolerance(pos.y, e.Line.end.y, mouseTolerance()) && !e.Removed && e.Type == CAD_LINE && snapLineEndpoints)
    {
      cadShowSelectionBox(e.Line.end);
      mouseLastMouseOverPoint = e.Line.end;
      mouseLastMouseOverEntity = e;
      e.MouseOver = true;
      cadEdit(a, e, false);
      return;
    }
    else if (geoInTolerance(pos.x, geoGetLineMidpoint(e.Line).x, mouseTolerance()) && geoInTolerance(pos.y, geoGetLineMidpoint(e.Line).y, mouseTolerance()) && !e.Removed && e.Type == CAD_LINE && snapLineMidpoint)
    {
      cadShowSelectionBox(geoGetLineMidpoint(e.Line));
      mouseLastMouseOverPoint = geoGetLineMidpoint(e.Line);
      mouseLastMouseOverEntity = e;
      e.MouseOver = true;
      cadEdit(a, e, false);
      return; //Only highlight one entity
    }
    else if (mouseVector(e, pos) && snapVector)
    {
      mouseLastMouseOverPoint = mouseVectorIntersection;
      mouseLastMouseOverEntity = e;
      e.MouseOver = true;
      cadEdit(a, e, false);
      cadRedraw();
      return;
    }
    else
    {
      e.MouseOver = false;
      cadEdit(a, e, false);
      cadHideSelectionBox();
      cadRedraw();
    }
  }
  //V fflush(stdout);
}
