#include <Machine.h>


using namespace std;

string machineCOM;
int mfd;

void machineInit()
{
  args_t args = mainGetArgs();
  if (args.args.find("-com") != std::string::npos)
  {
    vector<string> array = split(args.args, ' ');
    for (int x = 0; x < array.size(); x++)
    {
      if (array[x].find("-com") != std::string::npos)
      {
        machineCOM = array[x+1];
        V cout << KRED << "Machine Controller " << KGREEN << " -> Using " << machineCOM << KNORMAL << endl;
        machineConnect();
      }
    }
  }
}
int runLine = 0;
bool runningFile = false;
void machineUI_Run_Callback()
{
  runningFile = true;
  int x = 0;
  string line;
  ifstream ifs;
  ifs.open(dialogTextboxGetString("NCFile"));
  if (ifs.is_open())
  {
    while (getline(ifs, line))
    {
      if (runLine == x)
      {
          machineWrite(line);
          ifs.close();
          return;
      }
      x++;
    }
  }
  runLine = 0;
  runningFile = false;
  ifs.close();
}
void machineUI_Home_Callback()
{
  luaInit(dialogTextboxGetString("ControlFile"));
  machineWrite(luaCallFunction("Home"));
  luaClose();
}
void machineUI_Loading_Callback()
{
  luaInit(dialogTextboxGetString("ControlFile"));
  machineWrite(luaCallFunction("Loading"));
  luaClose();
}
void machineUI_XPlus_Callback()
{
  luaInit(dialogTextboxGetString("ControlFile"));
  machineWrite(luaCallFunction("Xinc " + geoSupressZeros(0.1)));
  luaClose();
}
void machineUI_XMinus_Callback()
{
  luaInit(dialogTextboxGetString("ControlFile"));
  machineWrite(luaCallFunction("Xinc " + geoSupressZeros(-0.1)));
  luaClose();
}
void machineUI_YPlus_Callback()
{
  luaInit(dialogTextboxGetString("ControlFile"));
  machineWrite(luaCallFunction("Yinc " + geoSupressZeros(0.1)));
  luaClose();
}
void machineUI_YMinus_Callback()
{
  luaInit(dialogTextboxGetString("ControlFile"));
  machineWrite(luaCallFunction("Yinc " + geoSupressZeros(-0.1)));
  luaClose();
}
void machineUI_SetXYZ_Callback()
{
  luaInit(dialogTextboxGetString("ControlFile"));
  machineWrite(luaCallFunction("SetXYZ"));
  luaClose();
}
void machineUI_Pause_Callback()
{
  luaInit(dialogTextboxGetString("ControlFile"));
  machineWrite(luaCallFunction("Pause"));
  luaClose();
}
void machineUI_Clear_Callback()
{
  luaInit(dialogTextboxGetString("ControlFile"));
  machineWrite(luaCallFunction("Clear"));
  luaClose();
}
void machineUI()
{
  point_t pos = point_t{-500, 1200};
  dialogAddLabel(pos, "NC File?");
  pos.y -= 120;
  dialogAddTextBox(pos, 500, 100, "NCFile", "out.nc");
  pos.y -= 150;
  dialogAddLabel(pos, "Control File?");
  pos.y -= 120;
  dialogAddTextBox(pos, 500, 100, "ControlFile", "control.lua");

  dialogAddLabel(point_t{500, 500}, "X: 0");
  dialogAddLabel(point_t{500, 600}, "Y: 0");
  dialogAddLabel(point_t{500, 700}, "Z: 0");
  dialogAddLabel(point_t{100, 800}, "");

  dialogAddButton(point_t{-600, 0}, 200, 100, "HOME", machineUI_Home_Callback);
  dialogAddButton(point_t{-400, 0}, 200, 100, "SETXYZ", machineUI_SetXYZ_Callback);
  dialogAddButton(point_t{0, 0}, 200, 100, "RUN", machineUI_Run_Callback);
  dialogAddButton(point_t{400, 0}, 200, 100, "LOADING", machineUI_Loading_Callback);
  dialogAddButton(point_t{400, -200}, 200, 100, "PAUSE", machineUI_Pause_Callback);
  dialogAddButton(point_t{400, -600}, 200, 100, "CLEAR", machineUI_Clear_Callback);

  dialogAddButton(point_t{-400, 300}, 200, 100, "X+", machineUI_XPlus_Callback);
  dialogAddButton(point_t{0, 300}, 200, 100, "X-", machineUI_XMinus_Callback);
  dialogAddButton(point_t{0, -400}, 200, 100, "Y+", machineUI_YPlus_Callback);
  dialogAddButton(point_t{0, 400}, 200, 100, "Y-", machineUI_YMinus_Callback);

  dialogSetPosition(300, 100);
  dialogOpen("Machine Interface");
  dialogSetSize(600, 700);
}
void machineConnect()
{
  /*mifs.open(machineCOM);
  mofs.open(machineCOM);
  if (!mifs.is_open() || !mofs.is_open())
  {
    V cout << KGREEN << "Machine Controller " << KRED << " -> Cant open!" << KNORMAL << endl;
    return;
  }*/
  mfd = open (machineCOM.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
  if (mfd < 0)
  {
      V cout << KGREEN << "Machine Controller " << KRED << " -> Cant open!" << KNORMAL << endl;
      return;
  }
  set_interface_attribs (mfd, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
  set_blocking (mfd, 0);                // set no blocking
  // send 7 character greeting

  //usleep ((7 + 25) * 100);             // sleep enough to transmit the 7 plus
                                      // receive 25:  approx 100 uS per char transmit
  /*char buf [100];
  int n = read (fd, buf, sizeof buf); */
}
void machineDisconnect()
{
  machineStopParse();
  close(mfd);
}
void machineWrite(string w)
{
  if (mfd < 0)
  {
      V cout << KRED << "Machine Controller" << KGREEN << " --> Comunication error! -- " << w << KNORMAL << endl;
  }
  else
  {
    V cout << KRED << "Machine Controller" << KGREEN << " Sent -> " << w << KNORMAL << endl;
    //machineRead();
    dialogEditLabel(point_t{100, 800}, w);
    write (mfd, w.c_str(), w.length());
    write (mfd, "\n", 1);
    machineParse();
    return;
  }
}
std::string machineRead()
{
  char c;
  while (read(mfd, &c, sizeof(char)) > 0)
  {
    printf("%c", c);
    fflush(stdout);
  }
  return "";
}
std::string machineReadLine()
{
  string buffer;
  char c;
  int x = 0;
  while (read(mfd, &c, sizeof(char)) > 0)
  {
    buffer.push_back(c);
    //printf("%c", c);
    //fflush(stdout);
    if (c == '\n')
    {
      //cout << "Found newline!" << endl;
      return buffer;
    }
    x++;
  }
  if (x == 0)
  {
    //cout << "Zero bytes!" << endl;
    return "NULL";
  }
  return "";
}
void machineIdleParse()
{
  auto Parse = [](string buffer)
  {
    vector<string> data = split(buffer, ':');
    //cout << "Data[0] = " << data[0] << " Data[1] = " << data[1] << endl;
    if (data[0] == "mpox")
    {
      //X = data[1];
      dialogEditLabel(point_t{500, 500}, "X: " + geoSupressZeros(atof(data[1].c_str()) / 25.4));
    }
    if (data[0] == "mpoy")
    {
      dialogEditLabel(point_t{500, 600}, "Y: " + geoSupressZeros(atof(data[1].c_str()) / 25.4));
    }
    if (data[0] == "mpoz")
    {
      dialogEditLabel(point_t{500, 700}, "Z: " + geoSupressZeros(atof(data[1].c_str()) / 25.4));
    }
    if (data[0] == "cycs" && data[1] == "0")
    {
      //cout << "End of cycle!" << endl;
      if (runningFile)
      {
        runLine++;
        machineUI_Run_Callback();
      }
      else
      {
          machineStopParse();
      }
      return true;
    }
    return false;
  };
  string buffer;

  buffer = machineReadLine();
  buffer.erase(std::remove(buffer.begin(), buffer.end(), ' '), buffer.end()); //Remove all whitespaces
  buffer.erase(std::remove(buffer.begin(), buffer.end(), '\n'), buffer.end()); //Remove all new lines
  buffer.erase(std::remove(buffer.begin(), buffer.end(), '\r'), buffer.end()); //Remove all carrage returns
  if (buffer == "NULL")
  {
    //cout << "Nothing to parse!" << endl;
    if (runningFile)
    {
      runLine++;
      machineUI_Run_Callback();
    }
    else
    {
        machineStopParse();
    }
    return;
  }
  if (buffer.find(",") != std::string::npos && buffer.find(":") != std::string::npos)
  {
    vector<string> pairs = split(buffer, ',');
    for (int x = 0; x < pairs.size(); x++)
    {
      if (Parse(pairs[x])) return;
    }
  }
  if (buffer.find(":") != std::string::npos)
  {
    if (Parse(buffer)) return;
  }
  return;
}
void machineParse()
{
  glutIdleFunc(machineIdleParse);
}
void machineStopParse()
{
  glutIdleFunc(NULL);
}
void machineParseBlocking()
{
  auto Parse = [](string buffer)
  {
    vector<string> data = split(buffer, ':');
    cout << "Data[0] = " << data[0] << " Data[1] = " << data[1] << endl;
    if (data[0] == "mpox")
    {
      //X = data[1];
      dialogEditLabel(point_t{500, 500}, "X: " + geoSupressZeros(atof(data[1].c_str()) / 25.4));
    }
    if (data[0] == "mpoy")
    {
      dialogEditLabel(point_t{500, 600}, "Y: " + geoSupressZeros(atof(data[1].c_str()) / 25.4));
    }
    if (data[0] == "mpoz")
    {
      dialogEditLabel(point_t{500, 700}, "Z: " + geoSupressZeros(atof(data[1].c_str()) / 25.4));
    }
    if (data[0] == "cycs" && data[1] == "0")
    {
      cout << "End of cycle!" << endl;
      return true;
    }
    return false;
  };
  string buffer;
  while(true)
  {
    buffer = machineReadLine();
    buffer.erase(std::remove(buffer.begin(), buffer.end(), ' '), buffer.end()); //Remove all whitespaces
    buffer.erase(std::remove(buffer.begin(), buffer.end(), '\n'), buffer.end()); //Remove all new lines
    buffer.erase(std::remove(buffer.begin(), buffer.end(), '\r'), buffer.end()); //Remove all carrage returns
    if (buffer == "NULL")
    {
      cout << "Nothing to parse!" << endl;
      return;
    }
    if (buffer.find(",") != std::string::npos && buffer.find(":") != std::string::npos)
    {
      vector<string> pairs = split(buffer, ',');
      for (int x = 0; x < pairs.size(); x++)
      {
        if (Parse(pairs[x])) return;
      }
    }
    if (buffer.find(":") != std::string::npos)
    {
      if (Parse(buffer)) return;
    }
  }
  return;
}
void machineTinyGCommand(string c)
{
  if (c.find("home") != std::string::npos)
  {
    machineWrite("G28.2 X0Y0");
  }
  if (c.find("loading") != std::string::npos)
  {
    machineWrite("G0 X2.5Y5");
  }
  if (c.find("setxyz") != std::string::npos)
  {
    machineWrite("G10 L2 P1 X0 Y0 Z0");
  }
  if (c.find("z+") != std::string::npos)
  {
    machineWrite("G91G0Z0.1G90");
  }
  if (c.find("z-") != std::string::npos)
  {
    machineWrite("G91G0Z0.1G90");
  }
  return;
}
