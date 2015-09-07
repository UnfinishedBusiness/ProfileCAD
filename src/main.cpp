#include <application.h>

using namespace std;

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  windowInit();
  sceneInit();
  menuInit();
  keyboardInit();
  mouseInit();
  uiInit();
  cadInit();
  glutMainLoop();
  return 0;             /* ANSI C requires main to return int. */
}
