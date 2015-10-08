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
void machineUI_Run_Callback()
{
  string line;
  ifstream ifs;
  ifs.open(dialogTextboxGetString("NCFile"));
  if (ifs.is_open())
  {
    while (getline(ifs, line))
    {
      machineWrite(line);
    }
  }
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

  dialogAddButton(point_t{-600, 0}, 200, 100, "HOME", machineUI_Home_Callback);
  dialogAddButton(point_t{-400, 0}, 200, 100, "SETXYZ", machineUI_SetXYZ_Callback);
  dialogAddButton(point_t{0, 0}, 200, 100, "RUN", machineUI_Run_Callback);
  dialogAddButton(point_t{400, 0}, 200, 100, "LOADING", machineUI_Loading_Callback);
  dialogAddButton(point_t{400, -200}, 200, 100, "PAUSE", machineUI_Pause_Callback);

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
    write (mfd, w.c_str(), w.length());
    write (mfd, "\n", 1);
    machineRead();
    return;
  }
}
string X;
string Y;
string Z;
string machineRead()
{
  string buffer;
  char c;
  while (true)
  {
      read(mfd, &c, sizeof(char));
      if (c == '\n' || c == '\r')
      {
        //cout << "Little Fucker!" << endl;
      }
      else
      {
        buffer.push_back(c);
        printf("%c", c);
      }
      if (c == '>')
      {
        break;
      }

  }
  //buffer.erase(std::remove(buffer.begin(), buffer.end(), ' '), buffer.end()); //Remove all whitespaces
  //vector<string> pre = split(buffer, '>');

  //vector<string> pairs = split(pre[2], ',');
  /*if (pairs.size() < 1)
  {
    pairs = split(buffer, '\n');
    if (pairs.size() < 1)
    {
      cout << "No Feedback information!" << endl;
    }
  }*/
  /*cout << endl;
  vector<string> data;
  for (int x = 0; x < pairs.size(); x++)
  {
    data = split(pairs[x], ':');
    cout << "Pair[" << x << "] Data[0] = " << data[0] << " Data[1] = " << data[1] << endl;
    if (data[0] == "mpox")
    {
      X = data[1];
    }
    if (data[0] == "mpoy")
    {
      Y = data[1];
    }
    if (data[0] == "mpoz")
    {
      Z = data[1];
    }
    dialogEditLabel(point_t{500, 500}, "X: " + X);
    dialogEditLabel(point_t{500, 600}, "Y: " + Y);
    dialogEditLabel(point_t{500, 700}, "Z: " + Z);
  }*/
  return buffer;
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
