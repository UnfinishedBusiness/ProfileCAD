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

void *cliCreateLineVerticalOrigin()
{
    if (TextReady == true)
    {
      TextReady = false;
      text.replace(text.find("> "), sizeof("> ")-1, "");
      point_t Start = { 0, 0};
      point_t End = { 0 , (float)atof(text.c_str()) };
      cadSetColor(CurrentColor);
      cadDrawLine(Start, End);
      return NULL;
    }
    textCallback = &cliCreateLineVerticalOrigin;
    TextInput = true;
    cliPush("> ");
    uiEdit(0, uiEntity{UI_TEXT, RED, "Input Y coordinant!", UI_MENU_POSITION});
}
void *cliCreateLineHorizontalOrigin()
{
  if (TextReady == true)
  {
    TextReady = false;
    text.replace(text.find("> "), sizeof("> ")-1, "");
    point_t Start = { 0, 0};
    point_t End = {(float)atof(text.c_str()), 0};
    cadSetColor(CurrentColor);
    cadDrawLine(Start, End);
    return NULL;
  }
  textCallback = &cliCreateLineHorizontalOrigin;
  TextInput = true;
  cliPush("> ");
  uiEdit(0, uiEntity{UI_TEXT, RED, "Input X coordinant!", UI_MENU_POSITION});
}
#define CLI_MENU_ITEMS 2
menu_item_t menu[CLI_MENU_ITEMS] = {
  { "l", "line",
      sub_menu_item_t{ "v", "verticle",
          sub_sub_menu_item_t{ "o", "origin", &cliCreateLineVerticalOrigin },
          sub_sub_menu_item_t{ "e", "endpoints" }
     },
     sub_menu_item_t{ "h", "horizontal",
         sub_sub_menu_item_t{ "o", "origin", &cliCreateLineHorizontalOrigin },
         sub_sub_menu_item_t{ "e", "endpoints" }
    },
     sub_menu_item_t{ "p", "parallel"},
  },
  { "x", "xform",
      sub_menu_item_t{ "t", "trim" },
      sub_menu_item_t{ "m", "mirror" }
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
        if (menu[Level1Selection].submenu[Level2Selection].submenu[x].cmd == NULL) break;
        (*menu[Level1Selection].submenu[Level2Selection].submenu[x].cmd)();
      }
    }
    if (Level == 1)
    {
      for (int x=0; x < CLI_MENU_ITEMS; x++)
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
  TextInput = false;
  TextReady = true;
  uiEdit(1, uiEntity{UI_TEXT, GREEN, "", UI_INPUT_POSITION});
  (*textCallback)();
  text = "";
  cliMenu();

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
