#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include <iostream>
#include <Scene.h>
#include <Keyboard.h>
#include <Menu.h>
#include <Window.h>

using namespace std;

/*void animation(void)
{
  ax += 5.0;
  ay -= 2.0;
  az += 5.0;

  if (ax >= 360)
    ax = 0.0;
  if (ay <= -360)
    ay = 0.0;
  if (az >= 360)
    az = 0.0;
  glutPostRedisplay();
  count++;
  if (count >= 60)
    glutIdleFunc(NULL);
}*/

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  windowInit();
  sceneInit();
  menuInit();
  keyboardInit();
  glutMainLoop();
  return 0;             /* ANSI C requires main to return int. */
}
