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
    return;
  }
}
string machineRead()
{
  /*string line;
  if (mifs.is_open())
  {
      getline (mifs, line);
      line.erase(std::remove(line.begin(), line.end(), ' '), line.end()); //Remove all whitespaces
      line.erase(std::remove(line.begin(), line.end(), '\n'), line.end()); //Remove all newline characters
      line.erase(std::remove(line.begin(), line.end(), '\r'), line.end()); //Remove all carage return characters
      V cout << KRED << "Machine Controller" << KGREEN << " Read -> " << line << KNORMAL << endl;
      return line;
  }*/
}
void machineTinyGCommand(string c)
{
  if (c.find("home") != std::string::npos)
  {
    machineConnect();
    machineWrite("G28.2 X0Y0");
    machineDisconnect();
  }
  if (c.find("loading") != std::string::npos)
  {
    machineConnect();
    machineWrite("G0 X2.5Y5");
    machineDisconnect();
  }
  return;
}
