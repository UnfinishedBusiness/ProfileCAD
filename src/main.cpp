#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include <iostream>
#include <Scene.h>
#include <Keyboard.h>
#include <Menu.h>
#include <Window.h>
#include <application.h>

using namespace std;

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
