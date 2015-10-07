#include <CLI.h>

using namespace std;

struct sub_sub_menu_item_t{
  string c; //keystroke
  string msg;
  void *(*cmd)();
};

struct sub_menu_item_t{
  string c; //keystroke
  string msg;
  sub_sub_menu_item_t submenu[10];
};

struct menu_item_t{
  string c; //keystroke
  string msg;
  sub_menu_item_t submenu[10];
};

void *(*textCallback)();
color_t CurrentColor = GREEN;
std::string CurrentFile;
int Level = 0;
int Level1Selection;
int Level2Selection;
bool TextInput = false;
bool TextReady = false;
string text = "";

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

#define CLI_MENU_ITEM_SEPERATOR "   "
#define CLI_MENU_ITEM_LINK  " - "

std::string cliGetCurrentFile()
{
  return CurrentFile;
}
float cliGetInput()
{
  if (text.size() == 0)
  {
    return 0;
  }
  text.replace(text.find("> "), sizeof("> ")-1, "");
  float input;
  input = (float)atof(luaEval(text).c_str());
  if (!isnan(input))
  {
    return input;
  }
  else
  {
    return 0;
  }
}
std::string cliGetTextInput()
{
  if (text.size() == 0)
  {
    return "";
  }
  text.replace(text.find("> "), sizeof("> ")-1, "");
  return text;
}
void *cliCreateLineEndpoints()
{
  std::vector<cadEntity> e = cadGetSelected();
  if (e.size() < 2)
  {
    return NULL;
  }
  point_t Start = e[0].SelectedAt;
  point_t End = e[1].SelectedAt;
  cadSetColor(CurrentColor);
  cadDrawLine(Start, End);
  cliScreenUnSelectAll();
  return NULL;
}
void *cliCreateLineVerticalEndpoint()
{
  if (TextReady == true)
  {
    TextReady = false;
    float input = cliGetInput();
    std::vector<cadEntity> e = cadGetSelected();
    if (e.size() < 1)
    {
      return NULL;
    }
    point_t Start = e[0].SelectedAt;
    point_t End = { e[0].SelectedAt.x , input };
    cadSetColor(CurrentColor);
    cadDrawLine(Start, End);
    cliScreenUnSelectAll();
    return NULL;
  }
  textCallback = &cliCreateLineVerticalEndpoint;
  TextInput = true;
  cliPush("> ");
  uiEdit(0, uiEntity{UI_TEXT, RED, "Input Y endpoint!", UI_MENU_POSITION});
  return NULL;
}
void *cliCreateLineHorizontalEndpoint()
{
  if (TextReady == true)
  {
    TextReady = false;
    float input = cliGetInput();
    std::vector<cadEntity> e = cadGetSelected();
    if (e.size() < 1)
    {
      return NULL;
    }
    point_t Start = e[0].SelectedAt;
    point_t End = { (float)atof(text.c_str()), e[0].SelectedAt.y };
    cadSetColor(CurrentColor);
    cadDrawLine(Start, End);
    cliScreenUnSelectAll();
    return NULL;
  }
  textCallback = &cliCreateLineHorizontalEndpoint;
  TextInput = true;
  cliPush("> ");
  uiEdit(0, uiEntity{UI_TEXT, RED, "Input X endpoint!", UI_MENU_POSITION});
  return NULL;
}
void *cliCreateLineVerticalOrigin()
{
    if (TextReady == true)
    {
      TextReady = false;
      float input = cliGetInput();
      point_t Start = { 0, 0};
      point_t End = { 0 , input };
      cadSetColor(CurrentColor);
      cadDrawLine(Start, End);
      mouseLiveClear();
      return NULL;
    }
    textCallback = &cliCreateLineVerticalOrigin;
    TextInput = true;
    mouseLiveShow("LineVerticalOrigin");
    cliPush("> ");
    uiEdit(0, uiEntity{UI_TEXT, RED, "Input Y coordinant!", UI_MENU_POSITION});
    return NULL;
}
void *cliCreateLineHorizontalOrigin()
{
  if (TextReady == true)
  {
    TextReady = false;
    float input = cliGetInput();
    point_t Start = { 0, 0};
    point_t End = {input, 0};
    cadSetColor(CurrentColor);
    cadDrawLine(Start, End);
    return NULL;
  }
  textCallback = &cliCreateLineHorizontalOrigin;
  TextInput = true;
  mouseLiveShow("LineHorizontalOrigin");
  cliPush("> ");
  uiEdit(0, uiEntity{UI_TEXT, RED, "Input X coordinant!", UI_MENU_POSITION});
  return NULL;
}
void *cliCreateLinePerpendicular()
{
  if (TextReady == true)
  {
    TextReady = false;
    float input = cliGetInput();
    std::vector<cadEntity> e = cadGetSelected();
    if (e.size() == 1) //Make sure we have only one entity seleced
    {
      line_t p = geoGetPerpendicularLine(line_t{ e[0].Line.start,  e[0].Line.end}, mouseCadLastClickPos(), input);
      point_t Start = p.start;
      point_t End = p.end;
      cadSetColor(CurrentColor);
      cadDrawLine(Start, End);
      cliScreenUnSelectAll();
      return NULL;
    }
    return NULL;
  }
  textCallback = &cliCreateLinePerpendicular;
  TextInput = true;
  cliPush("> ");
  uiEdit(3, uiEntity{UI_TEXT, RED, "Click to indecated offset direction!", UI_HINT_POSITION});
  uiEdit(0, uiEntity{UI_TEXT, RED, "Input distance!", UI_MENU_POSITION});
  return NULL;
}
void *cliCreateLineParallel()
{
  if (TextReady == true)
  {
    TextReady = false;
    float input = cliGetInput();
    std::vector<cadEntity> e = cadGetSelected();
    if (e.size() == 1) //Make sure we have only one entity seleced
    {
      //V printf("(cliCreateLinePerpendicular) %d Entitys selected!\n", e.size());
      line_t p = geoGetParallelLine(line_t{ e[0].Line.start,  e[0].Line.end}, mouseCadGetCurrentPos(), input);
      point_t Start = p.start;
      point_t End = p.end;
      cadSetColor(CurrentColor);
      cadDrawLine(Start, End);
      cliScreenUnSelectAll();
      return NULL;
    }
    return NULL;
  }
  textCallback = &cliCreateLineParallel;
  TextInput = true;
  cliPush("> ");
  uiEdit(0, uiEntity{UI_TEXT, RED, "Input distance!", UI_MENU_POSITION});
  uiEdit(3, uiEntity{UI_TEXT, RED, "Click to indecated offset direction!", UI_HINT_POSITION});
  mouseLiveShow("LineParallel");
  return NULL;
}
void *cliCreateCircleCenterDiameter()
{
  if (TextReady == true)
  {
    TextReady = false;
    float input = cliGetInput();
    std::vector<cadEntity> e = cadGetSelected();
    if (e.size() == 1) //Make sure we have only one entity seleced
    {
      point_t center = mouseCadLastSnapClick();
      float radius = input / 2;
      point_t endpoints = point_t{ center.x + radius, center.y };
      cadSetColor(CurrentColor);
      cadDrawArc(arc_t{ endpoints, endpoints, center, radius, ARC_CW });
      cliScreenUnSelectAll();
      return NULL;
    }
    return NULL;
  }
  textCallback = &cliCreateCircleCenterDiameter;
  TextInput = true;
  cliPush("> ");
  uiEdit(0, uiEntity{UI_TEXT, RED, "Circle Diameter?", UI_MENU_POSITION});
  uiEdit(3, uiEntity{UI_TEXT, RED, "Click for center point!", UI_HINT_POSITION});
  return NULL;
}
void *cliCreateCircleCenterRadius()
{
  if (TextReady == true)
  {
    TextReady = false;
    float input = cliGetInput();
    std::vector<cadEntity> e = cadGetSelected();
    if (e.size() == 1) //Make sure we have only one entity seleced
    {
      point_t center = mouseCadLastSnapClick();
      float radius = input;
      point_t endpoints = point_t{ center.x + radius, center.y };
      cadSetColor(CurrentColor);
      cadDrawArc(arc_t{ endpoints, endpoints, center, radius, ARC_CW });
      cliScreenUnSelectAll();
      return NULL;
    }
    return NULL;
  }
  textCallback = &cliCreateCircleCenterRadius;
  TextInput = true;
  cliPush("> ");
  uiEdit(0, uiEntity{UI_TEXT, RED, "Circle Radius?", UI_MENU_POSITION});
  uiEdit(3, uiEntity{UI_TEXT, RED, "Click for center point!", UI_HINT_POSITION});
  return NULL;
}
void *cliCreateCircleTwoPoints()
{
  //
}
void *cliScreenSelectAll()
{
  cliScreenUnSelectAll();
  int m = cadGetEntityArrayIndex();
  cadEntity e;
  for (int a = 0; a < m; a++)
  {
    e = cadGetEntityArray(a);
    e.Selected = true;
    e.SelectionIndex = a;
    cadEdit(a, e, false);
  }
}
void *cliScreenUnSelectAll()
{
  int m = cadGetEntityArrayIndex();
  cadEntity e;
  for (int a = 0; a < m; a++)
  {
    e = cadGetEntityArray(a);
    e.Selected = false;
    cadEdit(a, e, false);
  }
}
void *cliScreenDeleteSelected()
{
  cadRemoveSelected();
}
void *cliViewPlaneXY() { sceneSetViewAngle(0, 0, 0); return NULL; }
void *cliViewPlaneYZ() { sceneSetViewAngle(0, 90, 0); return NULL; }
void *cliViewPlaneZX() { sceneSetViewAngle(90, 0, 0); return NULL; }
void *cliViewPlaneOrtho() { sceneSetViewAngle(45, 45, 45); return NULL; }
void *cliFileExit() { EXIT; return NULL; };
void *cliFileGetNew()
{
  if (TextReady == true)
  {
    TextReady = false;
    cliScreenSelectAll();
    cliScreenDeleteSelected();
    CurrentFile = cliGetTextInput();
    int r = fileOpen(CurrentFile);
    glutSetWindowTitle(CurrentFile.c_str());
    if (r == FILE_OPEN_ERROR) uiEdit(0, uiEntity{UI_TEXT, RED, "Error reading file!", UI_MENU_POSITION});
    if (r == FILE_UNKNOWN_FORMAT) uiEdit(0, uiEntity{UI_TEXT, RED, "Unknown file format!", UI_MENU_POSITION});
    return NULL;
  }
  textCallback = &cliFileGetNew;
  TextInput = true;
  cliPush("> ");
  uiEdit(0, uiEntity{UI_TEXT, RED, "Get File?", UI_MENU_POSITION});
  return NULL;
}
void *cliFileGetMerge()
{
  if (TextReady == true)
  {
    TextReady = false;
    CurrentFile = cliGetTextInput();
    glutSetWindowTitle(CurrentFile.c_str());
    int r = fileOpen(CurrentFile);
    if (r == FILE_OPEN_ERROR) uiEdit(0, uiEntity{UI_TEXT, RED, "Error reading file!", UI_MENU_POSITION});
    if (r == FILE_UNKNOWN_FORMAT) uiEdit(0, uiEntity{UI_TEXT, RED, "Unknown file format!", UI_MENU_POSITION});
    return NULL;
  }
  textCallback = &cliFileGetMerge;
  TextInput = true;
  cliPush("> ");
  uiEdit(0, uiEntity{UI_TEXT, RED, "Merge File?", UI_MENU_POSITION});
  return NULL;
}
void *cliFilePutNew()
{
  if (TextReady == true)
  {
    TextReady = false;
    CurrentFile = cliGetTextInput();
    int r = fileSave(CurrentFile);
    if (r == FILE_OPEN_ERROR) uiEdit(0, uiEntity{UI_TEXT, RED, "Error writing file!", UI_MENU_POSITION});
    return NULL;
  }
  textCallback = &cliFilePutNew;
  TextInput = true;
  cliPush("> ");
  uiEdit(0, uiEntity{UI_TEXT, RED, "New File Name?", UI_MENU_POSITION});
  return NULL;
}
void *cliFilePutCurrent()
{
  int r = fileSave(CurrentFile);
  if (r == FILE_OPEN_ERROR)
  {
    uiEdit(0, uiEntity{UI_TEXT, RED, "Error writing file!", UI_MENU_POSITION});
  }
  else
  {
    string t = "Saving " + CurrentFile;
    uiEdit(0, uiEntity{UI_TEXT, RED, t, UI_MENU_POSITION});
  }

}
void *cliScreenColorRed()
{
  cadUndoPushState();
  CurrentColor = RED;
  cadEntity e;
  for (int x=0; x < cadGetEntityArrayIndex(); x++)
  {
    e = cadGetEntityArray(x);
    if (e.Selected && !e.Removed)
    {
      e.Color = CurrentColor;
      cadEdit(x, e, false);
    }
  }
  cliScreenUnSelectAll();
  return NULL;
}
void *cliScreenColorBlue()
{
  cadUndoPushState();
  CurrentColor = BLUE;
  cadEntity e;
  for (int x=0; x < cadGetEntityArrayIndex(); x++)
  {
    e = cadGetEntityArray(x);
    if (e.Selected && !e.Removed)
    {
      e.Color = CurrentColor;
      cadEdit(x, e, false);
    }
  }
  cliScreenUnSelectAll();
  return NULL;
}
void *cliScreenColorGreen()
{
  cadUndoPushState();
  CurrentColor = GREEN;
  cadEntity e;
  for (int x=0; x < cadGetEntityArrayIndex(); x++)
  {
    e = cadGetEntityArray(x);
    if (e.Selected && !e.Removed)
    {
      e.Color = CurrentColor;
      cadEdit(x, e, false);
    }
  }
  cliScreenUnSelectAll();
  return NULL;
}
void *cliScreenColorWhite()
{
  cadUndoPushState();
  CurrentColor = WHITE;
  cadEntity e;
  for (int x=0; x < cadGetEntityArrayIndex(); x++)
  {
    e = cadGetEntityArray(x);
    if (e.Selected && !e.Removed)
    {
      e.Color = CurrentColor;
      cadEdit(x, e, false);
    }
  }
  cliScreenUnSelectAll();
  return NULL;
}
void *cliScreenColorBlack()
{
  cadUndoPushState();
  CurrentColor = BLACK;
  cadEntity e;
  for (int x=0; x < cadGetEntityArrayIndex(); x++)
  {
    e = cadGetEntityArray(x);
    if (e.Selected && !e.Removed)
    {
      e.Color = CurrentColor;
      cadEdit(x, e, false);
    }
  }
  cliScreenUnSelectAll();
  return NULL;
}
void *cliScreenColorYellow()
{
  cadUndoPushState();
  CurrentColor = YELLOW;
  cadEntity e;
  for (int x=0; x < cadGetEntityArrayIndex(); x++)
  {
    e = cadGetEntityArray(x);
    if (e.Selected && !e.Removed)
    {
      e.Color = CurrentColor;
      cadEdit(x, e, false);
    }
  }
  cliScreenUnSelectAll();
  return NULL;
}
void *cliScreenColorCyan()
{
  cadUndoPushState();
  CurrentColor = CYAN;
  cadEntity e;
  for (int x=0; x < cadGetEntityArrayIndex(); x++)
  {
    e = cadGetEntityArray(x);
    if (e.Selected && !e.Removed)
    {
      e.Color = CurrentColor;
      cadEdit(x, e, false);
    }
  }
  cliScreenUnSelectAll();
  return NULL;
}
void *cliScreenColorMagenta()
{
  cadUndoPushState();
  CurrentColor = MAGENTA;
  cadEntity e;
  for (int x=0; x < cadGetEntityArrayIndex(); x++)
  {
    e = cadGetEntityArray(x);
    if (e.Selected && !e.Removed)
    {
      e.Color = CurrentColor;
      cadEdit(x, e, false);
    }
  }
  cliScreenUnSelectAll();
  return NULL;
}
void *cliScreenColorGrey()
{
  cadUndoPushState();
  CurrentColor = DARKGREY;
  cadEntity e;
  for (int x=0; x < cadGetEntityArrayIndex(); x++)
  {
    e = cadGetEntityArray(x);
    if (e.Selected && !e.Removed)
    {
      e.Color = CurrentColor;
      cadEdit(x, e, false);
    }
  }
  cliScreenUnSelectAll();
  return NULL;
}
void *cliScreenAnalyzeEntity()
{
  string t = "";
  cadEntity e;
  if (TextReady == true)
  {
    cliGetTextInput();
    TextReady = false;
    return NULL;
  }
  for (int x=0; x < cadGetEntityArrayIndex(); x++)
  {
    e = cadGetEntityArray(x);
    if (e.Selected && !e.Removed)
    {
      if (e.Type == CAD_ARC)
      {
        string direction;
        if (e.Arc.direction == ARC_CW)
        {
          direction = "CW";
        }
        else
        {
          direction = "CCW";
        }
        t.append("Type=> Arc, Direction=> " + direction + ", Diameter=> " + to_string(e.Arc.radius *2)
        + ", Radius=> " + to_string(e.Arc.radius)
        + ", Start Angle=> " + to_string(geoRadiansToDegrees(geoGetArcStartAngle(e.Arc)))
        + ", End Angle=> " + to_string(geoRadiansToDegrees(geoGetArcEndAngle(e.Arc)))
        + ", Length=> " + to_string(geoGetArcLength(e.Arc))
        );
      }
      if (e.Type == CAD_LINE)
      {
        t.append("Type=> Line, Length=> " + to_string(geoGetLineLength(e.Line))
        + ", Angle=> " + to_string(geoRadiansToDegrees(geoGetLineAngle(e.Line)))
        + ", Midpoint=> (" + to_string(geoGetLineMidpoint(e.Line).x) + ", " + to_string(geoGetLineMidpoint(e.Line).y) + ", " + to_string(geoGetLineMidpoint(e.Line).z) + ")"
        );
      }
    }
  }
  textCallback = &cliScreenAnalyzeEntity;
  TextInput = true;
  uiEdit(0, uiEntity{UI_TEXT, GREEN, t, UI_MENU_POSITION});
  return NULL;
}
void *cliXformTrim1()
{
  int index;
  std::vector<cadEntity> e = cadGetSelected();
  if (e.size() < 2)
  {
    return NULL;
  }
  point_t new_endpoint = geoGetIntersection(line_t{e[0].Line.start, e[0].Line.end}, line_t{e[1].Line.start, e[1].Line.end});
  V cout << KRED << "(cliXformTrim1) Intersection = ";
  V debugDumpPointStructure(new_endpoint);
  if (isnan(new_endpoint.x) || isnan(new_endpoint.y) || isnan(new_endpoint.z))
  {
    V cout << KRED << "(cliXformTrim1) No Intersection!" << KNORMAL << endl;
    return NULL;
  }
  //V printf("(cliXformTrim1) Intersection point is (%.6f, %.6f)\n", new_endpoint.x, new_endpoint.y);

  //Find which end of first selected line is closest to the Intersection point, then replace with intersection point
  float start_d = geoGetLineLength(line_t{ e[0].Line.start, new_endpoint });
  float end_d = geoGetLineLength(line_t{ e[0].Line.end, new_endpoint });
  //Doesnt work because were looking at first in array not first clicked
  //V printf("e[0].SelectionIndex = %d\ne[1].SelectionIndex = %d\n", e[0].SelectionIndex, e[1].SelectionIndex);
  if (start_d < end_d)
  {
      e[0].Line.start = new_endpoint;
  }
  else
  {
    e[0].Line.end = new_endpoint;
  }
  cadEdit(e[0].Index, e[0]);
  cliScreenUnSelectAll();
  return NULL;
}
void *cliXformTrim2()
{

}
void *cliXformTrimCircle()
{
  if (TextReady == true)
  {
    TextReady = false;
    //uiEdit(3, uiEntity{UI_TEXT, RED, "Click for center point!", UI_HINT_POSITION});
    cliGetTextInput();
    int index;
    std::vector<cadEntity> e = cadGetSelected();
    if (e.empty())
    {
      return NULL;
    }
    if (e.size() == 3)
    {
      if (e[0].Type == CAD_ARC && e[0].Line.start == e[0].Line.end && e[1].Type == CAD_LINE && e[2].Type == CAD_LINE) //Make sure first selection is a circle and second and third selections are lines
      {
        //printf("Line1 - X: %.6f, Y: %.6f\n", e[1].SelectedAt.x, e[1].SelectedAt.y);
        //printf("Line2 - X: %.6f, Y: %.6f\n", e[2].SelectedAt.x, e[2].SelectedAt.y);
        float d1 = geoGetLineLength(line_t{e[0].Arc.start, e[1].SelectedAt});
        float d2 = geoGetLineLength(line_t{e[0].Arc.end, e[1].SelectedAt});
        if (d1 < d2)
        {
          e[0].Arc.start = e[1].SelectedAt;
          e[0].Arc.end = e[2].SelectedAt;
        }
        else
        {
          e[0].Arc.end = e[1].SelectedAt;
          e[0].Arc.start = e[2].SelectedAt;
        }
        bool closest = false;
        std::vector<point_t> p = geoGetPointsOfArc(e[0].Arc);
        for (int z = 0; z < p.size(); z++)
        {
          if (geoInTolerance(p[z].x, e[0].SelectedAt.x, 0.050) && geoInTolerance(p[z].y, e[0].SelectedAt.y, 0.050))
          {
            closest = true;
          }
        }
        if (!closest)
        {
          e[0].Arc.direction = !e[0].Arc.direction;
        }
        cadEdit(e[0].Index, e[0]);
        cliScreenUnSelectAll();
      }
      else
      {
        //printf("Selection wasnt satisfied!\n");
      }
      return NULL;
    }
    return NULL;
  }
  textCallback = &cliXformTrimCircle;
  TextInput = true;
  cliPush("> ");
  uiEdit(3, uiEntity{UI_TEXT, RED, "Select Circle and two line endpoints and press enter!", UI_HINT_POSITION});
  return NULL;
}
void *cliXformFilletRadius()
{
  if (TextReady == true)
 {
   TextReady = false;
   arc_t fillet;
   fillet.radius = cliGetInput();
   std::vector<cadEntity> e = cadGetSelected();
   if (e.empty())
   {
     return NULL;
   }
   if (e.size() == 2) //Make sure we have two selections
   {
     if (e[0].Type == CAD_LINE && e[1].Type == CAD_LINE) //Make sure both selections are lines
     {
       point_t relative_center = geoGetLineMidpoint(line_t{e[0].SelectedAt, e[1].SelectedAt});
       V cout << KRED << "(cliXformFilletRadius) Relative Center = ";
       V debugDumpPointStructure(relative_center);
       line_t p_line_zero = geoGetParallelLine(e[0].Line, relative_center, fillet.radius);
       //cadDrawLine(p_line_zero);
       line_t p_line_one = geoGetParallelLine(e[1].Line, relative_center, fillet.radius);
       //cadDrawLine(p_line_one);
       //Get intersection point of both parallel lines to get fillet arc center point
       fillet.center = geoGetLineIntersection(p_line_zero, p_line_one);
       V cout << KRED << "(cliXformFilletRadius) Arc Center = ";
       V debugDumpPointStructure(fillet.center);

       //Get intersection of line_zero and p_line_one for fillet start point
       fillet.start = geoGetLineIntersection(e[0].Line, p_line_one);

       //Get intersection of line_one and p_line_zero for fillet start point
       fillet.end = geoGetLineIntersection(e[1].Line, p_line_zero);

       V cout << KRED << "(cliXformFilletRadius) fillet.start = ";
       V debugDumpPointStructure(fillet.start);
       V cout << KRED << "(cliXformFilletRadius) fillet.end = ";
       V debugDumpPointStructure(fillet.end);

       //Determine direction of fillet by drawing a line from center to start and end point, if included angle is positive were CW

       float start_angle = geoGetLineAngle(line_t{fillet.center, fillet.start});
       float end_angle = geoGetLineAngle(line_t{fillet.center, fillet.end});
       float relative_center_angle = geoGetLineAngle(line_t{e[0].SelectedAt, e[1].SelectedAt});
       V cout << KRED << "(cliXformFilletRadius) Arc Angle Difference = " << (start_angle - end_angle) << KNORMAL << endl;
       V cout << KRED << "(cliXformFilletRadius) Arc Start Angle = " << start_angle << KNORMAL << endl;
       V cout << KRED << "(cliXformFilletRadius) Arc End Angle = " << end_angle << KNORMAL << endl;
       V cout << KRED << "(cliXformFilletRadius) Relative center Angle = " << relative_center_angle << KNORMAL << endl;
       //if ((start_angle - end_angle) < start_angle)
       //if X vector has positive direction and
       //This approach is about 90% of the time
       /*fillet.direction = ARC_CW;
       vector<point_t> cw_ap = geoGetPointsOfArc(fillet);
       float cw_endpoint_distance = geoGetLineLength(line_t{fillet.end, cw_ap.back()});
       fillet.direction = ARC_CCW;
       vector<point_t> ccw_ap = geoGetPointsOfArc(fillet);
       float ccw_endpoint_distance = geoGetLineLength(line_t{fillet.end, ccw_ap.back()});
       if (cw_endpoint_distance < ccw_endpoint_distance )
       {
         fillet.direction = ARC_CW;
       }
       else
       {
         fillet.direction = ARC_CCW;
       }*/
       point_t original_corner = geoGetLineIntersection(e[0].Line, e[1].Line);
       fillet.direction = ARC_CW;
       vector<point_t> cw_ap = geoGetPointsOfArc(fillet);
       float cw_distance_to_corner = geoGetLineLength(line_t{original_corner, cw_ap.at(cw_ap.size()/2)});

       fillet.direction = ARC_CCW;
       vector<point_t> ccw_ap = geoGetPointsOfArc(fillet);
       float ccw_distance_to_corner = geoGetLineLength(line_t{original_corner, ccw_ap.at(ccw_ap.size()/2)});
       if (cw_ap.size() > 180 || cw_ap.size() > 180)
       {
         V cout << KRED << "(cliXformFilletRadius) Fillet 180 error!" << KNORMAL << endl;
         return NULL;
       }
       if (cw_distance_to_corner < ccw_distance_to_corner)
       {
         fillet.direction = ARC_CW;
       }
       else
       {
         fillet.direction = ARC_CCW;
       }

       V debugDumpArcStructure(fillet);
       e[0].Line = geoReplaceClosestEndpoint(e[0].Line, fillet.start);
       e[1].Line = geoReplaceClosestEndpoint(e[1].Line, fillet.end);


       cadEdit(e[0].Index, e[0], false); //Edit line 0
       cadEdit(e[1].Index, e[1]); //Edit line 1
       cadDrawArc(fillet); //Draw arc
       cliScreenUnSelectAll();
     }
   }
   return NULL;
 }
 textCallback = &cliXformFilletRadius;
 TextInput = true;
 cliPush("> ");
 uiEdit(0, uiEntity{UI_TEXT, RED, "Radius?", UI_MENU_POSITION});
 return NULL;
}
void *cliXformFilletDiameter()
{
  if (TextReady == true)
 {
   TextReady = false;
   arc_t fillet;
   fillet.radius = cliGetInput() / 2;
   std::vector<cadEntity> e = cadGetSelected();
   if (e.empty())
   {
     return NULL;
   }
   if (e.size() == 2) //Make sure we have two selections
   {
     if (e[0].Type == CAD_LINE && e[1].Type == CAD_LINE) //Make sure both selections are lines
     {
       point_t relative_center = geoGetLineMidpoint(line_t{e[0].SelectedAt, e[1].SelectedAt});
       V cout << KRED << "(cliXformFilletRadius) Relative Center = ";
       V debugDumpPointStructure(relative_center);
       line_t p_line_zero = geoGetParallelLine(e[0].Line, relative_center, fillet.radius);
       //cadDrawLine(p_line_zero);
       line_t p_line_one = geoGetParallelLine(e[1].Line, relative_center, fillet.radius);
       //cadDrawLine(p_line_one);
       //Get intersection point of both parallel lines to get fillet arc center point
       fillet.center = geoGetLineIntersection(p_line_zero, p_line_one);
       V cout << KRED << "(cliXformFilletRadius) Arc Center = ";
       V debugDumpPointStructure(fillet.center);

       //Get intersection of line_zero and p_line_one for fillet start point
       fillet.start = geoGetLineIntersection(e[0].Line, p_line_one);

       //Get intersection of line_one and p_line_zero for fillet start point
       fillet.end = geoGetLineIntersection(e[1].Line, p_line_zero);

       V cout << KRED << "(cliXformFilletRadius) fillet.start = ";
       V debugDumpPointStructure(fillet.start);
       V cout << KRED << "(cliXformFilletRadius) fillet.end = ";
       V debugDumpPointStructure(fillet.end);

       //Determine direction of fillet by drawing a line from center to start and end point, if included angle is positive were CW

       float start_angle = geoGetLineAngle(line_t{fillet.center, fillet.start});
       float end_angle = geoGetLineAngle(line_t{fillet.center, fillet.end});
       float relative_center_angle = geoGetLineAngle(line_t{e[0].SelectedAt, e[1].SelectedAt});
       V cout << KRED << "(cliXformFilletRadius) Arc Angle Difference = " << (start_angle - end_angle) << KNORMAL << endl;
       V cout << KRED << "(cliXformFilletRadius) Arc Start Angle = " << start_angle << KNORMAL << endl;
       V cout << KRED << "(cliXformFilletRadius) Arc End Angle = " << end_angle << KNORMAL << endl;
       V cout << KRED << "(cliXformFilletRadius) Relative center Angle = " << relative_center_angle << KNORMAL << endl;
       //if ((start_angle - end_angle) < start_angle)
       //if X vector has positive direction and
       //This approach is about 90% of the time
       /*fillet.direction = ARC_CW;
       vector<point_t> cw_ap = geoGetPointsOfArc(fillet);
       float cw_endpoint_distance = geoGetLineLength(line_t{fillet.end, cw_ap.back()});
       fillet.direction = ARC_CCW;
       vector<point_t> ccw_ap = geoGetPointsOfArc(fillet);
       float ccw_endpoint_distance = geoGetLineLength(line_t{fillet.end, ccw_ap.back()});
       if (cw_endpoint_distance < ccw_endpoint_distance )
       {
         fillet.direction = ARC_CW;
       }
       else
       {
         fillet.direction = ARC_CCW;
       }*/
       point_t original_corner = geoGetLineIntersection(e[0].Line, e[1].Line);
       fillet.direction = ARC_CW;
       vector<point_t> cw_ap = geoGetPointsOfArc(fillet);
       float cw_distance_to_corner = geoGetLineLength(line_t{original_corner, cw_ap.at(cw_ap.size()/2)});

       fillet.direction = ARC_CCW;
       vector<point_t> ccw_ap = geoGetPointsOfArc(fillet);
       float ccw_distance_to_corner = geoGetLineLength(line_t{original_corner, ccw_ap.at(ccw_ap.size()/2)});
       if (cw_ap.size() > 180 || cw_ap.size() > 180)
       {
         V cout << KRED << "(cliXformFilletRadius) Fillet 180 error!" << KNORMAL << endl;
         return NULL;
       }
       if (cw_distance_to_corner < ccw_distance_to_corner)
       {
         fillet.direction = ARC_CW;
       }
       else
       {
         fillet.direction = ARC_CCW;
       }

       V debugDumpArcStructure(fillet);
       e[0].Line = geoReplaceClosestEndpoint(e[0].Line, fillet.start);
       e[1].Line = geoReplaceClosestEndpoint(e[1].Line, fillet.end);


       cadEdit(e[0].Index, e[0], false); //Edit line 0
       cadEdit(e[1].Index, e[1]); //Edit line 1
       cadDrawArc(fillet); //Draw arc
       cliScreenUnSelectAll();
     }
   }
   return NULL;
 }
 textCallback = &cliXformFilletDiameter;
 TextInput = true;
 cliPush("> ");
 uiEdit(0, uiEntity{UI_TEXT, RED, "Diameter?", UI_MENU_POSITION});
 return NULL;
}
void cliXform_CopyClicked(bool c)
{
  dialogCheckboxSet("Move?", !c);
}
void cliXform_MoveClicked(bool c)
{
  dialogCheckboxSet("Copy?", !c);
}
void cliXform_LeftClicked(bool c)
{
  dialogCheckboxSet("Right?", !c);
}
void cliXform_RightClicked(bool c)
{
  dialogCheckboxSet("Left?", !c);
}
void cliXformRotate_Callback()
{
  bool Operator;
  if (dialogCheckboxGet("Copy?"))
  {
    Operator = false;
  }
  if (dialogCheckboxGet("Move?"))
  {
    Operator = true;
  }
  float Steps = fabs(atof(dialogTextboxGetString("Steps").c_str()));
  float Angle = atof(dialogTextboxGetString("Angle").c_str());
  bool Direction;
  if (Angle > 0)
  {
    Direction = ARC_CW;
  }
  else
  {
    Direction = ARC_CCW;
  }
  string OriginString = dialogTextboxGetString("Origin");
  vector<string> OriginArray = split(OriginString, ',');
  if (OriginArray.size() < 1)
  {
    //Implement some msg box dialog!
    //Origin error!
    return;
  }
  point_t Origin = point_t{(float)atof(OriginArray[0].c_str()), (float)atof(OriginArray[1].c_str())};
  V cout << KRED << "Steps" << KCYAN << " => " << KGREEN << Steps << KNORMAL << endl;
  V cout << KRED << "Angle" << KCYAN << " => " << KGREEN << Angle << KNORMAL << endl;
  V cout << KRED << "Operator" << KCYAN << " => " << KGREEN << dialogTextboxGetString("Operator") << KNORMAL << endl;
  V cout << KRED << "Origin" << KCYAN << " => ";
  V debugDumpPointStructure(Origin);
  cadUndoPushState();
  cadEntity e;
  int start = 0;
  if (!Operator) start = 1;
  if (!Operator) Steps++;
  for (int x = 0; x < cadGetEntityArrayIndex(); x++)
  {
      //cout << "X => " << x << " Index is => " << cadGetEntityArrayIndex() << endl;
      e = cadGetEntityArray(x);
      if (e.Selected && !e.Removed) //Make sure were selected and not removed
      {
        if (e.Type == CAD_LINE)
        {
          for (int i = start; i < Steps; i++)
          {
            e.Line.start = geoRotatePointAroundPoint(e.Line.start, Origin, Angle, Direction);
            e.Line.end = geoRotatePointAroundPoint(e.Line.end, Origin, Angle, Direction);
            if (Operator) //Default is move
            {
              e.Color = PURPLE;
              cadEdit(x, e, false);
            }
            else
            {
              e.Color = PURPLE;
              cadAppend(e, false);
            }
          }
        }
        if (e.Type == CAD_ARC)
        {
          for (int i = start; i < Steps; i++)
          {
            e.Arc.start = geoRotatePointAroundPoint(e.Arc.start, Origin, Angle, Direction);
            e.Arc.end = geoRotatePointAroundPoint(e.Arc.end, Origin, Angle, Direction);
            e.Arc.center = geoRotatePointAroundPoint(e.Arc.center, Origin, Angle, Direction);
            if (Operator) //Default is move
            {
              e.Color = PURPLE;
              cadEdit(x, e, false);
            }
            else
            {
              e.Color = PURPLE;
              cadAppend(e, false);
            }
          }
        }
      }
  }
  cliScreenUnSelectAll();
  dialogClose();
}
void *cliXformRotateAll()
{
  cliScreenSelectAll();
  cliXformRotateSelected();
}
void *cliXformRotateSelected()
{
  point_t snap = mouseCadLastSnapClick();
  string o;
  o.append(to_string(snap.x));
  o.append(", ");
  o.append(to_string(snap.y));

  point_t pos = point_t{-450, 320};
  dialogAddLabel(pos, "Steps?");
  pos.y -= 120;
  dialogAddTextBox(pos, 500, 100, "Steps", "1");
  pos.y -= 50;

  dialogAddLabel(pos, "Angle?");
  pos.y -= 120;
  dialogAddTextBox(pos, 500, 100, "Angle");
  pos.y -= 50;

  dialogAddLabel(pos, "Origin?");
  pos.y -= 120;
  dialogAddTextBox(pos, 500, 100, "Origin", o);
  pos.y -= 100;

  dialogAddCheckbox(pos, "Copy?", false, cliXform_CopyClicked);
  pos.y -= 120;

  dialogAddCheckbox(pos, "Move?", true, cliXform_MoveClicked);
  pos.y -= 50;

  dialogAddButton(point_t{200, -350}, 200, 100, "OK", cliXformRotate_Callback);
  dialogOpen("Xform Rotate");
}
void cliXformTranslate_Callback()
{
  bool Operator;
  if (dialogCheckboxGet("Copy?"))
  {
    Operator = false;
  }
  if (dialogCheckboxGet("Move?"))
  {
    Operator = true;
  }
  float Steps = fabs(atof(dialogTextboxGetString("Steps").c_str()));
  string FromString = dialogTextboxGetString("From");
  vector<string> FromArray = split(FromString, ',');
  if (FromArray.size() < 1)
  {
    //Implement some msg box dialog!
    //Origin error!
    return;
  }
  point_t From = point_t{(float)atof(FromArray[0].c_str()), (float)atof(FromArray[1].c_str())};
  V debugDumpPointStructure(From);
  string ToString = dialogTextboxGetString("To");
  vector<string> ToArray = split(ToString, ',');
  if (ToArray.size() < 1)
  {
    //Implement some msg box dialog!
    //Origin error!
    return;
  }
  point_t To = point_t{(float)atof(ToArray[0].c_str()), (float)atof(ToArray[1].c_str())};
  V debugDumpPointStructure(To);
  point_t TranslationVector = geoTranslateCalculateVector(From, To);
  V debugDumpPointStructure(TranslationVector);
  cadUndoPushState();
  cadEntity e;
  int start = 0;
  if (!Operator) start = 1;
  if (!Operator) Steps++;
  for (int x = 0; x < cadGetEntityArrayIndex(); x++)
  {
      //cout << "X => " << x << " Index is => " << cadGetEntityArrayIndex() << endl;
      e = cadGetEntityArray(x);
      if (e.Selected && !e.Removed) //Make sure were selected and not removed
      {
        if (e.Type == CAD_LINE)
        {
          for (int i = start; i < Steps; i++)
          {
            //e.Line.start = geoRotatePointAroundPoint(e.Line.start, Origin, Angle, Direction);
            //e.Line.end = geoRotatePointAroundPoint(e.Line.end, Origin, Angle, Direction);
            e.Line.start = geoTranslatePointByVector(e.Line.start, TranslationVector);
            e.Line.end = geoTranslatePointByVector(e.Line.end, TranslationVector);

            if (Operator) //Default is move
            {
              e.Color = PURPLE;
              cadEdit(x, e, false);
            }
            else
            {
              e.Color = PURPLE;
              cadAppend(e, false);
            }
          }
        }
        if (e.Type == CAD_ARC)
        {
          for (int i = start; i < Steps; i++)
          {
            //e.Arc.start = geoRotatePointAroundPoint(e.Arc.start, Origin, Angle, Direction);
            //e.Arc.end = geoRotatePointAroundPoint(e.Arc.end, Origin, Angle, Direction);
            //e.Arc.center = geoRotatePointAroundPoint(e.Arc.center, Origin, Angle, Direction);

            e.Arc.start = geoTranslatePointByVector(e.Arc.start, TranslationVector);
            e.Arc.end = geoTranslatePointByVector(e.Arc.end, TranslationVector);
            e.Arc.center = geoTranslatePointByVector(e.Arc.center, TranslationVector);
            if (Operator) //Default is move
            {
              e.Color = PURPLE;
              cadEdit(x, e, false);
            }
            else
            {
              e.Color = PURPLE;
              cadAppend(e, false);
            }
          }
        }
      }
  }
  cliScreenUnSelectAll();
  dialogClose();
}
void *cliXformTranslateAll()
{
  cliScreenSelectAll();
  cliXformTranslateSelected();
}
void *cliXformTranslateSelected()
{
  point_t snap = mouseCadLastSnapClick();
  string o;
  o.append(to_string(snap.x));
  o.append(", ");
  o.append(to_string(snap.y));

  point_t pos = point_t{-450, 320};
  dialogAddLabel(pos, "Steps?");
  pos.y -= 120;
  dialogAddTextBox(pos, 500, 100, "Steps", "1");
  pos.y -= 50;

  dialogAddLabel(pos, "From?");
  pos.y -= 120;
  dialogAddTextBox(pos, 500, 100, "From", o);
  pos.y -= 50;

  dialogAddLabel(pos, "To?");
  pos.y -= 120;
  dialogAddTextBox(pos, 500, 100, "To");
  pos.y -= 100;

  dialogAddCheckbox(pos, "Copy?", false, cliXform_CopyClicked);
  pos.y -= 120;

  dialogAddCheckbox(pos, "Move?", true, cliXform_MoveClicked);
  pos.y -= 50;

  dialogAddButton(point_t{200, -350}, 200, 100, "OK", cliXformTranslate_Callback);
  dialogOpen("Xform Translate");
}
void cliXformScale_Callback()
{
  cadUndoPushState();
  cadEntity e;
  float Scale = fabs(atof(luaEval(dialogTextboxGetString("Scale")).c_str()));
  //cout << "luaEval returned => " << luaEval(dialogTextboxGetString("Scale")) << endl;
  if (Scale != 0)
  {
    for (int x = 0; x < cadGetEntityArrayIndex(); x++)
    {
        e = cadGetEntityArray(x);
        if (e.Selected && !e.Removed) //Make sure were selected and not removed
        {
          e = geoScaleEntity(e, Scale);
          cadEdit(x, e, false);
        }
    }
    cliScreenUnSelectAll();
    dialogClose();
  }
}
void *cliXformScaleAll()
{
  cliScreenSelectAll();
  cliXformScaleSelected();
}
void *cliXformScaleSelected()
{
  point_t pos = point_t{-450, 320};
  dialogAddLabel(pos, "Scale Factor?");
  pos.y -= 120;
  dialogAddTextBox(pos, 500, 100, "Scale", "1");
  pos.y -= 50;

  dialogAddButton(point_t{200, -350}, 200, 100, "OK", cliXformScale_Callback);
  dialogOpen("Xform Scale");
}
void *cliDraftingDimensionPoint()
{
  if (TextReady == true)
  {
    TextReady = false;
    string input = cliGetTextInput();
    point_t snap_pos = mouseCadLastSnapClick();
    vector<string> textPosArray = split(input, ',');
    point_t text_pos = point_t{(float)atof(textPosArray[0].c_str()), (float)atof(textPosArray[1].c_str())};
    V debugDumpPointStructure(text_pos);

    dimension_t d;
    d.Type = DIMENSION_POINT;
    d.Point.snap_pos = snap_pos;
    d.Point.text_pos = text_pos;
    cadSetColor(CurrentColor);
    cadDrawDimension(d);
    mouseLiveClear();
    return NULL;
  }
  textCallback = &cliDraftingDimensionPoint;
  TextInput = true;
  mouseLiveShow("DimensionPoint");
  cliPush("> ");
  uiEdit(0, uiEntity{UI_TEXT, RED, "Draw dimension!", UI_MENU_POSITION});
  return NULL;
}
void cliXformOffsetContour_Callback()
{
  bool Side;
  if (dialogCheckboxGet("Right?"))
  {
    Side = CONTOUR_RIGHT;
  }
  if (dialogCheckboxGet("Left?"))
  {
    Side = CONTOUR_LEFT;
  }
  float Offset = fabs(atof(luaEval(dialogTextboxGetString("Offset")).c_str()));
  vector<cadEntity> o = geoOffsetContour(cadGetCurrentContour(), Side, Offset);
  cadUndoPushState();
  for (int x = 0; x < o.size(); x++)
  {
    //o[x].Color = cadGetCurrentColor();
    o[x].Color = PURPLE;
    cadAppend(o[x], false);
  }
  dialogClose();
}
void *cliXformOffsetContour()
{
  point_t pos = point_t{-450, 320};
  dialogAddLabel(pos, "Offset?");
  pos.y -= 120;
  dialogAddTextBox(pos, 500, 100, "Offset");
  pos.y -= 120;

  dialogAddCheckbox(pos, "Left?", false, cliXform_LeftClicked);
  pos.y -= 60;

  dialogAddCheckbox(pos, "Right?", true, cliXform_RightClicked);
  pos.y -= 60;

  dialogAddButton(point_t{200, -350}, 200, 100, "OK", cliXformOffsetContour_Callback);
  dialogOpen("Xform Offset Contour");
}
void *cliToolpathsViewShow()
{
  cadShowToolpaths();
}
void *cliToolpathsViewHide()
{
  cadHideToolpaths();
}
void cliToolpathsCreateContour_Callback()
{
  cadToolpath t;
  t.Cycle = CAD_CYCLE_CONTOUR;
  if (dialogCheckboxGet("Right?"))
  {
    t.Side = CONTOUR_RIGHT;
  }
  if (dialogCheckboxGet("Left?"))
  {
    t.Side = CONTOUR_LEFT;
  }
  t.ToolNumber = fabs(atof(dialogTextboxGetString("ToolNumber").c_str()));
  t.ToolDiameter = fabs(atof(luaEval(dialogTextboxGetString("ToolDiameter")).c_str()));
  t.SpindleSpeed = fabs(atof(dialogTextboxGetString("SpindleSpeed").c_str()));
  t.Coolant = dialogCheckboxGet("Coolant?");
  t.ContourCycle.feed = fabs(atof(luaEval(dialogTextboxGetString("CrossFeed")).c_str()));
  t.ContourCycle.plunge_feed = fabs(atof(luaEval(dialogTextboxGetString("PlungeFeed")).c_str()));
  t.ContourCycle.retract_feed = fabs(atof(luaEval(dialogTextboxGetString("RetractFeed")).c_str()));
  float LeaveStock = fabs(atof(luaEval(dialogTextboxGetString("LeaveStock")).c_str()));

  cout << "Offset => " << (t.ToolDiameter/2) + LeaveStock << endl;
  t.Path.Entitys = geoOffsetContour(cadGetCurrentContour(), t.Side, (t.ToolDiameter/2) + LeaveStock);
  cadAppendToolpath(t);
  cadShowToolpaths();
  dialogClose();
}
void *cliToolpathsCreateContour()
{
  if (cadGetCurrentContour().Entitys.size() > 0) //Make sure we have a chain selected
  {
    point_t pos = point_t{-500, 1200};
    dialogAddLabel(pos, "Tool Number?");
    pos.y -= 120;
    dialogAddTextBox(pos, 500, 100, "ToolNumber", "1");
    pos.y -= 120;

    dialogAddLabel(pos, "Tool Diameter?");
    pos.y -= 120;
    dialogAddTextBox(pos, 500, 100, "ToolDiameter", "1/2");
    pos.y -= 120;

    dialogAddLabel(pos, "Leave Stock?");
    pos.y -= 120;
    dialogAddTextBox(pos, 500, 100, "LeaveStock", "0.005");
    pos.y -= 120;

    dialogAddLabel(pos, "Plunge Feed?");
    pos.y -= 120;
    dialogAddTextBox(pos, 500, 100, "PlungeFeed", "1");
    pos.y -= 120;

    dialogAddLabel(pos, "Cross Feed?");
    pos.y -= 120;
    dialogAddTextBox(pos, 500, 100, "CrossFeed", "5");
    pos.y -= 120;

    dialogAddLabel(pos, "Retract Feed?");
    pos.y -= 120;
    dialogAddTextBox(pos, 500, 100, "RetractFeed", "7");
    pos.y -= 120;

    dialogAddLabel(pos, "Spindle Speed?");
    pos.y -= 120;
    dialogAddTextBox(pos, 500, 100, "SpindleSpeed", "3000");
    pos.y -= 120;

    dialogAddCheckbox(pos, "Coolant?", false, NULL);
    pos.y -= 120;

    dialogAddLabel(pos, "Contour Side?");
    pos.y -= 70;
    dialogAddCheckbox(pos, "Left?", false, cliXform_LeftClicked);
    pos.y -= 60;

    dialogAddCheckbox(pos, "Right?", true, cliXform_RightClicked);
    pos.y -= 150;


    dialogAddButton(point_t{400, -1300}, 200, 100, "OK", cliToolpathsCreateContour_Callback);
    dialogSetPosition(300, 100);
    dialogOpen("Toolpath Contour");
    dialogSetSize(400, 700);
  }
}

int backplotCycle = 0;
int backplotPosition = -1;
point_t backplotLastPos;
void cliToolpathsBackplotClose_Callback()
{
  backplotCycle = 0;
  backplotPosition = -1;
  cadHideLiveEntity();
  dialogClose();
}
void cliToolpathsBackplot_Callback()
{
  cadEntity e;
  std::vector<cadEntity> l;
  e.Type = CAD_ARC;
  e.Color = RED;
  if (backplotPosition < 0)
  {
    backplotPosition = 0;
    return;
  }
  if (backplotPosition > cadGetToolpaths()[backplotCycle].Path.Entitys.size() -1)
  {
    //backplotPosition = cadGetToolpaths()[backplotCycle].Path.Entitys.size() -1;
    //return;
    backplotPosition = 0;
  }
  cout << "Position: " << backplotPosition << endl;
  if (cadGetToolpaths()[backplotCycle].Path.Entitys[backplotPosition].Type == CAD_LINE)
  {
    point_t start = cadGetToolpaths()[backplotCycle].Path.Entitys[backplotPosition].Line.start;
    point_t end = cadGetToolpaths()[backplotCycle].Path.Entitys[backplotPosition].Line.end;
    if (geoInTolerance(backplotLastPos, start, 0.0005))
    {
      e.Arc = geoGetCircle(end, cadGetToolpaths()[backplotCycle].ToolDiameter / 2);
      backplotLastPos = end;
    }
    else
    {
      e.Arc = geoGetCircle(start, cadGetToolpaths()[backplotCycle].ToolDiameter / 2);
      backplotLastPos = start;
    }
    l.push_back(e);
  }
  else if (cadGetToolpaths()[backplotCycle].Path.Entitys[backplotPosition].Type == CAD_ARC)
  {
    point_t start = cadGetToolpaths()[backplotCycle].Path.Entitys[backplotPosition].Arc.start;
    point_t end = cadGetToolpaths()[backplotCycle].Path.Entitys[backplotPosition].Arc.end;
    if (geoInTolerance(backplotLastPos, start, 0.0005))
    {
      e.Arc = geoGetCircle(end, cadGetToolpaths()[backplotCycle].ToolDiameter / 2);
      backplotLastPos = end;
    }
    else
    {
      e.Arc = geoGetCircle(start, cadGetToolpaths()[backplotCycle].ToolDiameter / 2);
      backplotLastPos = start;
    }
    l.push_back(e);
  }
  cadShowLiveEntity(l);

}
void cliToolpathsBackplotNext_Callback()
{
  backplotPosition++;
  cliToolpathsBackplot_Callback();
}
void cliToolpathsBackplotPrevius_Callback()
{
  backplotPosition--;
  cliToolpathsBackplot_Callback();
}
void *cliToolpathsBackplot()
{
  if (cadGetToolpaths().size() > 0) //Make sure we have some toolpaths
  {
    point_t pos = point_t{-300, 50};
    dialogAddButton(pos, 200, 100, "Previus", cliToolpathsBackplotPrevius_Callback);
    pos.x += 400;
    dialogAddButton(pos, 200, 100, "Next", cliToolpathsBackplotNext_Callback);
    pos.y -= 200;
    dialogAddButton(pos, 200, 100, "Close", cliToolpathsBackplotClose_Callback);
    dialogSetPosition(800, 800);
    dialogOpen("Toolpath Backplot");
    dialogSetSize(300, 100);
  }
}
#define CLI_MENU_ITEMS 8
menu_item_t menu[CLI_MENU_ITEMS] = {
  { "f", "file",
    sub_menu_item_t{ "e", "exit",
         sub_sub_menu_item_t{ "y", "are you sure?", &cliFileExit },
     },
     sub_menu_item_t{ "g", "get",
          sub_sub_menu_item_t{ "n", "new", &cliFileGetNew },
          sub_sub_menu_item_t{ "m", "merge", &cliFileGetMerge },
      },
      sub_menu_item_t{ "p", "put",
           sub_sub_menu_item_t{ "c", "current file", &cliFilePutCurrent },
           sub_sub_menu_item_t{ "n", "new file", &cliFilePutNew },
       },
  },
  { "l", "line",
    sub_menu_item_t{ "v", "vertical",
         sub_sub_menu_item_t{ "o", "origin", &cliCreateLineVerticalOrigin },
         sub_sub_menu_item_t{ "e", "endpoint", &cliCreateLineVerticalEndpoint },
     },
     sub_menu_item_t{ "h", "horizontal",
         sub_sub_menu_item_t{ "o", "origin", &cliCreateLineHorizontalOrigin },
         sub_sub_menu_item_t{ "e", "endpoint", &cliCreateLineHorizontalEndpoint },
    },
    sub_menu_item_t{ "e", "endpoints",
         sub_sub_menu_item_t{ "d", "done", &cliCreateLineEndpoints },
    },
    sub_menu_item_t{ "d", "perpendicular",
         sub_sub_menu_item_t{ "d", "distance", &cliCreateLinePerpendicular },
    },
    sub_menu_item_t{ "p", "parallel",
         sub_sub_menu_item_t{ "s", "side distance", &cliCreateLineParallel },
    },
  },
  { "a", "arcs",
    sub_menu_item_t{ "c", "circle",
         sub_sub_menu_item_t{ "d", "center & diameter", &cliCreateCircleCenterDiameter },
         sub_sub_menu_item_t{ "r", "center & radius", &cliCreateCircleCenterRadius },
         sub_sub_menu_item_t{ "t", "two points", &cliCreateCircleTwoPoints },
     },
     sub_menu_item_t{ "a", "arc",
          sub_sub_menu_item_t{ "d", "center & diameter"},
          sub_sub_menu_item_t{ "r", "center & radius" },
          sub_sub_menu_item_t{ "t", "two points" },
      },
  },
  { "x", "xform",
      sub_menu_item_t{ "t", "trim",
        sub_sub_menu_item_t{ "1", "1 entity", &cliXformTrim1 },
        sub_sub_menu_item_t{ "2", "2 entitys", &cliXformTrim2 },
        sub_sub_menu_item_t{ "c", "circle two endpoints", &cliXformTrimCircle },
      },
      sub_menu_item_t{ "m", "mirror",
          sub_sub_menu_item_t{ "l", "across line" },
      },
      sub_menu_item_t{ "r", "rotate",
        sub_sub_menu_item_t{ "a", "all entitys", &cliXformRotateAll },
        sub_sub_menu_item_t{ "s", "selected entitys", &cliXformRotateSelected },
      },
      sub_menu_item_t{ "l", "translate",
        sub_sub_menu_item_t{ "a", "all entitys",  &cliXformTranslateAll },
        sub_sub_menu_item_t{ "s", "selected entitys", &cliXformTranslateSelected },
      },
      sub_menu_item_t{ "s", "scale",
        sub_sub_menu_item_t{ "a", "all entitys",  &cliXformScaleAll },
        sub_sub_menu_item_t{ "s", "selected entitys", &cliXformScaleSelected },
      },
      sub_menu_item_t{ "o", "offset",
      sub_sub_menu_item_t{ "c", "contour", &cliXformOffsetContour },
      },
      sub_menu_item_t{ "f", "fillet",
          sub_sub_menu_item_t{ "r", "radius" , &cliXformFilletRadius },
          sub_sub_menu_item_t{ "d", "diameter", &cliXformFilletDiameter },
      },
      sub_menu_item_t{ "c", "chamfer",
          sub_sub_menu_item_t{ "d", "distances" },
          sub_sub_menu_item_t{ "l", "length" },
      },
  },
  { "v", "view",
      sub_menu_item_t{ "p", "plane",
        sub_sub_menu_item_t{ "x", "xy", &cliViewPlaneXY },
        sub_sub_menu_item_t{ "y", "yz", &cliViewPlaneYZ },
        sub_sub_menu_item_t{ "z", "zx", &cliViewPlaneZX },
        sub_sub_menu_item_t{ "o", "ortho", &cliViewPlaneOrtho }
      },
  },
  { "s", "screen",
      sub_menu_item_t{ "s", "selection",
        sub_sub_menu_item_t{ "a", "select all", &cliScreenSelectAll },
        sub_sub_menu_item_t{ "u", "un-select all", &cliScreenUnSelectAll },
        sub_sub_menu_item_t{ "d", "delete selected", &cliScreenDeleteSelected },
      },
      sub_menu_item_t{ "a", "analyze",
        sub_sub_menu_item_t{ "e", "entity", &cliScreenAnalyzeEntity },
      },
      sub_menu_item_t{ "c", "color",
        sub_sub_menu_item_t{ "w", "white", &cliScreenColorWhite },
        sub_sub_menu_item_t{ "b", "black", &cliScreenColorBlack },
        sub_sub_menu_item_t{ "r", "red", &cliScreenColorRed },
        sub_sub_menu_item_t{ "g", "green", &cliScreenColorGreen },
        sub_sub_menu_item_t{ "l", "blue", &cliScreenColorBlue },
        sub_sub_menu_item_t{ "y", "yellow", &cliScreenColorYellow },
        sub_sub_menu_item_t{ "c", "cyan", &cliScreenColorCyan },
        sub_sub_menu_item_t{ "m", "magenta", &cliScreenColorMagenta },
        sub_sub_menu_item_t{ "e", "grey", &cliScreenColorGrey },
      },
  },
  { "d", "drafting",
      sub_menu_item_t{ "d", "dimension",
        sub_sub_menu_item_t{ "p", "point", &cliDraftingDimensionPoint },
      },
  },
  { "t", "toolpaths",
      sub_menu_item_t{ "v", "view",
        sub_sub_menu_item_t{ "s", "show", &cliToolpathsViewShow },
        sub_sub_menu_item_t{ "h", "hide", &cliToolpathsViewHide },
        sub_sub_menu_item_t{ "b", "backplot", &cliToolpathsBackplot },
      },
      sub_menu_item_t{ "c", "create",
        sub_sub_menu_item_t{ "c", "contour", &cliToolpathsCreateContour },
      },
  },
};

void cliPush(std::string c)
{
  if (TextInput)
  {
    if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
    {
        text.append(string(1, toupper(c[0])));
    }
    else
    {
        text.append(c);
    }

    uiEdit(1, uiEntity{UI_TEXT, UI_MENU_COLOR, text, UI_INPUT_POSITION});
  }
  else
  {
    if (Level == 2)
    {
      for (int x=0; x < 10; x++)
      {
        if (menu[Level1Selection].submenu[Level2Selection].submenu[x].c == c)
        {
          if (menu[Level1Selection].submenu[Level2Selection].submenu[x].cmd == NULL) break;
          (*menu[Level1Selection].submenu[Level2Selection].submenu[x].cmd)();
        }
      }
    }
    if (Level == 1)
    {
      for (int x=0; x < 10; x++)
      {
        if (menu[Level1Selection].submenu[x].c == c)
        {
          Level = 2;
          Level2Selection = x;
        }
      }
    }
    if (Level == 0)
    {
      for (int x=0; x < CLI_MENU_ITEMS; x++)
      {
        if (menu[x].c == c)
        {
          Level = 1;
          Level1Selection = x;
        }
      }
    }
    cliMenu();
  }
}
void cliBackup()
{
  if (TextInput)
  {
    if (text.size() > 2)
    {
      text.pop_back();
    }
    else
    {
      TextInput = false;
      text = "";
      mouseLiveClear();
      cliMenu();
    }
    uiEdit(1, uiEntity{UI_TEXT, UI_MENU_COLOR, text, UI_INPUT_POSITION});
  }
  else
  {
    if (Level > 0) Level--;
    cliMenu();
  }

}
void cliReturn()
{
  if (TextInput == true)
  {
    TextInput = false;
    TextReady = true;
    uiEdit(1, uiEntity{UI_TEXT, UI_MENU_COLOR, "", UI_INPUT_POSITION});
    if ((*textCallback)() != NULL) (*textCallback)();
    text = "";
    cliMenu();
  }
}
void cliMenu()
{
  if (TextInput)
  {
    return;
  }
  string text = "";
  if (Level == 0)
  {
    for (int x=0; x < CLI_MENU_ITEMS; x++)
    {
      if (x != 0)
      {
        text.append(CLI_MENU_ITEM_SEPERATOR);
      }
      text.append(menu[x].c);
      text.append(CLI_MENU_ITEM_LINK);
      text.append(menu[x].msg);
    }
  }
  if (Level == 1)
  {
    for (int x=0; x < 10; x++)
    {
      if (menu[Level1Selection].submenu[x].c == "") break;

      if (x != 0)
      {
        text.append(CLI_MENU_ITEM_SEPERATOR);
      }
      text.append(menu[Level1Selection].submenu[x].c);
      text.append(CLI_MENU_ITEM_LINK);
      text.append(menu[Level1Selection].submenu[x].msg);

    }
  }
  if (Level == 2)
  {
    for (int x=0; x < 10; x++)
    {
      if (menu[Level1Selection].submenu[Level2Selection].submenu[x].c == "") break;

      if (x != 0)
      {
        text.append(CLI_MENU_ITEM_SEPERATOR);
      }
      text.append(menu[Level1Selection].submenu[Level2Selection].submenu[x].c);
      text.append(CLI_MENU_ITEM_LINK);
      text.append(menu[Level1Selection].submenu[Level2Selection].submenu[x].msg);

    }
  }
  uiEdit(0, uiEntity{UI_TEXT, UI_MENU_COLOR, text, UI_MENU_POSITION});
  uiEdit(3, uiEntity{UI_TEXT, RED, "", UI_HINT_POSITION});
}
void cliInit()
{
  args_t args = mainGetArgs();
  if (args.args.find("-fullscreen") != std::string::npos)
  {
    glutFullScreen();
  }
  if (args.args.find("-pod") != std::string::npos)
  {
    std::cout << std::boolalpha;
    std::cout << "struct cadEntity => " << std::is_pod<cadEntity>::value << '\n';
    std::cout << "struct point_t => " << std::is_pod<point_t>::value << '\n';
    std::cout << "struct line_t => " << std::is_pod<line_t>::value << '\n';
    std::cout << "struct arc_t => " << std::is_pod<arc_t>::value << '\n';
    std::cout << "struct note_t => " << std::is_pod<note_t>::value << '\n';
    std::cout << "struct dimension_t => " << std::is_pod<dimension_t>::value << '\n';
    exit(0);
  }
  if (args.args.find("-tinyg") != std::string::npos)
  {
    vector<string> array = split(args.args, ' ');
    for (int x = 0; x < array.size(); x++)
    {
      if (array[x].find("-tinyg") != std::string::npos)
      {
         machineTinyGCommand(array[x+1]);
      }
    }
  }
  if (args.args.find("-file") != std::string::npos)
  {
    vector<string> array = split(args.args, ' ');
    for (int x = 0; x < array.size(); x++)
    {
      if (array[x].find("-file") != std::string::npos)
      {
        CurrentFile = array[x+1];
        fileOpen(CurrentFile);
        cout << KGREEN << "-> Opening " << CurrentFile << KNORMAL << endl;
      }
    }
  }
  if (args.args.find("-run") != std::string::npos)
  {
    vector<string> array = split(args.args, ' ');
    for (int x = 0; x < array.size(); x++)
    {
      if (array[x].find("-run") != std::string::npos)
      {
        cout << KGREEN << "-> Runing " << array[x+1] << KNORMAL << endl;
        luaExec(array[x+1]);
      }
    }
  }

}
