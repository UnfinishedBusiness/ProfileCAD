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
int Level = 0;
int Level1Selection;
int Level2Selection;
bool TextInput = false;
bool TextReady = false;
string text = "";

#define CLI_MENU_ITEM_SEPERATOR "   "
#define CLI_MENU_ITEM_LINK  " - "

float cliGetInput()
{
  text.replace(text.find("> "), sizeof("> ")-1, "");
  float input = (float)atof(text.c_str());
  if (!isnan(input))
  {
    return input;
  }
  else
  {
    return 0;
  }
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
      return NULL;
    }
    textCallback = &cliCreateLineVerticalOrigin;
    TextInput = true;
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
      //D printf("(cliCreateLinePerpendicular) %d Entitys selected!\n", e.size());
      line_t p = geoGetParallelLine(line_t{ e[0].Line.start,  e[0].Line.end}, mouseCadLastClickPos(), input);
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
  return NULL;
}
void *cliScreenSelectAll()
{
  int m = cadGetEntityArrayIndex();
  cadEntity e;
  for (int a = 0; a < m; a++)
  {
    e = cadGetEntityArray(a);
    e.Selected = true;
    cadEdit(a, e);
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
    cadEdit(a, e);
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

void *cliScreenColorRed()
{
  CurrentColor = RED;
  cadEntity e;
  for (int x=0; x < cadGetEntityArrayIndex(); x++)
  {
    e = cadGetEntityArray(x);
    if (e.Selected && !e.Removed)
    {
      e.Color = CurrentColor;
      cadEdit(x, e);
    }
  }
  cliScreenUnSelectAll();
  return NULL;
}
void *cliScreenColorBlue()
{
  CurrentColor = BLUE;
  cadEntity e;
  for (int x=0; x < cadGetEntityArrayIndex(); x++)
  {
    e = cadGetEntityArray(x);
    if (e.Selected && !e.Removed)
    {
      e.Color = CurrentColor;
      cadEdit(x, e);
    }
  }
  cliScreenUnSelectAll();
  return NULL;
}
void *cliScreenColorGreen()
{
  CurrentColor = GREEN;
  cadEntity e;
  for (int x=0; x < cadGetEntityArrayIndex(); x++)
  {
    e = cadGetEntityArray(x);
    if (e.Selected && !e.Removed)
    {
      e.Color = CurrentColor;
      cadEdit(x, e);
    }
  }
  cliScreenUnSelectAll();
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
  if (e[0].SelectionIndex < e[1].SelectionIndex) //Get line that was clicked first
  {
    index = 0;
  }
  else
  {
    index = 1;
  }
  point_t new_endpoint = geoGetLineIntersection(line_t{e[0].Line.start, e[0].Line.end}, line_t{e[1].Line.start, e[1].Line.end});
  //D printf("(cliXformTrim1) Intersection point is (%.6f, %.6f)\n", new_endpoint.x, new_endpoint.y);

  //Find which end of first selected line is closest to the Intersection point, then replace with intersection point
  float start_d = geoGetLineLength(line_t{ e[index].Line.start, new_endpoint });
  float end_d = geoGetLineLength(line_t{ e[index].Line.end, new_endpoint });
  //Doesnt work because were looking at first in array not first clicked
  //D printf("e[0].SelectionIndex = %d\ne[1].SelectionIndex = %d\n", e[0].SelectionIndex, e[1].SelectionIndex);
  if (start_d < end_d)
  {
      e[index].Line.start = new_endpoint;
  }
  else
  {
    e[index].Line.end = new_endpoint;
  }
  cadEdit(e[index].Index, e[index]);
  cliScreenUnSelectAll();
  return NULL;
}

#define CLI_MENU_ITEMS 5
menu_item_t menu[CLI_MENU_ITEMS] = {
  { "f", "file",
    sub_menu_item_t{ "e", "exit",
         sub_sub_menu_item_t{ "y", "are you sure?", &cliFileExit },
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
  { "x", "xform",
      sub_menu_item_t{ "t", "trim",
        sub_sub_menu_item_t{ "1", "1 entity", &cliXformTrim1 },
        sub_sub_menu_item_t{ "2", "2 entitys" },
      },
      sub_menu_item_t{ "m", "mirror" },
      sub_menu_item_t{ "r", "rotate" },
      sub_menu_item_t{ "o", "offset" },
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
      sub_menu_item_t{ "c", "color",
        sub_sub_menu_item_t{ "r", "red", &cliScreenColorRed },
        sub_sub_menu_item_t{ "b", "blue", &cliScreenColorBlue },
        sub_sub_menu_item_t{ "g", "green", &cliScreenColorGreen },
      },
  },
};

void cliPush(std::string c)
{
  if (TextInput)
  {
    text.append(c);
    uiEdit(1, uiEntity{UI_TEXT, GREEN, text, UI_INPUT_POSITION});
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
      cliMenu();
    }
    uiEdit(1, uiEntity{UI_TEXT, GREEN, text, UI_INPUT_POSITION});
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
    uiEdit(1, uiEntity{UI_TEXT, GREEN, "", UI_INPUT_POSITION});
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
  uiEdit(0, uiEntity{UI_TEXT, GREEN, text, UI_MENU_POSITION});
}
/*void cliParse(string CmdInput)
{
  if (CmdInput.find("plane") != std::string::npos)
  {
    std::size_t posSpace = CmdInput.find(" ");
    std::string plane = CmdInput.substr(posSpace+1, (CmdInput.length() - posSpace)-1);
    if (plane == "xy")
    {
      sceneSetViewAngle(0, 0, 0);
    }
    if (plane == "xz")
    {
      sceneSetViewAngle(90, 0, 0);
    }
    if (plane == "yz")
    {
      sceneSetViewAngle(0, 90, 0);
    }
  }
  else if (CmdInput.find("color") != std::string::npos)
  {
    std::size_t posSpace = CmdInput.find(" ");
    std::string color = CmdInput.substr(posSpace+1, (CmdInput.length() - posSpace)-1);
    if (color == "green")
    {
      CurrentColor = GREEN;
      uiEdit(1, uiEntity{UI_TEXT, CurrentColor, "> GREEN <", UI_MSG_POSITION});
    }
    if (color == "red")
    {
      CurrentColor = RED;
      uiEdit(1, uiEntity{UI_TEXT, CurrentColor, "> RED <", UI_MSG_POSITION});
    }
    if (color == "blue")
    {
      CurrentColor = BLUE;
      uiEdit(1, uiEntity{UI_TEXT, CurrentColor, "> BLUE <", UI_MSG_POSITION});
    }
  }
  else if (CmdInput.find("line") != std::string::npos)
  {
    if (CmdInput == "line") //no arguments supplied
    {
      uiEdit(1, uiEntity{UI_TEXT, CurrentColor, "? (Endpoints) line x0y0z0x1y1z1 | (Selected line) line distance (+ right, - left) ?", UI_MSG_POSITION});
      return;
    }
    cadSetColor(CurrentColor);
    std::vector<cadEntity> e = cadGetSelected();
    if (e.size() == 0) //No entitys are selected
    {

      //point_t Start = {(float)atof((char*)X1s.c_str()), (float)atof((char*)Y1s.c_str())};
      //point_t End = {(float)atof((char*)X2s.c_str()), (float)atof((char*)Y2s.c_str())};
      //cadDrawLine(Start, End);
      //uiEdit(1, uiEntity{UI_TEXT, CurrentColor, "> LINE ENDPOINTS <", UI_MSG_POSITION});
    }
  }
  else if (CmdInput == "test")
  {
    cadSetColor(GREEN);
    cadDrawLine(point_t{0, 0, 0}, point_t{2,0,0});
    cadDrawLine(point_t{0, 0, 0}, point_t{0,2,0});
    cadDrawLine(point_t{0, 0, 0}, point_t{0,0,2});
    cadRedraw();
  }
  else if (CmdInput == "rm")
  {
    uiEdit(1, uiEntity{UI_TEXT, RED, "> REMOVING SELECTED ENTITYS!", UI_MSG_POSITION});
    cadRemoveSelected();
  }
  else if (CmdInput == "exit")
  {
    uiEdit(1, uiEntity{UI_TEXT, RED, "Bye!", UI_MSG_POSITION});
    EXIT;
  }
  else
  {
    uiEdit(1, uiEntity{UI_TEXT, RED, "! NO SUCH COMMAND !", UI_MSG_POSITION});
  }
}*/
