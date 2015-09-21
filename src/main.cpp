#include <application.h>

using namespace std;

bool isVerbose = false;
bool mainVerbose()
{
  return isVerbose;
}
args_t args;
args_t mainGetArgs()
{
  return args;
}
int main(int argc, char **argv)
{
  string a = "";
  for (int x = 1; x < argc; x++)
  {
    a.append(string(argv[x]));
    a.append(" ");
  }
  args.argc = argc;
  args.args = a;
  if (args.args.find("-verbose") != std::string::npos)
  {
    isVerbose = true;
  }
  glutInit(&argc, argv);
  windowInit();
  sceneInit();
  menuInit();
  keyboardInit();
  mouseInit();
  uiInit();
  cadInit();
  cliInit();
  sceneSetViewAngle(0, 0, 0); //Set plane to XY
  glutMainLoop();
  return 0;             /* ANSI C requires main to return int. */
}
